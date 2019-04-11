#include <MyCommon.h>


// Error code blink

void error_blink(int pattern, int LED) {
  while(true) {
    int i;
    for (i=0; i<pattern; i++) {
      REG_PORT_OUTSET0= LED; // Set Pin High
      delay(80);
      REG_PORT_OUTCLR0= LED; // Set port How
      delay(80);
    }
    for (i=pattern; i<10; i++) {
      delay(160);
    }
  }
}

