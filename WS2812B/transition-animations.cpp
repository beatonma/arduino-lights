/** @file */
#include <FastLED.h>
#include "hardware-config.h"
#include "animations.h"

namespace animations {
uint8_t transition_progress_ = 0;

void transitionFadeToSolid(CRGB leds[], CRGB target_color) {
    CRGB blended = nblend(leds[0], target_color, transition_progress_);
    for (int i = 0; i < NUM_LEDS; i++) {
        leds[i] = blended;
    }
    transition_progress_ = qadd8(transition_progress_, 1);
}

void transitionLinearToSolid(CRGB leds[], CRGB target_color) {
    // CRGB blended = nblend(leds[0], target_color, transition_progress_);
    // uint16_t limit = beatsin16(20, 0, NUM_LEDS);

    int limit = map(transition_progress_, 0, 255, 0, NUM_LEDS);
    for (int i = 0; i < NUM_LEDS; i++) {
        if (i <= limit) {
            leds[i] = target_color;
        }
    }
    transition_progress_ = qadd8(transition_progress_, 5);
}

}
