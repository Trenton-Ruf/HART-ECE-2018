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

#define GPS_Enable_Pin  PORT_PA02 // A0 // 14 // pull low to enable, 
#define GPS_Reset_Pin   PORT_PB08 // A1 // 15 // Reset if pulled low. Keep high otherwise.
#define ACC_Interup_Pin 16        // A2 // PORT_PB09 

#define PMTK_SET_NMEA_FIX_1HZ  "$PMTK300,1000,0,0,0,0*1C"
#define PMTK_SET_NMEA_FIX_5HZ  "$PMTK300,200,0,0,0,0*2F"
#define PMTK_SET_NMEA_FIX_10HZ "$PMTK300,100,0,0,0,0*2"

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
void gps_enable();
void gps_disable();