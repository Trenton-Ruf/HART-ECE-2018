#include <MyCommon.h>


// Error code blink

void error_blink(int pattern, int LED) {
  while(true) {
    int i;
    for (i=0; i<pattern; i++) {
      digitalWrite(LED, HIGH);
      delay(80);
      digitalWrite(LED, LOW);
      delay(80);
    }
    for (i=pattern; i<10; i++) {
      delay(160);
    }
  }
}

