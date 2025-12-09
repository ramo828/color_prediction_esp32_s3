/*
 * MIT License
 *
 * Copyright (c) 2025
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "control_led.h"
#include <Arduino.h>

// Defolt konstruktor – pinlər sabit 6,4,5
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

// Parametrli konstruktor – kənardan pin verilə bilər
ControlLED::ControlLED(ControlLED::Leds *leds) {  // TAM AD LAZIMDIR!
  _R = leds->R;
  _G = leds->G;
  _BLUE = leds->B;
  _SENSOR = leds->S;
  Serial.printf("%d %d %d %d", _R, _G, _BLUE, _SENSOR);
  pinMode(_R, OUTPUT);
  pinMode(_G, OUTPUT);
  pinMode(_BLUE, OUTPUT);
}

// LED idarəetmə funksiyaları
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
// 10 ölçmənin ortalamasını hesabla
int ControlLED::readAvg() {
  int sum = 0;
  for (int i = 0; i < 10; i++) {
    sum += analogRead(_SENSOR);
    delay(2);
  }
  return sum / 10;
}
