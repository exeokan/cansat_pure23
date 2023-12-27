#include "SatComm.h"

/* data */
esp_now_peer_info_t peerInfo;  
// Create a struct_message called BME280Readings to hold sensor readings
CollectiveSensorData sensorData;
// Create a struct_message to hold incoming sensor readings
Command incomingCommand;
// REPLACE WITH THE MAC Address of your receiver 
//E0:5A:1B:A0:C8:E8

uint8_t broadcastAddress[] = {0xE0, 0x5A, 0x1B, 0xA0, 0xC8, 0xE8};

SatComm::SatComm(/* args */)
{
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
    // get the status of Trasnmitted packet
    esp_now_register_send_cb(OnDataSent);

    // Register peer
    memcpy(peerInfo.peer_addr, broadcastAddress, 6);
    peerInfo.channel = 0;  
    peerInfo.encrypt = false;

    // Add peer        
    if (esp_now_add_peer(&peerInfo) != ESP_OK){
        Serial.println("Failed to add peer");
        return;
    }
    // Register for a callback function that will be called when data is received
    esp_now_register_recv_cb(OnDataRecv);
}

// Callback when data is sent
void SatComm::OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

// Callback when data is received
void SatComm::OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  memcpy(&incomingCommand, incomingData, sizeof(incomingCommand));
  Serial.print("Bytes received: ");
  Serial.println(len);
  for(int i=0; i< incomingCommand.command.length(); i++){
    Serial.println((int)(incomingCommand.command[i]));
  }
}

void SatComm::sendData(const CollectiveSensorData& dataPacket)
{
    // Send message via ESP-NOW
    Serial.print("size:");
    Serial.println(sizeof(dataPacket));
    esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &dataPacket, sizeof(dataPacket));
}
void SatComm::sendData(const Command& dataPacket)
{
    // Send message via ESP-NOW
    Serial.print("size:");
    Serial.println(sizeof(dataPacket));
    esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &dataPacket, sizeof(dataPacket));
}
