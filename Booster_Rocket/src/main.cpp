#include <Arduino.h>
#include <MySensors.h>
#include <SPI.h>
#include <MyRadio.h>
#include <MySD.h>

//#include <samd21g18a.h>

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

  //Setup LED's
  REG_PORT_DIR0 |= LED_R;  // Set port to output, "PORT->Group[0].DIRSET.reg = PORT_PA17;" also works
  REG_PORT_OUTCLR0= LED_R; // Set port low

  REG_PORT_DIR0 |= LED_G;  // Set port to output, "PORT->Group[0].DIRSET.reg = PORT_PA06;" also works
  REG_PORT_OUTCLR0= LED_G; // Set port low

  //  Group[0] is port A
  //  Group[1] is port B

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

  time_code.time = millis(); // get time in milliseconds
  if(main_print){
    Serial.print("Time: ");
    Serial.println(time_code.time);
  }

  /////////////////////////
  //   Gather GPS Data   //
  /////////////////////////
  if (gather_gps(&GPS, &data_gps,&time_code)){ // if return 1 send gps data
  size_tx = sizeof(time_code) + sizeof(data_gps);
  memcpy(tx_buf, &time_code, sizeof(time_code));
  memcpy(&tx_buf[sizeof(time_code)], &data_gps, sizeof(data_gps) );
  time_code.code |= 1 << 0;

  /// Log Time and Gps Data //
  log_basic(&logfile, filename,&time_code,&flushtime);
  log_gpsData(&logfile, filename,&data_gps,&flushtime);

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
    memcpy(&tx_buf[sizeof(time_code)], &data_telemetry, sizeof(data_telemetry) );
    time_code.code &= ~(1 << 0);

    /// Log Time and Sensor Data //
    log_basic(&logfile, filename,&time_code,&flushtime);
    log_gpsData(&logfile, filename,&data_gps,&flushtime);
  }

  //////////////////////////
  // Send Data over Radio //
  //////////////////////////
  
  manager.sendto((uint8_t *)&tx_buf, size_tx, SERVER_ADDRESS);

}



