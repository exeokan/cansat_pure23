#pragma once
#include <Arduino.h>
#include <unordered_map> 
enum Modules{//->pin numbers
    bmp=0,
    mpu=1,
    qmc=2,
    gps=3,
    sd=4,
    cam=5
}
class SattaliteErrLights
{
private:
    /* data */
    std::unordered_map<Modules, bool> moduleStates;
public:
    SattaliteErrLights(/* args */);
    void RaiseError(Modules) ;
    void LiftError(Modules) ;
    bool isOkay(Modules) const;
};

void SattaliteErrLights::RaiseError(const Modules &module) {
    digitalWrite(module, HIGH);
    unordered_map[module]=true;
}

void SattaliteErrLights::LiftError(const Modules &module) {
    digitalWrite(module, LOW);
    unordered_map[module]=false;
}
bool SattaliteErrLights::isOkay(const Modules &module) {
    return moduleStates[module];
}
SattaliteErrLights::SattaliteErrLights(/* args */)
{
    pinMode(bmp, OUTPUT);
    pinMode(mpu, OUTPUT);
    pinMode(qmc, OUTPUT);
    pinMode(gps, OUTPUT);
    pinMode(cam, OUTPUT);
}
