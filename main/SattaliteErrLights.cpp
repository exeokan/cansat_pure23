#include "SattaliteErrLights.h"
void SattaliteErrLights::RaiseError(const Modules &module) {
  if(!moduleStates[module]){
    digitalWrite(module, HIGH);
    moduleStates[module]=true;
  }
}

void SattaliteErrLights::LiftError(const Modules &module) {
    if(moduleStates[module]){
      digitalWrite(module, LOW);
      moduleStates[module]=false;
    }
}
bool SattaliteErrLights::isOkay(const Modules &module) {
    return moduleStates[module];
}
SattaliteErrLights::SattaliteErrLights(/* args */)
{
  /*
    pinMode(bmp, OUTPUT);
    pinMode(mpu, OUTPUT);
    pinMode(qmc, OUTPUT);
    pinMode(gps, OUTPUT);
    pinMode(cam, OUTPUT);*/
}
