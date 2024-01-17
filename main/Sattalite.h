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

const uint32_t GPSBaud = 9600;
const byte BMP180_12C_Address= 0x77;
const int RXD1=14;
const int TXD1=15;
const int SerialBaud = 115200;

struct CollectiveSensorData
{
    std::string MISSION_ID, MISSION_TIME, PACKET_COUNT, MODE, STATE,
    ALTITUDE, PC_DEPLOYED, TEMPERATURE, PRESSURE, GPS_TIME,
    GPS_ALTITUDE, GPS_LATITUDE, GPS_LONGITUDE,
    GPS_SATS, ACC_X, ACC_Y, ACC_Z, MAG_X, MAG_Y, MAG_Z, CMD_ECHO;
};

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

    SatComm satComm;

    int n_packetsSent=0;
    std::string missionID;
    long missionStartTime;
    State state = State::standby;
    std::string fileName;
public:
    Sattalite(std::string);
    //test methods
    void SDCardTest();
    void BMP180Test();
    void MPUTest();
    void GPSTest();
    void fedGPSTest();
    void QMCTest();
    //make some const if you can
    void CommandRecieved(std::string);

    bool detectTakeOff();
    bool missionFinished() const;
    CollectiveSensorData GatherSensorData();
    void logToSD(const CollectiveSensorData&);
    void logToSD(const std::string&);
    void feedGPS();
    void handleTelemetry(const CollectiveSensorData&);

    void activateCAM() const;
    void sendDataToGC(const CollectiveSensorData&); 
    void sendDataToGC(const std::string&);
    State getState() const;
    void listenFromCam() const;
};
