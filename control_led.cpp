#include "control_led.h"
#include <Arduino.h>

// Default constructor – pinler sabit 6,4,5
ControlLED::ControlLED() {
  _R = 6;
  _G = 4;
  _BLUE = 5;
  _SENSOR = 7;
  pinMode(_R, OUTPUT);
  pinMode(_G, OUTPUT);
  pinMode(_BLUE, OUTPUT);
  pinMode(_SENSOR, INPUT);
}

// Parametreli constructor – dışardan pin verilebilir
ControlLED::ControlLED(ControlLED::Leds *leds) {  // TAM İSİM GEREK!
  _R = leds->R;
  _G = leds->G;
  _BLUE = leds->B;
  _SENSOR = leds->S;
  Serial.printf("%d %d %d %d", _R, _G, _BLUE, _SENSOR);
  pinMode(_R, OUTPUT);
  pinMode(_G, OUTPUT);
  pinMode(_BLUE, OUTPUT);
}

// LED kontrol fonksiyonları
void ControlLED::allLedsOff() {
  digitalWrite(_R, LOW);
  digitalWrite(_G, LOW);
  digitalWrite(_BLUE, LOW);
}

void ControlLED::allLedsOn() {
  digitalWrite(_R, HIGH);
  digitalWrite(_G, HIGH);
  digitalWrite(_BLUE, HIGH);
}

void ControlLED::ledOn(int p) {
  allLedsOff();
  if (p == _R || p == _G || p == _BLUE) {
    digitalWrite(p, HIGH);
  }
}

void ControlLED::ledOff(int p) {
  digitalWrite(p, LOW);
}
// 5 ölçüm ortala
int ControlLED::readAvg() {
  int sum = 0;
  for (int i = 0; i < 10; i++) {
    sum += analogRead(_SENSOR);
    delay(2);
  }
  return sum / 10;
}
