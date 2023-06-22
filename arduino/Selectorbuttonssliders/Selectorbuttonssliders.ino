/*
   Created by ArduinoGetStarted.com

   This example code is in the public domain

   Tutorial page: https://arduinogetstarted.com/tutorials/arduino-rfid-nfc
*/
#include "libs/comm.cpp"
#include <SPI.h>
#include <MFRC522.h>

#define SS_PIN 10
#define RST_PIN 5

MFRC522 rfid(SS_PIN, RST_PIN);

int buttonState = 0;

void setup() {
  Serial.begin(9600);
  SPI.begin(); // init SPI bus
  rfid.PCD_Init(); // init MFRC522
  pinMode(8, OUTPUT);

  // initialize the pushbutton pin as an input:
  pinMode(2, INPUT);

  Serial.println("Tap RFID/NFC Tag on reader");
}

#define ID "input" 

void loop() {
  // read the state of the pushbutton value:
  buttonState = digitalRead(2);

  if(Serial.available()){
    Message m = wait_for_message();
    handle_handshake(m, ID);
  }
  else if (buttonState == LOW) {
    digitalWrite(8, HIGH);
    
    countrySelect();
    decadeSelect();
    disasterSelect();
    
    delay(1000);
    buttonState = 0;
  }
  else if (buttonState == HIGH) {
    digitalWrite(8, LOW);
  }
}

void decadeSelect() {
  int potentioVaulueDecades = analogRead(A2); //Selected decade
  send_message(int_message('a',map(potentioVaulueDecades, 0, 1023, -0.49, 5.49)));
}

void disasterSelect() {
  int sliderValueDisaster = analogRead(A1); //Seleccted disaster
  send_message(int_message('b',map(sliderValueDisaster, 582, 667, -0.49, 2.49)));
}

void countrySelect() {
  //RFID TAG to select country
  static byte last_uuid[64] = {0};
  static int last_len = 0;
  
  if (rfid.PICC_IsNewCardPresent()) { // new tag is available
    if (rfid.PICC_ReadCardSerial()) { // NUID has been readed
      MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);
      last_len = rfid.uid.size; 
      memcpy(last_uuid,rfid.uid.uidByte, last_len);
    }
   }
   
   Message m = {0};
   m.label = 'c';
  
   for (int i = 0; i < last_len; i++) {
     sprintf(m.content + 3*i, " %02X", last_uuid );
   }
  

   rfid.PICC_HaltA(); // halt PICC
   rfid.PCD_StopCrypto1(); // stop encryption on PCD
 }
