#include <MyRadio.h>

bool radio_print = true; // true to print to serial

int RADIO_ADDRESS;

//ADDRESS 1 if booster, 2 if sustainer, 3 if ground station
void  setup_radio(RH_RF69 *rf69,  RHReliableDatagram *manager ,int ADDRESS){
  RADIO_ADDRESS = ADDRESS;

  pinMode(RFM69_RST, OUTPUT);
  digitalWrite(RFM69_RST, LOW);
  // change to register edits

  // rfm69hcw Radio module manual reset
  digitalWrite(RFM69_RST, HIGH);
  delay(10);
  digitalWrite(RFM69_RST, LOW);
  delay(10);

  if (!manager->init()){
    if(radio_print){
        Serial.println("radio init failed");
    }
    //error_blink(1);
  }

  if (!rf69->setFrequency(RF69_FREQ)) {
    if(radio_print){
        Serial.println("setFrequency failed");
    }
    //error_blink(2);
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

//MAKE FUNCTIONS WITH VOID POINTER PARAMETERS SO CAN DO BOTH GPS AND SENSORS

void sendTelemetry(RH_RF69 *rf69,  RHReliableDatagram *manager, struct dataPoint *telemetry){

  byte tx_buf[sizeof(*telemetry)] = {0};

  memcpy(tx_buf, telemetry, sizeof(*telemetry) );
  /*
  if (! manager->sendtoWait((uint8_t *)tx_buf, sizeof(*telemetry), SERVER_ADDRESS)) {
    if(radio_print){
      Serial.println("Sending failed (no ack)");
    }
  }  
  */
  manager->sendto((uint8_t *)tx_buf, sizeof(*telemetry), SERVER_ADDRESS);

  // change to not wait for ACK
}


void recieveTelemetry(RH_RF69 *rf69,  RHReliableDatagram *manager, struct dataPoint *telemetry) {

/*
  uint8_t buf[RH_RF69_MAX_MESSAGE_LEN];
  uint8_t len = sizeof(buf);
  uint8_t from;

  if (manager->recvfromAck(buf, &len, &from)){
    Serial.println((char *)buf);
  }
*/
  
  byte rx_buf[sizeof(*telemetry)] = {0};

  if (manager->available()){
    // Wait for a message addressed to us from the client
    uint8_t len = sizeof(rx_buf);
    uint8_t from;
    if (manager->recvfromAck(rx_buf, &len, &from)) {
      memcpy(telemetry, rx_buf, sizeof(*telemetry));
      //Serial.write((char *)telemetry, sizeof(*telemetry));

      Serial.print("temperature : ");
      Serial.println(telemetry->tmp);     
    }
  }



}

