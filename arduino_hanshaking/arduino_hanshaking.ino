/*=======================================================*
 * Arduino discovery handshake example
 * Performs handshake and echo's other packets 
 * - uses comm.cpp as it's protocol 
 *
 * Copyright 2023, Group Tantor (CreaTe M8 2023)
 * You may use the code under the terms of the MIT license
 *=======================================================*/
#include "libs/comm.cpp"

bool blinkstate = 0;

void setup(){
  // Serial setup
  Serial.begin(9600);
  pinMode(LED_BUILTIN, OUTPUT);
  
}

// This is the name you can use to find this arduino
#define ID "input"

void loop() {
  // Get a message 
  if(Serial.available()){
    Message m  = wait_for_message();

    // Respond to the handshakes 
    if(!handle_handshake(m,ID)){
       // Respond to anything else
       send_message(m);
    }
  } else {
    blinkstate = !blinkstate;
    digitalWrite(LED_BUILTIN, blinkstate);
    delay(200);  
  }
  
}
