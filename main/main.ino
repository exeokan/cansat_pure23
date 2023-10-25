#include "Sattalite.h"
Sattalite* cansat;
void setup(){
    Serial.begin(115200);
    cansat = new Sattalite();
    cansat->SDCardTest();
    cansat->BMP180Test();
    cansat->MPUTest();
}

void loop(){
    Serial.println("in loop");
    delay(1000);
}
