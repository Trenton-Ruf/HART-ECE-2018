#include <Arduino.h>

#ifndef COMMON_H_
#define COMMON_H_

struct gpsData{
  float latitude;
  float longitude;
  float speed;
  float angle;
  float altitude;
};

struct xyzData{
  float x;
  float y;
  float z;
};

struct dataPoint { 
  struct gpsData gps;
  struct xyzData acc;   // m/s^2  //might be able to get higher resolution by raw data instead
  struct xyzData gyro;  // rad/s
  float prs;            // Pa
  float tmp;            // C
  unsigned int time;    // ms


  unsigned int code;

  ///////////////////
  // code for code //
  ///////////////////

  // bit 0 -> gps fix -> 1 if fixed
  // bit 1 -> Address -> 0 if booster, 1 if sustainer
  // bit 2 -> event 0 -> 1 if launch happened
  // bit 3 -> event 1 -> 1 if separation/sustainer happened
  // bit 4 -> event 2 -> 1 if drouge shoot happened
  // bit 5 -> event 3 -> 1 if main shoot happened

  // might use these if neccisary 
  //char  lon; 
  //char  lat;

  // bool mybit = ((code >> 3)  & 0x01) //stores bit 3 of "code" in mybit
};
// struct dataPoint is 60 bytes! The max a radio packet can be! PERFECT!

#define LED_R         13 //Red LED
#define LED_G         8 //Green LED

void error_blink(int pattern, int LED);

#endif





