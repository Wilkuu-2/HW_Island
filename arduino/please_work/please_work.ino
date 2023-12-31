#include <comm.h>
#include <FastLED.h>
#define PIN_INJURIES 10
#define DROUGHT_PIN 9
#define NUM_LEDS_INJURIES 105
#define NUM_LEDS_DROUGHTS 14
//#define DEBUG // Debug flag
//---------------------------Variables------------------------------------------
int skipLights[] = {11, 12, 24, 25, 26, 38, 39, 51, 52, 53, 65, 66, 78, 79, 80};
int injury_value = 0;
bool blinkstate = 0;
bool drought_changed = false;
bool injuries_changed = false;
int drought_value = 0;
const int max_value = 255;
const int min_value = 0;
unsigned long start_time = 0;
unsigned long current_time = 0;
unsigned long previous_time = 0;
int  flicker_number = 150;
int green_value = 0;
int injury_index = 0;
int delay_time = 0;
//---------------------------LED STRIPS-----------------------------------------
CRGB injuries_leds[NUM_LEDS_INJURIES];
CRGB drought_leds[NUM_LEDS_DROUGHTS];

// -------------------------- COLORS--------------------------------------------
const CRGB green = CRGB(min_value, max_value, min_value);

void setup() {
  FastLED.addLeds<WS2812, PIN_INJURIES, GRB>(injuries_leds, NUM_LEDS_INJURIES);
  FastLED.addLeds<WS2812, DROUGHT_PIN, GRB>(drought_leds, NUM_LEDS_DROUGHTS);
  Serial.begin(9600);
  pinMode(LED_BUILTIN, OUTPUT);
}

#define ID "injuries"

bool skip_check(int i) {
  for (int j = 0; j < 15; j++) { //skip check
    if (skipLights[j] == i) {
      return true;
    }
  }
  return false;
}

void loop() {
  //Serial.println("Static green");

  if (Serial.available() > 0) {  // check if data is available in the serial buffer
    Message m = wait_for_message();
    if (!handle_handshake(m, ID)) {
      if (m.label == 'a') {
        injury_value = atoi(m.content);
        injury_index = int(map(injury_value, 0, 1023, 0, NUM_LEDS_INJURIES - 1)); // map the values 0-1023 to an index value to be used for certain number of LEDs
        injuries_changed = true;// Serial.println("Switch accessed-injury");
      }
      if (m.label == 'b') {
        // Serial.println(m.label);
        drought_changed = true;
        drought_value = atoi(m.content);
        green_value = map(drought_value, 0, 1, max_value, 60);
        int red_value = map(drought_value, 0, 1, 0, max_value);
      }
      send_message(m);
    }

  }


  if (drought_value > 0 ) {
    current_time = millis();
    delay_time = random(10, 30);

#ifdef DEBUG
    Serial.println("Current time");
    Serial.println(current_time);
    Serial.println("Previous time");
    Serial.println(previous_time);
    Serial.println("Delay time");
    Serial.println(delay_time);
    Serial.println("Flicker number");
    Serial.println(flicker_number);
#endif

    if (current_time - previous_time >= delay_time) {
      previous_time = current_time;

      //flicker_number = flicker_number - 1;

      // Display the flicker
      for (int i = 0; i <= NUM_LEDS_DROUGHTS; i++) {
        drought_leds[i] = CRGB(random(max_value - 60, max_value), random(green_value - 35, green_value + 5), 0);

      }
      FastLED.show();
      //      // Limiting is done through a message
      //      if (flicker_number <= 0) {
      //        // Serial.println("Shtastie");
      //        drought_value = 0;
      //        flicker_number = 150;
      //
      //        for (int i = 0; i <= NUM_LEDS_DROUGHTS; i++) {
      //          drought_leds[i] = CRGB(min_value, max_value, min_value);
      //        }
      //        FastLED.show();
      //      }
    }
  } else if (drought_changed) {  // Make the trees green when the drought is 0
    for (int i = 0; i <= NUM_LEDS_DROUGHTS; i++) {
      drought_leds[i] = CRGB(min_value, max_value, min_value);
    }
    FastLED.show();
  }

  if (injuries_changed) { //if there is a serial communication value input
    for (int i = 0; i <= injury_index; i++) {  // turn the number of LEDs equivalent to the number of injuries white
      // Serial.print("bro");
      injuries_leds[i] = CRGB(100, 100, 80);
      if (skip_check(i)) {
        injuries_leds[i] = CRGB(0, 0, 0);
        //injury_index ++;
      }

    }
    for (int i = injury_index + 1; i < NUM_LEDS_INJURIES; i++ ) {
      injuries_leds[i] = CRGB(0, 0, 0);
    }
    injuries_changed = false;
  }
  
  FastLED.show();

}
