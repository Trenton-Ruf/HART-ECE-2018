#include <Arduino.h>
#include <MyCommon.h>
#include <SPI.h>
#include <SD.h>


#define cardSelect 4 // for built in module, don't change

char * setup_sd(File *logfile);

void log_dataPoint(File * logfile, char * filename, dataPoint* store_dataPoint, int *flushtime);
void log_gpsData(File * logfile, char * filename,  gpsData * store_dataGPS, int *flushtime);
void log_basic(File * logfile, char * filename,  basic * store_basic, int *flushtime);

//void read_data(); // In case couldn't get access to sd card
