#include "Sattalite.h"
Sattalite* cansat;
void setup(){
    Serial.begin(115200);
    cansat = new Sattalite();
    cansat->SDCardTest();
}

void loop(){
    Serial.println("donuyor");
    delay(1000);
}
