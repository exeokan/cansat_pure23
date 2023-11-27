#pragma once

#include "SDCard.h"
#include <Arduino.h>
#include <Wire.h>
#include <BMP180I2C.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <TinyGPSPlus.h>
#include <string>
#include <QMC5883LCompass.h>

const int GPS_RX_Pin = 26,  GPS_TX_Pin = 25;
const uint32_t GPSBaud = 9600;
const byte BMP180_12C_Address= 0x77;
const int RXD1=14;
const int TXD1=15;

struct CollectiveSensorData
{
    std::string TEAM_ID, MISSION_TIME, PACKET_COUNT, MODE, STATE,
    ALTITUDE, PC_DEPLOYED, TEMPERATURE, VOLTAGE, PRESSURE, GPS_TIME,
    GPS_ALTITUDE, GPS_LATITUDE, GPS_LONGITUDE,
    GPS_SATS, TILT_X, TILT_Y, CMD_ECHO;
};
//std::string state_names[]={"STANDBY","ASCENT","DESCENT", "LANDED"};
class Sattalite
{
private:
    SDCard sdCard;
    BMP180I2C bmp180;
    Adafruit_MPU6050 mpu;
    TinyGPSPlus gps;
    QMC5883LCompass compass;

    int n_packetsSent=0;
    std::string missionID;
    long missionStartTime;
    int state = 0;
    bool pc_deployed=false;
    double tilt_xyz[3];
public:
    Sattalite(std::string);
    //test methods
    void SDCardTest();
    void BMP180Test();
    void MPUTest();
    void GPSTest();
    void QMCTest();
    //make some const if you can
    
    double* getTempPressure(); 
    bool detectTakeOff();
    bool missionFinished();
    bool isLanded();
    CollectiveSensorData GatherSensorData();
    void logToSD(CollectiveSensorData);

    void activateCAM();
    void establishGC_Communincation();
    void sendDataToGC(CollectiveSensorData); 
    void sendCommandToGC();

    void calculateTilt();
};
/*
static void smartDelay(unsigned long ms)
{
  unsigned long start = millis();
  do 
  {
    while (ss.available())
      gps.encode(ss.read());
  } while (millis() - start < ms);
}*/