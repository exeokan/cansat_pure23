#include "Sattalite.h"
Sattalite* cansat;
void setup(){
    Serial.begin(115200);
    Serial.println("Program begins...");
    cansat = new Sattalite(std::to_string(rand()%1000)); //problematic? memory leak?
    cansat->SDCardTest();
    cansat->BMP180Test();
    cansat->MPUTest();
    cansat->QMCTest();
    //cansat->establishGC_Communincation();
}

//TODO:
// error/status lights
// calculating tilt
// test esp cam
//feed gps in loop
long lastTime = millis();
const int feedingRate = 100; // in feed/ms

void loop(){
    //cansat->calculateTilt();
    cansat->BMP180Test();
    /*
    if(millis()-lastTime > feedingRate){
        cansat->calculateTilt();
        //cansat->feedGPS() //Gather sensorda yapmayı iptal et
    }
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
