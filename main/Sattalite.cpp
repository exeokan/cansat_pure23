#include "Sattalite.h"
#include "SattaliteTestFuncs.h"

std::string state_names[] = {"STANDBY", "ASCENT", "DESCENT", "LANDED"};

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
Sattalite::Sattalite(std::string missionID) : missionID(missionID), missionStartTime(millis()),
  satComm(std::bind(&Sattalite::CommandRecieved, this, std::placeholders::_1))
{
  if (!Serial)
  {
    Serial.begin(115200); // when usb is disconnected?
  }
  /*BMP180 init*/
  Wire.begin();
  if (!bmp.begin())
  {
    Serial.println("begin() failed. check your BMP180 Interface and I2C Address.");
    // error code
  }
  else
  {
    Serial.println("BMP180 Found!");
  }

  /*MPU6050 init*/
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
  Serial1.begin(115200, SERIAL_8N1, RXD1, TXD1);
  fileName = "/record" + missionID + ".txt";
}

void Sattalite::CommandRecieved(std::string command)
{
  if(command=="REL"){
    state=State::descent;
    //deploy parachute
  }
  else if (command=="CAM")
  {
    Serial.println("Activating cam...");
    activateCAM();
  }
}

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
  // parac deployed
  data.PC_DEPLOYED = std::to_string(pc_deployed);
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
  // cmd
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
  return false;
}

// Activate the camera module
void Sattalite::activateCAM() const
{
  Serial1.write(("B" + missionID).c_str());
}
