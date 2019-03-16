#include <MySD.h>

bool sd_print = true;

#define cardSelect 4 // for built in module, don't change
char filename[15];
int flushtime =0;
File logfile;

//uint8_t buf[RH_RF69_MAX_MESSAGE_LEN]; // for recieving
//char data[300]; // for sending (change size later)

void setup_sd(){

    if (!SD.begin(cardSelect)) {
        if(sd_print){
            Serial.println("Card init. failed!");
        } 
        error_blink(3, LED_R);
    }

    
    //Copied this code but modified the filename
    strcpy(filename, "/ECELOG00.TXT");
    for (uint8_t i = 0; i < 100; i++) {
        filename[7] = '0' + i/10;
        filename[8] = '0' + i%10;
        // create if does not exist, do not open existing, write, sync after write
        if (! SD.exists(filename)) {
            break;
        }
    }

    logfile = SD.open(filename,  O_CREAT | O_WRITE );
    if( !logfile ) {
        if(sd_print){
            Serial.print("Couldnt create "); 
            Serial.println(filename);
        }
        error_blink(4, LED_R);
    }
    if(sd_print){
        Serial.print("Writing to "); 
        Serial.println(filename);
    }
    //logfile.close(); leaving file open and controlling writes with flush();
    //only close after power saving mode. Even then not sure if necissary.

}

void log_data(struct dataPoint *telemetry){

  flushtime += 1;
  //logfile = SD.open(filename, O_CREAT | O_WRITE | O_APPEND );
  if( ! logfile ) {
    Serial.print("Couldnt create "); 
    Serial.println(filename);
  }
  else{
   // struct datastore mydata;
   // strcpy (mydata.stringlol,data);

    //logfile.print(data);

    logfile.write((const uint8_t *)telemetry, sizeof(*telemetry));
    if(flushtime == 8){ // flush stores 512 bytes. dataPoint struct is 60 bytes. 512/60=8.53
      logfile.flush();  
      flushtime = 0;
    }
    //logfile.close();
  }

}




