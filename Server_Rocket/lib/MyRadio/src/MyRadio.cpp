#include <MyRadio.h>

bool radio_print = false; // true to print to serial

int RADIO_ADDRESS;

//ADDRESS 1 if booster, 2 if sustainer, 3 if ground station
void  setup_radio(RH_RF69 *rf69,  RHReliableDatagram *manager ,int ADDRESS){
  RADIO_ADDRESS = ADDRESS;

  // Reset radio
  REG_PORT_DIR1 |= RFM69_RST;  // Set port to output, "PORT->Group[0].DIRSET.reg = PORT_PA17;" also works
  REG_PORT_OUTCLR1 |= RFM69_RST; // Set port low
  delay(10);
  REG_PORT_OUTSET1 |= RFM69_RST; // Set port high
  delay(10);
  REG_PORT_OUTCLR1 |= RFM69_RST; // Set port low


  if (!manager->init()){
    if(radio_print){
        Serial.println("radio init failed");
    }
    error_blink(4,EXTERNAL_LED);
  }

  rf69->setModemConfig(RH_RF69::FSK_Rb2Fd5); // Allows for non-defualt configurations. I'm using it to decrease baud rate. Possible pre-sets found in link below.
  // http://www.airspayce.com/mikem/arduino/RadioHead/classRH__RF69.html#a8b7db5c6e4eb542f46fec351b2084bbe

  if (!rf69->setFrequency(RF69_FREQ)) {
    if(radio_print){
        Serial.println("setFrequency failed");
    }
    error_blink(5,EXTERNAL_LED);
  }

  rf69->setTxPower(20, true); //14 is lowest power, 20 is highest

  uint8_t key[] = { 0x02, 0x02, 0x05, 0x04, 0x09, 0x07, 0x01, 0x04,
                    0x08, 0x02, 0x02, 0x09, 0x01, 0x05, 0x06, 0x08};
  rf69->setEncryptionKey(key);

  if(radio_print){
    Serial.println("encryption key set");
    Serial.println();
  }
}

// returns 1 if ADDRESS is armed
// returns 0 if ADDRESS failed to arm
// returns -1 if Sending failed (ADDRESS is unavaliable)
int send_arming(RHReliableDatagram *manager, int ADDRESS) {

  uint8_t buf[RH_RF69_MAX_MESSAGE_LEN];
  uint8_t len = sizeof(buf);
  uint8_t from;
  char data[15];
  strcpy(data,"arming");
  bool armed = false;

 if (!manager->sendtoWait((uint8_t *)data, strlen(data), ADDRESS)){
    Serial.println("sendtoWait failed");
    return -1;
  }

  if (manager->recvfromAck(buf, &len, &from)){ // recieve from ground station will wait forever instead consider recvFromAckTimout
    if (radio_print){
      Serial.print("got request from : 0x");
      Serial.print(from, HEX);
      Serial.print(": ");
      Serial.println((char*)buf);
    }
  }

  if(strstr((char*)buf, "success")){ // if the recieved message is "launch"
    armed = true;
  }

  return armed;
}


