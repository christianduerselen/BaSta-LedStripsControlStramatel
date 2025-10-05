#include "BasketballLedControl.h"

BasketballLedControl::BasketballLedControl(int controlPin, long timeout, long delay)
{
	_pin = controlPin;
  _timeout = timeout;
  _delay = delay;

  pinMode(_pin, OUTPUT);
  applyState();
};

bool BasketballLedControl::getState()
{
  return _state;
}

void BasketballLedControl::setState(bool state, bool force)
{
  _state = state;
  _startStamp = millis() + (force ? 0 : _delay);
  _stopStamp = _state ? _startStamp + _timeout : 0;

  updateState();
}

void BasketballLedControl::updateState()
{
  long currentStamp = millis();

  if (_startStamp != 0 && currentStamp >= _startStamp)
  {
    _startStamp = 0;
    applyState();
  }

  if (_stopStamp != 0 && currentStamp >= _stopStamp)
  {
    _stopStamp = 0;
    _state = false;
    applyState();
  }
}

void BasketballLedControl::applyState()
{
  digitalWrite(_pin, _state ? HIGH : LOW);
}