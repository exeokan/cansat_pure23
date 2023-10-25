#pragma once
#include "SDCard.h"
#include <Arduino.h>
#include <Wire.h>
#include <BMP180I2C.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>


static const int RXPin = 4, TXPin = 3;
static const uint32_t GPSBaud = 4800;


class Sattalite
{
private:
    SDCard sdCard;
    BMP180I2C bmp180(0x77);//default i2c address of bmp180
    Adafruit_MPU6050 mpu;
    TinyGPSPlus gps;
    SoftwareSerial ss(RXPin, TXPin);// The serial connection to the GPS device
public:
    Sattalite(/* args */);
    ~Sattalite();
    void SDCardTest();
    void BMP180Test();
};


