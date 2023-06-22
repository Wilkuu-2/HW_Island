#include <FastLED.h>
#define LED_PIN     10
#define NUM_LEDS    15
CRGB leds[NUM_LEDS];

void setup() {
  FastLED.addLeds<WS2812, LED_PIN, GRB>(leds, NUM_LEDS);
  Serial.begin(9600); // initialize serial communication
}

void loop() {
  if (Serial.available() > 0) { // check if data is available in the serial buffer
      int injury_value = Serial.parseInt(); // read the value from the serial buffer
      int injury_index = map(injury_value, 0, 1023, 0, NUM_LEDS); // map the values 0-1023 to an index value
      Serial.println("input value: " + String(injury_value) + ", index: " + String(injury_index));


      if (injury_value > 0) {
        for (int i = 0; i <= injury_index; i++) { // turn the RGB strip red with the mapped brightness level
          leds[i] = CRGB(255, 255, 255);
          FastLED.show();
        }
        delay(5000); // wait for 5 seconds
        for (int i = 0; i <= injury_index; i++) { // turn off the RGB strip
          leds[i] = CRGB(0, 0, 0);
          FastLED.show();
        }
      }
      else {
        for (int i = 0; i <= injury_index; i++) { // turn off the RGB strip
          leds[i] = CRGB(0, 0, 0);
          FastLED.show();
        }
      }  
  }
}
