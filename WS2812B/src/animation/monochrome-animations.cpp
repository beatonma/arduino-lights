/** @file */
#include <FastLED.h>
#include "animations.h"

FASTLED_USING_NAMESPACE
namespace animations {

CHSV static_color_hsv_ = CHSV(0, 0, 0);
int current_row_ = 0;
int current_column_ = 0;

void monochromeJuggle(CRGB leds[]) {
  // four colored dots, weaving in and out of sync with each other
  FADE(20);
  for (int i = 0; i < 3; i++) {
    leds[beatsin16(i + 4, 0, NUM_LEDS - 1)] |= CHSV(
        static_color_hsv_.hue, static_color_hsv_.sat, static_color_hsv_.val);
  }
}

void monochromeGlitter(CRGB leds[]) {
  FADE(3);
  if (random8() < CHANCE_OF_GLITTER) {
    leds[random16(NUM_LEDS)] += static_color_hsv_;
  }
}

void monochromeSinelon(CRGB leds[]) {
  // Similar to animationSinelon but all lights stay on at a low level
  // with the sweep 'overlayed'
  FADE(5);
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



void setColumn(CRGB leds[], int column, CRGB color) {
  for (int i = column; i < NUM_LEDS; i += PER_ROW_LEDS) {
    int id = i;
    if (id > NUM_LEDS) continue;
    leds[id] |= static_color_hsv_;
  }
}

void setRow(CRGB leds[], int row, CRGB color) {
  for (int i = 0; i < PER_ROW_LEDS; i++) {
    int id = (row * PER_ROW_LEDS) + i;
    if (id > NUM_LEDS) continue;

    leds[id] = static_color_hsv_;
  }
}

/**
 * Light up each ring in sequence from top to bottom.
 */
void halloweenRingPulse(CRGB leds[]) {
  FADE(10);
  int row = beatsin16(60, 0, ROWS_LEDS);

  if (row == ROWS_LEDS || row < current_row_) current_row_ = row;
  else return;

  setRow(leds, current_row_, static_color_hsv_);
}

void halloweenColumnPulse(CRGB leds[]) {
  FADE(15);

  int column = beatsin16(60, 0, PER_ROW_LEDS);
  if (column == PER_ROW_LEDS || column < current_column_) current_column_ = column;
  else return;
  // EVERY_N_MILLISECONDS(100) {
    // current_column_ = (current_column_ + 1) % PER_ROW_LEDS;
  // }

  setColumn(leds, current_column_, static_color_hsv_);
}

void halloween(CRGB leds[]) {
  // halloweenRingPulse(leds);
  // halloweenColumnPulse(leds);
}

void deactivate(CRGB leds[]) {
  FADE(10);
}

}  // namespace animations
FASTLED_NAMESPACE_END
