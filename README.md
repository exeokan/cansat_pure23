# Introduction

This repository contains the software for the CanSat Training project of PURE'23, Sabancı University. The goal was to drop a payload (sattelite) from 100 meters with a drone, and have the satelite send telemetry packets containing mission status and sensor data. 

The microprocessor used in the satellite module is an ESP-32, and the software is developed in Arduino IDE. The satellite module also has a BMP180 pressure sensor, MPU 6050 accelerometer, HMC5883L magnetometer, ESP32-CAM module, SDCard module, GY-NEO6MV2 GPS module.

The ground control interface is written in python, and offers basic controls of the mission. Because communication between satellite is established using ESP-NOW protocol(as it is advantegous in long ranges), a secondary ESP-32 module is used as a bridge between the computer and the satellite. 

The release mechanism controls include an ESP-32 and a servo motor. It recieves a command from the ground control to drop the payload.

# Software

## Satellite module



# Limitations and improvements for future

Communication problem
Camera problem
Auto launch/landing detection

