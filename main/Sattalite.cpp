#include "Sattalite.h"

std::string state_names[]={"STANDBY", "ASCENT", "DESCENT", "LANDED"};

void displayGPSInfo(TinyGPSPlus &gps);

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
    /*MPU6050 init*/
    if (!mpu.begin()) {
      Serial.println("Failed to find MPU6050 chip");
    }
    else{
      Serial.println("MPU6050 Found!");
      mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
      mpu.setGyroRange(MPU6050_RANGE_500_DEG);
      mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);
    }
    /*GPS init*/
    Serial2.begin(GPSBaud);
    /*QMC Init*/
    compass.init();
    /*Serial for espcam comm*/
    Serial1.begin(115200, SERIAL_8N1, RXD1, TXD1);  

}


void Sattalite::SDCardTest(){
  sdCard.testRun();
}

void Sattalite::BMP180Test()
{
   Serial.print("Temperature = ");
    Serial.print(bmp.readTemperature());
    Serial.println(" *C");
    
    Serial.print("Pressure = ");
    Serial.print(bmp.readPressure());
    Serial.println(" Pa");
    
    // Calculate altitude assuming 'standard' barometric
    // pressure of 1013.25 millibar = 101325 Pascal
    Serial.print("Altitude = ");
    Serial.print(bmp.readAltitude());
    Serial.println(" meters");

    Serial.print("Pressure at sealevel (calculated) = ");
    Serial.print(bmp.readSealevelPressure());
    Serial.println(" Pa");

    // you can get a more precise measurement of altitude
    // if you know the current sea level pressure which will
    // vary with weather and such. If it is 1015 millibars
    // that is equal to 101500 Pascals.
    Serial.print("Real altitude = ");
    Serial.print(bmp.readAltitude(101500));
    Serial.println(" meters");
}

void Sattalite::MPUTest()
{
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  /* Print out the values */
  Serial.print("Acceleration X: ");
  Serial.print(a.acceleration.x);
  Serial.print(", Y: ");
  Serial.print(a.acceleration.y);
  Serial.print(", Z: ");
  Serial.print(a.acceleration.z);
  Serial.println(" m/s^2");

  Serial.print("Rotation X: ");
  Serial.print(g.gyro.x);
  Serial.print(", Y: ");
  Serial.print(g.gyro.y);
  Serial.print(", Z: ");
  Serial.print(g.gyro.z);
  Serial.println(" rad/s");

  Serial.print("Temperature: ");
  Serial.print(temp.temperature);
  Serial.println(" degC");
}

void Sattalite::GPSTest()
{
  // This sketch displays information every time a new sentence is correctly encoded.
  long gps_begin= millis();
  while (Serial2.available() > 0)
    if (gps.encode(Serial2.read()))
      displayGPSInfo(gps); 
  if (gps.charsProcessed() < 10)
  {
    Serial.println(F("No GPS detected: check wiring."));
    //error code
  }
}
void Sattalite::fedGPSTest(){
      std::string GPS_ALTITUDE= gps.altitude.isValid() ? std::to_string(gps.altitude.meters()):  "?"; 
      std::string GPS_LONGITUDE= gps.location.isValid() ? std::to_string(gps.location.lng()) : "?"; 
      std::string GPS_LATITUDE= gps.location.isValid() ? std::to_string(gps.location.lat()) : "?"; 
      std::string GPS_TIME;
      if(gps.time.isValid()){
        GPS_TIME = std::to_string(gps.time.hour()) + ":" +
        std::to_string(gps.time.minute())+ ":" +
        std::to_string(gps.time.second());
      }
      else{
        GPS_TIME= "?";
      }
      std::string GPS_SATS= gps.satellites.isValid() ? std::to_string(gps.satellites.value()) : "?";
      //print
      Serial.print("Altitude: ");
      Serial.println(GPS_ALTITUDE.c_str());

      Serial.print("Longitude: ");
      Serial.println(GPS_LONGITUDE.c_str());

      Serial.print("Latitude: ");
      Serial.println(GPS_LATITUDE.c_str());

      Serial.print("Time: ");
      Serial.println(GPS_TIME.c_str());

      Serial.print("Satellites: ");
      Serial.println(GPS_SATS.c_str());
}
bool Sattalite::missionFinished()
{
  return false;
}

void Sattalite::QMCTest()
{
  int x, y, z;
  // Read compass values
  compass.read();
  byte a = compass.getAzimuth();
  Serial.print("Azimuth: ");
  Serial.println(a);

  char myArray[3];
  compass.getDirection(myArray, a);
  
  Serial.print(myArray[0]);
  Serial.print(myArray[1]);
  Serial.println(myArray[2]);

  // Return XYZ readings
  x = compass.getX();
  y = compass.getY();
  z = compass.getZ();
  // Print
  Serial.print("X: ");
  Serial.print(x);
  Serial.print(" Y: ");
  Serial.print(y);
  Serial.print(" Z: ");
  Serial.print(z);
  Serial.println();
  
  delay(250);
}

void Sattalite::activateCAM()
{
  Serial1.write("BEGIN");
}
void Sattalite::calculateTilt()
{
  //if mpu is fine->
  static long lastTime = millis(); 

  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);
  
  double timeDif= (millis()-lastTime)/1000.0; // in seconds
  Serial.println(timeDif);
  Serial.println(g.gyro.x);
  Serial.println(g.gyro.y);
  Serial.println(g.gyro.z);
  Serial.println("--------------");
  tilt_xyz[0] += g.gyro.x * timeDif;
  tilt_xyz[1] += g.gyro.y * timeDif;
  tilt_xyz[2] += g.gyro.z * timeDif;

  lastTime= millis();
}
/*
std::string TEAM_ID, MISSION_TIME, PACKET_COUNT, MODE, STATE,
    ALTITUDE, PC_DEPLOYED, TEMPERATURE, VOLTAGE, PRESSURE, GPS_TIME,
    GPS_ALTITUDE, GPS_LATITUDE, GPS_LONGITUDE,
    GPS_SATS, TILT_X, TILT_Y, CMD_ECHO;
*/

CollectiveSensorData Sattalite::GatherSensorData()
{
  CollectiveSensorData data;

  data.TEAM_ID = "5655";
  data.MISSION_TIME= (millis()-missionStartTime)/1000.0; //seconds
  data.PACKET_COUNT=n_packetsSent; n_packetsSent++;
  data.MODE="FLIGHT";
  data.STATE = state_names[state];

  long gps_begin= millis();
  //gps data
  //try two times?
  bool successfulRead = false;
  for (int i = 0; i < 2; i++)
  {
    while (Serial2.available() > 0) {
      if (gps.encode(Serial2.read())){
        successfulRead = true;
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
      }
    }
    if(successfulRead)
      break;
    delay(100);
  }
  if(!successfulRead){
    data.GPS_ALTITUDE, data.GPS_LONGITUDE, data.GPS_LATITUDE, data.GPS_TIME, data.GPS_SATS= "NaN";
  }   
  if (gps.charsProcessed() < 10)
  {
    Serial.println(F("No GPS detected: check wiring."));
    //error code
    
  }
  else{
    //remove error
  }

  data.PC_DEPLOYED= std::to_string(pc_deployed);
  data.VOLTAGE="9.0"; //?

  data.TEMPERATURE=std::to_string(bmp.readTemperature()); 
  // Calculate altitude assuming 'standard' barometric(!!!!!)
  // pressure of 1013.25 millibar = 101325 Pascal
  data.PRESSURE=std::to_string(bmp.readAltitude()); 

  data.TILT_X=tilt_xyz[0]; 
  data.TILT_Y=tilt_xyz[1]; 
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
        << "\"TILT_X\":\"" << data.TILT_X << "\","
        << "\"TILT_Y\":\"" << data.TILT_Y << "\","
        << "\"CMD_ECHO\":\"" << data.CMD_ECHO << "\""
        << "}";
  std::string str=oss.str();
  Serial.println( str.c_str() );
}

void displayGPSInfo(TinyGPSPlus &gps){
  Serial.print(F("Location: ")); 
  if (gps.location.isValid())
  {
    Serial.print(gps.location.lat(), 6);
    Serial.print(F(","));
    Serial.print(gps.location.lng(), 6);
  }
  else
  {
    Serial.print(F("INVALID"));
  }

  Serial.print(F("  Date/Time: "));
  if (gps.date.isValid())
  {
    Serial.print(gps.date.month());
    Serial.print(F("/"));
    Serial.print(gps.date.day());
    Serial.print(F("/"));
    Serial.print(gps.date.year());
  }
  else
  {
    Serial.print(F("INVALID"));
  }

  Serial.print(F(" "));
  if (gps.time.isValid())
  {
    if (gps.time.hour() < 10) Serial.print(F("0"));
    Serial.print(gps.time.hour());
    Serial.print(F(":"));
    if (gps.time.minute() < 10) Serial.print(F("0"));
    Serial.print(gps.time.minute());
    Serial.print(F(":"));
    if (gps.time.second() < 10) Serial.print(F("0"));
    Serial.print(gps.time.second());
    Serial.print(F("."));
    if (gps.time.centisecond() < 10) Serial.print(F("0"));
    Serial.print(gps.time.centisecond());
  }
  else
  {
    Serial.print(F("INVALID"));
  }

  Serial.println();
}

State Sattalite::getState(){
  return state;
}
