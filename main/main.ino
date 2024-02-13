#include "Satellite.h"

#include "bootloader_random.h"
#include "esp_random.h"

const int buzzerPin = 4;
long lastFeed = millis();
long lastTelemetry = millis();
const int feedingRate = 400; // in feed/ms
const int telemetryRate = 900; // in packet/ms, aim for 1Hz, set to 900ms to account for processing time
Sattalite* cansat;

void setup(){
    //initialize serial communication for debugging
    Serial.begin(115200);
    //get random seed
    bootloader_random_enable();
    int random = esp_random() % 10000;
    bootloader_random_disable();
    //initialize satellite object
    cansat = new Sattalite(std::to_string(random));
    //buzzer setup
    pinMode(buzzerPin, OUTPUT);
}

void loop(){   
    //feed GPS data
    if(millis()-lastFeed > feedingRate){
        cansat->feedGPS();
        lastFeed=millis();
    }
    //gather sensor data and send it to the ground control
    if(millis()-lastTelemetry > telemetryRate){
        CollectiveSensorData sensorData = cansat->GatherSensorData();
        cansat->handleTelemetry(sensorData);
        Serial.println("----------------------------"); //to seperate packet logs(sdcard and espnow)
        lastTelemetry=millis();
    }
    // if mission is finished, activate the buzzer and delete the satellite object
    if(cansat->missionFinished()){
        Serial.println("Mission Finished");
        delete cansat;
        while(true){
            //activate the buzzer to ease the recovery of the satellite
            digitalWrite(buzzerPin, HIGH);
            delay(100);
            digitalWrite(buzzerPin, LOW);
            delay(100);
        }
    }
}
