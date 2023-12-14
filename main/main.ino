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
        cansat->SDCardTest();
        cansat->BMP180Test();
        cansat->MPUTest();
        cansat->QMCTest();
    */
    cansat->activateCAM();
    //cansat->establishGC_Communincation();
}

//TODO:
// error/status lights
// calculating tilt
// test esp cam
// feed gps in loop[done]
// python code for interpretation and graphing

long lastFeed = millis();
long lastDisplay = millis();
const int feedingRate = 100; // in feed/ms
const int displayRate = 900; // in feed/ms
void loop(){
    //cansat->calculateTilt();
    cansat->listenFromCam();
    /*
    if(millis()-lastFeed > feedingRate){
        cansat->feedGPS(); //Gather sensorda yapmayı iptal et
        lastFeed=millis();
    }
    if(millis()-lastDisplay > displayRate){
        cansat->logToSD(cansat->GatherSensorData());
        Serial.println("----------------------------");
        lastDisplay=millis();
    }*/
    /*
    //bunu bmp için de yap? yeni kütüpe bağlı
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
