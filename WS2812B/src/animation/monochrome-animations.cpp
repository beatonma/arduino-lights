/** @file */
#include <FastLED.h>
#include "animations.h"

FASTLED_USING_NAMESPACE
namespace animations {

#define COLUMNS PER_ROW_LEDS
#define ROWS ROWS_LEDS

CHSV static_color_hsv_ = CHSV(0, 0, 0);
int current_row_ = 0;
int current_column_ = 0;
boolean direction = true;

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

void nextRow() {
  current_row_ = (current_row_ + 1) % ROWS;
}

void nextColumn() {
  current_column_ = (current_column_ + 1) % COLUMNS;
}

void setGrid(CRGB leds[], CRGB color, int x, int y) {
  int index = (x * COLUMNS) + y;
  if (index > NUM_LEDS) return;

  leds[index] = color;
}

/**
 * Light up each ring in sequence from top to bottom.
 */
void halloweenRingPulse(CRGB leds[]) {
  FADE(15);
  int row = beatsin16(60, 0, ROWS);

  // Lock to one vertical direction.
  if (row == ROWS || row < current_row_) current_row_ = row;
  else return;

  setRow(leds, current_row_, static_color_hsv_);
}

/**
 * Light up each column in sequence.
 */
void halloweenColumnPulse(CRGB leds[]) {
  FADE(15);

  // Lock to a single rotation direction
  int column = beatsin16(60, 0, COLUMNS);
  if (column == COLUMNS || column < current_column_) current_column_ = column;
  else return;

  setColumn(leds, current_column_, static_color_hsv_);
}

void halloweenVortex(CRGB leds[]) {
  FADE(15);

  EVERY_N_MILLISECONDS(30) {
    nextRow();
  }

  EVERY_N_MILLISECONDS(60) {
    nextColumn();
  }

  setGrid(leds, static_color_hsv_, current_row_, current_column_);
}

/**
 * Each column travels the opposite direction from its neighbour.
 */
void halloweenAlternatingColumns(CRGB leds[]) {
  FADE(15);

  int rowAscending = beatsin16(30, 0, ROWS);
  int rowDescending = ROWS - rowAscending;

  for (int i = 0; i < COLUMNS; i++) {
    if (i % 2 == 0) {
      setGrid(leds, static_color_hsv_, rowAscending, i);
    }
    else {
      setGrid(leds, static_color_hsv_, rowDescending, i);
    }
  }
}

/**
 * Even columns travel one way then odd columns travel the other way.
 */
void halloweenAlternatingColumns2(CRGB leds[]) {
  FADE(15);

  int row = beatsin16(60, 0, ROWS);

  if (row == 0) {
    direction = true;
  }
  else if (row == ROWS) {
    direction = false;
  }

  current_row_ = row;

  for (int i = 0; i < COLUMNS; i++) {
    if (i % 2 == 0 && direction) {
      setGrid(leds, static_color_hsv_, row, i);
    }
    else if (i % 2 == 1 && !direction) {
      setGrid(leds, static_color_hsv_, row, i);
    }
  }
}

void deactivate(CRGB leds[]) {
  FADE(10);
}

}  // namespace animations
FASTLED_NAMESPACE_END
