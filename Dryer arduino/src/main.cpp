#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <string.h>
#include <EEPROM.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1

const int button_4 = A3;
const int button_3 = A2;
const int button_2 = A1;
const int button_1 = A0;
const int outHeat = 2;
const int outFan = 3;
const int oneWireBus = 7;

bool outHeat_state = LOW;
bool outFan_state = LOW;

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

OneWire oneWire(oneWireBus);
DallasTemperature sensors(&oneWire);

int sensorTemp = 0;
int setTemp = 20;
int leftTime = 0;
int setTime = 90;
String state = "Init";

bool buttonPressed(int pin) {
  const unsigned long debounceDelay = 30;

  int index;
  switch (pin) {
    case A0: index = 0; break;
    case A1: index = 1; break;
    case A2: index = 2; break;
    case A3: index = 3; break;
    default: return false;
  }

  static bool stableLevel[4]      = {HIGH, HIGH, HIGH, HIGH}; 
  static bool lastReportedPress[4]= {false, false, false, false};
  static unsigned long lastChangeTime[4] = {0,0,0,0};
  static bool lastRawLevel[4]     = {HIGH, HIGH, HIGH, HIGH}; 

  bool raw = digitalRead(pin);
  unsigned long now = millis();

  if (raw != lastRawLevel[index]) {
    lastRawLevel[index] = raw;
    lastChangeTime[index] = now;
  }

  if ((now - lastChangeTime[index]) > debounceDelay) {
    if (raw != stableLevel[index]) {
      stableLevel[index] = raw;

      if (stableLevel[index] == LOW) {
        lastReportedPress[index] = true;
      } else {
        lastReportedPress[index] = false;
      }
    }
  }

  if (lastReportedPress[index]) {
    lastReportedPress[index] = false;
    return true;
  }

  return false;
}

void displayInit(){
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 7);
  display.print(setTime / 60);
  display.print(":");
  display.print(setTime % 60);
  display.print("m-");
  display.print(setTemp);
  display.println("C");

  display.setCursor(0, 38);
  display.print(leftTime / 60);
  display.print(":");
  display.print(leftTime % 60);
  display.print("m-");
  display.print(sensorTemp);
  display.println("C");

  display.setCursor(116, 0);
  display.println("C");
  display.setCursor(116, 15);
  display.println("T");
  display.setCursor(116, 45);
  display.println("S");

  display.display();
}

void displayTime(){
  display.clearDisplay();
  display.setTextSize(3);
  display.setCursor(0, 18);
  display.print(setTime / 60);
  display.print(":");
  display.print(setTime % 60);
  display.print("m ");

  display.setTextSize(2);
  display.setCursor(116, 0);
  display.println("^");
  display.setCursor(116, 15);
  display.println("v");
  display.setCursor(116, 30);
  display.println("G");
  display.setCursor(100, 45);
  display.println("OK");

  display.display();
}

void displayTemp(){
  display.clearDisplay();
  display.setTextSize(3);
  display.setCursor(0, 18);
  display.print(setTemp);
  display.print(" C");

  display.setTextSize(2);
  display.setCursor(116, 0);
  display.println("^");
  display.setCursor(116, 15);
  display.println("v");
  display.setCursor(100, 45);
  display.println("OK");

  display.display();
}

void displayWork(){
  static bool on = 1;
  static unsigned long tick = 0;

  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 7);
  display.print(setTime / 60);
  display.print(":");
  display.print(setTime % 60);
  display.print("m-");
  display.print(setTemp);
  display.println("C");

  display.setCursor(0, 38);
  display.print(leftTime / 60);

  if (millis() - tick > 700) {
    on = !on;
    tick = millis();
  }

  if (on){
    display.print(":");
  }else {
    display.print(" ");
  }

  display.print(leftTime % 60);
  display.print("m-");
  display.print(sensorTemp);
  display.println("C");

  display.setTextSize(3);
  display.setCursor(110, 40);

  if (on){
    display.print("X");
  }else {
    display.print(" ");
  }

  display.display();
}


void setup() {
  Serial.begin(9600);

  pinMode(button_1, INPUT_PULLUP);
  pinMode(button_2, INPUT_PULLUP);
  pinMode(button_3, INPUT_PULLUP);
  pinMode(button_4, INPUT_PULLUP);
  pinMode(outHeat, OUTPUT);
  pinMode(outFan, OUTPUT);
  digitalWrite(outFan, LOW);
  digitalWrite(outHeat, LOW);

  sensors.begin();

  EEPROM.get(0, setTemp);
  EEPROM.get(2, setTime);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("OLED initiation error!"));
    for (;;);
  }

  displayInit();
}

void loop() {
  static unsigned long lastTempRead = 0;
  static unsigned long lastMinuteTick = 0;
  static unsigned long lastMinuteTick2 = 0;
  static bool lastHeatState = 0;

  if (millis() - lastTempRead > 5000) {
    sensors.requestTemperatures();
    sensorTemp = sensors.getTempCByIndex(0);
    if (sensorTemp < 0) sensorTemp = 0;
    lastTempRead = millis();
  }

  if (state.equals("Init")) {
    digitalWrite(outFan, LOW);
    digitalWrite(outHeat, LOW);

    if (buttonPressed(button_1)) state = "Time";
    if (buttonPressed(button_2)) state = "Temp";
    if (buttonPressed(button_3) || buttonPressed(button_4)) {
      state = "Work";
      leftTime = setTime;
      lastMinuteTick = millis();
      lastMinuteTick2 = millis();
    }
    displayInit();
  }

  else if (state.equals("Time")) {
    if (buttonPressed(button_1)) {
      setTime += 1;
      if (setTime > 1440) setTime = 1440;
    }

    if (buttonPressed(button_2)) {
      setTime -= 1;
      if (setTime < 0) setTime = 0;
    }

    if (buttonPressed(button_3)) {
      setTime += 60;
      if (setTime > 1440) setTime = setTime - 1440;
    }

    if (buttonPressed(button_4)) {
      state = "Init";
      EEPROM.put(2, setTime);
    }

    displayTime();
  }

  else if (state.equals("Temp")) {

    if (buttonPressed(button_1)) {
      setTemp += 1;
      if (setTemp > 80) setTemp = 80;
    }

    if (buttonPressed(button_2)) {
      setTemp -= 1;
      if (setTemp < 20) setTemp = 20;
    }

    if (buttonPressed(button_3) || buttonPressed(button_4)) {
      state = "Init";
      EEPROM.put(0, setTemp);
    }
    displayTemp();
  }

  else if (state.equals("Work")) {
    if (sensorTemp < setTemp) {
      digitalWrite(outFan, HIGH);
      digitalWrite(outHeat, HIGH);
    } 
    else if (sensorTemp >= setTemp) {
      digitalWrite(outHeat, LOW);
    }
    
    if (millis() - lastMinuteTick > 60000) {
      leftTime -= 1;
      lastMinuteTick = millis();
    }

    if (digitalRead(outHeat) == 0 && lastHeatState == 1) lastMinuteTick2 = millis();

    if (millis() - lastMinuteTick2 > 60000 && digitalRead(outHeat) == 0) digitalWrite(outFan, LOW);
    
    if (leftTime <= 0){
      digitalWrite(outFan, LOW);
      digitalWrite(outHeat, LOW);
      state = "Init";
    }

    if (buttonPressed(button_3) || buttonPressed(button_4)) {
      digitalWrite(outFan, LOW);
      digitalWrite(outHeat, LOW);
      leftTime = 0;
      state = "Init";
    }
    lastHeatState = digitalRead(outHeat);
    displayWork();
  }
}