#include <Arduino.h>
#include <MyCommon.h>
#include <SPI.h>
#include <SD.h>


#define cardSelect 4 // for built in module, don't change

char * setup_sd(File *logfile);

void log_data(File * logfile, char * filename, struct dataPoint *telemetry, int *flushtime);

void read_data();
