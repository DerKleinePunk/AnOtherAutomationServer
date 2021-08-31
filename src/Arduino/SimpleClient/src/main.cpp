#include <Arduino.h>

String input;
bool stringComplete = false;

void setup() {
  Serial.begin(9600);
  input.reserve(200); 
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
  if (stringComplete) {
    Serial.print("Ich habe erhalten:");
    Serial.println(input);
    // clear the string:
    input = "";
    stringComplete = false;
  }
}

void serialEvent() {
  while (Serial.available()) {
    digitalWrite(LED_BUILTIN, HIGH);
    // get the new byte:
    char inChar = (char)Serial.read();
    // add it to the inputString:
    input += inChar;
    // if the incoming character is a newline, set a flag so the main loop can
    // do something about it:
    if (inChar == '\n') {
      stringComplete = true;
    }
  }
  digitalWrite(LED_BUILTIN, LOW);
}