#ifndef WS2812B_H
#define WS2812B_H
#include "input-buttons.cpp"
#include "input-potentiometer.cpp"

#define NUM_MODES 3
enum Mode: byte {
  Static = 0,              // All lights same color, no animation
  MonochromeAnimated = 1,  // All lights same base hue with animation
  Animated = 2,            // Animations with any color
};

void setupInputHandlers(void);
void updateInputHandlers(void);

void nextPattern(void);
void nextMonochromePattern(void);
void nextStaticColor(void);
void nextMode(void);

// Animations with any color
void animatePowerOn(void);

// Solid, non animated color - all lights are the same
void fillStaticColor(void);
CRGB getCurrentColor(void);
void blendColors(CRGB currentColor, CRGB nextColor, int speed = 30);

class ModeButtonHandler: public AbstractButtonHandler {
  public:
  ModeButtonHandler(byte pin): AbstractButtonHandler(pin) {}
  void onButtonPressed(void);
};

class OptionButtonHandler: public AbstractButtonHandler {
  public:
  OptionButtonHandler(byte pin): AbstractButtonHandler(pin) {}
  void onButtonPressed(void);
  void onLongPress(void);
};

class BrightnessPotentiometerHandler: public AbstractPotentiometerHandler {
  public:
  BrightnessPotentiometerHandler(byte pin): AbstractPotentiometerHandler(pin) {}
  void onValueChanged(int value);
};

#endif
