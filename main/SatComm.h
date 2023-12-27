#pragma once
#include <esp_now.h>
#include <WiFi.h>

struct CollectiveSensorData
{
    std::string TEAM_ID, MISSION_TIME, PACKET_COUNT, MODE, STATE,
    ALTITUDE, PC_DEPLOYED, TEMPERATURE, VOLTAGE, PRESSURE, GPS_TIME,
    GPS_ALTITUDE, GPS_LATITUDE, GPS_LONGITUDE,
    GPS_SATS, ACC_X, ACC_Y, ACC_Z, MAG_X, MAG_Y, MAG_Z, TILT_X, TILT_Y, CMD_ECHO;
};
struct Command{
    std::string command;
};

class SatComm
{
private:
public:
    SatComm(/* args */);
    void sendData(const CollectiveSensorData&);
    void sendData(const Command&);
    static void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len);
    static void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status);
};
