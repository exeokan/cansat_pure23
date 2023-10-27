#pragma once

#include "SDCard.h"
#include <Arduino.h>
#include <Wire.h>
#include <BMP180I2C.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <TinyGPSPlus.h>
#include <SoftwareSerial.h>
#include <string>

const int GPS_RX_Pin = 26,  GPS_TX_Pin = 25;
const uint32_t GPSBaud = 4800;
const byte BMP180_12C_Address= 0x77;

/*
“TEAM_ID, MISSION_TIME, PACKET_COUNT, MODE, STATE,
ALTITUDE, HS_DEPLOYED, PC_DEPLOYED, MAST_RAISED,
TEMPERATURE, VOLTAGE, PRESSURE, GPS_TIME,
GPS_ALTITUDE, GPS_LATITUDE, GPS_LONGITUDE,
GPS_SATS, TILT_X, TILT_Y, CMD_ECHO”
*/
struct CollectiveSensorData
{
    std::string TEAM_ID, MISSION_TIME, PACKET_COUNT, MODE, STATE,
    ALTITUDE, PC_DEPLOYED, TEMPERATURE, VOLTAGE, PRESSURE, GPS_TIME,
    GPS_ALTITUDE, GPS_LATITUDE, GPS_LONGITUDE,
    GPS_SATS, TILT_X, TILT_Y, CMD_ECHO;
};

class Sattalite
{
private:
    SDCard sdCard;
    BMP180I2C bmp180;
    Adafruit_MPU6050 mpu;
    TinyGPSPlus gps;
    SoftwareSerial ss;// The serial connection to the GPS device
public:
    Sattalite(/* args */);
    ~Sattalite();
    void SDCardTest();
    void BMP180Test();
    void MPUTest();
    void GPSTest();

    void establishConnection();
    void isLanded() const;
    CollectiveSensorData GatherSensorData();
    void logToSD(CollectiveSensorData);
    void sendDataToGC(CollectiveSensorData) const; 
    void sendCommandToGC();
    void savePictures();
};
