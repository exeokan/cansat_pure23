#include "Sattalite.h"
#include "SattaliteTestFuncs.h"

std::string state_names[]={"STANDBY", "ASCENT", "DESCENT", "LANDED"};

Sattalite::Sattalite(std::string missionID): missionID(missionID)
  , missionStartTime(millis())
{
    if(!Serial){
        Serial.begin(115200); //when usb is disconnected?
    }
    /*BMP180 init*/
    Wire.begin();
    if (!bmp.begin())
    {
      Serial.println("begin() failed. check your BMP180 Interface and I2C Address.");
      //error code
    }
    else
    {
      Serial.println("BMP180 Found!");
    }
    
    /*MPU6050 init*/
    if (!mpu.begin()) {
      Serial.println("Failed to find MPU6050 chip");
    }
    else{
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
    fileName= "/record" + missionID + ".txt";
}

void Sattalite::feedGPS()
{
  while (Serial2.available())
    gps.encode(Serial2.read());

  if (gps.charsProcessed() < 10)
  {
    Serial.println(F("No GPS detected: check wiring."));
    //error code
  }
  else{
    //remove error
  }
}
void Sattalite::calculateTilt()
{
  //if mpu is fine-> otw null pointer error will come!
  static long lastTime = millis(); 
  
  sensors_event_t g;
  mpu_gyro->getEvent(&g);

  double timeDif= (millis()-lastTime)/1000.0; // in seconds
  tilt_xyz[0] += g.gyro.x * timeDif;
  tilt_xyz[1] += g.gyro.y * timeDif;
  tilt_xyz[2] += g.gyro.z * timeDif;

  lastTime= millis();
}

CollectiveSensorData Sattalite::GatherSensorData()
{
  CollectiveSensorData data;

  data.TEAM_ID = "5655";
  data.MISSION_TIME= std::to_string((millis()-missionStartTime)/1000.0); //seconds
  data.PACKET_COUNT= std::to_string(n_packetsSent); n_packetsSent++;
  data.MODE="FLIGHT";
  data.STATE = state_names[state];
  //gps data
  data.GPS_ALTITUDE= gps.altitude.isValid() ? std::to_string(gps.altitude.meters()):  "?"; 
  data.GPS_LONGITUDE= gps.location.isValid() ? std::to_string(gps.location.lng()) : "?"; 
  data.GPS_LATITUDE= gps.location.isValid() ? std::to_string(gps.location.lat()) : "?"; 
  if(gps.time.isValid()){
    data.GPS_TIME = std::to_string(gps.time.hour()) + ":" +
    std::to_string(gps.time.minute())+ ":" +
    std::to_string(gps.time.second());
  }
  else{
    data.GPS_TIME= "?";
  }
  data.GPS_SATS= gps.satellites.isValid() ? std::to_string(gps.satellites.value()) : "?";

  data.PC_DEPLOYED= std::to_string(pc_deployed);
  data.VOLTAGE="9.0"; //?
  //bmp data
  data.TEMPERATURE=std::to_string(bmp.readTemperature()); 
  // Calculate altitude assuming 'standard' barometric(!!!!!)
  // pressure of 1013.25 millibar = 101325 Pascal
  data.PRESSURE=std::to_string(bmp.readPressure()); 
  data.ALTITUDE=std::to_string(bmp.readAltitude()); 
  //mpu
  //if mpu is fine-> otw null pointer error will come!
  sensors_event_t accel;
  mpu_accel->getEvent(&accel);

  data.ACC_X = std::to_string(accel.acceleration.x);
  data.ACC_Y = std::to_string(accel.acceleration.y);
  data.ACC_Z = std::to_string(accel.acceleration.z);

  data.TILT_X = std::to_string(tilt_xyz[0]); 
  data.TILT_Y = std::to_string(tilt_xyz[1]); 
  //qmc
  compass.read();
  data.MAG_X = std::to_string(compass.getX());
  data.MAG_Y = std::to_string(compass.getY());
  data.MAG_Z = std::to_string(compass.getZ());
  //cmd
  data.CMD_ECHO="";
  return data;
}

void Sattalite::logToSD(CollectiveSensorData data)
{
  std::ostringstream oss;
    oss << "{"
        << "\"TEAM_ID\":\"" << data.TEAM_ID << "\","
        << "\"MISSION_TIME\":\"" << data.MISSION_TIME << "\","
        << "\"PACKET_COUNT\":\"" << data.PACKET_COUNT << "\","
        << "\"MODE\":\"" << data.MODE << "\","
        << "\"STATE\":\"" << data.STATE << "\","
        << "\"ALTITUDE\":\"" << data.ALTITUDE << "\","
        << "\"PC_DEPLOYED\":\"" << data.PC_DEPLOYED << "\","
        << "\"TEMPERATURE\":\"" << data.TEMPERATURE << "\","
        << "\"VOLTAGE\":\"" << data.VOLTAGE << "\","
        << "\"PRESSURE\":\"" << data.PRESSURE << "\","
        << "\"GPS_TIME\":\"" << data.GPS_TIME << "\","
        << "\"GPS_ALTITUDE\":\"" << data.GPS_ALTITUDE << "\","
        << "\"GPS_LATITUDE\":\"" << data.GPS_LATITUDE << "\","
        << "\"GPS_LONGITUDE\":\"" << data.GPS_LONGITUDE << "\","
        << "\"GPS_SATS\":\"" << data.GPS_SATS << "\","
        << "\"ACC_X\":\"" << data.ACC_X << "\","
        << "\"ACC_Y\":\"" << data.ACC_Y << "\","
        << "\"ACC_Z\":\"" << data.ACC_Z << "\","
        << "\"MAG_X\":\"" << data.MAG_X << "\","
        << "\"MAG_Y\":\"" << data.MAG_Y << "\","
        << "\"MAG_Z\":\"" << data.MAG_Z << "\""
        << "}\n";
  std::string str=oss.str();
  Serial.println( str.substr(0,150).c_str() );
  Serial.println( str.substr(150).c_str() );
  Serial.print("Length:"); Serial.println(str.length());

  sdCard.appendFile(fileName.c_str(), oss.str().c_str());
}

State Sattalite::getState(){
  return state;
}

void Sattalite::listenFromCam()
{
  while(Serial1.available()){
    int inByte = Serial1.read();
    Serial.write(inByte);
  }
}

bool Sattalite::missionFinished()
{
  return false;
}


void Sattalite::activateCAM()
{
  Serial1.write("BEGIN");
}
void Sattalite::sendDataToGC(CollectiveSensorData){}
