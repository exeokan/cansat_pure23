#include "Sattalite.h"
#include "SattaliteTestFuncs.h"

std::string state_names[] = {"STANDBY", "ASCENT", "DESCENT", "LANDED"};

//Concatenate the sensor data into a single string to be sent to the ground control as telemetry
std::string concatenateSensorData(const CollectiveSensorData &data)
{
  std::stringstream ss;

  ss << data.MISSION_ID << "," << data.MISSION_TIME << "," << data.PACKET_COUNT << "," << data.MODE << ","
     << data.STATE << "," << data.ALTITUDE << "," << data.PC_DEPLOYED << "," << data.TEMPERATURE << ","
     << data.PRESSURE << "," << data.GPS_TIME << "," << data.GPS_ALTITUDE << ","
     << data.GPS_LATITUDE << "," << data.GPS_LONGITUDE << "," << data.GPS_SATS << "," << data.ACC_X << ","
     << data.ACC_Y << "," << data.ACC_Z << "," << data.MAG_X << "," << data.MAG_Y << "," << data.MAG_Z << ","
     << data.CMD_ECHO << "\n";

  return ss.str();
}

/**
 * @brief Constructor for the Sattalite class
 * @param missionID The mission ID of the satellite, a random string of numerals
*/
Sattalite::Sattalite(std::string missionID) : missionID(missionID), missionStartTime(millis()),
  satComm(std::bind(&Sattalite::CommandRecieved, this, std::placeholders::_1)), // bind the command received method to the satComm
  n_packetsSent(0), state(State::standby), 
  fileName("/record" + missionID + ".txt") // Set the file name for the SD card
{
  if (!Serial)
  {
    Serial.begin(SerialBaud);
  }
  /* BMP180 init */
  Wire.begin();
  if (!bmp.begin())
    Serial.println("begin() failed. check your BMP180 Interface and I2C Address.");
  else
    Serial.println("BMP180 Found!");
  /* MPU6050 init */
  if (!mpu.begin())
  {
    Serial.println("Failed to find MPU6050 chip");
  }
  else
  {
    Serial.println("MPU6050 Found!");
    mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
    mpu.setGyroRange(MPU6050_RANGE_500_DEG);
    mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);

    mpu_accel = mpu.getAccelerometerSensor();
    mpu_gyro = mpu.getGyroSensor();
  }
  /*GPS init*/
  Serial2.begin(GPSBaud);
  /*QMC Init*/
  compass.init();
  /*Serial for espcam comm*/
  Serial1.begin(SerialBaud, SERIAL_8N1, RXD1, TXD1);
}

// Process the command received from the ground control
void Sattalite::CommandRecieved(std::string recievedCommand)
{
  if (recievedCommand=="STA"){
    state = State::ascent;
    activateCAM();
  }
  else if(recievedCommand=="DSC"){
    state = State::descent;
  }
  else if(recievedCommand=="FIN"){
    //this will enable buzzer in the main loop and stop sending data to the ground control
    state = State::landed;
  }
}


// Feed the GPS data to the TinyGPSPlus object
void Sattalite::feedGPS()
{
  while (Serial2.available() > 0)
  {
    gps.encode(Serial2.read());
  }
  if (gps.charsProcessed() < 10)
  {
    Serial.println(F("No GPS detected: check wiring."));
  }
}


//Gather sensor data from the satellite
CollectiveSensorData Sattalite::GatherSensorData()
{
  CollectiveSensorData data;

  data.MISSION_ID = missionID;
  data.MISSION_TIME = std::to_string((millis() - missionStartTime) / 1000.0); // seconds
  data.PACKET_COUNT = std::to_string(n_packetsSent);
  n_packetsSent++;
  data.MODE = "FLIGHT";
  data.STATE = state_names[state];
  // gps data
  data.GPS_ALTITUDE = gps.altitude.isValid() ? std::to_string(gps.altitude.meters()) : "?";
  data.GPS_LONGITUDE = gps.location.isValid() ? std::to_string(gps.location.lng()) : "?";
  data.GPS_LATITUDE = gps.location.isValid() ? std::to_string(gps.location.lat()) : "?";
  if (gps.time.isValid())
  {
    data.GPS_TIME = std::to_string(gps.time.hour()) + ":" +
                    std::to_string(gps.time.minute()) + ":" +
                    std::to_string(gps.time.second());
  }
  else
  {
    data.GPS_TIME = "?";
  }
  data.GPS_SATS = gps.satellites.isValid() ? std::to_string(gps.satellites.value()) : "?";
  // parachute deployed
  data.PC_DEPLOYED = std::to_string(state == State::descent);
  // bmp data
  data.TEMPERATURE = std::to_string(bmp.readTemperature());
  // Calculate altitude assuming 'standard' barometric(!!!!!) pressure of 1013.25 millibar = 101325 Pascal
  data.PRESSURE = std::to_string(bmp.readPressure());
  data.ALTITUDE = std::to_string(bmp.readAltitude());
  // mpu
  // if mpu is fine-> otw null pointer error will come!
  sensors_event_t accel;
  mpu_accel->getEvent(&accel);

  data.ACC_X = std::to_string(accel.acceleration.x);
  data.ACC_Y = std::to_string(accel.acceleration.y);
  data.ACC_Z = std::to_string(accel.acceleration.z);
  // qmc
  compass.read();
  data.MAG_X = std::to_string(compass.getX());
  data.MAG_Y = std::to_string(compass.getY());
  data.MAG_Z = std::to_string(compass.getZ());
  // return the last command received from the ground control
  data.CMD_ECHO = satComm.lastCommand;
  return data;
}

// Log sensor data to the SD card
void Sattalite::logToSD(const CollectiveSensorData &data)
{
  std::string str = concatenateSensorData(data);
  sdCard.appendFile(fileName.c_str(), str.c_str());
}

// Log string data to the SD card
void Sattalite::logToSD(const std::string &str)
{
  sdCard.appendFile(fileName.c_str(), str.c_str());
}

// Handle telemetry by logging sensor data to the SD card and sending it to the ground control
void Sattalite::handleTelemetry(const CollectiveSensorData& sensorData)
{
  std::string concatenatedSensorData = concatenateSensorData(sensorData);
  logToSD(concatenatedSensorData);
  sendDataToGC(concatenatedSensorData);
}

// Send telemetry data to the ground control
void Sattalite::sendDataToGC(const std::string &concatenatedSensorData)
{
  Packet packet;
  strcpy(packet.str, concatenatedSensorData.c_str());
  satComm.sendDataToGC(packet);
}

// Send telemetry data to the ground control
void Sattalite::sendDataToGC(const CollectiveSensorData &sensorData)
{
  Packet packet;
  std::string concatenatedSensorData = concatenateSensorData(sensorData);
  Serial.println(concatenatedSensorData.c_str());
  strcpy(packet.str, concatenatedSensorData.c_str());
  satComm.sendDataToGC(packet);
}
// Get the current state of the satellite
State Sattalite::getState() const
{
  return state;
}

// Listen for serial data from the camera module
// this is necessary since the camera module is connected to the ESP32 via its only serial bus
void Sattalite::listenFromCam() const
{
  while (Serial1.available())
  {
    int inByte = Serial1.read();
    Serial.write(inByte);
  }
}

// Check if the mission has finished
bool Sattalite::missionFinished() const
{
  return state == State::landed;
}

// Activate the camera module
void Sattalite::activateCAM() const
{
  Serial1.write(("B" + missionID).c_str());
}
