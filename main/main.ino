#include "Sattalite.h"
Sattalite* cansat;
void setup(){
    Serial.begin(115200);
    cansat = new Sattalite(); //problematic? memory leak?
    cansat->SDCardTest();
    cansat->BMP180Test();
    cansat->MPUTest();
    cansat->GPSTest();
    pinMode(16, OUTPUT);
}

void loop(){
    Serial.println("in loop");
    //cansat->HMCTest();
    digitalWrite(16, 1);
    delay(1000);
    digitalWrite(16, 0);
    delay(1000);
    if(cansat->missionFinished()){
        delete cansat;
        //buzzer code
    }
   
}
