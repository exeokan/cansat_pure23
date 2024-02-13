/**
 * @file gc_module.ino
 * @brief This file contains the code for the bridge between ground control software and the satellite.
 * It recieves telemetry data from the satellite over ESP-NOW and writes it to the serial port.
 * It recieves commands from the ground control software via serial and sends commands to the satellite over ESP-NOW.
 * @note Serial prints are to be handled by the ground control software.
*/
#include <esp_now.h>
#include <WiFi.h>

//Packet structure for satellite communication
struct Packet {
  char str[200];
};
//Packet structure for release module communication
struct Release_message {
  bool pressed;
};
Packet packetBuffer;
uint8_t broadcastAddress[] = {0xA0, 0xB7, 0x65, 0xDD, 0x96, 0xCC};
uint8_t broadcastAddress_Release[] = {0xA0, 0xB7, 0x65, 0xDB, 0xD2, 0x34};

esp_now_peer_info_t peerInfo;//of ground control
esp_now_peer_info_t peerInfo_Release;
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

void OnDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len) {
  memcpy(&packetBuffer, incomingData, sizeof(packetBuffer));
  Serial.print("Bytes received: ");
  Serial.println(len);
  Serial.println(packetBuffer.str);
}

void setup() {
  Serial.begin(115200);
  // Print the MAC address of the ESP board
  Serial.print("ESP Board MAC Address:  ");
  Serial.println(WiFi.macAddress());
  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  // set up the callback functions to handle received and sent data
  esp_now_register_send_cb(OnDataSent);
  esp_now_register_recv_cb(OnDataRecv);

  // Register peer GC
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;
  // Add peer GC
  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add peer");
    return;
  }

  // Register peer Release
  memcpy(peerInfo_Release.peer_addr, broadcastAddress_Release, 6);
  peerInfo_Release.channel = 0;
  peerInfo_Release.encrypt = false;
  // Add peer Release
  if (esp_now_add_peer(&peerInfo_Release) != ESP_OK) {
    Serial.println("Failed to add peer");
    return;
  }
}

std::string serialBuffer="";
void loop() {
  while (Serial.available() > 0) {//when there is data on the serial port
    // Read the incoming byte
    char incomingByte = Serial.read();
    serialBuffer += incomingByte;
    delay(10);
  }
  if(serialBuffer!=""){
    // If release button is pressed, send a message to the release module, and descent command to the satellite
    if(serialBuffer=="REL"){
      Release_message release_message;
      release_message.pressed = true;
      esp_err_t result = esp_now_send(broadcastAddress_Release, (uint8_t *)&release_message, sizeof(release_message));
    
      Packet packet;
      strcpy(packet.str, "DSC");
      esp_now_send(broadcastAddress, (uint8_t *)&packet, sizeof(packet));
      serialBuffer="";
    }
    // If the command is not "REL", it is sent only to the satellite
    else{
      Packet packet;
      strcpy(packet.str, serialBuffer.c_str());
      esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *)&packet, sizeof(packet));
      serialBuffer="";
    }
  }
  delay(100);
}

  
