#ifndef BasketballLedControl_h
#define BasketballLedControl_h

#include "Arduino.h"

class BasketballLedControl
{
  public:
    BasketballLedControl(int controlPin, long timeout, long delay);
    bool getState();
    void setState(bool state, bool force = false);
    void updateState();
  private:
    void applyState();
    int _pin;
    long _timeout = 0;
    long _delay = 0;
    bool _state = false;
    unsigned long _startStamp = 0;
    unsigned long _stopStamp = 0;
};

#endif