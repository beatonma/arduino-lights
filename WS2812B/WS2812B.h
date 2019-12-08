#ifndef WS2812B_H
#define WS2812B_H
#include "input-buttons.cpp"
#include "input-potentiometer.cpp"

enum Mode: byte {
  Static = 0,              // All lights same color, no animation
  MonochromeAnimated = 1,  // All lights same base hue with animation
  Animated = 2,            // Animations with any color
};

void setupInputHandlers();
void updateInputHandlers();

void nextPattern();
void nextMonochromePattern();
void nextStaticColor();
void nextMode();

// Animations with any color
void animatePowerOn();
void animationAddGlitter(fract8 chanceOfGlitter);
void animationBpm();
void animationConfetti();
void animationJuggle();
void animationRainbow();
void animationRainbowWithGlitter();
void animationSinelon();

// Animatinos that use a single color
void monochromeJuggle();

// Solid, non animated color - all lights are the same
void fillStaticColor();
CRGB getCurrentColor();
void blendColors(CRGB currentColor, CRGB nextColor, int speed = 30);

class ModeButtonHandler: public AbstractButtonHandler {
  public:
  ModeButtonHandler(byte pin): AbstractButtonHandler(pin) {}
  void onButtonPressed();
};

class OptionButtonHandler: public AbstractButtonHandler {
  public:
  OptionButtonHandler(byte pin): AbstractButtonHandler(pin) {}
  void onButtonPressed();
  void onLongPress();
};

class BrightnessPotentiometerHandler: public AbstractPotentiometerHandler {
  public:
  BrightnessPotentiometerHandler(byte pin): AbstractPotentiometerHandler(pin) {}
  void onValueChanged(int value);
};

#endif
