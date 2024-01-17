#include <esp_now.h>
#include <WiFi.h>

struct Packet {
  char str[200];  // Adjust the size based on your needs
};
struct Release_message {
  bool pressed;
};
Packet packetBuffer;
uint8_t broadcastAddress[] = {0xA0, 0xB7, 0x65, 0xDD, 0x96, 0xCC};
uint8_t broadcastAddress_Release[] = {0xA0, 0xB7, 0x65, 0xDB, 0xD2, 0x34};

esp_now_peer_info_t peerInfo;
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
  Serial.print("ESP Board MAC Address:  ");
  Serial.println(WiFi.macAddress());

  WiFi.mode(WIFI_STA);

  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  esp_now_register_send_cb(OnDataSent);
  // gc
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

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
  esp_now_register_recv_cb(OnDataRecv);
  Serial.println("ESP32 Serial Communication Example");
}
std::string serialBuffer="";
void loop() {
  while (Serial.available() > 0) {
    // Read the incoming byte
    char incomingByte = Serial.read();
    serialBuffer+= incomingByte;
    delay(10);
  }
  if(serialBuffer!=""){
    if(serialBuffer=="REL"){
      Release_message release_message;
      release_message.pressed = true;
      esp_err_t result = esp_now_send(broadcastAddress_Release, (uint8_t *)&release_message, sizeof(release_message));
    
      Packet packet;
      strcpy(packet.str, "DSC");
      esp_now_send(broadcastAddress, (uint8_t *)&packet, sizeof(packet));
      serialBuffer="";
    }
    else{
      Packet packet;
      strcpy(packet.str, serialBuffer.c_str());
      esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *)&packet, sizeof(packet));
      serialBuffer="";
    }
  }
  delay(100);
}

  
