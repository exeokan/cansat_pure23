/**
 * @file release_module.ino
 * @brief This file contains the code for a release module using ESP32 and servo motor.
 * 
 * The release module is controlled wirelessly using ESP-NOW protocol. When a specific message is received,
 * indicating that the release button is pressed, the servo motor releases the module by moving to a specific position.
 * 
 * The code initializes ESP-NOW, sets up Wi-Fi as a station, and registers a callback function to handle received data.
 * It also sets up the servo motor and moves it to the start position. The loop function does nothing.
 */
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
  if (myData.pressed) {// If Release button is pressed, release the module
    servoMotor.attach(SERVO_PIN);
    servoMotor.write(releasePos);
    delay(servoDelay);
    servoMotor.detach();
  }
  delay(100);
}

void setup() {
  Serial.begin(115200);
  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);
  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  // Once ESPNow is successfully Init, we will register for recv CB to get recv packer info
  esp_now_register_recv_cb(OnDataRecv);

  //Set servo motor to start position
  servoMotor.attach(SERVO_PIN);
  servoMotor.write(startPos);
  delay(servoDelay);
  servoMotor.detach();
}

void loop() {
  // do nothing
}
