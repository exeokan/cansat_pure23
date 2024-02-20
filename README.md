# Introduction

This repository contains the software for the CanSat Training project of PURE'23, Sabancı University. The goal was to drop a payload (satellite) from 100 meters with a drone and have the satellite send telemetry packets containing mission status and sensor data. 

The microprocessor used in the satellite module is an ESP-32, and the software is developed in Arduino IDE. The satellite module also has a BMP180 pressure sensor, MPU 6050 accelerometer, HMC5883L magnetometer, ESP32-CAM module, SDCard module, GY-NEO6MV2 GPS module, and a buzzer.

The ground control interface is written in Python and offers basic controls of the mission. Because the selected communication protocol is ESP-NOW (as it is advantageous in long ranges), a secondary ESP-32 module is used as a bridge between the computer and the satellite. 

The release mechanism controls include an additional ESP-32 and a servo motor. It receives a command from the ground control to drop the payload.

### Supervisors
Emrah Kalemci, Ayhan Bozkurt
### Students contributed
Can Gelisen, Mete Kerem Berk, Ege Okan Unaldi, Hogler Chepchirchir Tuwei, Ahmet Alperen Oznam, Taneera Tajammal

# Software

## Satellite module

The main functionalities of the satellite module are to gather sensor data, send telemetry packets to the ground station and save them to the SD card, process commands from the ground station, and take photos of the mission. Software for it can be found under the main folder. Telemetry data and photos taken during
flight can be found under the test_data folder.

Telemetry data is of format 

```c++
MISSION_ID, MISSION_TIME, PACKET_COUNT, MODE, STATE,
ALTITUDE, PC_DEPLOYED, TEMPERATURE, PRESSURE, GPS_TIME,
GPS_ALTITUDE, GPS_LATITUDE, GPS_LONGITUDE,
GPS_SATS, ACC_X, ACC_Y, ACC_Z, MAG_X, MAG_Y, MAG_Z, CMD_ECHO
```

- MISSION_ID: Represents the ID of the mission.
- MISSION_TIME: Represents the time of the mission.
- PACKET_COUNT: Represents the count of telemetry packets.
- MODE: Represents the current mode of operation. (always set to FLIGHT)
- STATE: Represents the current state of the mission. (STANDBY/ASCENT/DESCENT/LANDED)
- ALTITUDE: Represents the altitude of the satellite (from bmp180).
- PC_DEPLOYED: Represents whether the payload has been deployed or not.
- TEMPERATURE: Represents the temperature reading(from bmp180).
- PRESSURE: Represents the pressure reading (from bmp180).
- GPS_TIME: Represents the time obtained from GPS.
- GPS_ALTITUDE: Represents the altitude obtained from the GPS module.
- GPS_LATITUDE: Represents the latitude obtained from the GPS module.
- GPS_LONGITUDE: Represents the longitude obtained from the GPS module.
- GPS_SATS: Represents the number of GPS satellites in view.
- ACC_X, ACC_Y, ACC_Z: Represent the accelerometer readings in the X, Y, and Z axes respectively (from MPU6050).
- MAG_X, MAG_Y, MAG_Z: Represent the magnetometer readings in the X, Y, and Z axes respectively (from HMC5883L).
- CMD_ECHO: Represents the echo of the command received from the ground station.

### ESP-32 CAM board

The software for ESP-32 CAM that is on the satellite module is under the cam_module folder. The board expects `B{MISSION_ID}` from the main ESP-32 of the satellite module. After that, it captures pictures and saves them to its internal SD card until it is turned off.

Parameters for the camera configuration were adjusted with experimentation.

### Notes

Adafruit BMP085 library also supports BMP180 modules and it is used in the program.

Because the HMC5883L module we had has an alternative chip, the QMC5883LCompass library was used.

Because ESP-32 doesn't support the SoftwareSerial library of Arduino, the GPS module is connected via TX2/RX2 pins on ESP-32 and the TinyGPSPlus library is used with the Serial2 object of ESP-32 software.

Uploading software to ESP32-CAM was a bit tricky, for our module, we had to connect the IO0 pin to the ground next to it when we turned it on. That way it goes into download mode and you can upload the code from the Arduino IDE. (with ESP32-CAM AI-Thinker board selected)

## Release module

The program that runs on the ESP-32 board in the release module expects a command from the ground station to move a servo to the release position and drop the payload.

## Ground control

An additional ESP-32 that is connected to the computer is used to connect to release module ESP-32 and main ESP-32 on the payload. ESP-NOW protocol is utilized as it offers long-range communication.

The ground control software that runs on the computer is a Python program with a simple GUI. It has the following controls:

- Sending any command message to ESP-32 (only STA/DSC/FIN has an effect on the satellite, which are coded into buttons)
- Starting the mission (sets the mission state to ASCENT)
- Activating the camera on the satellite
- Releasing the payload (sends a command to release module ESP-32 and main ESP-32, setting the mission state to DESCENT)
- Setting the  mission state to DESCENT (in case the board fails to capture the first time)
- Finishing the mission (sets the mission state to LANDED, finishes telemetry, and activates the buzzer on the satellite)

### Notes

Unlike the serial monitor in the Arduino IDE, the ground station program doesn't send any end-of-line characters to ESP-32.

In the ground control software, the serial port that the board is connected to is hard-coded.

# Limitations and improvements for future

Due to time limitations, some aspects of the project are not implemented or need further improvement.

- Communication problems

In our early tests, we were able to have stable communication with the ESP-32 up to 100 meters when it was loaded into the drone. However, in the final test, the aluminum case of the satellite module disrupted the communication and we received very little telemetry. A better design of the case, the use of an external antenna, or an alternative communication method (such as using an RC transmitter/receiver) may be the solution.

- Camera problem

The case allowed very little room for the camera to take pictures, therefore most of the pictures are unclear. Also, the calibration of the camera parameters was adjusted according to experiments indoors, thus brightness was too much in some of the pictures. Better placement of the camera and a case redesign might solve these issues.

- Auto ascent/descent/landing detection

Due to the insufficient number of tests, we couldn't be sure if we could detect ascent/descent and landing. In our final tests, we learned that pressure measurements from bmp180 give consistent altitude calculations, and the time derivative of altitude can be used to detect ascent/descent. 

Landing can also be detected using measurements from the mpu6050 module. The payload very rarely lands orthogonal to the surface, so gravitational acceleration is distributed to sensors x,y, and z axes.

- Plotting

One objective of the project was to plot the numerical values of the telemetry packets such as altitude, pressure, acceleration, and so on.

- Constants file

For better modularity, every constant in the software could be gathered together in one file.

