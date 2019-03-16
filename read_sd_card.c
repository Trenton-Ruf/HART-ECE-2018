#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct gpsData{
  float latitude;
  float longitude;
  float speed;
  float angle;
  float altitude;
};

struct xyzData{
  float x;
  float y;
  float z;
};

struct dataPoint { 
  struct gpsData gps;
  struct xyzData acc;   // m/s^2  //might be able to get higher resolution by raw data instead
  struct xyzData gyro;  // rad/s
  float prs;            // Pa
  float tmp;            // C
  unsigned int time;    // ms


  unsigned int code;

  ///////////////////
  // code for code //
  ///////////////////

  // bit 0 -> gps fix -> 1 if fixed
  // bit 1 -> event 0 -> 1 if happened
  // bit 2 -> event 1 -> 1 if happened
  // bit 3 -> event 2 -> 1 if happened
  // bit 4 -> 

  // might use these if neccisary 
  //char  lon; 
  //char  lat;

  // bool mybit = ((code >> 3)  & 0x01) //stores bit 3 of "code" in mybit
};
// struct dataPoint is 60 bytes! The max a radio packet can be! PERFECT!




FILE *fp_read;
FILE *fp_write;

int main(int argc, char *argv[]){

  struct dataPoint data_telemetry;
  char filename_read [250];
  char filename_write [250];

  char buffer[700];
  char little_buf[100];
  //strcpy (filename

  strcpy(filename_read,argv[1]);
  //strcpy(filename_write,argv[2]);

  fp_read = fopen(filename_read,"r");
  if(fp_read == NULL){
    perror("Couldn't find the file");   
    exit(1);
  }

  perror("\nFile opened\n");
/*
  fp_write = fopen(filename_write,"w");
  if(fp_write == NULL){
    perror("Couldn't make the file");   
    exit(1);
  }
*/


  while(1){
  //fseek(fp_read, 0, SEEK_SET); // go to beginning of file

  // 0 if read incorrectly
  if( 0 == fread(&data_telemetry, sizeof(struct dataPoint),1,fp_read)){
    break;
  }

  sprintf(buffer, "TIME: %i", data_telemetry.time);
  sprintf(little_buf, "\tGPS\tlatitude: %f", data_telemetry.gps.latitude);
  strcat(buffer, little_buf);
  sprintf(little_buf, "\tlongitude: %f", data_telemetry.gps.longitude);
  strcat(buffer, little_buf);
  sprintf(little_buf, "\tspeed: %f", data_telemetry.gps.speed);
  strcat(buffer, little_buf);
  sprintf(little_buf, "\tangle: %f", data_telemetry.gps.angle);
  strcat(buffer, little_buf);
  sprintf(little_buf, "\taltitude: %f", data_telemetry.gps.altitude);
  strcat(buffer, little_buf);

  sprintf(little_buf, "\tACC\tx: %f", data_telemetry.acc.x);
  strcat(buffer, little_buf);
  sprintf(little_buf, "\ty: %f", data_telemetry.acc.y);
  strcat(buffer, little_buf);
  sprintf(little_buf, "\tz: %f", data_telemetry.acc.z);
  strcat(buffer, little_buf);

  sprintf(little_buf, "\tGYRO\tx: %f", data_telemetry.gyro.x);
  strcat(buffer, little_buf);
  sprintf(little_buf, "\ty: %f", data_telemetry.gyro.y);
  strcat(buffer, little_buf);
  sprintf(little_buf, "\tz: %f", data_telemetry.gyro.z);
  strcat(buffer, little_buf);

  sprintf(little_buf, "\tTMP: %f", data_telemetry.tmp);
  strcat(buffer, little_buf);
  sprintf(little_buf, "\tPRS: %f", data_telemetry.prs);
  strcat(buffer, little_buf);
  sprintf(little_buf, "\tCODE: %i", data_telemetry.code);
  strcat(buffer, little_buf);

  printf("\n");
  printf(buffer);
  printf("\n");
  //fwrite (buffer , sizeof(char), sizeof(buffer), fp_write);

  //sleep(1);
  }

// fclose(fp_write); 
  fclose(fp_read); 
  return 0;
}





// If you want to read raw integers, you need to deal with endian issues.
// One technique is to store in network endian order which means you use
// htonl before writing and ntohl after reading.