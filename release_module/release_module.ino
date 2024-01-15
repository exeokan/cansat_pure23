#include <ESP32Servo.h>
#include <esp_now.h>
#include <WiFi.h>

#define SERVO_PIN 26 // ESP32 pin GPIO26 connected to servo motor

typedef struct struct_message {
    bool pressed;
} struct_message;

struct_message myData;
Servo servoMotor;

const int startPos = 90, servoDelay = 2000, releasePos = 0;

void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  memcpy(&myData, incomingData, sizeof(myData));
  if (myData.pressed) {
    servoMotor.attach(SERVO_PIN);
    servoMotor.write(releasePos);
    delay(servoDelay);
    servoMotor.detach();
  }
  delay(100);
}

void setup() {
  // Initialize Serial Monitor
  Serial.begin(115200);
  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);
  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  servoMotor.attach(SERVO_PIN);
  servoMotor.write(startPos);
  delay(servoDelay);
  servoMotor.detach();
  // Once ESPNow is successfully Init, we will register for recv CB to
  // get recv packer info
  esp_now_register_recv_cb(OnDataRecv);
}

void loop() {
  // do nothing
}
