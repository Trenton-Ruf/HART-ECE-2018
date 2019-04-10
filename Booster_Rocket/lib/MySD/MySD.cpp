#include <MySD.h>

bool sd_print = true;


//uint8_t buf[RH_RF69_MAX_MESSAGE_LEN]; // for recieving
//char data[300]; // for sending (change size later)

char * setup_sd(File *logfile){

    char *filename =(char *) malloc(15 * sizeof(char)); //char is 1 byte might not need size of char

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

    *logfile = SD.open(filename,  O_CREAT | O_WRITE );

    if( ! *logfile ) {
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

    return filename;

    //logfile.close(); leaving file open and controlling writes with flush();
    //only close after power saving mode. Even then not sure if necissary.

}

void log_dataPoint(File * logfile, char * filename, dataPoint * store_dataPoint, int *flushtime){

  *flushtime += 32; // Add number of bytes of dataPoint to flushtime
    if(*flushtime > 512){ // flush stores 512 bytes. dataPoint struct is 60 bytes. 512/60=8.53
      logfile->flush();  
      *flushtime = 32;
    }
  //logfile = SD.open(filename, O_CREAT | O_WRITE | O_APPEND );
  if( ! *logfile ) {
    Serial.print("Couldnt open "); 
    Serial.println(filename);
  }
  else{
    logfile->write((const uint8_t *)store_dataPoint, sizeof(*store_dataPoint));
  }
    //logfile.close();
}
void log_gpsData(File * logfile, char * filename, gpsData * store_gpsData, int *flushtime){

  *flushtime += 21; // Add number of bytes of gpsData to flushtime
    if(*flushtime > 512){ // flush stores 512 bytes. gpsData struct is 60 bytes. 512/60=8.53
      logfile->flush();  
      *flushtime = 21;
    }
  //logfile = SD.open(filename, O_CREAT | O_WRITE | O_APPEND );
  if( ! *logfile ) {
    Serial.print("Couldnt open "); 
    Serial.println(filename);
  }
  else{
    logfile->write((const uint8_t *)store_gpsData, sizeof(*store_gpsData));
  }
    //logfile.close();
}

void log_basic(File * logfile, char * filename, basic * store_basic, int *flushtime){

  *flushtime += 4; // Add number of bytes of dataPoint to flushtime
    if(*flushtime > 512){ // flush stores 512 bytes. dataPoint struct is 60 bytes. 512/60=8.53
      logfile->flush();  
      *flushtime = 4;
    }
  //logfile = SD.open(filename, O_CREAT | O_WRITE | O_APPEND );
  if( ! *logfile ) {
    Serial.print("Couldnt open "); 
    Serial.println(filename);
  }
  else{
    logfile->write((const uint8_t *)store_basic, sizeof(*store_basic));
  }
    //logfile.close();
}


/*
void read_data_to_Serial(){

    File logfile;
    dataPoint telemetry;
    byte buf[60];

    if (!SD.begin(4)) {
        Serial.println("initialization failed!");
        while (1);
    }

    logfile = SD.open("test.txt"); // change name

    while (logfile.available()) {
        Serial.write(logfile.read());
      //memcpy(telemetry, rx_buf, sizeof(*telemetry));
    }

    //logfile.read((const uint8_t *)&telemetry, sizeof(telemetry));

}

*/

