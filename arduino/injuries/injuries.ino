#include <comm.h>
#include <FastLED.h>
#define LED_PIN 10
#define NUM_LEDS 15


bool blinkstate = 0 : CRGB leds[NUM_LEDS];

void setup() {
  FastLED.addLeds<WS2812, LED_PIN, GRB>(leds, NUM_LEDS);
  Serial.begin(9600);
  pinMode(LED_BUILTIN, OUTPUT);
}

#define ID "input"

void loop() {
  if (Serial.available() > 0) {  // check if data is available in the serial buffer
    Message m = wait_for_message();
    if (!handle_handshake(m, ID)) {
      int injury_value = atoi(m.content);                          // read the value from the serial buffer
      int injury_index = map(injury_value, 0, 1023, 0, NUM_LEDS);  // map the values 0-1023 to an index value to be used for certain number of LEDs

      if (injury_value > 0) {                      //if there is a serial communication value input
        for (int i = 0; i <= injury_index; i++) {  // turn the number of LEDs equivalent to the number of injuries white
          leds[i] = CRGB(255, 255, 255);
          FastLED.show();
        }
        delay(5000);  // wait for 5 seconds

        for (int i = 0; i <= injury_index; i++) {  // reset the LEDs that were turned on
          leds[i] = CRGB(0, 0, 0);
          FastLED.show();
        }
      } else {
        for (int i = 0; i <= injury_index; i++) {  // otherwise there is no input, keep strip turned off
          leds[i] = CRGB(0, 0, 0);
          FastLED.show();
        }
      }
      send_message(m);
    }
  } else {
    blinkstate = !blinkstate;
    digitalWrite(LED_BUILTIN, blinkstate)
    delay(200);
  }
}
