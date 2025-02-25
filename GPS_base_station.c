#include "serial_telemetry.h"

struct gpsData data_gps;
struct dataPoint data_telemetry;
struct basic time_code;

int main(){

    //////////////////
    // Setup Serial //
    //////////////////

    int gps_or_sensors;
    char *portname =  "/dev/ttyACM0";     // Trenton's Laptop  "/dev/ttyS11"; //Trenton's Desktop 

    int fd = setup_serial(portname);

    if(fd == -1) // if errors in setup_serial exit
    return -1;

    /////////////////////////////
    // Gather serial telemetry //
    /////////////////////////////

    printf("\nWaiting for GPS data\n\n");

    while(1){ 

        gps_or_sensors = gather_telemetry(fd,&time_code,&data_telemetry,&data_gps);
        if(gps_or_sensors == 0){
            // time_code and data_telemetry was updated
            // do stuff
           // printf("acc_x : %f\n", data_telemetry.acc.x);
            //printf("pressure : %f\n", data_telemetry.prs);
           // printf("temperature : %f\n", data_telemetry.tmp);
        }
        else if(gps_or_sensors == 1){
            // time_code and data_gps was updated
            //do other stuff
            printf("latitude : %f\n", data_gps.latitude);
            printf("longitude : %f\n", data_gps.longitude);
        }
        else{ // if gather_telemetry failed
            return -1;
        }
        //printf("time: %i\n", time_code.time);
        //printf("code: %i\n", time_code.code);
        
    }

    return 0;
}
