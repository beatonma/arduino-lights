#define FASTLED_INTERNAL // Disable pragma version message on compilation
#include <FastLED.h>
#include "WS2812B.h"
#include "input-buttons.cpp"
FASTLED_USING_NAMESPACE

// FastLED "100-lines-of-code" demo reel, showing just a few
// of the kinds of animation patterns you can quickly and easily
// compose using FastLED.
//
// This example also shows one easy way to define multiple
// animations patterns and have them automatically rotate.
//
// -Mark Kriegsman, December 2014
//
//
// Edited/extended by Michael Beaton, December 2019
// - Implemented modes:
//   - Static: stay on a single color
//   - Animated: use the animations from the example
//   - Mixed: animate some pixels but keep most fixed to a color.
// - Accepts input from potentiometer to change brightness.
// 
// TODO:
// - Change color/animation when next button is pressed.
// - Change mode when mode button is pressed.
// 


#if defined(FASTLED_VERSION) && (FASTLED_VERSION < 3001000)
#warning "Requires FastLED 3.1 or later; check github for latest code."
#endif

#define DATA_PIN    9
#define LED_TYPE    WS2811
#define COLOR_ORDER GRB
#define NUM_LEDS    150
#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))

#define NUM_MODES 3
#define BRIGHTNESS_POT_PIN 3
#define MODE_BUTTON_PIN 6
#define NEXT_BUTTON_PIN 5

// Available values for temperature/correction can be found at https://github.com/FastLED/FastLED/blob/master/color.h
#define COLOR_CORRECTION TypicalPixelString
#define COLOR_TEMPERATURE ClearBlueSky



CRGB leds[NUM_LEDS];

const byte framesPerSecond_ = 120;

enum Brightness: byte {Low = 10, Medium = 127, High = 255};
enum Mode: byte {
  Static = 0,             // All lights same color, no animation
  MonochromeAnimated = 1,  // All lights same base hue with animation
  Animated = 2,           // Animations with any color
};

byte mode_ = Mode::Animated;
byte mixedModeAnimatedPixelFrequency = 4; // 1 in n pixels will be animated
byte brightness_ = Brightness::High;
int brightnessPotValue_ = 0; // Position of potentiometer that controls brightness


byte pattern_index_ = 0; // Index of current pattern
byte monochrome_pattern_index_ = 0; // Index of current monochrome pattern
int hue_ = 0; // rotating "base color" used by many of the patterns
byte static_color_index_ = 0;


class ModeButtonHandler: public AbstractButtonHandler {
public:
  ModeButtonHandler(byte pin): AbstractButtonHandler(pin) {

  }

  void onButtonPressed() {
    switch (mode_) {
      case Mode::Static:
        nextStaticColor();
        break;
      case Mode::MonochromeAnimated:
        nextMonochromePattern();
        break;
      case Mode::Animated:
        nextPattern();
        break;
    }
  }

  void onLongPress() {
    nextMode();
    Serial.println(mode_);
  }

  void onLongPressHeld() {
    if (mode_ == Mode::MonochromeAnimated) {
      hue_++;
    }
  }
};


// Input handlers
ModeButtonHandler modeButtonHandler(MODE_BUTTON_PIN);

// List of patterns to cycle through.  Each is defined as a separate function below.
typedef void (*SimplePatternList[])();
const SimplePatternList patterns_ = {
  rainbow,
  rainbowWithGlitter,
  confetti,
  sinelon,
  juggle,
  bpm,
};
const SimplePatternList monochrome_patterns_ = {
  confetti,
  sinelon,
  juggle,
  bpm,
};

// Available color definitions can be found at https://github.com/FastLED/FastLED/blob/master/pixeltypes.h
const CRGB::HTMLColorCode colors_[] = {
  CRGB::Purple,
  CRGB::White,
  CRGB::Red,
  CRGB::OrangeRed,
  CRGB::Yellow,
  CRGB::Green,
  CRGB::Cyan,
  CRGB::Blue,
};


void setup() {
  Serial.begin(9600);
  Serial.println("Starting...");

  delay(3000); // 3 second delay for recovery

  setupInputHandlers();

  // tell FastLED about the LED strip configuration
  FastLED.addLeds<LED_TYPE,DATA_PIN,COLOR_ORDER>(leds, NUM_LEDS).setCorrection(COLOR_CORRECTION);
  FastLED.setTemperature(COLOR_TEMPERATURE);
  Serial.println("OK GO");
}

void loop() {
  updateInputHandlers();

  // EVERY_N_MILLISECONDS(20) {
  //   // slowly cycle the "base color" through the rainbow
  //   hue_++;
  // }
  // EVERY_N_SECONDS(10) {
  //   // change patterns periodically
  //   nextPattern();
  // }
  // EVERY_N_SECONDS(3) {
  //   nextStaticColor();
  // }
  // EVERY_N_SECONDS(5) {
  //   mode_ = !mode_;
  // }

  switch (mode_) {
    case Mode::Static:
      staticColor();
      break;
    case Mode::MonochromeAnimated:
      monochrome_patterns_[monochrome_pattern_index_]();
      break;
    case Mode::Animated:
      hue_++;
      patterns_[pattern_index_]();
      break;
  }

  draw();
}

void draw() {
  FastLED.setBrightness(brightness_);
  FastLED.show();
  FastLED.delay(1000 / framesPerSecond_);
}

void setupInputHandlers() {
  modeButtonHandler.setup();
}

void updateInputHandlers() {
  modeButtonHandler.update();

  brightnessPotValue_ = analogRead(BRIGHTNESS_POT_PIN);

  // Handle fuzziness at extreme positions to avoid flickering.
  if (brightnessPotValue_ < 20) {
    brightness_ = 0;
  }
  else if (brightnessPotValue_ > 1010) {
    brightness_ = 255;
  }
  else {
    brightness_ = map(brightnessPotValue_, 0, 1023, 0, 255);
  }
}

void nextPattern() {
  pattern_index_ = (pattern_index_ + 1) % ARRAY_SIZE(patterns_);
}

void nextMonochromePattern() {
  monochrome_pattern_index_ = (monochrome_pattern_index_ + 1) % ARRAY_SIZE(monochrome_patterns_);
}

void nextStaticColor() {
  static_color_index_ = (static_color_index_ + 1) % ARRAY_SIZE(colors_);
}

void nextMode() {
  mode_ = (mode_ + 1) % NUM_MODES;
}

void rainbow() {
  // FastLED's built-in rainbow generator
  fill_rainbow(leds, NUM_LEDS, hue_, 7);
}

void rainbowWithGlitter() {
  // built-in FastLED rainbow, plus some random sparkly glitter
  rainbow();
  addGlitter(80);
}

void addGlitter(fract8 chanceOfGlitter) {
  if(random8() < chanceOfGlitter) {
    leds[ random16(NUM_LEDS) ] += CRGB::Pink;
  }
}

void confetti() {
  // random colored speckles that blink in and fade smoothly
  fadeToBlackBy(leds, NUM_LEDS, 10);
  int pos = random16(NUM_LEDS);
  leds[pos] += CHSV(hue_ + random8(64), 200, 255);
}

void sinelon() {
  // a colored dot sweeping back and forth, with fading trails
  fadeToBlackBy(leds, NUM_LEDS, 20);
  int pos = beatsin16(13, 0, NUM_LEDS-1 );
  leds[pos] += CHSV(hue_, 255, 192);
}

void bpm() {
  // colored stripes pulsing at a defined Beats-Per-Minute (BPM)
  byte BeatsPerMinute = 62;
  CRGBPalette16 palette = PartyColors_p;
  byte beat = beatsin8(BeatsPerMinute, 64, 255);
  for (int i = 0; i < NUM_LEDS; i++) { //9948
    leds[i] = ColorFromPalette(palette, hue_+(i*2), beat-hue_+(i*10));
  }
}

void staticColor() {
  for (int dot = 0; dot < NUM_LEDS; dot++) {
    leds[dot] = colors_[static_color_index_];
  }
}

void juggle() {
  // eight colored dots, weaving in and out of sync with each other
  fadeToBlackBy(leds, NUM_LEDS, 20);
  byte dothue = 0;
  for (int i = 0; i < 8; i++) {
    leds[beatsin16(i+7, 0, NUM_LEDS-1 )] |= CHSV(dothue, 200, 255);
    dothue += 32;
  }
}
