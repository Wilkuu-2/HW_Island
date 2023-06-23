/*=======================================================*
 * Data selector
 * Uses multiple types of inputs to determine which data will be physicalized:
        a. RFID: Country
        b. Slider: Disaster 
        c. Potentiometer: Year
        -: Push button: Start
 *
 * -- uses comm.cpp as it's protocol 
 * -- discovery id: input
 *
 * Copyright 2023, Group Tantor (CreaTe M8 2023)
 * You may use the code under the terms of the MIT license
 *=======================================================*/

#include <comm.h>
#include <SPI.h>
#include <MFRC522.h>

#define SS_PIN 10
#define RST_PIN 5

MFRC522 rfid(SS_PIN, RST_PIN);

int buttonState = 0;

void setup() {
  Serial.begin(9600); // init serial
  SPI.begin(); // init SPI bus
  rfid.PCD_Init(); // init MFRC522

  // LED 
  pinMode(8, OUTPUT);

  // initialize the pushbutton pin as an input:
  pinMode(2, INPUT);

}

#define ID "input" 

void loop() {
  // Read button value 
  buttonState = digitalRead(2);

  // Handle messages  
  if(Serial.available() > 1){
    Message m = wait_for_message();
    if (!handle_handshake(m, ID)){
      if(m.label = 'X'){
         buttonState = atoi(m.content) > 0;
      }
    }
  }

  // On activation 
  if (buttonState == LOW) {
    digitalWrite(8, HIGH);

    // Sense and send inputs 
    countrySelect();
    decadeSelect();
    disasterSelect();
    
    delay(1000); // Cooldown 
    buttonState = 0;
  }

  // After activation
  else if (buttonState == HIGH) {
    digitalWrite(8, LOW);
  }
}

// Potentiometer
void decadeSelect() {
  int potentioVaulueDecades = analogRead(A2); //Selected decade
  send_message(int_message('a',map(potentioVaulueDecades, 0, 1023, -0.49, 5.49)));
}

// Slider 
void disasterSelect() {
  int sliderValueDisaster = analogRead(A1); //Seleccted disaster
  send_message(int_message('b',map(sliderValueDisaster, 582, 667, -0.49, 2.49)));
}

// RFID 
void countrySelect() {

  // Remeber what the last tag was
  static byte last_uuid[8] = {0}; // 64 bits max, our tags have 32 bits of output AFAIK
  static int last_len = 0;        // Keep the length as well 

  // Read the tag when available
  // TODO: It might be beneficial for the reliability of this to wait here instead of just checking
  // REMEMBER: This should work without crashes for at least a few hours
  
  if (rfid.PICC_IsNewCardPresent()) { // new tag is available
    // TODO: See above 
    if (rfid.PICC_ReadCardSerial()) { // NUID has been read 

      // What does this do? {! Compiles without !}
      MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);

      // Copy over the tag uid
      last_len = rfid.uid.size; 
      memcpy(last_uuid,rfid.uid.uidByte, last_len);
    }
   }
   
   Message m = {0};
   m.label = 'c';
  
   for (int i = 0; i < last_len; i++) {
     sprintf(m.content + 3*i, "%02X ", last_uuid[i] ); // Prints a byte 
   }

   // Shorten the message by removing the last space 
   m.content[3*last_len-1] = 0x0;

   send_message(m);

   // TODO: Determine if this halting and stopping has to be done periodically
   //       Do we need to do this every time a new card is detected?
   //       It might be beneficial to read the card state in loop, since we already store state (semi)globally 
   
   rfid.PICC_HaltA(); // halt PICC
   rfid.PCD_StopCrypto1(); // stop encryption on PCD
 }
