#include <comm.h>
#include <Stepper.h>
const int stepsPerRevolution = 200;  // change this to fit the number of steps per revolution

int position = 0; 
bool blinkstate = 0;
int inMessage;
// for your motor
// 21 on pinion, 24 gear 
const int n_pinion = 21;
const int n_gear = 24;
const int steps_range = n_pinion * stepsPerRevolution / n_gear;

void stepper_movement(int);



// initialize the stepper library on pins 8 through 11:
Stepper myStepper(stepsPerRevolution, 4, 5, 6, 7);

void move_to(int pos){
  Serial.print('l');
  Serial.print(pos);
  Serial.print(' ');
  Serial.print(position);
  Serial.print(' ');
  Serial.print(pos-position);
  
  myStepper.step(pos-position);
  position = pos; 
  
}

void setup() {
  // set the speed at 60 steps/sec:
  myStepper.setSpeed(60);
  // initialize the serial port:
  Serial.begin(9600);
  pinMode(LED_BUILTIN, OUTPUT);
}
#define ID "bank"

void loop() {
  // step one revolution  in one direction:
   if(Serial.available() > 0){
    Message m  = wait_for_message();

    // Respond to the handshakes 
    if(!handle_handshake(m,ID)){
      if (m.label = 'a'){
        int pos = steps_range - int(map(atoi(m.content),0, 1024, 0, steps_range));
        move_to(pos);
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
