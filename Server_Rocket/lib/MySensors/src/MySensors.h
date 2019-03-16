#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_LIS3DH.h> // For Accelerometer
#include <Adafruit_L3GD20_U.h> // For Gyroscope
#include <MS5xxx.h> //For pressure and temperature
#include <MyCommon.h> //For dataPoint struct and error_blink
#include <Adafruit_GPS.h>


void setup_accelerometer(void);
void setup_gyroscope(void); 
void setup_barometer(void);
void setup_gps(void); 

void gather_accelerometer(struct dataPoint *telemetry);
void gather_gyroscope(struct dataPoint *telemetry); 
void gather_barometer(struct dataPoint *telemetry);
void gather_gps(struct dataPoint *telemetry);