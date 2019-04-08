#include <errno.h>
#include <fcntl.h> 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

#include <stdbool.h>

// Serial code based off of 
// https://stackoverflow.com/questions/6947413/how-to-open-read-and-write-from-serial-port-in-c


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



int set_interface_attribs(int fd, int speed)
{
    struct termios tty;

    if (tcgetattr(fd, &tty) < 0) {
        printf("Error from tcgetattr: %s\n", strerror(errno));
        return -1;
    }

    cfsetospeed(&tty, (speed_t)speed);
    cfsetispeed(&tty, (speed_t)speed);

    tty.c_cflag |= (CLOCAL | CREAD);    /* ignore modem controls */
    tty.c_cflag &= ~CSIZE;
    tty.c_cflag |= CS8;         /* 8-bit characters */
    tty.c_cflag &= ~PARENB;     /* no parity bit */
    tty.c_cflag &= ~CSTOPB;     /* only need 1 stop bit */
    tty.c_cflag &= ~CRTSCTS;    /* no hardware flowcontrol */

    /* setup for non-canonical mode */
    tty.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL | IXON);
    tty.c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);
    tty.c_oflag &= ~OPOST;

    /* fetch bytes as they become available */
    tty.c_cc[VMIN] = 1;
    tty.c_cc[VTIME] = 1;

    if (tcsetattr(fd, TCSANOW, &tty) != 0) {
        printf("Error from tcsetattr: %s\n", strerror(errno));
        return -1;
    }
    return 0;
}

void set_mincount(int fd, int mcount)
{
    struct termios tty;

    if (tcgetattr(fd, &tty) < 0) {
        printf("Error tcgetattr: %s\n", strerror(errno));
        return;
    }

    tty.c_cc[VMIN] = mcount ? 1 : 0;
    tty.c_cc[VTIME] = 5;        /* half second timer */

    if (tcsetattr(fd, TCSANOW, &tty) < 0)
        printf("Error tcsetattr: %s\n", strerror(errno));
}


int main()
{
    char *portname = "/dev/ttyACM0";
    int fd;
    int wlen;

    fd = open(portname, O_RDWR | O_NOCTTY | O_SYNC);  
    if (fd < 0) {
        printf("Error opening %s: %s\n", portname, strerror(errno));
        return -1;
    }
    /*baudrate 115200, 8 bits, no parity, 1 stop bit */
    set_interface_attribs(fd, B115200);
    //set_mincount(fd, 0);                /* set to pure timed read */


/*
    // simple output 
    wlen = write(fd, "Hello!\n", 7);
    if (wlen != 7) {
        printf("Error from write: %d, %d\n", wlen, errno);
    }
    tcdrain(fd);    // delay for output 
*/



    do {
        unsigned char buf[60];
        int rdlen;

        rdlen = read(fd, buf, sizeof(buf)); //originally was rdlen = read(fd, buf, sizeof(buf)-1);
        if (rdlen > 0) {
          //usleep(1000);
          memcpy(&time_code, buf, sizeof(time_code)); // copy first part of buffer to "basic" struct
            if(!(time_code.code & (1 << 0))){ // if reading dataPoint
                memcpy(&data_telemetry, &buf[sizeof(time_code)], sizeof(data_telemetry)); // copy second part to "dataPoint" struct
                // Return 0;
            }

            else{ // if reading gpsData
                memcpy(&data_gps, &rx_buf[sizeof(time_code)], sizeof(data_gps)); // copy second part of buf to gpsData struct
               /// return 1;
            }

          
          //printf("latitude : %f\n", data_telemetry.gps.latitude);
          //printf("longitude : %f\n", data_telemetry.gps.longitude);
          printf("acc_x : %f\n", data_telemetry.acc.x);
          printf("pressure : %f\n", data_telemetry.prs);
          printf("temperature : %f\n", data_telemetry.tmp);
          //tcdrain(fd);    // delay for output 
        } 
        else if (rdlen < 0) {
            printf("Error from read: %d: %s\n", rdlen, strerror(errno));
        } 
        else {  /* rdlen == 0 */
            printf("Timeout from read\n");
        }              
        /* repeat read to get full message */
    } while (1);
}