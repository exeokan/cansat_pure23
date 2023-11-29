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
};
class SattaliteErrLights
{
private:
    /* data */
    std::unordered_map<Modules, bool> moduleStates;
public:
    SattaliteErrLights(/* args */);
    void RaiseError(const Modules&) ;
    void LiftError(const Modules&) ;
    bool isOkay(const Modules&);
};

