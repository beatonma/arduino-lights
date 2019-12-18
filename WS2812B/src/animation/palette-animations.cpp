/** @file */
#include "animations.h"
#include <FastLED.h>

FASTLED_USING_NAMESPACE
namespace animations {

CRGBPalette16 palette_;

void paletteFlow(CRGB leds[]) {
    fill_palette(leds, NUM_LEDS, hue_, 15, palette_, 240, LINEARBLEND);
}

void paletteFlowWithGlitter(CRGB leds[]) {
    fill_palette(leds, NUM_LEDS, hue_, 15, palette_, 240, LINEARBLEND);
    addGlitter(leds);
}

void paletteGlitter(CRGB leds[]) {
    FADE(3);
    if (random8() < CHANCE_OF_GLITTER) {
        leds[random16(NUM_LEDS)] += ColorFromPalette(palette_, random8());
    }
}

}
