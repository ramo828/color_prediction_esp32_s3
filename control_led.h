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

#ifndef CONTROL_LED_H
#define CONTROL_LED_H

class ControlLED {
public:
  struct Leds {
    int R; // Qırmızı LED pini
    int G; // Yaşıl LED pini
    int B; // Mavi LED pini
    int S; // Sensor pini
  };

  ControlLED(); // Defolt konstruktor
  ControlLED(Leds *leds); // Parametrli konstruktor

  void allLedsOff(); // Bütün LED-ləri söndür
  void allLedsOn(); // Bütün LED-ləri yandır
  void ledOn(int pin); // Müəyyən bir LED-i yandır
  void ledOff(int pin); // Müəyyən bir LED-i söndür
  int readAvg(); // Orta sensor dəyərini oxu
private:
  int _R;
  int _G;
  int _BLUE;
  int _SENSOR;
};

#endif