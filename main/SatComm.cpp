// SatComm.cpp
#include "SatComm.h"


uint8_t broadcastAddress_GC[] = {0xE0, 0x5A, 0x1B, 0xA0, 0xC8, 0xE8};

esp_now_peer_info_t peerInfo_GC;

SatComm* globalSatCommInstance = nullptr;

Packet packetBuffer;

void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\rLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

void OnDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len) {
  memcpy(&packetBuffer, incomingData, sizeof(packetBuffer));
  globalSatCommInstance->lastCommand = packetBuffer.str;
  globalSatCommInstance->commandCallbackFunc(packetBuffer.str);
}
SatComm::SatComm(std::function<void(std::string)> _commandCallbackFunc): commandCallbackFunc(_commandCallbackFunc) {
  // Set the global pointer in the constructor
  globalSatCommInstance = this;
  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);
  Serial.print("ESP Board MAC Address:  ");
  Serial.println(WiFi.macAddress());
  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  // Once ESPNow is successfully Init, we will register for Send CB to
  // get the status of Transmitted packet
  esp_now_register_send_cb(OnDataSent);

  // Register peer GC
  memcpy(peerInfo_GC.peer_addr, broadcastAddress_GC, 6);
  peerInfo_GC.channel = 0;
  peerInfo_GC.encrypt = false;

  // Add peer GC
  if (esp_now_add_peer(&peerInfo_GC) != ESP_OK) {
    Serial.println("Failed to add peer");
    return;
  }
  // Register for a callback function that will be called when data is received
  esp_now_register_recv_cb(OnDataRecv);
}

void SatComm::sendDataToGC(const Packet& dataPacket) {
  // Send message via ESP-NOW
  esp_err_t result = esp_now_send(broadcastAddress_GC, (uint8_t *)&dataPacket, sizeof(dataPacket));
}
