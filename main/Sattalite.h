#pragma once
#include "SDCard.h"
class Sattalite
{
private:
    SDCard sdCard;
public:
    Sattalite(/* args */);
    ~Sattalite();
    void SDCardTest();
};

Sattalite::Sattalite(/* args */)
{
    if(!Serial){
        Serial.begin(115200); // DANGEROUS CODE!!!!!!!!!!11
        //this Serial is for USB??
        //https://www.arduino.cc/reference/en/language/functions/communication/serial/
    }
}

Sattalite::~Sattalite()
{
}

void Sattalite::SDCardTest(){
    sdCard.testRun();
}