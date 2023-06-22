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
    int death_value = Serial.parseInt(); // read the value from the serial buffer
    Serial.println(death_value);
    int brightness = map(death_value, 0, 1023, 0, 255); // map the values 0-1023 to a brightness level between 0-255
    for (int i = 0; i <= NUM_LEDS/2; i++) { // turn the RGB strip red with the mapped brightness level
      leds[i] = CRGB(brightness, 0, 0);
      leds[i+7] = CRGB(255, 0, 0);
      FastLED.show();
    }
    delay(5000); // wait for 5 seconds
    for (int i = 0; i <= NUM_LEDS; i++) { // turn off the RGB strip
      leds[i] = CRGB(0, 0, 0);
      FastLED.show();
    }
  }
}
