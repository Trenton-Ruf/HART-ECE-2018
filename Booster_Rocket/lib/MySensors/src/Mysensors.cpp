#include "MySensors.h"

bool sensor_print = true; // set true to print to Serial

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
    Serial.print("Acceleration:");
    Serial.print("\tX: "); Serial.print(event.acceleration.x);
    Serial.print("\t\t\tY: "); Serial.print(event.acceleration.y); 
    Serial.print("\t\t\tZ: "); Serial.print(event.acceleration.z); 
    Serial.println(" m/s^2 ");
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
    Serial.print("Gyroscope:");
    Serial.print("\tX: "); Serial.print(event.gyro.x);
    Serial.print("\t\t\tY: "); Serial.print(event.gyro.y);
    Serial.print("\t\tZ: "); Serial.print(event.gyro.z);
    Serial.print(" rad/s ");
    Serial.println();
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
  if(sensor_print)
  Serial.println("Barometer/Thermometer Test"); Serial.println("");
  if(sensor.connect()>0) {
    if(sensor_print){
      Serial.println("Error connecting...");
      //ERROR
    }
    setup();
  }
}

void gather_barometer(struct dataPoint *telemetry) {
  sensor.ReadProm();
  sensor.Readout();
  if(sensor_print){
    Serial.print("Temperature [C]: ");
    Serial.println(sensor.GetTemp()/100);
    Serial.print("Pressure [Pa]: ");
    Serial.println(sensor.GetPres());
  }
  telemetry->prs=sensor.GetPres();
  telemetry->tmp=sensor.GetTemp()/100;
}

/////////
// GPS //
/////////



//#define GPSECHO true // Set true to echo gps data to USB serial
uint32_t gps_timer = millis(); 
uint32_t gps_time_interval = 3000; // updates gps every 2 seconds 

void setup_gps(Adafruit_GPS * GPS){
  if(sensor_print)
  Serial.println("GPS Test"); Serial.println("");

  GPS->begin(9600); //change to 57000 or something look at data sheet
  GPS->sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA); //Turns on RMC and GGA
  GPS->sendCommand(PMTK_SET_NMEA_UPDATE_1HZ); // Sets update rate to 1 Hz  
  GPS->sendCommand(PGCMD_ANTENNA); // requests antenna status
  //delay(1000);
} 

int gather_gps(Adafruit_GPS * GPS, struct gpsData *gpsdata,struct basic *time_code){
  GPS->read();

  if (GPS->newNMEAreceived()) {
    if(sensor_print){
      Serial.println(GPS->lastNMEA()); 
    }
    if (!GPS->parse(GPS->lastNMEA()))
      return 0;
  }

  if(millis() - gps_timer > gps_time_interval ){
    gps_timer = millis();
    if (GPS->fix>0) {
      gpsdata->latitude=GPS->latitude;
      gpsdata->longitude=GPS->longitude;
      if( GPS->lon == 'E'){
        gpsdata->misc &= ~(1 << 1);
      }
      else{
        gpsdata->misc |= 1 << 1; 
      }
       if( GPS->lat == 'N'){
        gpsdata->misc &= ~(1 << 0);
      }
      else{
        gpsdata->misc |= 1 << 0; 
      }
      gpsdata->speed=GPS->speed;
      gpsdata->angle=GPS->angle;
      gpsdata->altitude=GPS->altitude;
      time_code->code |= 1 << 1; //sets "gps fixed" bit high

      if (sensor_print){// print to USB serial
        Serial.print("\n\nGPS Fix!\n\n");
        Serial.print("GPS Latitude: "); Serial.println(gpsdata->latitude);
        //Serial.println(gpsdata->lat);
        Serial.print("GPS Longitude: "); Serial.println(gpsdata->longitude);
        //Serial.println(gpsdata->lon);
        Serial.print("GPS Speed: ");  Serial.println(gpsdata->speed);
        Serial.print("GPS Angle: "); Serial.println(gpsdata->angle);
        Serial.print("GPS Altitude: "); Serial.println(gpsdata->altitude);
        delay(500);

        return 1;
      }
    }
    else{
      time_code->code &= ~(1 << 1);
      if(sensor_print){
        Serial.println("\nNo GPS Fix");
        delay(500);
      }
    }
  } 
  return 0; 
}

