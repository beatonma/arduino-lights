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
 *
 * - Implemented hardware controls:
 *  - Change brightness by turning pot: BrightnessPotentiometerHandler::onValueChangedNoModifier
 *  - Change mode by pressing Mode button: ModeButtonHandler::onButtonPressed
 *  - Change color or animation (depending on current ::Mode) by pressing Option button: OptionButtonHandler::onButtonPressed
 *  - Change animation speed by holding Mode button while turning pot: BrightnessPotentiometerHandler::onValueChangedWithModeButton
 *  - Change color temperature by holding Option button while turning pot: BrightnessPotentiometerHandler::onValueChangedWithOptionButton
 *  - Some modes also allow control of a second parameter by long-pressing the Option button: OptionButtonHandler::onLongPress
 *  - Change color temperature by holding Option button while turning pot: BrightnessPotentiometerHandler::onValueChangedWithOptionButton
 */
// #define DEBUG
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

const uint8_t frames_per_second_ = FRAMES_PER_SECOND_DEFAULT;

uint8_t mode_ = Mode::Halloween;
uint8_t brightness_ = MAX_BRIGHTNESS;

// Index of current static color.
uint8_t color_index_ = 0;

// Index of current halloween animation.
uint8_t halloween_animations_index_ = 0;

// Input handlers
PirHandler proximity_handler_(PROXIMITY_DETECTOR_PIN);

typedef void (*AnimationList[])(CRGB* leds);

boolean activated = true;
const long deactivate_timeout = 3000;
long deactivate_at = -1;

AnimationList halloween_animations_ = {
  animations::halloweenColumnPulse,
  animations::halloweenAlternatingColumns2,
  animations::halloweenAlternatingColumns2,
  animations::halloweenAlternatingColumns,
  animations::halloweenAlternatingColumns,
  animations::halloweenColumnPulse,
  animations::halloweenColumnPulse,
  animations::halloweenColumnPulse,
  animations::halloweenRingPulse,
  animations::halloweenRingPulse,
  animations::halloweenRingPulse,
  animations::monochromePulse,
  animations::monochromeJuggle,
};


/**
 * Available color definitions can be found at
 * https://github.com/FastLED/FastLED/blob/master/pixeltypes.h
 */
const uint32_t colors_[] = {
  ColorCode::Red,
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
  animations::color_hsv_ = rgb2hsv_approximate(getCurrentColor());
  animations::hue_ = animations::color_hsv_.hue;

  // tell FastLED about the LED strip configuration
  FastLED.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(leds_, NUM_LEDS)
      .setCorrection(COLOR_CORRECTION);
  FastLED.setTemperature(COLOR_TEMPERATURE);

  deactivate_at = millis();

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

  if (deactivate_at > 0) {
    long diff = millis() - deactivate_at;
    if (diff > deactivate_timeout) {
      mode_ = Mode::Deactivated;
      deactivate_at = -1;
    }
  }

  switch (mode_) {
    case Mode::Deactivated:
      animations::deactivate(leds_);
      break;

    case Mode::Halloween:
      halloween_animations_[halloween_animations_index_](leds_);
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
  proximity_handler_.setup();
}

void updateInputHandlers(void) {
  proximity_handler_.update();
}

void nextHalloweenAnimation(void) {
  halloween_animations_index_ = (halloween_animations_index_ + 1) % ARRAY_SIZE(halloween_animations_);
}

void randomHalloweenAnimation(void) {
  halloween_animations_index_ = ANY_INDEX(halloween_animations_);
}

void nextStaticColor(void) {
  color_index_ = (color_index_ + 1) % ARRAY_SIZE(colors_);

  animations::color_hsv_ = rgb2hsv_approximate(getCurrentColor());
  animations::hue_ = animations::color_hsv_.hue;
  animations::transition_progress_ = 0;
  animations::alt_color_hsv_ = CHSV(animations::color_hsv_.hue + 30, animations::color_hsv_.sat, animations::color_hsv_.val);
}

void randomColor(void) {
  color_index_ = ANY_INDEX(colors_);

  animations::color_hsv_ = rgb2hsv_approximate(getCurrentColor());
  animations::hue_ = animations::color_hsv_.hue;
  animations::transition_progress_ = 0;
  animations::alt_color_hsv_ = CHSV(animations::color_hsv_.hue + 30, animations::color_hsv_.sat, animations::color_hsv_.val);
}

CRGB getCurrentColor(void) {
  return toCRGB(colors_[color_index_]);
}

void PirHandler::onMotionEventStart(void) {
  if (mode_ != Mode::Deactivated) {
    deactivate_at = millis();
    return;
  }


  randomHalloweenAnimation();
  randomColor();

  animations::multicolor_ = rand() > RAND_MAX / 2;

  deactivate_at = -1;
  mode_ = Mode::Halloween;
}

void PirHandler::onMotionEventContinued(void) {

}

void PirHandler::onMotionEventEnd(void) {
  deactivate_at = millis();
  PRINTLN(deactivate_at);
}

void PirHandler::onIdle(void) {

}

void PirHandler::onChange(void) {

}

FASTLED_NAMESPACE_END
