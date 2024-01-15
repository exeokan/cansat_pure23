#include "Sattalite.h"

#include "bootloader_random.h"
#include "esp_random.h"

const int buzzerPin = 4;

Sattalite* cansat;
void setup(){
    Serial.begin(115200);
    //get random seed
    bootloader_random_enable();
    int random=esp_random() % 10000;
    bootloader_random_disable();
    cansat = new Sattalite(std::to_string(random));
    //buzzer setup
    pinMode(4, OUTPUT);
}
//TODO:
//1. add buzzer
//2. test compass
//3. test release esp comm
//4. plot data
//5. add constants.h
long lastFeed = millis();
long lastDisplay = millis();
const int feedingRate = 400; // in feed/ms
const int displayRate = 900; // in feed/ms
void loop(){   
    if(millis()-lastFeed > feedingRate){
        cansat->feedGPS();
        lastFeed=millis();
    }
    if(millis()-lastDisplay > displayRate){
        CollectiveSensorData sensorData = cansat->GatherSensorData();
        cansat->handleTelemetry(sensorData);
        Serial.println("----------------------------");
        lastDisplay=millis();
    }
    if(cansat->missionFinished()){
        Serial.println("Mission Finished");
        delete cansat;
        while(true){
            digitalWrite(buzzerPin, HIGH);
            delay(100);
            digitalWrite(buzzerPin, LOW);
            delay(100);
        }
    }
    delay(100);
}
