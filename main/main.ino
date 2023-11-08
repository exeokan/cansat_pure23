#include "Sattalite.h"
Sattalite* cansat;
void setup(){
    Serial.begin(115200);
    cansat = new Sattalite(); //problematic? memory leak?
    cansat->SDCardTest();
    cansat->BMP180Test();
    cansat->MPUTest();
    cansat->GPSTest();
}

void loop(){
    //cansat->HMCTest();
    if(cansat->missionFinished()){
        delete cansat;
        //buzzer code
    }
   
}
