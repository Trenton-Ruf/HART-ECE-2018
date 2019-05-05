#include <Arduino.h>
#include <MySensors.h>
#include <SPI.h>
#include <MyRadio.h>
#include <MySD.h>

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

void setup() {

  /////////////////////
  // Setup GPIO PINS //
  /////////////////////

  pinMode(A0,OUTPUT);
  digitalWrite(A0, LOW);
  pinMode(A1,OUTPUT);
  digitalWrite(A1, HIGH);

  //Setup GPS Pins
  //REG_PORT_DIR0 |= GPS_Enable_Pin; // Set port to output
  //REG_PORT_OUTCLR0 |= GPS_Enable_Pin; // Set port low

  //Setting GPS reset high or low will cause it to become non-responsive. need to solve
  //if changing gps refresh rate
  //REG_PORT_DIR1 |= GPS_Reset_Pin; // Set port to output
  //REG_PORT_OUTSET1 |= GPS_Reset_Pin; // Set port High
  

  pinMode(10,OUTPUT); // Radio enable
  digitalWrite(10,HIGH);

  //Setup LED's
  REG_PORT_DIR0 |= LED_R;  // Set port to output, "PORT->Group[0].DIRSET.reg = PORT_PA17;" also works
  REG_PORT_OUTCLR0= LED_R; // Set port low

  REG_PORT_DIR0 |= LED_G;  // Set port to output, "PORT->Group[0].DIRSET.reg = PORT_PA06;" also works
  REG_PORT_OUTCLR0= LED_G; // Set port low

  //  Group[0] is port A
  //  Group[1] is port B

  // Set up serial monitor (comment out while loop if not using USB or it will stall here)
  Serial.begin(115200); //serial USB
/*
  if(main_print){ // if main_print is manually set 
    while (!Serial);
    Serial.println("Starting AV board test");
    set_sensor_print(true);//Does this work lmao?
  }
  else{
    delay(9000); 
    if(Serial){ // if usb-serial is plugged in within 5 seconds
      main_print = true; // enable serial print for debugging
      set_sensor_print(true);
      Serial.println("Starting AV board test");
    }
  }
*/

  
set_sensor_print(true);

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
/*
int i = 0;
  while(i < 1000){
    gather_gps(&GPS, &data_gps,&time_code);
    i++;
  }
  */
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
    GPS.read();
    gather_accelerometer(&data_telemetry);
    GPS.read();
    gather_gyroscope(&data_telemetry);
    GPS.read();
    gather_barometer(&data_telemetry);

    GPS.read();

    time_code.code &= ~(1 << 0);
    size_tx = sizeof(time_code) + sizeof(data_telemetry);
    memcpy(tx_buf, &time_code, sizeof(time_code));
    memcpy(&tx_buf[sizeof(time_code)], &data_telemetry, sizeof(data_telemetry) );

    GPS.read();
    /// Log Time and Sensor Data //
    log_basic(&logfile, filename,&time_code,&flushtime);
    log_dataPoint(&logfile, filename,&data_telemetry,&flushtime);

    GPS.read();
  }



  //////////////////////////
  // Send Data over Radio //
  //////////////////////////
  
  if(main_print){
   Serial.print("Start Data Transmission\n");
  }
  delay(5); 
  manager.sendto((uint8_t *)&tx_buf, size_tx, SERVER_ADDRESS);

  if(main_print){
   Serial.print("END Data Transmission\n");
  }

}



