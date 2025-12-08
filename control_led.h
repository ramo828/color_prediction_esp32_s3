#ifndef CONTROL_LED_H
#define CONTROL_LED_H

class ControlLED {
public:
  struct Leds {
    int R;
    int G;
    int B;
    int S;
  };

  ControlLED();
  ControlLED(Leds *leds);

  void allLedsOff();
  void allLedsOn();
  void ledOn(int pin);
  void ledOff(int pin);
  int readAvg();
private:
  int _R;
  int _G;
  int _BLUE;
  int _SENSOR;
};

#endif