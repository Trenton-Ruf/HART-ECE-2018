#include "MySensors.h"

bool sensor_print = false; // set true to print to Serial

///////////////////
// Accelerometer //
///////////////////

// Accelerometer I2C
Adafruit_LIS3DH lis = Adafruit_LIS3DH();

void setup_accelerometer(void) {

  if(sensor_print)
    Serial.println("LIS3DH test!");
  
  if (! lis.begin(0x18)) {   // change this to 0x19 for alternative i2c address
    if(sensor_print)
      Serial.println("Couldnt start");
    //ERROR
  }

  if(sensor_print){
    Serial.println("LIS3DH found!");
    
    lis.setRange(LIS3DH_RANGE_16_G);   // 2, 4, 8 or 16 G!
    
    Serial.print("Range = "); Serial.print(2 << lis.getRange());  
    Serial.println("G");
  }
}

void gather_accelerometer(struct dataPoint *telemetry) {

  lis.read();      // get X Y and Z data at once
  // Then print out the raw data
  /*
  Serial.print("X:  "); Serial.print(lis.x); 
  Serial.print("  \tY:  "); Serial.print(lis.y); 
  Serial.print("  \tZ:  "); Serial.print(lis.z); 
  */
  // Or....get a new sensor event, normalized 

  sensors_event_t event; 
  lis.getEvent(&event);
  
  // Display the results (acceleration is measured in m/s^2) 
  if(sensor_print){
    Serial.print("\t\tX: "); Serial.print(event.acceleration.x);
    Serial.print(" \tY: "); Serial.print(event.acceleration.y); 
    Serial.print(" \tZ: "); Serial.print(event.acceleration.z); 
    Serial.println(" m/s^2 ");
    Serial.println();
    delay(200);
  }

  telemetry->acc.x = event.acceleration.x;
  telemetry->acc.y = event.acceleration.y;
  telemetry->acc.z = event.acceleration.z;
}

///////////////
// Gyroscope //
///////////////

/* Assign a unique ID to this sensor at the same time */
Adafruit_L3GD20_Unified gyro = Adafruit_L3GD20_Unified(20);

void displaySensorDetails(void)
{
  sensor_t sensor;
  gyro.getSensor(&sensor);
  Serial.println("------------------------------------");
  Serial.print  ("Sensor:       "); Serial.println(sensor.name);
  Serial.print  ("Driver Ver:   "); Serial.println(sensor.version);
  Serial.print  ("Unique ID:    "); Serial.println(sensor.sensor_id);
  Serial.print  ("Max Value:    "); Serial.print(sensor.max_value); Serial.println(" rad/s");
  Serial.print  ("Min Value:    "); Serial.print(sensor.min_value); Serial.println(" rad/s");
  Serial.print  ("Resolution:   "); Serial.print(sensor.resolution); Serial.println(" rad/s");  
  Serial.println("------------------------------------");
  Serial.println("");
  delay(500);
}

void setup_gyroscope(void) 
{

  if(sensor_print)
  Serial.println("Gyroscope Test"); Serial.println("");

  /* Initialise the sensor */
  if(!gyro.begin(GYRO_RANGE_2000DPS)) // initialized to highest range
  {
    /* There was a problem detecting the L3GD20 ... check your connections */
    if(sensor_print){
      Serial.println("Ooops, no L3GD20 detected ... Check your wiring!");
      //ERROR
    }
  }
  
  /* Display some basic information on this sensor */

  if(sensor_print)
  displaySensorDetails();
}

void gather_gyroscope(struct dataPoint *telemetry) 
{
  /* Get a new sensor event */ 
  sensors_event_t event; 
  gyro.getEvent(&event);
 
  //Display the results (speed is measured in rad/s)
  if(sensor_print){
    Serial.print("X: "); Serial.print(event.gyro.x); Serial.print("  ");
    Serial.print("Y: "); Serial.print(event.gyro.y); Serial.print("  ");
    Serial.print("Z: "); Serial.print(event.gyro.z); Serial.print("  ");
    Serial.println("rad/s ");
    delay(500);
  } 

  telemetry->gyro.x=event.gyro.x;
  telemetry->gyro.y=event.gyro.y;
  telemetry->gyro.z=event.gyro.z;

}

//////////////////////////////////////
// Barometer and Temperature sensor //
//////////////////////////////////////

MS5xxx sensor(&Wire);

void setup_barometer(void) {
  if(sensor.connect()>0) {
    if(sensor_print){
      Serial.println("Error connecting...");
      //ERROR
    }
    delay(500); // Don't need
    setup();
  }
}

void gather_barometer(struct dataPoint *telemetry) {
  sensor.ReadProm();
  sensor.Readout();
  if(sensor_print){
    Serial.print("Temperature [C]: ");
    Serial.println(sensor.GetTemp());
    Serial.print("Pressure [Pa]: ");
    Serial.println(sensor.GetPres());
    delay(500);
  }
  telemetry->prs=sensor.GetPres();
  telemetry->tmp=sensor.GetTemp();
}

 //altitude = 44330 * (1.0 - pow((pressure / 100) / 1013.25, 0.1903));

/////////
// GPS //
/////////


#define GPSSerial Serial1
Adafruit_GPS GPS(&GPSSerial);
//#define GPSECHO true // Set true to echo gps data to USB serial
uint32_t gps_timer; 
uint32_t gps_time_interval = 2000; // updates gps every 2 seconds 

void setup_gps(void){

  GPS.begin(9600); //change to 57000 or something look at data sheet
  GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA); //Turns on RMC and GGA
  GPS.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ); // Sets update rate to 1 Hz  
  //GPS.sendCommand(PGCMD_ANTENNA); // requests antenna status

} 

void gather_gps(struct dataPoint *telemetry){
  // do this whole thing only once and a while, write an if statement for time
  GPS.read();

  if (GPS.newNMEAreceived()) {
   // Serial.println(GPS.lastNMEA()); 
    if (!GPS.parse(GPS.lastNMEA()))
      return;
  }

  if(millis() - gps_timer > gps_time_interval ){
    gps_timer = millis();
    if (GPS.fix) {
      telemetry->gps.latitude=GPS.latitude;
      telemetry->gps.longitude=GPS.longitude;
      //telemetry->gps.lon=GPS.lon;
      //telemetry->gps.lat=GPS.lat;
      telemetry->gps.speed=GPS.speed;
      telemetry->gps.angle=GPS.angle;
      telemetry->gps.altitude=GPS.altitude;
      //telemetry->gps.updated=true;
      telemetry->code |= 1 << 0; //sets "gps fixed" bit high


      if (sensor_print){// print to USB serial
        Serial.print(telemetry->gps.latitude);
        //Serial.println(telemetry->gps.lat);
        Serial.print(telemetry->gps.longitude);
        //Serial.println(telemetry->gps.lon);
        Serial.println(telemetry->gps.speed);
        Serial.println(telemetry->gps.angle);
        Serial.println(telemetry->gps.altitude);
      }
    }
    else{
      telemetry->code &= ~(1 << 0);
      if(sensor_print){
        Serial.println("No GPS Fix");
      }
    }
  }/*
  else{
    telemetry->gps.updated=false;
  }
  */
}


