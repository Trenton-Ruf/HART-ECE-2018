#include <Arduino.h>
#include <MySensors.h>
#include <SPI.h>
#include <MyRadio.h>
#include <MySD.h>

//#include <samd21g18a.h>

///////////////
// TODO list //
///////////////

/*
  Fix temperature
  change gps hz and baud 
  Change SD code to my own.
  Change Radio protocol to be more like UDP instead of TCP.
  Maanually change registers using SAMD21 stuff.
  Change encryption key to be non-default
  change recieve_launch(); to be more power efficient (implementing radio sleep)
*/

bool main_print = false; // true print to USB Serial

bool launched = false; 

unsigned int current_time;
unsigned int start_time;

RH_RF69 rf69(RFM69_CS, RFM69_INT); // instantiate radio driver
RHReliableDatagram manager(rf69, BOOSTER_ADDRESS); //manages delivery and recipt

File logfile; //file descriptor
char * filename; //name of file ECELOG--.txt , about 15bytes
int flushtime = 0;


#define GPSSerial Serial1
Adafruit_GPS GPS(&GPSSerial);

dataPoint data_telemetry;
gpsData data_gps;
basic time_code;

byte tx_buf[60];
int size_tx;

void setup() {


  //Set Led's to outputs (change to direct registry manipulations later)
  pinMode(LED_R, OUTPUT);
  digitalWrite(LED_R, LOW);
  pinMode(LED_G, OUTPUT);
  digitalWrite(LED_G, LOW);

  //PORT->Group[0].DIR.reg |= PORT_PA17; 

  // Set up serial monitor (comment out while loop if not using USB or it will stall here)
  Serial.begin(115200); //serial USB

  delay(5000);
  if(Serial){
    main_print = true;
  }

  if(main_print){
    while (!Serial);
    Serial.println("Starting AV board test");
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
}


void loop() {
  // mini state machine



  ///////////////////////////
  // Gather Telemetry Data //
  ///////////////////////////

  time_code.time = millis();
  if(main_print){
    Serial.print("Time: ");
    Serial.println(time_code.time);
  }

   if (gather_gps(&GPS, &data_gps,&time_code)){ // if return 1 send gps data
    /////////////////////////
    //   Gather GPS Data   //
    /////////////////////////

    size_tx = sizeof(time_code) + sizeof(data_gps);
    memcpy(tx_buf, &time_code, sizeof(time_code));
    memcpy(tx_buf + 4, &data_gps, sizeof(data_gps) );
  }

  else{
    ///////////////////////////
    //   Gather Sensor Data  //
    ///////////////////////////

    gather_accelerometer(&data_telemetry);
    gather_gyroscope(&data_telemetry);
    gather_barometer(&data_telemetry);

    size_tx = sizeof(time_code) + sizeof(data_telemetry);
    memcpy(tx_buf, &time_code, sizeof(time_code));
    memcpy(tx_buf + 4, &data_gps, sizeof(data_gps) );
  }

  ///////////////
  // Send Data //
  ///////////////
  
  manager.sendto((uint8_t *)&tx_buf, size_tx, SERVER_ADDRESS);


  /////////////////////
  //   Store Data    //
  /////////////////////

 // log_data(&logfile, filename,&data_telemetry,&flushtime); // Store to sd card

  /////////////////////////
  // Send Telemetry Data //
  /////////////////////////

  //sendTelemetry(&rf69,&manager,&data_telemetry);
}



