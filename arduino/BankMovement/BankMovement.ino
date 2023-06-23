#include <Stepper.h>
#include "libs/comm.cpp"
const int stepsPerRevolutions = 20;  // change this to fit the number of steps per revolution

bool blinkstate = 0;
int inMessage;
// for your motor

// initialize the stepper library on pins 8 through 11:
Stepper myStepper(stepsPerRevolution, 8, 9, 10, 11);

void setup() {
  // set the speed at 60 rpm:
  myStepper.setSpeed(60);
  // initialize the serial port:
  Serial.begin(9600);
  pinMode(LED_BUILTIN, OUTPUT);
}
#define ID "input"

void loop() {
  // step one revolution  in one direction:
   if(Serial.available()){
    Message m  = wait_for_message();

    // Respond to the handshakes 
    if(!handle_handshake(m,ID)){
      inMessage = atoi(m.content);
      if(inMessage = 1){
        stepper_movement(stepsPerRevolution);
      }
      else if(inMessage = 2){
        stepper_movement(-stepsPerRevolution);
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

void stepper_movement(int steps){
   myStepper.step(steps);
   delay(5000);
   myStepper.step(-steps);

}