/** @file */
#define FASTLED_INTERNAL  // Disable pragma version message on compilation
#ifndef ANIMATIONS_H
#define ANIMATIONS_H
#include <FastLED.h>
#include "../../hardware-config.h"
#define DEBUG
#include "../../debug.h"

FASTLED_USING_NAMESPACE

#define FADE(A) fadeToBlackBy(leds, NUM_LEDS, A)

namespace animations {
#define CHANCE_OF_GLITTER 80
#define GLITTER_COLOR CRGB::Pink
extern uint8_t hue_;
extern CHSV color_hsv_;
extern CHSV alt_color_hsv_;
extern boolean multicolor_;
extern float animation_speed_multiplier_;
extern CRGBPalette16 palette_;
extern uint8_t transition_progress_;

void addGlitter(CRGB leds[], CRGB glitter_color = GLITTER_COLOR, fract8 chance_of_glitter = CHANCE_OF_GLITTER);

// Halloween infinity mirror tunnel animations.
void halloweenRingPulse(CRGB leds[]);
void halloweenColumnPulse(CRGB leds[]);
void halloweenVortex(CRGB leds[]);
void halloweenAlternatingColumns(CRGB leds[]);
void halloweenAlternatingColumns2(CRGB leds[]);
void halloweenFullPulse(CRGB leds[]);
void deactivate(CRGB leds[]);

// Animations with any color
void polychromeBpm(CRGB leds[]);
void polychromeConfetti(CRGB leds[]);
void polychromeJuggle(CRGB leds[]);
void polychromeRainbow(CRGB leds[]);
void polychromeRainbowWithGlitter(CRGB leds[]);
void polychromeSinelon(CRGB leds[]);
void polychromeStorm(CRGB leds[]);   // Lightning
void polychromeSplash(CRGB leds[]);  // Spread out from central point
void monochromeRainbow(CRGB leds[]);

// Monochromatic animations
void monochromeJuggle(CRGB leds[]);
void monochromeGlitter(CRGB leds[]);
void monochromeSinelon(CRGB leds[]);
void monochromePulse(CRGB leds[]);

// Palette animations
void paletteFlow(CRGB leds[]);
void paletteFlowWithGlitter(CRGB leds[]);
void paletteGlitter(CRGB leds[]);

// Transitional animations
void transitionFadeToSolid(CRGB leds[], CRGB target_color);
void transitionLinearToSolid(CRGB leds[], CRGB target_color);

}  // namespace animations

FASTLED_NAMESPACE_END

#endif
