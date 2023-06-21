#include "comm.h"

void setup(){
  Serial.begin(9600);
}

#define ID "input"

void loop() {
  Message m  = wait_for_line_val();

  HANDLE_HANDSHAKE(m,ID)

}
