#ifndef WS2812B_H
#define WS2812B_H
#include "input-buttons.cpp"

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

void rainbow();
void rainbowWithGlitter();
void addGlitter(fract8 chanceOfGlitter);
void confetti();
void sinelon();
void bpm();
void staticColor();
void juggle();

class ModeButtonHandler: public AbstractButtonHandler {
  public:
  ModeButtonHandler(byte pin): AbstractButtonHandler(pin) {}
  void onButtonPressed();
  void onLongPress();
  void onLongPressHeld();
};
#endif
