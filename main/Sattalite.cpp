#include "Sattalite.h"
void displayGPSInfo(SoftwareSerial &ss, TinyGPSPlus &gps);

Sattalite::Sattalite(std::string missionID): bmp180(BMP180_12C_Address), ss(GPS_RX_Pin, GPS_TX_Pin), missionID(missionID)
{
    if(!Serial){
        Serial.begin(115200); //when usb is disconnected?
    }
    /*BMP180 init*/
    Wire.begin();
    if (!bmp180.begin())
    {
      Serial.println("begin() failed. check your BMP180 Interface and I2C Address.");
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
    ss.begin(GPSBaud);
    /*QMC Init*/
    compass.init();
    //Serial for espcam comm
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
  while (ss.available() > 0)
    if (gps.encode(ss.read()))
      displayGPSInfo(ss, gps);

  if (millis() > 5000 && gps.charsProcessed() < 10)
  {
    Serial.println(F("No GPS detected: check wiring."));
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
/*
std::string TEAM_ID, MISSION_TIME, PACKET_COUNT, MODE, STATE,
    ALTITUDE, PC_DEPLOYED, TEMPERATURE, VOLTAGE, PRESSURE, GPS_TIME,
    GPS_ALTITUDE, GPS_LATITUDE, GPS_LONGITUDE,
    GPS_SATS, TILT_X, TILT_Y, CMD_ECHO;
*/
CollectiveSensorData Sattalite::GatherSensorData()
{
  CollectiveSensorData data;

  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  data.TEAM_ID = "5655";
  data.MISSION_TIME="";
  data.PACKET_COUNT=n_packetsSent; n_packetsSent++; //burada mı yapmalı
  data.MODE="FLIGHT";
  data.STATE="";
  data.ALTITUDE="";
  data.PC_DEPLOYED="";
  data.TEMPERATURE=""; 
  data.VOLTAGE=""; 
  data.PRESSURE=""; 
  data.GPS_TIME="";
  data.GPS_ALTITUDE=""; 
  data.GPS_LATITUDE=""; 
  data.GPS_LONGITUDE="";
  data.GPS_SATS=""; 
  data.TILT_X=""; 
  data.TILT_Y=""; 
  data.CMD_ECHO="";
}

void Sattalite::logToSD(CollectiveSensorData)
{

}

void displayGPSInfo(SoftwareSerial &ss, TinyGPSPlus &gps){
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
