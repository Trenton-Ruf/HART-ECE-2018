#include <Arduino.h>

#ifndef COMMON_H_
#define COMMON_H_

struct gpsData{
  float latitude;
  float longitude;
  float speed;
  float angle;
  float altitude;
  char misc;

  ///////////////////
  // code for misc //
  ///////////////////

  // bit 0 -> lat -> 0 if North, 1 if South
  // bit 1 -> lon -> 0 if East, 1 if West

};

struct xyzData{
  float x;
  float y;
  float z;
};

struct dataPoint { 
  struct xyzData acc;   // m/s^2  //might be able to get higher resolution by raw data instead
  struct xyzData gyro;  // degrees/s
  float prs;            // Pa
  float tmp;            // C
};

struct basic{
  unsigned int time;    // ms
  unsigned int code;
  ///////////////////
  // code for code //
  ///////////////////

  // bit 0 -> dataPoint -> 1 Data
  // bit 1 -> 0 -> no gps fix
  // bit 2 -> Address -> 0 if booster, 1 if sustainer
  // bit 3 -> event 0 -> 1 if launch happened
  // bit 4 -> event 1 -> 1 if separation/sustainer happened
  // bit 5 -> event 2 -> 1 if drouge shoot happened
  // bit 6 -> event 3 -> 1 if main shoot happened
  // bit 7 -> 1 if sending battery info // maybe implement

  // INT IS 16 BITS YOU CAN ADD MORE CODES

  // bool mybit = ((code >> 3)  & 0x01) //stores bit 3 of "code" in mybit
};

#define LED_R         PORT_PA17 //Red   LED Pin 13
#define LED_G         PORT_PA06 //Green LED Pin 8
#define EXTERNAL_LED PORT_PA15

void error_blink(int pattern, int LED);

#endif





