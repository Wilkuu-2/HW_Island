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

    // Remeber what the last tag was
byte last_uuid[8] = {0xde,0xad,0xbe,0xef,0x0,0x0,0x0,0x0}; // 64 bits max, our tags have 32 bits of output AFAIK
int last_len = 4;        // Keep the length as well 

int buttonState = 0;

void setup() {
  Serial.begin(9600); // init serial
  SPI.begin(); // init SPI bus
  rfid.PCD_Init(); // init MFRC522

  // LED 
  pinMode(8, OUTPUT);

  // initialize the pushbutton pin as an input:
  pinMode(2, INPUT);
  pinMode(A2, INPUT);
  pinMode(A1, INPUT);

}

#define ID "input" 

void loop() {
  // Read button value 
  buttonState = digitalRead(2);

  // Handle messages  
  if(Serial.available() > 0){
    Message m = wait_for_message();
    if (!handle_handshake(m, ID)){
      if(m.label == 'X'){
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

  // Read the tag when available
  // TODO: It might be beneficial for the reliability of this to wait here instead of just checking
  // REMEMBER: This should work without crashes for at least a few hours
  
  if (rfid.PICC_IsNewCardPresent()) { // new tag is available
    // TODO: See above 
    if (rfid.PICC_ReadCardSerial()) { // NUID has been read 
      Serial.println("Found card");
      // What does this do? {! Compiles without !}
      MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);

      // Copy over the tag uid
      last_len = rfid.uid.size; 
      memcpy(last_uuid,rfid.uid.uidByte, last_len);
    }
      rfid.PICC_HaltA(); // halt PICC
      rfid.PCD_StopCrypto1(); // stop encryption on PCD
   }

}

// Potentiometer
void decadeSelect() {
  int potentioVaulueDecades = analogRead(A1); //Selected decade
  //Serial.println(potentioVaulueDecades);
  send_message(int_message('a',int(floor(potentioVaulueDecades/1023.0 * 6.0))));
}

// Slider 
void disasterSelect() {
  int sliderValueDisaster = analogRead(A2); //Seleccted disaster
  Serial.println(sliderValueDisaster);
  int index = 0;
  if(sliderValueDisaster > 980){
    index = 2;
  }
  else if(sliderValueDisaster > 900){
    index = 1; 
  }
  send_message(int_message('b',index));
}

// RFID 
void countrySelect() {
   
   Message m = {0};
   m.label = 'c';
  
   for (int i = 0; i < last_len; i++) {
     sprintf(m.content + 3*i, "%02X ", last_uuid[i] ); // Prints a byte 
   }

   // Shorten the message by removing the last space 
   m.content[3*last_len-1] = 0x0;

   send_message(m);

 }
