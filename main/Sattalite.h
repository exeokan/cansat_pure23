#pragma once

#include "SDCard.h"
#include <Arduino.h>
#include <Wire.h>
#include <BMP180I2C.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <TinyGPSPlus.h>
#include <SoftwareSerial.h>

const int GPS_RX_Pin = 4,  GPS_TX_Pin = 3;
const uint32_t GPSBaud = 4800;
const byte BMP180_12C_Address= 0x77;

class Sattalite
{
private:
    SDCard sdCard;
    BMP180I2C bmp180;//default i2c address of bmp180
    Adafruit_MPU6050 mpu;
    TinyGPSPlus gps;
    SoftwareSerial ss;// The serial connection to the GPS device
public:
    Sattalite(/* args */);
    ~Sattalite();
    void SDCardTest();
    void BMP180Test();
};
