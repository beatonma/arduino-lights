#include <FastLED.h>
#include "animations.h"
#include "hardware-config.h"

FASTLED_USING_NAMESPACE
namespace animations {

CHSV static_color_hsv_ = CHSV(0, 0, 0);

void monochromeJuggle(CRGB leds[]) {
  // four colored dots, weaving in and out of sync with each other
  fadeToBlackBy(leds, NUM_LEDS, 20);
  for (int i = 0; i < 3; i++) {
    leds[beatsin16(i + 4, 0, NUM_LEDS - 1)] |= CHSV(
        static_color_hsv_.hue, static_color_hsv_.sat, static_color_hsv_.val);
  }
}

void monochromeGlitter(CRGB leds[]) {
  fadeToBlackBy(leds, NUM_LEDS, 3);
  if (random8() < CHANCE_OF_GLITTER) {
    leds[random16(NUM_LEDS)] += static_color_hsv_;
  }
}

void monochromeSinelon(CRGB leds[]) {
  // Similar to animationSinelon but all lights stay on at a low level
  // with the sweep 'overlayed'
  fadeToBlackBy(leds, NUM_LEDS, 5);
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] |= CHSV(static_color_hsv_.hue, static_color_hsv_.sat, 60);
  }
  for (int i = 0; i < 1; i++) {  // i = number of fliers
    leds[beatsin16((i + 3) * animation_speed_multiplier_, 0, NUM_LEDS - 1)] |= static_color_hsv_;
  }
}

void monochromePulse(CRGB leds[]) {
  // Pulse the brightness of all lights together
  fill_solid(leds, NUM_LEDS,
             CHSV(static_color_hsv_.hue, static_color_hsv_.sat,
                  beatsin16(30 * animation_speed_multiplier_, 120, 255)));
}

void monochromeRainbow(CRGB leds[]) {
  // A temporal rainbow - all lights are the same color but that color changes over time
  fill_solid(leds, NUM_LEDS, CHSV(hue_, 240, 255));
}

}  // namespace animations
FASTLED_NAMESPACE_END
