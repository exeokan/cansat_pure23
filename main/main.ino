#include "Sattalite.h"
Sattalite* cansat;
void setup(){
    Serial.begin(115200);
    cansat = new Sattalite(std::to_string(rand()%1000)); //problematic? memory leak?
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
// implement bmp180 using the adafruit bmp085 library

void loop(){
  cansat->calculateTilt();
  Serial.println("in loop");
  delay(100);
  /*if(in_drop){

  }
  else if(in_ascent){

  }
  else if(){

  }
  */
    /*
    if(lastTime-millis()>1000){
        cansat->logToSD(cansat->GatherSensorData());
    }
    while(!cansat->detectTakeOff())
        delay(10)
    while(!cansat->detectDrop()){
        cansat->activateCAM()
        delay(10)   
    }
    auto lastTime = millis() - 1000;
    while(!cansat->isLanded()){         
    }
    if(cansat->missionFinished()){
        delete cansat;
        //buzzer code
    }
   */
}
