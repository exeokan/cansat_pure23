#include "Sattalite.h"
Sattalite cansat;
void setup(){
    Serial.begin(115200);
    cansat.SDCardTest();
}

void loop(){
}
