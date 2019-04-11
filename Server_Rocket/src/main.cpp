#include <Arduino.h>
#include <SPI.h>
#include <MyRadio.h>




bool print_main = false; // set true for debugging

dataPoint data_telemetry;
gpsData data_gps;
basic time_code;

RH_RF69 rf69(RFM69_CS, RFM69_INT); // instantiate radio driver
RHReliableDatagram manager(rf69, SERVER_ADDRESS); //manages delivery and recipt


void setup() {

  //Setup LED's
  REG_PORT_DIR0 |= LED_R;  // Set port to output, "PORT->Group[0].DIRSET.reg = PORT_PA17;" also works
  REG_PORT_OUTCLR0= LED_R; // Set port low

  REG_PORT_DIR0 |= LED_G;  // Set port to output, "PORT->Group[0].DIRSET.reg = PORT_PA06;" also works
  REG_PORT_OUTCLR0= LED_G; // Set port low

  //  Group[0] is port A
  //  Group[1] is port B

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
        if((time_code.code & (1 << 1))){ // if GPS has fix
          if((time_code.code & (1 << 2))){ // if sustainer
            //Turn on an LED
            REG_PORT_OUTSET0= LED_R; 
          }
        }
        else{ // if Booster 
          //Turn on different LED
          REG_PORT_OUTSET0= LED_G; 
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