#define FASTLED_INTERNAL  // Disable pragma version message on compilation
#ifndef ANIMATIONS_H
#define ANIMATIONS_H
#include <FastLED.h>

FASTLED_USING_NAMESPACE

namespace animations {
#define CHANCE_OF_GLITTER 80
extern byte hue_;
extern CHSV static_color_hsv_;
extern float animation_speed_multiplier_;// = 1.0f;

// Animations with any color
void animationAddGlitter(CRGB leds[], fract8 chance_of_glitter = CHANCE_OF_GLITTER);
void animationBpm(CRGB leds[]);
void animationConfetti(CRGB leds[]);
void animationJuggle(CRGB leds[]);
void animationRainbow(CRGB leds[]);
void animationRainbowWithGlitter(CRGB leds[]);
void animationSinelon(CRGB leds[]);

// Monochromatic animations
void monochromeJuggle(CRGB leds[]);
void monochromeGlitter(CRGB leds[]);
void monochromeSinelon(CRGB leds[]);
void monochromePulse(CRGB leds[]);
void monochromeRainbow(CRGB leds[]);

}  // namespace animations

FASTLED_NAMESPACE_END

#endif
