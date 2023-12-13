#include "Sattalite.h"
void displayGPSInfo(TinyGPSPlus &gps);
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
