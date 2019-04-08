#include <Arduino.h>
#include <SPI.h>
#include <MyRadio.h>



//#define LAUNCH_BUTTON 6

bool print_main = false; // set true for debugging

dataPoint data_telemetry;
gpsData data_gps;
basic time_code;

RH_RF69 rf69(RFM69_CS, RFM69_INT); // instantiate radio driver
RHReliableDatagram manager(rf69, SERVER_ADDRESS); //manages delivery and recipt


void setup() {
  // put your setup code here, to run once:

  //pinMode(LAUNCH_BUTTON, INPUT);
  pinMode(LED_R, OUTPUT);
  digitalWrite(LED_R, LOW);
  pinMode(LED_G, OUTPUT);
  digitalWrite(LED_G, LOW);

  // Set up serial monitor (comment out if not using USB or it will stall here)
  Serial.begin(115200);
  while (!Serial);

  setup_radio(&rf69,&manager,SERVER_ADDRESS);

  if(print_main){
    Serial.println();
    Serial.println("Radio Ground Station test");
    Serial.println();
  }

}

void loop() {

 //recieveTelemetry(&rf69,&manager, &data_telemetry); //revieves and sends over usb serial

  byte rx_buf[60] = {0};

  if (manager.available()){
    // Wait for a message addressed to us from the client
    uint8_t len = sizeof(rx_buf);
    uint8_t from;
    //recvfromack
    if (manager.recvfrom(rx_buf, &len, &from)) {
      memcpy(&time_code, rx_buf, sizeof(time_code));// dont need this step if only sending over serial, if no onboard computation
      if(print_main){
        Serial.print("time: ");
        Serial.println(time_code.time);
        Serial.print("code: ");
        Serial.println(time_code.code);
      }
      if(!(time_code.code & (1 << 0))){ // if reading dataPoint
        memcpy(&data_telemetry, &rx_buf[sizeof(time_code)], sizeof(data_telemetry)); // don't need if only sending on serial
       if(print_main){
          Serial.print("temperature : ");
          Serial.println(data_telemetry.tmp);     
        }
      }
      else{ // if reading gpsData
        memcpy(&data_gps, &rx_buf[sizeof(time_code)], sizeof(data_gps)); // don't need if only sending on serial
      }

      Serial.write(rx_buf,60); //write to Serial without parsing

    }
  }
}




//altitude = 44330 * (1.0 - pow((pressure / 100) / 1013.25, 0.1903));
//https://www.scadacore.com/tools/rf-path/rf-line-of-sight/
//cntl+alt+p  select default shell