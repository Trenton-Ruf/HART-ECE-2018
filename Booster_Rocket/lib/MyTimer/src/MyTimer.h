#include <arduino.h>
// Credit for library goes to jdneo 
// https://gist.github.com/jdneo/43be30d85080b175cb5aed3500d3f989

#define CPU_HZ 48000000
#define TIMER_PRESCALER_DIV 1024

void startTimer(int frequencyHz);
void setTimerFrequency(int frequencyHz);
