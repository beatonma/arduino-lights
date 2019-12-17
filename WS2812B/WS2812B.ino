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
 * - Implemented hardware controls:
 *  - Change brightness by turning pot: BrightnessPotentiometerHandler::onValueChangedNoModifier
 *  - Change mode by pressing Mode button: ModeButtonHandler::onButtonPressed
 *  - Change color or animation (depending on current ::Mode) by pressing Option button: OptionButtonHandler::onButtonPressed
 *  - Change animation speed by holding Mode button while turning pot: BrightnessPotentiometerHandler::onValueChangedWithModeButton
 *  - Change color temperature by holding Option button while turning pot: BrightnessPotentiometerHandler::onValueChangedWithOptionButton
 *  - Some modes also allow control of a second parameter by long-pressing the Option button: OptionButtonHandler::onLongPress
 *  - Change color temperature by holding Option button while turning pot: BrightnessPotentiometerHandler::onValueChangedWithOptionButton
 */
#define DEBUG
#include "debug.h"

#define FASTLED_INTERNAL  // Disable pragma version message on compilation
#include <FastLED.h>
#include <stdint.h>
#include "WS2812B.h"
#include "arrays.h"
#include "colors.h"
#include "hardware-config.h"
#include "src/animation/animations.h"
FASTLED_USING_NAMESPACE

#if defined(FASTLED_VERSION) && (FASTLED_VERSION < 3001000)
#warning "Requires FastLED 3.1 or later; check github for latest code."
#endif

#define FRAMES_PER_SECOND_DEFAULT 120

CRGB leds_[NUM_LEDS];

uint8_t frames_per_second_ = FRAMES_PER_SECOND_DEFAULT;

uint8_t mode_ = Mode::Static;
uint8_t brightness_ = MAX_BRIGHTNESS;

/// Index of current pattern.
uint8_t animation_index_ = 0;

/// Index of current monochrome pattern.
uint8_t monochrome_animation_index_ = 0;

/// Index of current static color.
uint8_t static_color_index_ = 0;

/// Index of current color palette.
uint8_t palette_index_ = 0;

/// Index of current palette animation.
uint8_t palette_animation_index_ = 0;

bool auto_cycle_ = false; ///< If true, modes and colors will be changed automatically.

// Input handlers
ModeButtonHandler mode_button_handler_(MODE_BUTTON_PIN);
OptionButtonHandler option_button_handler_(OPTION_BUTTON_PIN);
BrightnessPotentiometerHandler brightness_potentiometer_handler_(BRIGHTNESS_POT_PIN);

typedef void (*AnimationList[])(CRGB* leds);
/**
 * Animations used when mode_ is ::Animated
 */
AnimationList full_color_animations_ = {
  // animations::polychromeSplash,
  animations::polychromeRainbow,
  animations::polychromeRainbowWithGlitter,
  animations::polychromeConfetti,
  animations::polychromeSinelon,
  animations::polychromeJuggle,
  animations::polychromeBpm,
  animations::monochromeRainbow,
};

/**
 * Animations used when mode_ is ::MonochromeAnimated
 */
AnimationList monochrome_animations_ = {
  animations::monochromeGlitter,
  animations::monochromeSinelon,
  animations::monochromeJuggle,
  animations::monochromePulse,
};

/**
 * Animations used when mode_ is ::PaletteAnimated
 */
AnimationList palette_animations_ = {
  animations::paletteFlow,
  animations::paletteFlowWithGlitter,
  animations::paletteGlitter,
};

/**
 * Available color definitions can be found at
 * https://github.com/FastLED/FastLED/blob/master/pixeltypes.h
 */
const uint32_t colors_[] = {
  ColorCode::Purple,
  ColorCode::Magenta,
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

/**
 * Themed collections of colors used in ::PaletteAnimated.
 */
CRGBPalette16 palettes_[] = {
  palettes::unicornPalette(),
  PartyColors_p,
  palettes::summerPalette(),
  CloudColors_p,
  OceanColors_p,
  ForestColors_p,
  LavaColors_p,
};

/**
 * LED color temperature may be adjusted to any of these values.
 */
const uint32_t temperatures_[] = {
  ColorTemperature::Candle,
  ColorTemperature::Tungsten40W,
  ColorTemperature::Tungsten100W,
  ColorTemperature::Halogen,
  ColorTemperature::CarbonArc,
  ColorTemperature::HighNoonSun,
  ColorTemperature::DirectSunlight,
  ColorTemperature::OvercastSky,
  ColorTemperature::ClearBlueSky,
  ColorTemperature::UncorrectedTemperature,
};


/**
 * Arduino setup runs when first powered on.
 */
void setup(void) {
  #ifdef DEBUG
  Serial.begin(SERIAL_BAUD_RATE);
  Serial.println("Starting...");
  #endif

  setupInputHandlers();
  animations::palette_ = palettes_[palette_index_];
  animations::static_color_hsv_ = rgb2hsv_approximate(getCurrentColor());
  animations::hue_ = animations::static_color_hsv_.hue;

  // tell FastLED about the LED strip configuration
  FastLED.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(leds_, NUM_LEDS)
      .setCorrection(COLOR_CORRECTION);
  FastLED.setTemperature(COLOR_TEMPERATURE);

  PRINTLN("OK GO");
}

/**
 * Arduino loop runs repeatedly after setup() completes.
 */
void loop(void) {
  updateInputHandlers();
  EVERY_N_MILLISECONDS(20) {
    animations::hue_++;
  }

  // if (mode_ == Mode::Auto) {
  if (auto_cycle_) {
    EVERY_N_SECONDS(30) {
      nextAuto();
    }
  }

  switch (mode_) {
    case Mode::Static:
      animations::transitionLinearToSolid(leds_, getCurrentColor());
      break;
    case Mode::MonochromeAnimated:
      monochrome_animations_[monochrome_animation_index_](leds_);
      break;
    case Mode::Animated:
      full_color_animations_[animation_index_](leds_);
      break;
    case Mode::PaletteAnimated:
      palette_animations_[palette_animation_index_](leds_);
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

void nextPaletteAnimation(void) {
  palette_animation_index_ = (palette_animation_index_ + 1) % ARRAY_SIZE(palette_animations_);
}

void nextPalette(void) {
  palette_index_ = (palette_index_ + 1) % ARRAY_SIZE(palettes_);
  animations::palette_ = palettes_[palette_index_];
}

void nextStaticColor(void) {
  static_color_index_ = (static_color_index_ + 1) % ARRAY_SIZE(colors_);

  animations::static_color_hsv_ = rgb2hsv_approximate(getCurrentColor());
  animations::hue_ = animations::static_color_hsv_.hue;
  animations::transition_progress_ = 0;
}

void nextMode(void) {
  mode_ = (mode_ + 1) % NUM_MODES;
}

/**
 * Randomly choose a new ::Mode, animation or color.
 */
void nextAuto(void) {
  const uint8_t result = random8();
  if (result < 85) {
    // Change the mode.
    nextMode();
  }
  else if (result < 170) {
    // Change the animation.
    nextPattern();
    nextMonochromePattern();
    nextPaletteAnimation();
  }
  else {
    // Change the color.
    nextPalette();
    nextStaticColor();
  }
}

CRGB getCurrentColor(void) {
  return toCRGB(colors_[static_color_index_]);
}

/**
 * Cycle through ::Mode%s
 */
void ModeButtonHandler::onButtonPressed(void) {
  nextMode();
  PRINT("nextMode:");
  PRINTLN(mode_);
}

/**
 * Toggle automatic selection of colors.
 */
void ModeButtonHandler::onLongPress(void) {
  auto_cycle_ = !auto_cycle_;
  PRINT("auto: ");
  PRINTLN(auto_cycle_);
}

/**
 * Cycle through main color/animation options.
 */
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
      PRINT("nextStaticColor: ");
      PRINT(static_color_index_);
      PRINT(" ");
      PRINTLN(colors_[static_color_index_]);
      break;
    case Mode::PaletteAnimated:
      nextPalette();
      PRINT("nextPalette:");
      PRINTLN(palette_index_);
      break;
  }
}

/**
 * Cycle through additional animation/color options.
 */
void OptionButtonHandler::onLongPress(void) {
  switch (mode_) {
    case Mode::MonochromeAnimated:
      nextStaticColor();
      break;
    case Mode::PaletteAnimated:
      nextPaletteAnimation();
      break;
  }
}

void BrightnessPotentiometerHandler::onValueChanged(const int value) {
  if (mode_button_handler_.isDown()) {
    onValueChangedWithModeButton(value);
  }
  else if (option_button_handler_.isDown()) {
    onValueChangedWithOptionButton(value);
  }
  else {
    onValueChangedNoModifier(value);
  }
}

/**
 * Change LED brightness based on pot position.
 */
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

/**
 * Change animation speed by turning pot while Mode button is held.
 */
void BrightnessPotentiometerHandler::onValueChangedWithModeButton(const int value) {
  mode_button_handler_.consumeAction();  // Cancel any further callbacks from the button.

  animations::animation_speed_multiplier_ = (float) map(value, 0, 1023, 1, 20) / 10.f;
  frames_per_second_ = animations::animation_speed_multiplier_ * FRAMES_PER_SECOND_DEFAULT;

  PRINT(animations::animation_speed_multiplier_);
  PRINT("x -> ");
  PRINT(frames_per_second_);
  PRINTLN("fps");
}

/**
 * Change LED color temperature by turning pot while Option button is held.
 */
void BrightnessPotentiometerHandler::onValueChangedWithOptionButton(const int value) {
  option_button_handler_.consumeAction();  // Cancel any further callbacks from the button.

  // Change color temperature by holding Option button while turning the brightness pot.
  uint32_t temperature;
  if (value < 20) {
    temperature = temperatures_[0];
  }
  else if (value > 1010) {
    temperature = temperatures_[ARRAY_SIZE(temperatures_) - 1];
  }
  else {
    temperature = temperatures_[map(value, 20, 1010, 0, ARRAY_SIZE(temperatures_) - 1)];
  }
  FastLED.setTemperature(CRGB(temperature));
}

FASTLED_NAMESPACE_END
