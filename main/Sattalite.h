#pragma once

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_BMP085.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <TinyGPSPlus.h>
#include <string>
#include <QMC5883LCompass.h>
#include <sstream>

#include "SatComm.h"
#include "SDCard.h"


const uint32_t GPSBaud = 4800;
const byte BMP180_12C_Address= 0x77;
const int RXD1=14;
const int TXD1=15;

enum State{
    standby = 0, 
    ascent = 1,
    descent = 2,
    landed = 3
};
extern std::string state_names[];
class Sattalite
{
private:
    SDCard sdCard;
    Adafruit_BMP085 bmp;
    Adafruit_MPU6050 mpu;
    Adafruit_Sensor *mpu_accel, *mpu_gyro;

    TinyGPSPlus gps;
    QMC5883LCompass compass;

    int n_packetsSent=0;
    std::string missionID;
    long missionStartTime;
    State state = State::standby;
    bool pc_deployed=false;
    std::string fileName;
    SatComm satComm;
public:
    Sattalite(std::string);
    double tilt_xyz[3]={0, 0, 0}; //!
    //test methods
    void SDCardTest();
    void BMP180Test();
    void MPUTest();
    void GPSTest();
    void fedGPSTest();
    void QMCTest();
    //make some const if you can
    
    bool detectTakeOff();
    bool missionFinished();
    bool isLanded();
    CollectiveSensorData GatherSensorData();
    void logToSD(const CollectiveSensorData&);
    void feedGPS();

    void activateCAM();
    void sendDataToGC(const CollectiveSensorData&); 

    void calculateTilt();
    State getState();
    void listenFromCam();
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
