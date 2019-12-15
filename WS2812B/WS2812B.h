/** @file
 * Animation controller for WS2812B addressable LEDs.
 *
 * Based on FastLED "100-lines-of-code" demo reel by Mark Kriegsman, December 2014:
 * https://github.com/FastLED/FastLED/blob/master/examples/DemoReel100/DemoReel100.ino
 *
 *
 * Edited/extended by Michael Beaton, December 2019:
 * - Implemented modes:
 *   - ::Static
 *   - ::MonochromeAnimated
 *   - ::PaletteAnimated
 *   - ::Animated
 *   - ::Auto
 * - Accepts input from potentiometer to change brightness.
 * - Accepts input from 2 buttons to change modes and choose colors/animations.
 * - Buttons and potentiometer can be combined to change animation speed and color temperature.
 */
#ifndef WS2812B_H
#define WS2812B_H
#include <stdint.h>
#include "input-buttons.cpp"
#include "input-potentiometer.cpp"

/**
 * Must be equal to number of values in ::Mode
 */
#define NUM_MODES 4
enum Mode: uint8_t {
  Static = 0,              ///< All lights same color, no animation
  MonochromeAnimated = 1,  ///< All lights same base hue with animation
  PaletteAnimated = 2,     ///< Same as MonochromeAnimated but using predefined sets of colors
  Animated = 3,            ///< Animations with any color,
  // Auto = 4,                ///< Periodically change between any of the above, with any set of options.
};



void setupInputHandlers(void);
void updateInputHandlers(void);

void nextPattern(void);
void nextMonochromePattern(void);
void nextStaticColor(void);
void nextMode(void);
void nextAuto(void);      ///< Randomly choose a new mode, color, pattern, or animation type.

// Animations with any color
void animatePowerOn(void);

// Solid, non animated color - all lights are the same
void fillStaticColor(void);
CRGB getCurrentColor(void);
void blendColors(CRGB currentColor, CRGB nextColor, int speed = 30);

class ModeButtonHandler: public AbstractButtonHandler {
  public:
  ModeButtonHandler(uint8_t pin): AbstractButtonHandler(pin) {}
  void onButtonPressed(void);
  void onLongPress(void);
};

class OptionButtonHandler: public AbstractButtonHandler {
  public:
  OptionButtonHandler(uint8_t pin): AbstractButtonHandler(pin) {}
  void onButtonPressed(void);
  void onLongPress(void);
};

class BrightnessPotentiometerHandler: public AbstractPotentiometerHandler {
  public:
  BrightnessPotentiometerHandler(uint8_t pin): AbstractPotentiometerHandler(pin) {}
  void onValueChanged(int value);
  void onValueChangedNoModifier(int value);       ///< Called when the pot is turned with no other inputs
  void onValueChangedWithModeButton(int value);   ///< Called when the pot is turned while the Mode button is held down
  void onValueChangedWithOptionButton(int value); ///< Called when the pot is turned while the Option button is held down
};

#endif
