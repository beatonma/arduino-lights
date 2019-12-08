#include <FastLED.h>
#include "animations.h"
#include "hardware-config.h"

FASTLED_USING_NAMESPACE
namespace animations {
CHSV static_color_hsv_ = CHSV(0, 0, 0);

void monochromeJuggle(CRGB leds[]) {
  // four colored dots, weaving in and out of sync with each other
  fadeToBlackBy(leds, NUM_LEDS, 20);
  for (int i = 0; i < 4; i++) {
    leds[beatsin16(i + 7, 0, NUM_LEDS - 1)] |= CHSV(
        static_color_hsv_.hue, static_color_hsv_.sat, static_color_hsv_.val);
  }
}

void monochromeGlitter(CRGB* leds) {
    fadeToBlackBy(leds, NUM_LEDS, 10);
    if (random8() < CHANCE_OF_GLITTER) {
        leds[random16(NUM_LEDS)] += static_color_hsv_;
    }
}
}  // namespace animations
FASTLED_NAMESPACE_END
