/** @file
 * Hardware configuration
 */
#ifndef HARDWARE_CONFIG_H
#define HARDWARE_CONFIG_H
#include <FastLED.h>

FASTLED_USING_NAMESPACE
#define LED_TYPE WS2812B
#define COLOR_ORDER GRB

/**
 * Available values for temperature/correction can be found at
 * https://github.com/FastLED/FastLED/blob/master/color.h
 */
#define COLOR_CORRECTION UncorrectedColor

/**
 * Available values for temperature/correction can be found at
 * https://github.com/FastLED/FastLED/blob/master/color.h
 */
#define COLOR_TEMPERATURE UncorrectedTemperature
// #define COLOR_CORRECTION TypicalPixelString
// #define COLOR_TEMPERATURE ClearBlueSky
FASTLED_NAMESPACE_END

#define SERIAL_BAUD_RATE 9600

#define NUM_LEDS 42

// For halloween
#define ROWS_LEDS 6
#define PER_ROW_LEDS 8

#define DATA_PIN 4
#define BRIGHTNESS_POT_PIN 0
#define MODE_BUTTON_PIN 9
#define OPTION_BUTTON_PIN 8
#define PROXIMITY_DETECTOR_PIN 3

#define MAX_BRIGHTNESS 200
#define MIN_BRIGHTNESS 5

#define ARDUINO

#endif
