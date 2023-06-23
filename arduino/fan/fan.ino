const int analogOutPin = 9;
int startValue = 255;
int stopValue = 0;
#include "libs/comm.cpp"

bool blinkstate = 0;

void setup() {
Serial.begin(9600);
pinMode(LED_BUILTIN, OUTPUT);
pinMode(analogOutPin, OUTPUT);
}
#define ID "input"

void loop() {

  if(Serial.available()){
    Message m  = wait_for_message();

    // Respond to the handshakes 
    if(!handle_handshake(m,ID)){
      int start_fan = atoi(m.content);
      if (start_fan > 0){
        analogWrite(analogOutPin, outputValue);
        delay(5000);
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
