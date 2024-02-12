/**
 * @file SatComm.h
 * @brief This file contains the declaration of the SatComm class and related structures and enums.
*/
#pragma once
#include <esp_now.h>
#include <WiFi.h>
#include <functional>

struct Packet {
  char str[200];
};

/**
 * @class SatComm
 * @brief This class is responsible for handling the communication between the satellite and the ground control.
*/
class SatComm
{
private:
    
public:
    std::string lastCommand = "";
    std::function<void(std::string)> commandCallbackFunc; 
    SatComm(std::function<void(std::string)> _commandCallbackFunc);
    void sendDataToGC(const Packet&);
};
