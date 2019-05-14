#include <MySensors.h>

bool sensor_print = false; // set true to print to Serial (For debugging).

void set_sensor_print(bool set_bool){
  sensor_print = set_bool;
}

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
    error_blink(1, LED_G);
  }

  if(sensor_print){
    Serial.println("LIS3DH found!");
    lis.setRange(LIS3DH_RANGE_16_G);   // 2, 4, 8 or 16 G!
    Serial.print("Range = "); Serial.print(2 << lis.getRange());  
    Serial.println("G");
  }
}

void gather_accelerometer(struct dataPoint *telemetry) {

  lis.read(); // Check if necissary 
  
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

  // Map orientation of the accelerometer to how its mounted on the PCB
  // When mounted: Z-axis up, X-axis left, Y-axis out.
  telemetry->acc.x = event.acceleration.x;
  telemetry->acc.z = event.acceleration.y;
  telemetry->acc.y = -1 * event.acceleration.z;
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
      Serial.println("L3GD20, not detected.");
      //maybe blink ERROR
    error_blink(2, LED_G);
    }
  }

  // Map orientation of the gyroscope to how its mounted on the PCB
  // When mounted: Z-axis up, X-axis left, Y-axis out.
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

  telemetry->gyro.x=event.gyro.y;
  telemetry->gyro.y=event.gyro.z;
  telemetry->gyro.z=event.gyro.x;
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
      //maybe ERROR blink
      error_blink(3, LED_G);
    }
    setup();
  }
}

void gather_barometer(struct dataPoint *telemetry) {
  sensor.ReadProm();
  sensor.Readout();
  if(sensor_print){
    Serial.print("Temperature [C]: ");
    Serial.println(sensor.GetTemp()/100); // Have to divide by 100 for accurate Temp
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
uint32_t gps_time_interval = 2000; // set to twice NMEA_UPDATE period

void setup_gps(Adafruit_GPS * GPS){
  if(sensor_print){
    Serial.println("GPS Setup");
  }

  GPS->begin(9600); // set default baud rate.

  GPS->sendCommand(PMTK_SET_NMEA_OUTPUT_OFF); // Stop GPS from outputting data while changing settings
  GPS->sendCommand("$PMTK251,38400*27<CR><LF>"); // Change GPS baud to 38400
 // GPS->sendCommand("$PMTK251,57600*2C"); // Change GPS baud to 57600
  delay(1000);
  GPS->begin(38400); // reconnect on new baud-rate
  
  GPS->sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA); //Turns on RMC and GGA
  //GPS->sendCommand(PMTK_SET_NMEA_FIX_1HZ); // Set GPS fix interval to 1,5, or 10hz
  //delay(100);
  GPS->sendCommand(PMTK_SET_NMEA_UPDATE_1HZ); // Sets update rate to 1,5, or 10z
  
  GPS->sendCommand(PGCMD_ANTENNA); // requests antenna status, might not need
  //delay(1000);

  digitalWrite(A1, LOW);
  delay(1000);
  digitalWrite(A1,HIGH);
  
} 

int gather_gps(Adafruit_GPS * GPS, struct gpsData *gpsdata,struct basic *time_code){
  //GPS->read();
  
  if (GPS->newNMEAreceived()) {
    if(sensor_print){
        Serial.println("Printing NMEA");
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
        //Serial.print("GPS lat: "); Serial.println(gpsdata->lat);
        Serial.print("GPS Longitude: "); Serial.println(gpsdata->longitude);
        //Serial.print("GPS lon: "); Serial.println(gpsdata->lon);
        Serial.print("GPS Speed: ");  Serial.println(gpsdata->speed);
        Serial.print("GPS Angle: "); Serial.println(gpsdata->angle);
        Serial.print("GPS Altitude: "); Serial.println(gpsdata->altitude);
        //delay(500);
      }
      return 1; // must be outsude of sensor_print
    }
    else{
      time_code->code &= ~(1 << 1);
      if(sensor_print){
        Serial.println("\nNo GPS Fix");
        //delay(500);
      }
    }
  } 
  return 0; 
}

