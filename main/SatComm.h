#pragma once
#include <esp_now.h>
#include <WiFi.h>


struct Packet {
  char str[200];  // Adjust the size based on your needs
};

class SatComm
{
private:

public:
    SatComm(/* args */);
    void sendData(const Packet&);
    static void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len);
    static void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status);
};
