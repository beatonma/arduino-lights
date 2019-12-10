#include "input.h"
#include <Arduino.h>

enum ButtonAction : byte
{
  None = 0,
  Press = 1,
  LongPress = 2,
};

class AbstractButtonHandler: AbstractInputHandler
{
public:
  // How long (milliseconds) does a button need to be pressed to count as a press)
  static const int MIN_PRESS_DURATION = 50;

  // How long (milliseconds) does a button need to be pressed to count as a long press
  static const int LONG_PRESS_DURATION = 600;

  AbstractButtonHandler(byte pin): AbstractInputHandler(pin)
  {

  }

  // Triggered when button is released after being pressed for
  // a duration between MIN_PRESS_DURATION and LONG_PRESS_DURATION
  // i.e. a simple press and release
  virtual void onButtonPressed(void) = 0;

  // Triggered once when the button is held down for LONG_PRESS_DURATION
  virtual void onLongPress(void) {}

  // Triggered before onButtonDown and onButtonUp
  virtual void onButtonToggle(void) {}

  // Triggered once when the button is pressed down
  virtual void onButtonDown(void) {}

  // Triggered once when the button is released
  virtual void onButtonUp(void) {}

  // Triggered continuously for as long as the button is held after LONG_PRESS_DURATION
  virtual void onLongPressHeld(void) {}

  bool isDown(void) const {
    return action_started_timestamp_ != 0;
  }

  // Called in arduino setup() function
  void setup(void)
  {
    pinMode(pin_, INPUT);
    digitalWrite(pin_, HIGH);
  }

  // Called in arduino loop() function
  void update(void)
  {
    current_value_ = !digitalRead(pin_);
    current_timestamp_ = millis();

    if (current_value_ != previous_value_)
    {
      onButtonToggle();
      if (current_value_ == true)
      {
        action_started_timestamp_ = millis();
        onButtonDown();
      }
      else if (current_value_ == false)
      {
        if (action_ == ButtonAction::Press)
        {
          onButtonPressed();
        }
        onButtonUp();
        action_ = ButtonAction::None;
        action_started_timestamp_ = 0;
      }
    }
    else if (action_started_timestamp_ > 0)
    {
      if (current_timestamp_ > action_started_timestamp_ + LONG_PRESS_DURATION)
      {
        if (action_ == ButtonAction::None || action_ == ButtonAction::Press)
        {
          onLongPress();
          action_ = ButtonAction::LongPress;
        }
        else
        {
          onLongPressHeld();
        }
      }
      else if (current_timestamp_ > action_started_timestamp_ + MIN_PRESS_DURATION)
      {
        action_ = ButtonAction::Press;
      }
    }

    previous_value_ = current_value_;
    current_timestamp_ = 0;
  }

protected:
  byte action_ = ButtonAction::None;
  bool previous_value_ = false;
  bool current_value_ = false;

  // When did the current button press start
  unsigned long action_started_timestamp_ = 0;
  unsigned long current_timestamp_ = 0;
};
