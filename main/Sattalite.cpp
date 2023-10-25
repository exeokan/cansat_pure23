#include "Sattalite.h"
Sattalite::Sattalite(/* args */)
{
    if(!Serial){
        Serial.begin(115200); // DANGEROUS CODE!!!!!!!!!!11
        //this Serial is for USB??
        //https://www.arduino.cc/reference/en/language/functions/communication/serial/
        //Serial faillerse noluyor
    }

    /*BMP180 init*/
    Wire.begin();
    if (!bmp180.begin())
	{
		Serial.println("begin() failed. check your BMP180 Interface and I2C Address.");
	}
	bmp180.resetToDefaults();
	//enable ultra high resolution mode for pressure measurements
	bmp180.setSamplingMode(BMP180MI::MODE_UHR);

    /*MPU6050 init*/
    if (!mpu.begin()) {
        Serial.println("Failed to find MPU6050 chip");
    }
    Serial.println("MPU6050 Found!");
    mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
    mpu.setGyroRange(MPU6050_RANGE_500_DEG);
    mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);

    /*GPS Module*/
    if(!ss.begin(GPSBaud)){
        Serial.println("Failed to find GPS chip");
    }
}

Sattalite::~Sattalite()
{
}

void Sattalite::SDCardTest(){
    sdCard.testRun();
}

void Sattalite::BMP180Test()
{

}