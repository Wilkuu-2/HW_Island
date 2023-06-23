/* Hello Wokwi! */
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 20, 4);
void setup() {
lcd.init();
lcd.backlight();
}
void loop() {
lcd.setCursor(0, 0);
lcd.print("Temperature:");
lcd.setCursor(0,1);
lcd.print("46 graden");
}
