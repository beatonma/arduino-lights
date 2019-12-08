#ifndef HARDWARE_CONFIG_H
#define HARDWARE_CONFIG_H
#include <FastLED.h>

// Hardware configuration

FASTLED_USING_NAMESPACE
#define LED_TYPE WS2811
#define COLOR_ORDER GRB

// Available values for temperature/correction can be found at
// https://github.com/FastLED/FastLED/blob/master/color.h
#define COLOR_CORRECTION UncorrectedColor
#define COLOR_TEMPERATURE UncorrectedTemperature
FASTLED_NAMESPACE_END

#define SERIAL_BAUD_RATE 115200

#define NUM_LEDS 150

#define DATA_PIN 4
#define BRIGHTNESS_POT_PIN 0
#define MODE_BUTTON_PIN 9
#define OPTION_BUTTON_PIN 8

#define MAX_BRIGHTNESS 255
#define MIN_BRIGHTNESS 5

#endif
