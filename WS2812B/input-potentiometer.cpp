#include "input.h"
#include <Arduino.h>

class AbstractPotentiometerHandler: AbstractInputHandler
{

public:
  // Minimum change between previous_value_ and current_value_ that is considered
  // to be a purposeful action and not just caused by noise in the circuit.
  static const int NOISE_FLOOR = 5;

  AbstractPotentiometerHandler(byte pin): AbstractInputHandler(pin)
  {

  }

  virtual void onValueChanged(int new_value) = 0;

  void setup() {

  }

  void update()
  {
    current_value_ = analogRead(pin_);
    if (current_value_ != previous_value_) {
      // TODO calculate running average to ignore signal noise
      onValueChanged(current_value_);
    }
    previous_value_ = current_value_;
  }

protected:
  int previous_value_ = 512;
  int current_value_ = 512;

  unsigned long action_started_timestamp_ = 0;
  unsigned long current_timestamp_ = 0;
};
