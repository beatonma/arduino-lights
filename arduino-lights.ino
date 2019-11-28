#define LED_RED 5
#define LED_BLUE 6
#define LED_GREEN 9

byte brightness = 255;
int red = 0;
int green = 0;
int blue = 0;

void setup() {
  Serial.begin(9600);
  pinMode(LED_RED, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_BLUE, OUTPUT);
}

void loop() {
  brightness = brightness > 0 ? 0 : 255;
//  brightness += 1;
  setRed(brightness);
  logColor("r", red);
  delay(250);
}

void logColor(String name, int value) {
  Serial.println();
  Serial.print(name + " ");
  Serial.print(value);
}

void setRed(int value) {
  Serial.println(analogRead(LED_RED));
  Serial.println();
  red = value;
  analogWrite(LED_RED, value);
}
