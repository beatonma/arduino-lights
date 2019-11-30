#define DEBUG true

// Pins labelled as seen on elegoo arduino uno
const byte LED_RED = 9; // D9
const byte LED_BLUE = 10; // D10
const byte LED_GREEN = 11; // D11

const byte UPDATE_DELAY_MS = 1500;

/**
 * Color RGB definitions, assuming full brightness.
 */
const byte RED[3] = {255, 0, 0};
const byte GREEN[3] = {0, 255, 0};
const byte BLUE[3] = {0, 0, 255};

const byte PURPLE[3] = {255, 0, 255};
const byte CYAN[3] = {0, 255, 255};
const byte YELLOW[3] = {255, 255, 0};

const byte ORANGE[3] = {255, 10, 0};
const byte PINK[3] = {255, 0, 10};

const byte WHITE[3] = {255, 255, 255};
const byte OFF[3] = {0, 0, 0};

// Array of pointers to color constants
const int COLORS[] = {
  &RED,
  &GREEN,
  &BLUE,
  &PURPLE,
  &CYAN,
  &YELLOW,
  &ORANGE,
  &PINK,
  &WHITE
};

byte brightness_ = 255;
byte red_ = 0;
byte green_ = 0;
byte blue_ = 0;

int index_ = 0;


void setup() {
  Serial.begin(9600);
  printDebugHeader();
  pinMode(LED_RED, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_BLUE, OUTPUT);

  Serial.print("DELAY:");
  Serial.print(UPDATE_DELAY_MS);

  Serial.println();
  Serial.println("Setup complete!");
}

void loop() {
  setColorIndex(index_ % 9);
  index_ += 1;

  delay(UPDATE_DELAY_MS);
}

void setColorIndex(int index) {
  byte *addr = COLORS[index];
  const byte r = *addr;
  const byte g = *(addr + 1);
  const byte b = *(addr + 2);
  const byte rgb[3] = {r, g, b};
  setRgb(rgb);
}

void setRgb(byte* rgb) {
  red_ = rgb[0];
  green_ = rgb[1];
  blue_ = rgb[2];

  printDebugColor(red_, green_, blue_);

  analogWrite(LED_RED, red_);
  analogWrite(LED_GREEN, green_);
  analogWrite(LED_BLUE, blue_);
}

void setColor(byte rgb, byte brightness) {
  setRgb(rgb);
}

void setOff() {
  setRgb(OFF);
}

void printDebugColor(byte r, byte g, byte b) {
  if (DEBUG) {
    Serial.print(red_);
    Serial.print(", ");
    Serial.print(green_);
    Serial.print(", ");
    Serial.print(blue_);
    Serial.println();
  }
}

void printDebugHeader() {
  if (DEBUG) {
    Serial.println();
    Serial.println();
    Serial.println("-----");
    Serial.println("START");
    Serial.println("-----");
    Serial.println();
    Serial.println();
    Serial.print("Using LED pins: ");
    Serial.print(LED_RED);
    Serial.print(", ");
    Serial.print(LED_BLUE);
    Serial.print(", ");
    Serial.print(LED_GREEN);
    Serial.println();
  }
}