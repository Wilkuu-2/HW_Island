/*
   Created by ArduinoGetStarted.com

   This example code is in the public domain

   Tutorial page: https://arduinogetstarted.com/tutorials/arduino-rfid-nfc
*/

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

void loop() {
  // read the state of the pushbutton value:
  buttonState = digitalRead(2);

  if (buttonState == LOW) {
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
  Serial.print("a");//Selected decade
  Serial.println(map(potentioVaulueDecades, 0, 1023, -0.49, 5.49));
}

void disasterSelect() {
  int sliderValueDisaster = analogRead(A1); //Seleccted disaster
  Serial.print("b"); //Type of Disaster
//  Serial.println(sliderValueDisaster);
  Serial.println(map(sliderValueDisaster, 582, 667, -0.49, 2.49));
}

void countrySelect() {
  //RFID TAG to select country
  if (rfid.PICC_IsNewCardPresent()) { // new tag is available
    if (rfid.PICC_ReadCardSerial()) { // NUID has been readed
      MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);
      //Serial.print("RFID/NFC Tag Type: ");
      //Serial.println(rfid.PICC_GetTypeName(piccType));

      // print NUID in Serial Monitor in the hex format
      Serial.print("UID:");
      for (int i = 0; i < rfid.uid.size; i++) {
        Serial.print(rfid.uid.uidByte[i] < 0x10 ? " 0" : " ");
        Serial.print(rfid.uid.uidByte[i], HEX);
      }
      Serial.println();

      rfid.PICC_HaltA(); // halt PICC
      rfid.PCD_StopCrypto1(); // stop encryption on PCD
    }
  }
}
