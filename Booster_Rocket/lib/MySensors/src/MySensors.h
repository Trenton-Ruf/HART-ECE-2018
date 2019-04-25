#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_LIS3DH.h> // For Accelerometer
#include <Adafruit_L3GD20_U.h> // For Gyroscope
#include <MS5xxx.h> //For pressure and temperature
#include <MyCommon.h> //For dataPoint struct and error_blink
#include <Adafruit_GPS.h>


#ifndef MYSENSORS_H_
#define MYSENSORS_H_
//int GPS_Enable_Pin = A0; // pll low to enable, 
//int GPS_Reset_Pin = A1; // Reset if pulled low. Keep high otherwise.
#endif

void setup_accelerometer(void);
void setup_gyroscope(void); 
void setup_barometer(void);
void setup_gps(Adafruit_GPS* GPS); 

void set_sensor_print(bool set_bool);

void gather_accelerometer(struct dataPoint *telemetry);
void gather_gyroscope(struct dataPoint *telemetry); 
void gather_barometer(struct dataPoint *telemetry);
int gather_gps(Adafruit_GPS *GPS, struct gpsData *gpsdata,struct basic *time_code);