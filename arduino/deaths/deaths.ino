#include <comm.h>
#include <FastLED.h>
#define LED_PIN     10
#define NUM_LEDS    15


bool blinkstate = 0; 
CRGB leds[NUM_LEDS];

void setup() {
  FastLED.addLeds<WS2812, LED_PIN, GRB>(leds, NUM_LEDS);
  Serial.begin(9600); // initialize serial communication
  pinMode(LED_BUILTIN, OUTPUT);
}

#define ID "input"

void loop() {
  if (Serial.available() > 0) { // check if data is available in the serial buffer
    Message m = wait_for_message();
    if(!handle_handshake(m,ID)){

    int death_value = atoi(m.content); // read the value from the serial buffer
    int brightness = map(death_value, 0, 1023, 0, 255); // map the death values (max 1023 atm) to a brightness level between 0-255
    
    for (int i = 0; i <= NUM_LEDS; i++) { // turn the RGB strip red with the mapped brightness level
      leds[i] = CRGB(brightness, 0, 0);
      FastLED.show();
    }
    delay(5000); // let RGBs shine for 5 seconds
    
    for (int i = 0; i <= NUM_LEDS; i++) { // turn off the RGB strip
      leds[i] = CRGB(0, 0, 0);
      FastLED.show();
    }
    } else { 
      blinkstate = !blinkstate;
      digitalWrite(LED_BUILTIN, blinkstate);
    }
    
    
   
  }
}
