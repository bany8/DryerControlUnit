#include <Arduino.h>

const int buttonPin1 = A3;
const int buttonPin2 = A2;
const int ledPin1 = A7;
const int ledPin2 = A6;
int buttonState1 = 0;
int buttonState2 = 0;

void setup() {
  Serial.begin(9600);
  pinMode(buttonPin1, INPUT_PULLUP);
  pinMode(ledPin1, OUTPUT);
  digitalWrite(ledPin1, LOW);

  pinMode(buttonPin2, INPUT_PULLUP);
  pinMode(ledPin2, OUTPUT);
  digitalWrite(ledPin2, LOW);
}

void loop() {
  buttonState1 = digitalRead(buttonPin1);
  buttonState2 = digitalRead(buttonPin2);

  if (buttonState1 == LOW) {
    Serial.println("Przycisk 1 wcisniety!");
    digitalWrite(ledPin1, HIGH);
  } else {
    digitalWrite(ledPin1, LOW);
  }

    if (buttonState2 == LOW) {
    Serial.println("Przycisk 2 wcisniety!");
    digitalWrite(ledPin2, HIGH);
  } else {
    digitalWrite(ledPin2, LOW);
  }

  delay(50);
}
