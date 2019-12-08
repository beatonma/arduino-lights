#define FASTLED_INTERNAL // Disable pragma version message on compilation
#include <Arduino.h>
#include <FastLED.h>
#include "WS2812B.h"
FASTLED_USING_NAMESPACE

// FastLED "100-lines-of-code" demo reel, showing just a few
// of the kinds of animation patterns you can quickly and easily
// compose using FastLED.
//
// This example also shows one easy way to define multiple
// animations patterns and have them automatically rotate.
//
// -Mark Kriegsman, December 2014
//
//
// Edited/extended by Michael Beaton, December 2019
// - Implemented modes:
//   - Static: stay on a single color
//   - MonochromeAnimated: enable animations but only using the color chosen in Static
//   - Animated: use the animations from the example
// - Accepts input from potentiometer to change brightness.
// - Accepts input from 2 buttons to change modes and choose colors/animations.
//

#if defined(FASTLED_VERSION) && (FASTLED_VERSION < 3001000)
#warning "Requires FastLED 3.1 or later; check github for latest code."
#endif

// Hardware configuration
#define LED_TYPE WS2811
#define COLOR_ORDER GRB
#define NUM_LEDS 150

#define DATA_PIN 4
#define BRIGHTNESS_POT_PIN 0
#define MODE_BUTTON_PIN 9
#define OPTION_BUTTON_PIN 8

#define MAX_BRIGHTNESS 255
#define MIN_BRIGHTNESS 5

#define NUM_MODES 3


// Available values for temperature/correction can be found at https://github.com/FastLED/FastLED/blob/master/color.h
#define COLOR_CORRECTION UncorrectedColor
#define COLOR_TEMPERATURE UncorrectedTemperature

#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))
#define PALETTEOF(A) CRGBPalette16(A, A, A, A, A, A, A, A, A, A, A, A, A, A, A, A)

CRGB leds[NUM_LEDS];

const byte framesPerSecond_ = 120;

byte mode_ = Mode::Static;
byte brightness_ = MAX_BRIGHTNESS;

byte pattern_index_ = 0;            // Index of current pattern
byte monochrome_pattern_index_ = 0; // Index of current monochrome pattern
byte static_color_index_ = 0;       // Index of current static color.
byte hue_ = 0;                      // rotating "base color" used by many of the patterns
CHSV static_color_hsv_;

// Input handlers
const ModeButtonHandler mode_button_handler_(MODE_BUTTON_PIN);
const OptionButtonHandler option_button_handler_(OPTION_BUTTON_PIN);
const BrightnessPotentiometerHandler brightness_potentiometer_handler_(BRIGHTNESS_POT_PIN);

// List of patterns to cycle through.  Each is defined as a separate function below.
typedef void (*SimplePatternList[])();
const SimplePatternList patterns_ = {
    animationRainbow,
    animationRainbowWithGlitter,
    animationConfetti,
    animationSinelon,
    animationJuggle,
    animationBpm,
};
const SimplePatternList monochrome_patterns_ = {
    monochromeJuggle,
    animationBpm,
};

// Available color definitions can be found at https://github.com/FastLED/FastLED/blob/master/pixeltypes.h
// TODO replace HTMLColorCode with palletes (via PALETTEOF) for each color so we can mix in 'themes' like cloud/lava/ocean: https://github.com/FastLED/FastLED/blob/master/colorpalettes.h
const CRGB::HTMLColorCode colors_[] = {
    CRGB::Purple,
    CRGB::White,
    CRGB::Red,
    CRGB::OrangeRed,
    CRGB::Yellow,
    CRGB::Green,
    CRGB::Cyan,
    CRGB::Blue,
};

void setup()
{
  Serial.begin(115200);
  Serial.println("Starting...");

  delay(3000); // 3 second delay for recovery

  setupInputHandlers();

  // tell FastLED about the LED strip configuration
  FastLED.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(COLOR_CORRECTION);
  FastLED.setTemperature(COLOR_TEMPERATURE);

  Serial.println("OK GO");

  animatePowerOn();

  // nextStaticColor();
}

void loop()
{
  updateInputHandlers();

  // EVERY_N_MILLISECONDS(20) {
  //   // slowly cycle the "base color" through the rainbow
  //   hue_++;
  // }
  // EVERY_N_SECONDS(10) {
  //   // change patterns periodically
  //   nextPattern();
  // }
  // EVERY_N_SECONDS(3) {
  //   nextStaticColor();
  // }
  // EVERY_N_SECONDS(5) {
  //   mode_ = !mode_;
  // }

  switch (mode_)
  {
  case Mode::Static:
    break;
  case Mode::MonochromeAnimated:
    monochrome_patterns_[monochrome_pattern_index_]();
    break;
  case Mode::Animated:
    hue_++;
    patterns_[pattern_index_]();
    break;
  }

  draw();
}

void draw()
{
  FastLED.setBrightness(brightness_);
  FastLED.show();
  FastLED.delay(1000 / framesPerSecond_);
}

void setupInputHandlers()
{
  mode_button_handler_.setup();
  option_button_handler_.setup();
}

void updateInputHandlers()
{
  // for (int i = 0; i < ARRAY_SIZE(input_handlers_); i++) {
  //   (*(input_handlers_[i])).update();
  // }
  brightness_potentiometer_handler_.update();
  mode_button_handler_.update();
  option_button_handler_.update();
}

void nextPattern()
{
  pattern_index_ = (pattern_index_ + 1) % ARRAY_SIZE(patterns_);
}

void nextMonochromePattern()
{
  monochrome_pattern_index_ = (monochrome_pattern_index_ + 1) % ARRAY_SIZE(monochrome_patterns_);
}

void nextStaticColor()
{
  CRGB currentColor = getCurrentColor();
  static_color_index_ = (static_color_index_ + 1) % ARRAY_SIZE(colors_);
  CRGB nextColor = getCurrentColor();

  static_color_hsv_ = rgb2hsv_approximate(getCurrentColor());
  hue_ = static_color_hsv_.hue;
  // fillStaticColor();
  blendColors(currentColor, nextColor);
}

void nextMode()
{
  mode_ = (mode_ + 1) % NUM_MODES;
  if (mode_ == Mode::Static)
  {
    fillStaticColor();
  }
}

void animationRainbow()
{
  // FastLED's built-in rainbow generator
  fill_rainbow(leds, NUM_LEDS, hue_, 7);
}

void animationRainbowWithGlitter()
{
  // built-in FastLED rainbow, plus some random sparkly glitter
  animationRainbow();
  animationAddGlitter(80);
}

void animationAddGlitter(fract8 chanceOfGlitter)
{
  if (random8() < chanceOfGlitter)
  {
    leds[random16(NUM_LEDS)] += CRGB::Pink;
  }
}

void animationConfetti()
{
  // random colored speckles that blink in and fade smoothly
  fadeToBlackBy(leds, NUM_LEDS, 10);
  int pos = random16(NUM_LEDS);
  leds[pos] += CHSV(hue_ + random8(64), 200, 255);
}

void animationSinelon()
{
  // a colored dot sweeping back and forth, with fading trails
  fadeToBlackBy(leds, NUM_LEDS, 20);
  int pos = beatsin16(13, 0, NUM_LEDS - 1);
  leds[pos] += CHSV(hue_, 255, 192);
}

void animationBpm()
{
  // colored stripes pulsing at a defined Beats-Per-Minute (BPM)
  byte BeatsPerMinute = 62;
  CRGBPalette16 palette = PartyColors_p;
  byte beat = beatsin8(BeatsPerMinute, 64, 255);
  for (int i = 0; i < NUM_LEDS; i++)
  { //9948
    leds[i] = ColorFromPalette(palette, hue_ + (i * 2), beat - hue_ + (i * 10));
  }
}

void animationJuggle()
{
  // eight colored dots, weaving in and out of sync with each other
  fadeToBlackBy(leds, NUM_LEDS, 20);
  byte dothue = 0;
  for (int i = 0; i < 8; i++)
  {
    leds[beatsin16(i + 7, 0, NUM_LEDS - 1)] |= CHSV(dothue, 200, 255);
    dothue += 32;
  }
}

void monochromeJuggle()
{
  // four colored dots, weaving in and out of sync with each other
  fadeToBlackBy(leds, NUM_LEDS, 20);
  for (int i = 0; i < 4; i++)
  {
    leds[beatsin16(i + 3, 0, NUM_LEDS - 1)] |= CHSV(static_color_hsv_.hue, static_color_hsv_.sat, static_color_hsv_.val);
  }
}

void animatePowerOn() {
  // Animation that runs when the device is switched on.
  updateInputHandlers();

  static_color_hsv_ = rgb2hsv_approximate(getCurrentColor());
  hue_ = static_color_hsv_.hue;

  fadeToBlackBy(leds, NUM_LEDS, 255);
  FastLED.setBrightness(brightness_);

  // blendColors(CRGB::Black, colors_[static_color_index_], 15);

  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CRGB::Purple;

    FastLED.show();
    delay(1000 / framesPerSecond_);
  }

  fillStaticColor();
}

void blendColors(CRGB currentColor, CRGB nextColor, int speed = 30) {
  for (int progress = 0; progress < 255; progress += speed) {
    CRGB blended = blend(currentColor, nextColor, progress);
    fill_solid(leds, NUM_LEDS, blended);

    FastLED.show();
    delay(1000 / framesPerSecond_);
  }
  fillStaticColor();
}

void fillStaticColor()
{
  fill_solid(leds, NUM_LEDS, getCurrentColor());
}


CRGB getCurrentColor() {
  return colors_[static_color_index_];
}

void ModeButtonHandler::onButtonPressed()
{
  nextMode();
  Serial.print("nextMode:");
  Serial.println(mode_);
}

void OptionButtonHandler::onButtonPressed()
{
  switch (mode_)
  {
  case Mode::Animated:
    nextPattern();
    Serial.print("nextPattern:");
    Serial.println(pattern_index_);
    break;
  case Mode::MonochromeAnimated:
    nextMonochromePattern();
    Serial.print("nextMonochromePattern:");
    Serial.println(monochrome_pattern_index_);
    break;
  case Mode::Static:
    nextStaticColor();
    Serial.print("nextStaticColor:");
    Serial.println(static_color_index_);
    break;
  }
}

void OptionButtonHandler::onLongPress()
{
  switch (mode_) {
  case Mode::MonochromeAnimated:
    nextStaticColor();
    break;
  }
}

void BrightnessPotentiometerHandler::onValueChanged(int value)
{
  // Handle fuzziness at extreme positions to avoid flickering.
  if (value < 20)
  {
    brightness_ = MIN_BRIGHTNESS;
  }
  else if (value > 1010)
  {
    brightness_ = MAX_BRIGHTNESS;
  }
  else
  {
    brightness_ = map(value, 0, 1023, MIN_BRIGHTNESS, MAX_BRIGHTNESS);
  }
}

FASTLED_NAMESPACE_END
