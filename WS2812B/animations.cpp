#include "animations.h"
#include <FastLED.h>
#include "hardware-config.h"

FASTLED_USING_NAMESPACE
namespace animations {
#define GLITTER_COLOR CRGB::Pink

byte hue_ = 0;
float animation_speed_multiplier_ = 1.0f;

void animationRainbow(CRGB leds[]) {
  // FastLED's built-in rainbow generator
  fill_rainbow(leds, NUM_LEDS, hue_, 7);
}

void animationRainbowWithGlitter(CRGB leds[]) {
  // built-in FastLED rainbow, plus some random sparkly glitter
  animationRainbow(leds);
  animationAddGlitter(leds);
}

void animationAddGlitter(CRGB leds[], fract8 chance_of_glitter = CHANCE_OF_GLITTER) {
  if (random8() < CHANCE_OF_GLITTER) {
    leds[random16(NUM_LEDS)] += GLITTER_COLOR;
  }
}

void animationConfetti(CRGB leds[]) {
  // random colored speckles that blink in and fade smoothly
  fadeToBlackBy(leds, NUM_LEDS, 10);
  int pos = random16(NUM_LEDS);
  leds[pos] += CHSV(hue_ + random8(64), 200, 255);
}

void animationSinelon(CRGB leds[]) {
  // a colored dot sweeping back and forth, with fading trails
  fadeToBlackBy(leds, NUM_LEDS, 20);
  int pos = beatsin16(13, 0, NUM_LEDS - 1);
  leds[pos] += CHSV(hue_, 255, 192);
}

void animationBpm(CRGB leds[]) {
  // colored stripes pulsing at a defined Beats-Per-Minute (BPM)
  byte BeatsPerMinute = 62;
  CRGBPalette16 palette = PartyColors_p;
  byte beat = beatsin8(BeatsPerMinute, 64, 255);
  for (int i = 0; i < NUM_LEDS; i++) {  // 9948
    leds[i] = ColorFromPalette(palette, hue_ + (i * 2), beat - hue_ + (i *
    10));
  }
}

void animationJuggle(CRGB leds[]) {
  // eight colored dots, weaving in and out of sync with each other
  fadeToBlackBy(leds, NUM_LEDS, 20);
  byte dothue = 0;
  for (int i = 0; i < 8; i++) {
    leds[beatsin16(i + 7, 0, NUM_LEDS - 1)] |= CHSV(dothue, 200, 255);
    dothue += 32;
  }
}

}  // namespace animations
FASTLED_NAMESPACE_END
