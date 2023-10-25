#include "Sattalite.h"
Sattalite::Sattalite(/* args */): bmp180(BMP180_12C_Address), ss(GPS_RX_Pin, GPS_TX_Pin)
{
    Serial.println("Sat init...");
    if(!Serial){
        Serial.begin(115200); // DANGEROUS CODE!!!!!!!!!!11
        //this Serial is for USB??
        //https://www.arduino.cc/reference/en/language/functions/communication/serial/
        //Serial faillerse noluyor
    }

    /*BMP180 init*/
    Serial.println("bmp start");
    Wire.begin();
    Serial.println("Wire done");
    if (!bmp180.begin())
	{
		Serial.println("begin() failed. check your BMP180 Interface and I2C Address.");
	}
	bmp180.resetToDefaults();
	//enable ultra high resolution mode for pressure measurements
	bmp180.setSamplingMode(BMP180MI::MODE_UHR);
    Serial.println("mpu starts");
    /*MPU6050 init*/
    if (!mpu.begin()) {
        Serial.println("Failed to find MPU6050 chip");
    }
    Serial.println("MPU6050 Found!");
    mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
    mpu.setGyroRange(MPU6050_RANGE_500_DEG);
    mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);
}

Sattalite::~Sattalite()
{
}

void Sattalite::SDCardTest(){
    sdCard.testRun();
}

void Sattalite::BMP180Test()
{
    if (!bmp180.measureTemperature())
	{
		Serial.println("could not start temperature measurement, is a measurement already running?");
		return;
	}

	//wait for the measurement to finish. proceed as soon as hasValue() returned true. 
	do
	{
		delay(100);
	} while (!bmp180.hasValue());

	Serial.print("Temperature: "); 
	Serial.print(bmp180.getTemperature()); 
	Serial.println(" degC");

	//start a pressure measurement. pressure measurements depend on temperature measurement, you should only start a pressure 
	//measurement immediately after a temperature measurement. 
	if (!bmp180.measurePressure())
	{
		Serial.println("could not start perssure measurement, is a measurement already running?");
		return;
	}

	//wait for the measurement to finish. proceed as soon as hasValue() returned true. 
	do
	{
		delay(100);
	} while (!bmp180.hasValue());

	Serial.print("Pressure: "); 
	Serial.print(bmp180.getPressure());
	Serial.println(" Pa");
}

void Sattalite::MPUTest()
{
     sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  /* Print out the values */
  Serial.print("Acceleration X: ");
  Serial.print(a.acceleration.x);
  Serial.print(", Y: ");
  Serial.print(a.acceleration.y);
  Serial.print(", Z: ");
  Serial.print(a.acceleration.z);
  Serial.println(" m/s^2");

  Serial.print("Rotation X: ");
  Serial.print(g.gyro.x);
  Serial.print(", Y: ");
  Serial.print(g.gyro.y);
  Serial.print(", Z: ");
  Serial.print(g.gyro.z);
  Serial.println(" rad/s");

  Serial.print("Temperature: ");
  Serial.print(temp.temperature);
  Serial.println(" degC");
}
