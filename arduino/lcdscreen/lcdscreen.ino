#include <comm.h>
#include <LiquidCrystal_I2C.h>


bool blinkstate = 0;
String name_and_temperature = " ";
LiquidCrystal_I2C lcd(0x27, 20, 4);

void setup() {
lcd.init();
lcd.backlight();
lcd.setCursor(0, 0);
lcd.print("The average");
lcd.setCursor(0,1);
lcd.print("temp is ");
Serial.begin(9600);

}
void loop() {
  if(Serial.available()){
    Message m  = wait_for_message();

    // Respond to the handshakes 
    if(!handle_handshake(m,"ledscreen")){
      name_and_temperature = m.content;
      lcd.setCursor(0,1);
      lcd.print("temp is " + name_and_temperature);
      send_message(m);
    }
  } else {
    blinkstate = !blinkstate;
    digitalWrite(LED_BUILTIN, blinkstate);
    delay(200);  
  }
}
