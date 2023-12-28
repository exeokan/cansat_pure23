#pragma once
#include <esp_now.h>
#include <WiFi.h>
#include <functional>

struct Packet {
  char str[200];  // Adjust the size based on your needs
};

class SatComm
{
private:
    std::string lastCommand;
    std::function<void(std::string)> commandCallbackFunc; 
public:
    SatComm(std::function<void(std::string)> _commandCallbackFunc);
    void sendDataToGC(const Packet&);
    static void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len);
    static void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status);
};
