#include <FastLED.h>
#include <comm.h>
#define LED_PIN_DEATHS 10
#define NUM_LEDS_DEATHS 150



#define ID "deaths"
bool deaths_changed = false;
int death_value = 0;
bool indicator = qfalse;
const int max_value = 255;
const int min_value = 0;
unsigned long start_time = 0;
unsigned long current_time = 0;
const unsigned long stop_interval = 10000;
CRGB deaths_leds[NUM_LEDS_DEATHS];


void display_led(int nr_of_leds, CRGB b[], int red_col, int green_col, int blue_col) {
  for (int i = 0; i <= nr_of_leds; i++) {  // turn the number of LEDs equivalent to the number of injuries white
    b[i] = CRGB(red_col, green_col, blue_col);
  }
  FastLED.show();
}

void setup() {
  // put your setup code here, to run once:
  FastLED.addLeds<WS2812, LED_PIN_DEATHS, GRB>(deaths_leds, NUM_LEDS_DEATHS);
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(9600);
  display_led(NUM_LEDS_DEATHS , deaths_leds, 255,0,0);

}

void loop() {
  // put your main code here, to run repeatedly:

  Message m = {0};
  if (Serial.available() > 0) {
    m = wait_for_message();

    if (handle_handshake(m, ID)) {
      return;
    }

    switch (m.label) {
      case 'a':
        death_value = atoi(m.content);
        int brightness = map(death_value, 0, 1023, 0, 255);
        display_led(NUM_LEDS_DEATHS, deaths_leds, 255,255 -brightness, 255 - brightness);
        break;

      default:
        break;
    }
  }
  int brightness = map(death_value, 0, 1023, 0, 255);
  //if (deaths_changed)
  //display_led(NUM_LEDS_DEATHS, deaths_leds, 255,255 -brightness, 255 - brightness);

  //display_led(NUM_LEDS_DEATHS , deaths_leds, 255,0,0);
  
  if (m.label != 0x0) {
    send_message(m);
  }

  indicator = !indicator;
  digitalWrite(LED_BUILTIN, indicator);
  delay(10);
}
