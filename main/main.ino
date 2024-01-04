#include "Sattalite.h"

#include "bootloader_random.h"
#include "esp_random.h"

Sattalite* cansat;
void setup(){
    Serial.begin(115200);
    Serial.println("Program begins...");
    bootloader_random_enable();
    int random=esp_random() % 10000;
    bootloader_random_disable();
    cansat = new Sattalite(std::to_string(random)); //problematic? memory leak?
    /*
    buzzer setup
    pinMode(4, OUTPUT);*/
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
    cansat->listenFromCam();
    if(millis()-lastFeed > feedingRate){
        cansat->feedGPS();
        lastFeed=millis();
        //bunu bmp için de yap? yeni kütüpe bağlı
    }
    if(millis()-lastDisplay > displayRate){
        CollectiveSensorData sensorData = cansat->GatherSensorData();
        cansat->handleTelemetry(sensorData);
        Serial.println("----------------------------");
        lastDisplay=millis();
    }
    /*

    delay(100);
    State currentState= cansat->getState();
    if(currentState==State::standby){
    cansat->detectTakeOff();
    }
    else if(currentState==State::ascent){
    //detect dropoff
    //if detected activate cam
    }
    else if (currentState==descent)
    {
        //detect landing
    }
    else {
        //buzzer code
    }*/
}
