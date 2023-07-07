const int analogOutPin = 9;
int startValue = 255;
int stopValue = 0;
#include <comm.h>

bool blinkstate = 0;

void setup() {
Serial.begin(9600);

pinMode(LED_BUILTIN, OUTPUT);
pinMode(analogOutPin, OUTPUT);
}
#define ID "fan"

void loop() {

  if(Serial.available() > 0){
    Message m = wait_for_message();
    //Serial.println(m.label);

    // Respond to the handshakes 
    if(!handle_handshake(m,ID)){
      int start_fan = atoi(m.content);
      if (start_fan > 0){
        analogWrite(analogOutPin, startValue);
        //delay(5000);
        analogWrite(analogOutPin, stopValue);
      }


       // Respond to anything else
       send_message(m);
    }

  } else {
    blinkstate = !blinkstate;
    digitalWrite(LED_BUILTIN, blinkstate);
    delay(200);  
  }
}
