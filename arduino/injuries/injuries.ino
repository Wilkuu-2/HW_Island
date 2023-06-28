#include <FastLED.h>
#include <comm.h>
#define LED_PIN_DROUGHTS 11
#define LED_PIN_INJURIES 10
#define LED_PIN_DEATHS 9
#define NUM_LEDS_DEATH 15
#define NUM_LEDS_INJURIES 44
#define NUM_LEDS_DROUGHTS 15

const int max_value = 255;
const int min_value = 0;
unsigned long start_time = 0;
unsigned long current_time = 0;
const unsigned long stop_interval = 10000;

CRGB injuries_leds[NUM_LEDS_INJURIES];
CRGB deaths_leds[NUM_LEDS_INJURIES];
CRGB droughts_leds[NUM_LEDS_DROUGHTS];

int injury_value = 0;
int death_value = 0;
int drought_value = 0;

int injury_skip[] = {100,200,300}; 
int iskip_len = 3 

void setup() {
  FastLED.addLeds<WS2812, LED_PIN_INJURIES, GRB>(injuries_leds, NUM_LEDS_INJURIES);
  FastLED.addLeds<WS2812, LED_PIN_DEATHS, GRB>(deaths_leds, NUM_LEDS_DEATH);
  FastLED.addLeds<WS2812, LED_PIN_DROUGHTS, GRB>(droughts_leds, NUM_LEDS_DROUGHTS);
  start_time = millis();
  Serial.begin(9600);
}

#define ID = "leds" 

void loop() {
  int delay_time_drought = random(10, 30);
  int flicker_number = 150;
  static unsigned long previousMillis = 0;
  static bool flickering = false;
  unsigned long currentMillis = millis();
  if (!flickering) {
    display_led(NUM_LEDS_DROUGHTS, drougts_leds, min_value, max_value, min_value);
  }
  if (Serial.available() > 0) {
    Message m = wait_for_message()
    if (handle_handshake(m,ID))
        return; // check if data is available in the serial buffer
    
    switch (m.label){
      case 'a':
         injury_value = atoi(m.content);
      break;

      case 'b':
         death_value = atoi(m.content);
      break;

      case 'c': 
         drought_value = atoi(m.content);
      break; 

      default:

    }

    // int injury_value = Serial.parseInt();  // read the value from the serial buffer
    // int death_value = Serial.parseInt();
    // int drought_value = Serial.parseInt();

    int greenValue = map(drought_value, 0, 1, defaultGreen, 60);  // 0 or 1 as input values are mapped to red-green spectrum
    int redValue = map(drought_value, 0, 1, 0, maxRed);
    int injury_index = map(injury_value, 0, 1023, 0, NUM_LEDS_INJURIES);  // map the values 0-1023 to an index value to be used for certain number of LEDs
    int brightness = map(death_value, 0, 1023, 0, 255);
    if (drought_value > 0 && !flickering) {
      flickering = true;
      previousMillis = currentMillis;
    }
    display_led(NUM_LEDS_DEATHS, deaths_leds, brightness, min_value, min_value)
    display_led(injury_index, injuries_leds, max_value, max_value, max_value);
    if (flickering) {
      if (currentMillis - previousMillis >= delayTime) {
        previousMillis = currentMillis;
        flickerNumber--;
        display_led(NUM_LEDS_DROUGHTS, drougts_leds, random(max_value - 30, max_value + 30), random(greenValue - 35, greenValue + 5), 0);
        if (flickerNumber <= 0) {
          flickering = false;
          flickerNumber = 150;
          display_led(NUM_LEDS_DROUGHTS, drougts_leds, min_value, max_value, min_value);
        }
      }
    }
  }
}

void display_led(int nr_of_leds, CRGB b[], int red_col, int green_col, int blue_col) {
  for (int i = 0; i <= nr_of_leds; i++) {  // turn the number of LEDs equivalent to the number of injuries white
    b[i] = CRGB(red_col, green_col, blue_col);
  }
  FastLED.show();
}
void display_led_skip(int nr_of_leds, CRGB b[], int red_col, int green_col, int blue_col){
  for (int i = 0; i <= nr_of_leds; i++) {  // turn the number of LEDs equivalent to the number of injuries white
    CRGB col = CRGB(red_col, green_col, blue_col)
    
    for (int j = 0; j < iskip_len; j++){
        if (i == injuries_skip[j]){
           col = CRGB(0,0,0)
           break
        }
        
    } 
    
    b[i] = col;
  }

  FastLED.show();
}
