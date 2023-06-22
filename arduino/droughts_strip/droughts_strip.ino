#include <FastLED.h>
#define LED_PIN     10
#define NUM_LEDS    25
CRGB leds[NUM_LEDS];

void setup() {
  FastLED.addLeds<WS2812, LED_PIN, GRB>(leds, NUM_LEDS);
  Serial.begin(9600); // initialize serial communication
}

void loop() {
  int defaultGreen = 255;
  int maxRed = 255;
  int delayTime = random(10, 30);
  int flickerNumber = 150;

  for (int i = 0; i <= NUM_LEDS; i++) { // turn off the RGB strip
          leds[i] = CRGB(0, defaultGreen, 0);
          FastLED.show();
  }
        
  if (Serial.available() > 0) { // check if data is available in the serial buffer
      int drought_value = Serial.parseInt(); // read the value from the serial buffer
      Serial.println("drought value: " + String(drought_value));
      
      int greenValue = map(drought_value, 0, 1, defaultGreen, 60); // on or off with drought lights
      int redValue = map(drought_value, 0, 1, 0, maxRed);

      if (drought_value > 0) {
        for (int i = 0; i <= flickerNumber; i++) {
          for (int i = 0; i <= NUM_LEDS; i++) { // turn the RGB strip red with the mapped brightness level
            leds[i] = CRGB(random(redValue-30, redValue+30), random(greenValue-35, greenValue+5), 0);
            FastLED.show();
          }
          delay(delayTime); // wait for a random time
        }

        
        for (int i = 0; i <= NUM_LEDS; i++) { // reset strip
          leds[i] = CRGB(0, defaultGreen, 0);
          FastLED.show();
        }
      }  
  }
}
