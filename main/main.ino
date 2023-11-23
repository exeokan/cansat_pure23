#include "Sattalite.h"
Sattalite* cansat;
void setup(){
    Serial.begin(115200);
    cansat = new Sattalite("1"); //problematic? memory leak?
    cansat->SDCardTest();
    cansat->BMP180Test();
    cansat->MPUTest();
    cansat->GPSTest();
    cansat->QMCTest();

    //cansat->establishGC_Communincation();
}
//TODO:
// error/status lights
// calculating tilt
// 
void loop(){
    /*
    while(!cansat->detectTakeOff())
        delay(10)
    while(!cansat->detectDrop()){
        cansat->activateCAM()
        delay(10)   
    }
    auto lastTime = millis() - 1000;
    while(!cansat->isLanded()){
        if(lastTime-millis()>1000){
            cansat->logToSD(cansat->GatherSensorData());
        }
    }
    if(cansat->missionFinished()){
        delete cansat;
        //buzzer code
    }
   */
}
