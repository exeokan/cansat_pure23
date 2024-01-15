#pragma once
#include <esp_now.h>
#include <WiFi.h>
#include <functional>

struct Packet {
  char str[200];  // Adjust the size based on your needs
};
struct Release_message {
    bool pressed;
};
class SatComm
{
private:
    
public:
    std::string lastCommand = "";
    std::function<void(std::string)> commandCallbackFunc; 
    SatComm(std::function<void(std::string)> _commandCallbackFunc);
    void sendDataToGC(const Packet&);
    void sendDataToRelease(const Release_message&);
};
