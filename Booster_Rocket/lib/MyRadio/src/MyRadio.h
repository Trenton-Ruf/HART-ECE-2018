#include <Arduino.h>
#include <RHReliableDatagram.h>
#include <RH_RF69.h>
#include <MyCommon.h>

#ifndef MYRADIO_H_
#define MYRADIO_H_

#define RF69_FREQ 433 // can be between 400-460 MHz according to adafruit

#define RFM69_CS      5 // 5   . can be any digital pin (output)
#define RFM69_INT     6 // 6   . can be any interupt capable pin (input) "All pins can be interrupt inputs"
#define RFM69_RST     PORT_PA07 // 9   . can be any digital pin (output)
#define RFM69_ENABLE  PORT_PA18 //  . can be any digital pin (output)

#define BOOSTER_ADDRESS 1 
#define SUSTAINER_ADDRESS 2
#define SERVER_ADDRESS 3 // Ground station

#endif

void  setup_radio(RH_RF69 *rf69,  RHReliableDatagram *manager ,int ADDRESS);
void sendTelemetry(RH_RF69 *rf69,  RHReliableDatagram *manager ,struct dataPoint *telemetry);
void recieveTelemetry(RH_RF69 *rf69,  RHReliableDatagram *manager, struct dataPoint *telemetry);
int recieve_arming(RHReliableDatagram *manager); 