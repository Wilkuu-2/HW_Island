const int analogOutPin = 9;

int outputValue = 0;

void setup() {

pinMode(analogOutPin, OUTPUT);
}

void loop() {

outputValue = map(1023, 0, 1023, 0, 255);

analogWrite(analogOutPin, outputValue);

delay(2);
}
