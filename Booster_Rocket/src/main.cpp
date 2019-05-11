#include <Arduino.h>
#include <MySensors.h>
#include <SPI.h>
#include <MyRadio.h>
#include <MySD.h>
#include <MyTimer.h>

//#include <samd21g18a.h>

//Mosfets gates, set high to Ignite.
int terminal_left = 19;
int terminal_middle = 18;
int terminal_right = 17;

bool main_print = true; // true print to USB Serial //for debugging

RH_RF69 rf69(RFM69_CS, RFM69_INT); // instantiate radio driver
RHReliableDatagram manager(rf69, BOOSTER_ADDRESS); //manages delivery and recipt

File logfile; //file descriptor
char * filename; //name of file ECELOG--.txt , about 15bytes
int flushtime = 0;

#define GPSSerial Serial1
Adafruit_GPS GPS(&GPSSerial);

// STRUCTS //
dataPoint data_telemetry; // acc,gyro,prs,tmpr
gpsData data_gps; // GPS
basic time_code; // time (ms), code 

byte tx_buf[60];
int size_tx;

// A timer handler that is called at the frequency declared in startTimer(int frequency)
void TC3_Handler() {
  TcCount16* TC = (TcCount16*) TC3;
  // If this interrupt is due to the compare register matching the timer count
  // we Read the GPS.
  if (TC->INTFLAG.bit.MC0 == 1) {
    TC->INTFLAG.bit.MC0 = 1;
    // Write callback here!!!
    GPS.read(); 
  }
}

void setup() {

  /////////////////////
  // Setup GPIO PINS //
  /////////////////////

  //  Group[0] is port A
  //  Group[1] is port B

  //Setup GPS Pins
  REG_PORT_DIR0 |= GPS_Enable_Pin; // Set port to output
  REG_PORT_OUTCLR0 |= GPS_Enable_Pin; // Set port low

  //if changing gps refresh rate
  REG_PORT_DIR1 |= GPS_Reset_Pin; // Set port to output
  REG_PORT_OUTSET1 |= GPS_Reset_Pin; // Set port High
  
  // Radio enable
  REG_PORT_DIR0 |= RFM69_ENABLE; // Set port to output
  REG_PORT_OUTSET0 |= RFM69_ENABLE; // Set port low

  //Setup LED's
  REG_PORT_DIR0 |= LED_R;  // Set port to output, "PORT->Group[0].DIRSET.reg = PORT_PA17;" also works
  REG_PORT_OUTCLR0 |= LED_R; // Set port low

  REG_PORT_DIR0 |= LED_G;  // Set port to output, "PORT->Group[0].DIRSET.reg = PORT_PA06;" also works
  REG_PORT_OUTCLR0 |= LED_G; // Set port low


  // Set up serial monitor
  Serial.begin(115200); //serial USB

  if(main_print){ // if main_print is manually set 
    while (!Serial);
    Serial.println("Starting AV board test");
    set_sensor_print(true);
  }
  else{
    delay(5000); 
    if(Serial){ // if usb-serial is plugged in within 5 seconds
      main_print = true; // enable serial print for debugging
      set_sensor_print(true);
      Serial.println("Starting AV board test");
    }
  }

  //////////////////////
  // Initialize Radio //
  //////////////////////
  setup_radio(&rf69,&manager,BOOSTER_ADDRESS);

  ////////////////////////
  // Initialize SD card //
  ////////////////////////
  filename = setup_sd(& logfile);

  ////////////////////////
  // Initialize Sensors //
  ////////////////////////
  setup_accelerometer();
  setup_gyroscope();
  setup_barometer();
  setup_gps(&GPS);
  
  if(main_print){
    Serial.println("Radio AV Board test");
    Serial.println();
  }

  startTimer(1000); 
  // calls TC3_Handler() every millisecond
  // Make sure to dissable the interupt during time critical applications
  // Such as transmitting
}


void loop() {

  time_code.time = millis(); // get time in milliseconds
  if(main_print){
    Serial.print("Time: ");
    Serial.println(time_code.time);
    
  }

  /////////////////////////
  //   Gather GPS Data   //
  /////////////////////////
  
  if (gather_gps(&GPS, &data_gps,&time_code)){ // if return 1 send gps data
  time_code.code |= 1 << 0;
  size_tx = sizeof(time_code) + sizeof(data_gps);
  memcpy(tx_buf, &time_code, sizeof(time_code));
  memcpy(&tx_buf[sizeof(time_code)], &data_gps, sizeof(data_gps) );

  delay(10);
  /// Log Time and Gps Data //
  if(main_print){
    Serial.print("starting GPS log\n");
    }
  log_basic(&logfile, filename,&time_code,&flushtime);
  log_gpsData(&logfile, filename,&data_gps,&flushtime);
  if(main_print){
    Serial.print("Finish GPS log\n");
    }
  }

  else{
    ///////////////////////////
    //   Gather Sensor Data  //
    ///////////////////////////

    // Need multiple GPS.read() functions to complete a GPS datagram. 
    // Will move to interupt driven method.
    // GPS.read() is recommended to be called every 1ms

    gather_accelerometer(&data_telemetry);
    gather_gyroscope(&data_telemetry);
    gather_barometer(&data_telemetry);

    time_code.code &= ~(1 << 0);
    size_tx = sizeof(time_code) + sizeof(data_telemetry);
    memcpy(tx_buf, &time_code, sizeof(time_code));
    memcpy(&tx_buf[sizeof(time_code)], &data_telemetry, sizeof(data_telemetry) );

    /// Log Time and Sensor Data //
    log_basic(&logfile, filename,&time_code,&flushtime);
    log_dataPoint(&logfile, filename,&data_telemetry,&flushtime);
  }

  //////////////////////////
  // Send Data over Radio //
  //////////////////////////
  
  if(main_print){
   Serial.print("Start Data Transmission\n");
  }
  delay(5); // Need delay to complete GPS read before transmitting data
  //Dissable timer interupts
  manager.sendto((uint8_t *)&tx_buf, size_tx, SERVER_ADDRESS);
  //Enable timer interupts
  if(main_print){
   Serial.print("END Data Transmission\n");
  }

}



