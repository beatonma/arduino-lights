#define FASTLED_INTERNAL  // Disable pragma version message on compilation
#include "animations.h"
#include "hardware-config.h"
#include "WS2812B.h"
#include <FastLED.h>
FASTLED_USING_NAMESPACE

//
// Based on FastLED "100-lines-of-code" demo reel by Mark Kriegsman, December 2014:
// https://github.com/FastLED/FastLED/blob/master/examples/DemoReel100/DemoReel100.ino
//
//
// Edited/extended by Michael Beaton, December 2019
// - Implemented modes:
//   - Static: stay on a single color
//   - MonochromeAnimated: enable animations but only using the color chosen in
//   Static
//   - Animated: use the animations from the example
// - Accepts input from potentiometer to change brightness.
// - Accepts input from 2 buttons to change modes and choose colors/animations.
//

#if defined(FASTLED_VERSION) && (FASTLED_VERSION < 3001000)
#warning "Requires FastLED 3.1 or later; check github for latest code."
#endif

#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))
#define PALETTEOF(A) \
  CRGBPalette16(A, A, A, A, A, A, A, A, A, A, A, A, A, A, A, A)

CRGB leds[NUM_LEDS];

const byte framesPerSecond_ = 120;

byte mode_ = Mode::Static;
byte brightness_ = MAX_BRIGHTNESS;

byte pattern_index_ = 0;             // Index of current pattern
byte monochrome_pattern_index_ = 0;  // Index of current monochrome pattern
byte static_color_index_ = 0;        // Index of current static color.
CHSV static_color_hsv_;

// Input handlers
ModeButtonHandler mode_button_handler_(MODE_BUTTON_PIN);
OptionButtonHandler option_button_handler_(OPTION_BUTTON_PIN);
BrightnessPotentiometerHandler brightness_potentiometer_handler_(
    BRIGHTNESS_POT_PIN);

// List of patterns to cycle through.  Each is defined as a separate function
// below.
typedef void (*AnimationList[])(CRGB* leds);
AnimationList patterns_ = {
    animations::animationRainbow,  animations::animationRainbowWithGlitter,
    animations::animationConfetti, animations::animationSinelon,
    animations::animationJuggle,   animations::animationBpm,
};
AnimationList monochrome_patterns_ = {
    // monochromeJuggle,
    // animations::animationBpm,
    // animations::animationRainbow,
};

// Available color definitions can be found at
// https://github.com/FastLED/FastLED/blob/master/pixeltypes.h
// TODO replace HTMLColorCode with palletes (via PALETTEOF) for each color so we
// can mix in 'themes' like cloud/lava/ocean:
// https://github.com/FastLED/FastLED/blob/master/colorpalettes.h
const int colors_[] = {
    CRGB::Purple, CRGB::White, CRGB::Red,  CRGB::OrangeRed,
    CRGB::Yellow, CRGB::Green, CRGB::Cyan, CRGB::Blue,
};

void setup(void) {
  Serial.begin(SERIAL_BAUD_RATE);
  Serial.println("Starting...");

  delay(3000);  // 3 second delay for recovery

  setupInputHandlers();

  // tell FastLED about the LED strip configuration
  FastLED.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS)
      .setCorrection(COLOR_CORRECTION);
  FastLED.setTemperature(COLOR_TEMPERATURE);

  Serial.println("OK GO");

  animatePowerOn();
}

void loop(void) {
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

  switch (mode_) {
    case Mode::Static:
      break;
    case Mode::MonochromeAnimated:
      monochrome_patterns_[monochrome_pattern_index_](leds);
      break;
    case Mode::Animated:
      EVERY_N_MILLISECONDS(20) {
        animations::hue_++;
      }
      patterns_[pattern_index_](leds);
      break;
  }

  draw();
}

void draw(void) {
  FastLED.setBrightness(brightness_);
  FastLED.show();
  FastLED.delay(1000 / framesPerSecond_);
}

void setupInputHandlers(void) {
  mode_button_handler_.setup();
  option_button_handler_.setup();
}

void updateInputHandlers(void) {
  // for (int i = 0; i < ARRAY_SIZE(input_handlers_); i++) {
  //   (*(input_handlers_[i])).update();
  // }
  brightness_potentiometer_handler_.update();
  mode_button_handler_.update();
  option_button_handler_.update();
}

void nextPattern(void) {
  pattern_index_ = (pattern_index_ + 1) % ARRAY_SIZE(patterns_);
}

void nextMonochromePattern(void) {
  monochrome_pattern_index_ =
      (monochrome_pattern_index_ + 1) % ARRAY_SIZE(monochrome_patterns_);
}

void nextStaticColor(void) {
  CRGB current_color = getCurrentColor();
  static_color_index_ = (static_color_index_ + 1) % ARRAY_SIZE(colors_);
  CRGB next_color = getCurrentColor();

  static_color_hsv_ = rgb2hsv_approximate(getCurrentColor());
  animations::hue_ = static_color_hsv_.hue;
  // fillStaticColor();
  blendColors(current_color, next_color);
}

void nextMode(void) {
  mode_ = (mode_ + 1) % NUM_MODES;
  if (mode_ == Mode::Static) {
    fillStaticColor();
  }
}


void monochromeJuggle(void) {
  // four colored dots, weaving in and out of sync with each other
  fadeToBlackBy(leds, NUM_LEDS, 20);
  for (int i = 0; i < 4; i++) {
    leds[beatsin16(i + 3, 0, NUM_LEDS - 1)] |= CHSV(
        static_color_hsv_.hue, static_color_hsv_.sat, static_color_hsv_.val);
  }
}

void animatePowerOn(void) {
  // Animation that runs when the device is switched on.
  updateInputHandlers();

  static_color_hsv_ = rgb2hsv_approximate(getCurrentColor());
  animations::hue_ = static_color_hsv_.hue;

  fadeToBlackBy(leds, NUM_LEDS, 255);
  FastLED.setBrightness(brightness_);

  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CRGB::Purple;

    FastLED.show();
    delay(1000 / framesPerSecond_);
  }

  fillStaticColor();
}

void blendColors(CRGB current_color, CRGB next_color, int speed = 30) {
  for (int progress = 0; progress < 255; progress += speed) {
    CRGB blended = blend(current_color, next_color, progress);
    fill_solid(leds, NUM_LEDS, blended);

    FastLED.show();
    delay(1000 / framesPerSecond_);
  }
  fillStaticColor();
}

void fillStaticColor(void) { fill_solid(leds, NUM_LEDS, getCurrentColor()); }

CRGB getCurrentColor(void) { return colors_[static_color_index_]; }

void ModeButtonHandler::onButtonPressed(void) {
  nextMode();
  Serial.print("nextMode:");
  Serial.println(mode_);
}

void OptionButtonHandler::onButtonPressed(void) {
  switch (mode_) {
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

void OptionButtonHandler::onLongPress(void) {
  switch (mode_) {
    case Mode::MonochromeAnimated:
      nextStaticColor();
      break;
  }
}

void BrightnessPotentiometerHandler::onValueChanged(int value) {
  // Handle fuzziness at extreme positions to avoid flickering.
  if (value < 20) {
    brightness_ = MIN_BRIGHTNESS;
  } else if (value > 1010) {
    brightness_ = MAX_BRIGHTNESS;
  } else {
    brightness_ = map(value, 0, 1023, MIN_BRIGHTNESS, MAX_BRIGHTNESS);
  }
}

FASTLED_NAMESPACE_END
