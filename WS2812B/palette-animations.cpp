#include "animations.h"
#include <FastLED.h>
#include "hardware-config.h"

FASTLED_USING_NAMESPACE
namespace animations {

CRGBPalette16 palette_;

void paletteAnimation(CRGB leds[]) {
    FADE(20);
    fill_palette(leds, NUM_LEDS, hue_, 15, palette_, 240, LINEARBLEND);
}

}
