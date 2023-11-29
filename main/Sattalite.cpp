#include "Sattalite.h"
void displayGPSInfo(TinyGPSPlus &gps);

Sattalite::Sattalite(std::string missionID): bmp180(BMP180_12C_Address), missionID(missionID)
  , missionStartTime(millis())
{
    if(!Serial){
        Serial.begin(115200); //when usb is disconnected?
    }
    /*BMP180 init*/
    Wire.begin();
    if (!bmp180.begin())
    {
      Serial.println("begin() failed. check your BMP180 Interface and I2C Address.");
      //error code
    }
    else{
      bmp180.resetToDefaults();
      //enable ultra high resolution mode for pressure measurements
      bmp180.setSamplingMode(BMP180MI::MODE_UHR);
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
  if (!bmp180.measureTemperature())
	{
		Serial.println("could not start temperature measurement, is a measurement already running?");
		return;
	}

	//wait for the measurement to finish. proceed as soon as hasValue() returned true. 
	do
	{
		delay(100);
	} while (!bmp180.hasValue());

	Serial.print("Temperature: "); 
	Serial.print(bmp180.getTemperature()); 
	Serial.println(" degC");

	//start a pressure measurement. pressure measurements depend on temperature measurement, you should only start a pressure 
	//measurement immediately after a temperature measurement. 
	if (!bmp180.measurePressure())
	{
		Serial.println("could not start perssure measurement, is a measurement already running?");
		return;
	}

	//wait for the measurement to finish. proceed as soon as hasValue() returned true. 
	do
	{
		delay(100);
	} while (!bmp180.hasValue());

	Serial.print("Pressure: "); 
	Serial.print(bmp180.getPressure());
	Serial.println(" Pa");
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
    while (ss.available() > 0) {
      if (gps.encode(Serial2.read())){
        succesfulRead = true;
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
    if(succesfulRead)
      break;
    delay(100);
  }
  if(!succesfulRead){
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

  double* bmp_res= getTempPressure();//if bmp fine
  if(bmp_res[0] == -1 || bmp_res[1] == -1);
    //error code
  data.TEMPERATURE=std::to_string(bmp_res[0]); 
  data.PRESSURE=std::to_string(bmp_res[1]); 
  delete bmp_res;

  data.TILT_X=tilt_xyz[0]; 
  data.TILT_Y=tilt_xyz[1]; 
  data.CMD_ECHO="";
}

void Sattalite::logToSD(CollectiveSensorData)
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
  Serial.println( oss.str() );
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

double* Sattalite::getTempPressure()
{
  double* result = new double[2]{-1, -1};
  //check error flags?
  if (!bmp180.measureTemperature())
	{
		Serial.println("could not start temperature measurement, is a measurement already running?");
		return result;
	}

	//wait for the measurement to finish. proceed as soon as hasValue() returned true. 
	do
	{
		delay(100);
	} while (!bmp180.hasValue());

	result[0] = bmp180.getTemperature(); 

	//start a pressure measurement. pressure measurements depend on temperature measurement, you should only start a pressure 
	//measurement immediately after a temperature measurement. 
	if (!bmp180.measurePressure())
	{
		return result;
	}

	//wait for the measurement to finish. proceed as soon as hasValue() returned true. 
	do
	{
		delay(100);
	} while (!bmp180.hasValue());

	result[1] = bmp180.getPressure();
  return result;
}

bool Sattalite::getState(){
  return state;
}