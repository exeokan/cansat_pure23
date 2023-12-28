// SatComm.cpp
#include "SatComm.h"

//E0:5A:1B:A0:C8:E8
uint8_t broadcastAddress[] = {0xE0, 0x5A, 0x1B, 0xA0, 0xC8, 0xE8};
esp_now_peer_info_t peerInfo;
Packet packetBuffer;
SatComm::SatComm() {
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

  // Register peer
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  // Add peer
  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add peer");
    return;
  }
  // Register for a callback function that will be called when data is received
  esp_now_register_recv_cb(OnDataRecv);
}

void SatComm::OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\rLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

void SatComm::OnDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len) {
  memcpy(&packetBuffer, incomingData, sizeof(packetBuffer));
  Serial.print("Bytes received: ");
  Serial.println(len);
  // print content
  Serial.print("String: ");
  Serial.println(packetBuffer.str);
}

void SatComm::sendData(const Packet& dataPacket) {
  // Send message via ESP-NOW
  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *)&dataPacket, sizeof(dataPacket));
}
