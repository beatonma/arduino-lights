#include <Arduino.h>

enum ButtonAction: byte{
  None = 0,
  Press = 1,
  LongPress = 2,
};

class AbstractButtonHandler {
protected:
  bool previous_value_ = false;
  bool current_value_ = false;
  bool consumed_value_ = false;

  // When did the current button press start
  unsigned long action_started_timestamp_ = 0;
  unsigned long current_timestamp_ = 0;

  void debug(String message) {
    Serial.print("[");
    Serial.print(pin_);
    Serial.print("] ");
    Serial.println(message);
  }

  void debug(String message, int value) {
    Serial.print("[");
    Serial.print(pin_);
    Serial.print("] ");
    Serial.print(message);
    Serial.println(value);
  }

public:
  // How long (milliseconds) does a button need to be pressed to count as a press)
  static const int MIN_PRESS_DURATION = 50;

  // How long (milliseconds) does a button need to be pressed to count as a long press
  static const int LONG_PRESS_DURATION = 600;

  byte pin_;
  const bool& pressed_ = current_value_;
  byte action_ = ButtonAction::None;

  AbstractButtonHandler(byte pin) {
      pin_ = pin;
  }

  // Triggered when button is released after being pressed for
  // a duration between MIN_PRESS_DURATION and LONG_PRESS_DURATION
  // i.e. a simple press and release
  virtual void onButtonPressed() = 0;

  // Triggered once when the button is held down for LONG_PRESS_DURATION
  virtual void onLongPress() {}

  // Triggered before onButtonDown and onButtonUp
  virtual void onButtonToggle() {}

  // Triggered once when the button is pressed down
  virtual void onButtonDown() {}

  // Triggered once when the button is released
  virtual void onButtonUp() {}

  // Triggered continuously for as long as the button is held after LONG_PRESS_DURATION
  virtual void onLongPressHeld() {}


  // Called in arduino setup() function
  void setup() {
    pinMode(pin_, INPUT);
    digitalWrite(pin_, HIGH);
  }

  // Called in arduino loop() function
  void update() {
    current_value_ = !digitalRead(pin_);
    current_timestamp_ = millis();

    if (current_value_ != previous_value_)  {
      onButtonToggle();
      if (current_value_ == true) {
        action_started_timestamp_ = millis();
        onButtonDown();
      }
      else if (current_value_ == false) {
        if (action_ == ButtonAction::Press) {
          onButtonPressed();
        }
        onButtonUp();
        action_ = ButtonAction::None;
        action_started_timestamp_ = 0;
      }
    }
    else if (action_started_timestamp_ > 0) {
      if (current_timestamp_ > action_started_timestamp_ + LONG_PRESS_DURATION) {
        if (action_ == ButtonAction::None || action_ == ButtonAction::Press) {
          onLongPress();
          action_ = ButtonAction::LongPress;
        }
        else {
          onLongPressHeld();
        }
      }
      else if (current_timestamp_ > action_started_timestamp_ + MIN_PRESS_DURATION) {
        action_ = ButtonAction::Press;
      }
    }

    previous_value_ = current_value_;
    current_timestamp_ = 0;
  }
};


// class ModeButtonHandler: public AbstractButtonHandler {
// public:
//   ModeButtonHandler(byte pin): AbstractButtonHandler(pin) {

//   }

//   void onButtonDown() {
//     debug("onButtonDown()");
//   }

//   void onButtonUp() {
//     debug("onButtonUp()");
//   }

//   void onLongPress() {
//     debug("onLongPress()");
//   }

//   void onButtonToggle() {
//     debug("onButtonToggle()");
//   }

//   void onButtonPressed() {
//     debug("onButtonPressed()");
//     Serial.println();
//   }

//   void onLongPressHeld() {

//   }
// };


