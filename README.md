# HART-ECE-2018
HART code for the ECE subteam 2018-2019

The following items are used by the CS team to get telemetry data from the base station microcontroller <br />
serial_telemetry.h <br />
serial_telemetry.c <br />
serial_example.c // just an example of implementation

How to read from SD card:
1.) Open SD card from Avionics-Bay subsystem.<br />
2.) Copy the "ECELOGXX" file from the SD card to the same directory as "read_sd_card". <br />
3.) run "read_sd_card" with the logfile as a parameter and redirect stdout to a .cvs file.  <br />
Examplbe: "read_sd_card ECELOGXX > ECELOGXX.cvs"
