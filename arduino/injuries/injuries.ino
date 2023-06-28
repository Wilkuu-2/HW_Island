#include <FastLED.h>
#include <comm.h>
#define LED_PIN_DROUGHTS 9
#define LED_PIN_INJURIES 10

#define NUM_LEDS_INJURIES 180
#define NUM_LEDS_DROUGHTS 14

const int max_value = 255;
const int min_value = 0;
unsigned long start_time = 0;
unsigned long current_time = 0;
const unsigned long stop_interval = 10000;

CRGB injuries_leds[NUM_LEDS_INJURIES];
//CRGB droughts_leds[NUM_LEDS_DROUGHTS];

int injury_value = 0;
int drought_value = 0;

bool injury_changed = false;
bool drought_changed = false;

int injury_skip[] = {100, 200, 300};
int iskip_len = 3;

void display_led(int nr_of_leds, CRGB b[], int red_col, int green_col, int blue_col) {
  for (int i = 0; i <= nr_of_leds; i++) {  // turn the number of LEDs equivalent to the number of injuries white
    b[i] = CRGB(red_col, green_col, blue_col);
  }
  FastLED.show();
}
void display_led_skip(int nr_of_leds, CRGB b[], int red_col, int green_col, int blue_col) {
  for (int i = 0; i <= nr_of_leds; i++) {  // turn the number of LEDs equivalent to the number of injuries white
    CRGB col = CRGB(red_col, green_col, blue_col);

    for (int j = 0; j < iskip_len; j++) {
      if (i == injury_skip[j]) {
        col = CRGB(0, 0, 0);
        nr_of_leds++;
        break;
      }

    }

    b[i] = col;
  }

  FastLED.show();
}


void setup() {
  FastLED.addLeds<WS2812, LED_PIN_INJURIES, GRB>(injuries_leds, NUM_LEDS_INJURIES);
  //FastLED.addLeds<WS2812, LED_PIN_DROUGHTS, GRB>(droughts_leds, NUM_LEDS_DROUGHTS);
  start_time = millis();
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(9600);
}

#define ID "leds"
bool indicator = false;
int flicker_number = 150;
unsigned long previousMillis = 0;
static bool flickering = false;

void loop() {
  //int delay_time_drought = random(10, 30);
  unsigned long currentMillis = millis();

  Message m = {0};
  if (Serial.available() > 0) {
    m = wait_for_message();
    if (handle_handshake(m, ID)) {
      return;
    }

    switch (m.label) {
      case 'a':
        injury_value = atoi(m.content);
        injury_changed = true;
        break;
      case 'b':
        drought_value = atoi(m.content);
        drought_changed = true;
        break;

      default:
        break;
    }
    int injury_index = int(map(injury_value, 0, 1023, 0, NUM_LEDS_INJURIES));  // map the values 0-1023 to an index value to be used for certain number of LEDs
    Serial.print(injury_index);
    display_led(NUM_LEDS_INJURIES, injuries_leds, max_value, max_value, max_value);



    // int injury_value = Serial.parseInt();  // read the value from the serial buffer
    // int death_value = Serial.parseInt();
    // int drought_value = Serial.parseInt();
  }
  //int greenValue = map(drought_value, 0, 1, 255, 60);  // 0 or 1 as input values are mapped to red-green spectrum
  // int redValue = map(drought_value, 0, 1, 0, 255);


//  if (drought_changed && drought_value > 0 && !flickering) {
//    flickering = true;
//    drought_changed = false;
//    flicker_number = 150; // Restore number
//    previousMillis = currentMillis;
//  }
//
//  if (!flickering) {
//    //display_led(NUM_LEDS_DROUGHTS, droughts_leds, min_value, max_value, min_value);
//  }
//
//  if (flickering) {
//    if (currentMillis - previousMillis >= delay_time_drought) { // Delay
//      previousMillis = currentMillis; // Time measure
//      flicker_number--; // Limit
//      // Serial.println(flicker_number);
//      // Display
//      display_led(NUM_LEDS_DROUGHTS, droughts_leds, random(max_value - 30, max_value + 30), random(greenValue - 35, greenValue + 5), 0);
//      if (flicker_number <= 0) { // Limit check
//        flickering = false; // Disable
//
//        display_led(NUM_LEDS_DROUGHTS, droughts_leds, min_value, max_value, min_value);
//      }
//    }
//  }

  if (m.label != 0x0) {
    send_message(m);
  }

  indicator = !indicator;
  digitalWrite(LED_BUILTIN, indicator);
  delay(100);

}
