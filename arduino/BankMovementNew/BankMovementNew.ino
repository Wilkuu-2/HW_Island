#include <comm.h>
#define in1 6
#define in2 7

bool blinkstate = 0;
unsigned long current_time = 0;
int damage_value = 0;
unsigned long time_to_move = 0;
unsigned long ref_time_to_move = 0;
bool start_move = false;

void setup() {
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(9600);
}

#define ID "bank"

void loop() {
  if (Serial.available() > 0) {
    Message m = wait_for_message();
    if (!handle_handshake(m, ID)) {
      damage_value = atoi(m.content);
      time_to_move = map(damage_value, 0, 1, 0, 100);
      if (time_to_move > 0) {
        digitalWrite(in1, LOW);
        digitalWrite(in2, HIGH);
        delay(time_to_move);
        digitalWrite(in1, LOW);
        digitalWrite(in2, LOW);
        ref_time_to_move = time_to_move;
      }
      if (time_to_move == 0) {
        digitalWrite(in1, HIGH);
        digitalWrite(in2, LOW);
        delay(ref_time_to_move);
        digitalWrite(in1, LOW);
        digitalWrite(in2, LOW);
      }

    }
    send_message(m);
  }
}
