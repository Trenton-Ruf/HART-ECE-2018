#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct gpsData{
  float latitude;
  float longitude;
  float speed;
  float angle;
  float altitude;
  char misc;

  ///////////////////
  // code for misc //
  ///////////////////

  // bit 0 -> lat
  // bit 1 -> lon

}data_gps;

struct xyzData{
  float x;
  float y;
  float z;
};

struct dataPoint { 
  struct xyzData acc;   // m/s^2  //might be able to get higher resolution by raw data instead
  struct xyzData gyro;  // rad/s
  float prs;            // Pa
  float tmp;            // C
}data_telemetry;

struct basic{
  unsigned int time;    // ms
  unsigned int code;
  ///////////////////
  // code for code //
  ///////////////////

  // bit 0 -> dataPoint -> 1 Data
  // bit 1 -> 0 -> no gps fix
  // bit 2 -> Address -> 0 if booster, 1 if sustainer
  // bit 3 -> event 0 -> 1 if launch happened
  // bit 4 -> event 1 -> 1 if separation/sustainer happened
  // bit 5 -> event 2 -> 1 if drouge shoot happened
  // bit 6 -> event 3 -> 1 if main shoot happened
  // bit 7 -> 1 if sending battery info // maybe implement

  // INT IS 16 BITS YOU CAN ADD MORE CODES

  // bool mybit = ((code >> 3)  & 0x01) //stores bit 3 of "code" in mybit
}time_code;



FILE *fp_read;
FILE *fp_write;

int main(int argc, char *argv[]){

  struct dataPoint data_telemetry;
  char filename_read [250];

  char buffer[700];
  char little_buf[100];

  strcpy(filename_read,argv[1]);

  fp_read = fopen(filename_read,"r");
  if(fp_read == NULL){
    perror("Couldn't find the file");   
    exit(1);
  }

  perror("\nFile opened\n"); // not really an error just don't want to print to standard out

  printf("Time,Accel_X,Accel_Y,Accel_Z,Gyro_X,Gyro_Y,Gyro_Z,Pressure,Temperature,");
  printf("GPS_Fix,Latitude,Longitude,GPS_Speed,GPS_Angle,GPS_Altitude,Events\n");

  while(1){

  if( 0 == fread(&time_code, sizeof(struct basic),1,fp_read)){
    break;
  }
  printf("%d,",time_code.time);

  if(!(time_code.code & (1 << 0))){ // if reading dataPoint
    if( 0 == fread(&data_telemetry, sizeof(struct dataPoint),1,fp_read)){
      break;
    }

    printf("%f,",data_telemetry.acc.x);
    printf("%f,",data_telemetry.acc.y);
    printf("%f,",data_telemetry.acc.z);

    printf("%f,",data_telemetry.gyro.x);
    printf("%f,",data_telemetry.gyro.y);
    printf("%f,",data_telemetry.gyro.z);

    printf("%f,",data_telemetry.prs);
    printf("%f,",data_telemetry.tmp);

    printf("%i," ,((time_code.code >> 1 )) & 0x01); // GPS_fix
  }

  else{
    if( 0 == fread(&data_gps, sizeof(struct gpsData),1,fp_read)){ // if reading gpsData
      break;
    }

    printf(",,,,,,,,%i," ,((time_code.code >> 1 )) & 0x01); // GPS_fix
    printf("%f", data_gps.latitude);
    if((data_gps.misc >> 0 ) & 0x01){
      printf("S,");
    }
    else{
      printf("N,");
    }
    printf("%f", data_gps.longitude);
    if((data_gps.misc >> 1 ) & 0x01){
      printf("E,");
    }
    else{
      printf("W,");
    }
    printf("%f,", data_gps.speed);
    printf("%f,", data_gps.angle);
    printf("%f,", data_gps.angle);

  }

  //print events
  
  if((time_code.code & (1 << 6)))
  printf("Main_chute");
  else if((time_code.code & (1 << 5)))
  printf("drouge_chute");
  else if((time_code.code & (1 << 4))){
    if((time_code.code & (1 << 2)))
    printf("sustainer");
    else ("seperation");
  }
  else if((time_code.code & (1 << 3)))
  printf("launch");

  printf("\n");
  }

  fclose(fp_read); 
  return 0;
}





//fwrite (buffer , sizeof(char), sizeof(buffer), fp_write);
// If you want to read raw integers, you need to deal with endian issues.
// One technique is to store in network endian order which means you use
// htonl before writing and ntohl after reading.