#define DEBUG
#include "debug.h"

#define FASTLED_INTERNAL  // Disable pragma version message on compilation
#include <stdint.h>
#include "arrays.h"
#include <FastLED.h>
#include "animations.h"
#include "colors.h"
#include "hardware-config.h"
#include "WS2812B.h"
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

#define FRAMES_PER_SECOND_DEFAULT 120
#define PALETTEOF(A) CRGBPalette16(A, A, A, A, A, A, A, A, A, A, A, A, A, A, A, A)

CRGB leds_[NUM_LEDS];

uint8_t frames_per_second_ = FRAMES_PER_SECOND_DEFAULT;

uint8_t mode_ = Mode::Static;
uint8_t brightness_ = MAX_BRIGHTNESS;

uint8_t animation_index_ = 0;             // Index of current pattern.
uint8_t monochrome_animation_index_ = 0;  // Index of current monochrome pattern.
uint8_t static_color_index_ = 0;          // Index of current static color.
uint8_t palette_index_ = 0;               // Index of current color palette.

// Input handlers
ModeButtonHandler mode_button_handler_(MODE_BUTTON_PIN);
OptionButtonHandler option_button_handler_(OPTION_BUTTON_PIN);
BrightnessPotentiometerHandler brightness_potentiometer_handler_(BRIGHTNESS_POT_PIN);

typedef void (*AnimationList[])(CRGB* leds);
AnimationList full_color_animations_ = {
    animations::animationRainbow,
    animations::animationRainbowWithGlitter,
    animations::animationConfetti,
    animations::animationSinelon,
    animations::animationJuggle,
    animations::animationBpm,
};
AnimationList monochrome_animations_ = {
    animations::monochromeGlitter,
    animations::monochromeSinelon,
    animations::monochromeJuggle,
    animations::monochromePulse,
    animations::monochromeRainbow,
};
AnimationList palette_animations_ = {
  animations::paletteAnimation,
};

// Available color definitions can be found at
// https://github.com/FastLED/FastLED/blob/master/pixeltypes.h
// TODO replace HTMLColorCode with palletes (via PALETTEOF) for each color so we
// can mix in 'themes' like cloud/lava/ocean:
// https://github.com/FastLED/FastLED/blob/master/colorpalettes.h
const ColorCode colors_[] = {
    ColorCode::Purple,
    ColorCode::HotPink,
    ColorCode::FairyLightNCC,
    ColorCode::White,
    ColorCode::OrangeRed,
    ColorCode::Red,
    ColorCode::Yellow,
    ColorCode::Green,
    ColorCode::Cyan,
    ColorCode::Blue,
};

CRGBPalette16 palettes_[] = {
  CloudColors_p,
  OceanColors_p,
  ForestColors_p,
  LavaColors_p,
  HeatColors_p,
  PartyColors_p,
};

void setup(void) {
  #ifdef DEBUG
  Serial.begin(SERIAL_BAUD_RATE);
  Serial.println("Starting...");
  #endif

  setupInputHandlers();
  animations::palette_ = palettes_[palette_index_];

  // tell FastLED about the LED strip configuration
  FastLED.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(leds_, NUM_LEDS)
      .setCorrection(COLOR_CORRECTION);
  FastLED.setTemperature(COLOR_TEMPERATURE);

  PRINTLN("OK GO");

  animatePowerOn();
}

void loop(void) {
  updateInputHandlers();
  EVERY_N_MILLISECONDS(20) {
    animations::hue_++;
  }

  switch (mode_) {
    case Mode::Static:
      break;
    case Mode::MonochromeAnimated:
      monochrome_animations_[monochrome_animation_index_](leds_);
      break;
    case Mode::Animated:
      full_color_animations_[animation_index_](leds_);
      break;
    case Mode::PaletteAnimated:
      palette_animations_[0](leds_);
      break;
  }

  draw();
}

void draw(void) {
  FastLED.setBrightness(brightness_);
  FastLED.show();
  FastLED.delay(1000 / frames_per_second_);
}

void setupInputHandlers(void) {
  mode_button_handler_.setup();
  option_button_handler_.setup();
}

void updateInputHandlers(void) {
  brightness_potentiometer_handler_.update();
  mode_button_handler_.update();
  option_button_handler_.update();
}

void nextPattern(void) {
  animation_index_ = (animation_index_ + 1) % ARRAY_SIZE(full_color_animations_);
}

void nextMonochromePattern(void) {
  monochrome_animation_index_ = (monochrome_animation_index_ + 1) % ARRAY_SIZE(monochrome_animations_);
}

void nextPalette(void) {
  palette_index_ = (palette_index_ + 1) % ARRAY_SIZE(palettes_);
  animations::palette_ = palettes_[palette_index_];
}

void nextStaticColor(void) {
  // CRGB current_color = getCurrentColor();
  static_color_index_ = (static_color_index_ + 1) % ARRAY_SIZE(colors_);
  // CRGB next_color = getCurrentColor();

  animations::static_color_hsv_ = rgb2hsv_approximate(getCurrentColor());
  animations::hue_ = animations::static_color_hsv_.hue;
  fillStaticColor();
  // blendColors(current_color, next_color);
}

void nextMode(void) {
  mode_ = (mode_ + 1) % NUM_MODES;
  if (mode_ == Mode::Static) {
    fillStaticColor();
  }
}

void animatePowerOn(void) {
  // Animation that runs when the device is switched on.
  updateInputHandlers();

  animations::static_color_hsv_ = rgb2hsv_approximate(getCurrentColor());
  animations::hue_ = animations::static_color_hsv_.hue;

  fadeToBlackBy(leds_, NUM_LEDS, 255);
  FastLED.setBrightness(brightness_);

  for (int i = 0; i < NUM_LEDS; i++) {
    leds_[i] = toCRGB(ColorCode::Purple);

    FastLED.show();
    delay(1000 / frames_per_second_);
  }

  fillStaticColor();
}

void blendColors(CRGB current_color, CRGB next_color, int speed = 30) {
  for (int progress = 0; progress < 255; progress += speed) {
    CRGB blended = blend(current_color, next_color, progress);
    fill_solid(leds_, NUM_LEDS, blended);

    FastLED.show();
    delay(1000 / frames_per_second_);
  }
  fillStaticColor();
}

void fillStaticColor(void) {
  fill_solid(leds_, NUM_LEDS, getCurrentColor());
}

CRGB getCurrentColor(void) {
  return toCRGB(colors_[static_color_index_]);
}

void ModeButtonHandler::onButtonPressed(void) {
  nextMode();
  PRINT("nextMode:");
  PRINTLN(mode_);
}

void OptionButtonHandler::onButtonPressed(void) {
  switch (mode_) {
    case Mode::Animated:
      nextPattern();
      PRINT("nextPattern:");
      PRINTLN(animation_index_);
      break;
    case Mode::MonochromeAnimated:
      nextMonochromePattern();
      PRINT("nextMonochromePattern:");
      PRINTLN(monochrome_animation_index_);
      break;
    case Mode::Static:
      nextStaticColor();
      PRINT("nextStaticColor:");
      PRINTLN(static_color_index_);
      break;
    case Mode::PaletteAnimated:
      nextPalette();
      PRINT("nextPalette:");
      PRINTLN(palette_index_);
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

void BrightnessPotentiometerHandler::onValueChanged(const int value) {
  if (mode_button_handler_.isDown()) {
    onValueChangedWithModeButton(value);
  }
  else {
    onValueChangedNoModifier(value);
  }
}

void BrightnessPotentiometerHandler::onValueChangedNoModifier(const int value) {
  // Handle fuzziness at extreme positions to avoid flickering.
  if (value < 20) {
    brightness_ = MIN_BRIGHTNESS;
  } else if (value > 1010) {
    brightness_ = MAX_BRIGHTNESS;
  } else {
    brightness_ = map(value, 0, 1023, MIN_BRIGHTNESS, MAX_BRIGHTNESS);
  }
}

void BrightnessPotentiometerHandler::onValueChangedWithModeButton(const int value) {
  // Change animation speed by holding Mode button while turning the brightness pot.
  animations::animation_speed_multiplier_ = (float) map(value, 0, 1023, 1, 20) / 10.f;
  frames_per_second_ = animations::animation_speed_multiplier_ * FRAMES_PER_SECOND_DEFAULT;

  PRINT(animations::animation_speed_multiplier_);
  PRINT("x -> ");
  PRINT(frames_per_second_);
  PRINTLN("fps");
}

FASTLED_NAMESPACE_END
