#include <MyCommon.h>


// Error code blink

void error_blink(int pattern, int LED) {
  while(true) {
    int i;
    for (i=0; i<pattern; i++) {
      REG_PORT_OUTSET0= LED; // Set Pin High
      delay(160);
      REG_PORT_OUTCLR0= LED; // Set port low
      delay(160);
    }
      delay(900);
  }
}

