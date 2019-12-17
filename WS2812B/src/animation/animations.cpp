/** @file */
#include "animations.h"
#include <FastLED.h>

FASTLED_USING_NAMESPACE
namespace animations {

uint8_t hue_ = 0;
float animation_speed_multiplier_ = 1.0f;


void addGlitter(CRGB leds[], CRGB glitter_color, fract8 chance_of_glitter) {
  if (random8() < CHANCE_OF_GLITTER) {
    leds[random16(NUM_LEDS)] += glitter_color;
  }
}

void polychromeRainbow(CRGB leds[]) {
  // FastLED's built-in rainbow generator
  fill_rainbow(leds, NUM_LEDS, hue_, 7);
}

void monochromeRainbow(CRGB leds[]) {
  // A temporal rainbow - all lights are the same color but that color changes
  // over time
  fill_solid(leds, NUM_LEDS, CHSV(hue_, 240, 255));
}

void polychromeRainbowWithGlitter(CRGB leds[]) {
  // built-in FastLED rainbow, plus some random sparkly glitter
  polychromeRainbow(leds);
  addGlitter(leds);
}

void polychromeConfetti(CRGB leds[]) {
  // random colored speckles that blink in and fade smoothly
  fadeToBlackBy(leds, NUM_LEDS, 10);
  int pos = random16(NUM_LEDS);
  leds[pos] += CHSV(hue_ + random8(64), 200, 255);
}

void polychromeSinelon(CRGB leds[]) {
  // a colored dot sweeping back and forth, with fading trails
  fadeToBlackBy(leds, NUM_LEDS, 20);
  int pos = beatsin16(13, 0, NUM_LEDS - 1);
  leds[pos] += CHSV(hue_, 255, 192);
}

void polychromeBpm(CRGB leds[]) {
  // colored stripes pulsing at a defined Beats-Per-Minute (BPM)
  uint8_t BeatsPerMinute = 62;
  CRGBPalette16 palette = PartyColors_p;
  uint8_t beat = beatsin8(BeatsPerMinute, 64, 255);
  for (int i = 0; i < NUM_LEDS; i++) {  // 9948
    leds[i] = ColorFromPalette(palette, hue_ + (i * 2), beat - hue_ + (i * 10));
  }
}

void polychromeJuggle(CRGB leds[]) {
  // eight colored dots, weaving in and out of sync with each other
  fadeToBlackBy(leds, NUM_LEDS, 20);
  uint8_t dothue = 0;
  for (int i = 0; i < 8; i++) {
    leds[beatsin16(i + 7, 0, NUM_LEDS - 1)] |= CHSV(dothue, 200, 255);
    dothue += 32;
  }
}

void polychromeColliders(CRGB leds[]) {
  // Dots move along from random colors and positions. When they collide
  // they each add the opponent's color to themselves until they reach full rgb.
}

void polychromeSplash(CRGB leds[]) {}

}  // namespace animations
FASTLED_NAMESPACE_END
