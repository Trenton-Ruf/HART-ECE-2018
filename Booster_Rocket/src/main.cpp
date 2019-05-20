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

bool main_print = false; // true print to USB Serial //for debugging
int ADDRESS = BOOSTER_ADDRESS; //change to SUSTAINER_ADDRESS if in sustainer

RH_RF69 rf69(RFM69_CS, RFM69_INT); // instantiate radio driver
RHReliableDatagram manager(rf69, ADDRESS); //manages delivery and recipt

File logfile; //file descriptor
char * filename; //name of file ECELOG--.txt , about 12bytes
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

/////////////////////////
// Setup State Machine //
/////////////////////////
typedef enum {
  UNARMED,
  ARMED,
  LAUNCHED
} launch_state;
launch_state current_state = UNARMED;

void setup() {

  /////////////////////
  // Setup GPIO PINS //
  /////////////////////

  //  Group[0] is port A
  //  Group[1] is port B

  //LEAVE THESE PORTS COMMENTED OUT FOR NOW, IT WORKS WITHOUT SETTING THEM.

  /*
  //Setup GPS Pins
  REG_PORT_DIR0 |= GPS_Enable_Pin; // Set port to output
  REG_PORT_OUTCLR0 |= GPS_Enable_Pin; // Set port low

  //if changing gps refresh rate
  REG_PORT_DIR1 |= GPS_Reset_Pin; // Set port to output
  REG_PORT_OUTSET1 |= GPS_Reset_Pin; // Set port High
  */

  //Radio enable
  //REG_PORT_DIR0 |= RFM69_ENABLE; // Set port to output
  //REG_PORT_OUTCLR0 |= RFM69_ENABLE; // Set port low //wait... maybe high lmao

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

  //Sets address in time_code.code bit 2 to be 0 for booster 1 for sustainer
  if (ADDRESS == BOOSTER_ADDRESS){
    time_code.code &= ~(1 << 2);
  }
  else{
    time_code.code |= 1 << 2; 
  }

  //startTimer(1000); 
  // calls TC3_Handler() every millisecond
  // Make sure to dissable the interupt during time critical applications
  // Such as transmitting
  
  gps_disable(); // Set gps to low power before arming
}


void arming(){
  // SLEEP //
  if (! rf69.sleep()){ // Put Radio to sleep
    if(main_print)
      Serial.println("Failed to put Radio to sleep");
  }

  // WAIT FOR 1 MINUTE //change to microcontroller sleep instead of delay
  delay(60000);

  // WAKE //
  //Check for arming signal
  if (recieve_arming(&manager)){
    current_state = ARMED;
    // Set all sensors to high power 
    gps_enable();
  }
}


void send_telemetry() {

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

  delay(1);
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

  delay(1); // Need delay to complete GPS read before transmitting data
  //NVIC_DisableIRQ(TC3_IRQn); //Dissable timer interupts
  manager.sendto((uint8_t *)&tx_buf, size_tx, SERVER_ADDRESS);
  //NVIC_EnableIRQ(TC3_IRQn); //Enable timer interupts
  if(main_print){
   Serial.print("END Data Transmission\n");
  }

}

void loop(){
  if (current_state == UNARMED){
    arming();
  }
  else if(current_state == ARMED){
    if(main_print)
    Serial.println("ARMED");
  }
}

