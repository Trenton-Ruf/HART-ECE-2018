#include <Arduino.h>
#include <RHReliableDatagram.h>
#include <RH_RF69.h>
#include <MyCommon.h>


#define RF69_FREQ 433 // can be between 400-460 MHz according to adafruit

#define RFM69_CS      15 //  A1 . can be any digital pin (output)
#define RFM69_INT     17 //  A2 . can be any interupt capable pin (input) "All pins can be interrupt inputs"
#define RFM69_RST     16 //  A3 . can be any digital pin (output)

#define BOOSTER_ADDRESS 1 // 1 if booster, 2 if sustainer
#define SUSTAINER_ADDRESS 2
#define SERVER_ADDRESS 3 // Ground station


void  setup_radio(RH_RF69 *rf69,  RHReliableDatagram *manager ,int ADDRESS);

void sendTelemetry(RH_RF69 *rf69,  RHReliableDatagram *manager ,struct dataPoint *telemetry);
void recieveTelemetry(RH_RF69 *rf69,  RHReliableDatagram *manager, struct dataPoint *telemetry);

//void recieve_launch();// waits for the launch command