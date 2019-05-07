# HART-ECE-2018
HART code for the ECE subteam 2018-2019



The following items are used by the CS team to get telemetry data from the base station microcontroller <br />
serial_telemetry.h <br />
serial_telemetry.c <br />
serial_example.c // just an example of implementation



**How to read from SD card:**<br />
1.) Open SD card from Avionics-Bay subsystem.<br />
2.) Copy the "ECELOGXX" file from the SD card to the same directory as "read_sd_card". <br />
3.) run "read_sd_card" with the logfile as a parameter and redirect stdout to a .cvs file.  <br />
Example: "read_sd_card ECELOGXX.TXT > ECELOGXX.cvs"

The reason this process is necissary is because writing ASCII directly to the .TXT file was taking too long while logging telemetry data to the microSD card. Therefore I decreased the amount of data to be written by storing raw binary.   
**caution : **
In order for the "read_sd_card" program to report accurate values make sure the endianess of the machine you are running it on is the same as what an ARM cortex m0 uses. Also make sure that data types have the same sizes. 


PlatformIO plugin for VScode was used to upload code to the microcontroller.<br />




