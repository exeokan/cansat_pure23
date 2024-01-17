#pragma once
#include <esp_now.h>
#include <WiFi.h>
#include <functional>

struct Packet {
  char str[200];
};
class SatComm
{
private:
    
public:
    std::string lastCommand = "";
    std::function<void(std::string)> commandCallbackFunc; 
    SatComm(std::function<void(std::string)> _commandCallbackFunc);
    void sendDataToGC(const Packet&);
};
