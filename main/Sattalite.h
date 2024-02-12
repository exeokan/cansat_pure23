/**
 * @file Sattalite.h
 * @brief This file contains the declaration of the Sattalite class and related structures and enums.
 */
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
extern std::string state_names[]; //defined in Sattalite.cpp, used to convert enum state data into string equivalent

/**
 * @class Sattalite
 * @brief The Sattalite class represents a satellite object that collects sensor data and communicates with ground control.
 * It contains member variables and methods for handling various sensors such as BMP085, MPU6050, GPS, and QMC5883LCompass.
 * The class also includes methods for testing the functionality of each sensor, logging data to an SD card, and sending data to ground control.
 * The class provides methods for detecting takeoff, checking if the mission is finished, gathering sensor data, and handling telemetry.
 * Additionally, it has methods for activating the camera(ESP32-CAM), listening for data from the camera, and receiving commands from ground control.
 */
class Sattalite
{
private:
    Adafruit_BMP085 bmp;
    Adafruit_MPU6050 mpu;
    Adafruit_Sensor *mpu_accel, *mpu_gyro;
    TinyGPSPlus gps;
    QMC5883LCompass compass;

    SDCard sdCard;
    SatComm satComm;

    int n_packetsSent;
    std::string missionID, fileName;
    long missionStartTime;
    State state;
public:
    Sattalite(std::string);
    //test methods
    void SDCardTest();
    void BMP180Test();
    void MPUTest();
    void GPSTest();
    void fedGPSTest();
    void QMCTest();
    
    void CommandRecieved(std::string);

    bool detectTakeOff(); // hasn't been implemented yet
    bool missionFinished() const;
    State getState() const;
    
    void feedGPS();

    void activateCAM() const;
    void listenFromCam() const;

    //Telemetry related methods
    CollectiveSensorData GatherSensorData();
    void sendDataToGC(const CollectiveSensorData&); 
    void sendDataToGC(const std::string&);
    void handleTelemetry(const CollectiveSensorData&);
    void logToSD(const CollectiveSensorData&);
    void logToSD(const std::string&);
};
