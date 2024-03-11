#define PHOTODIODE_LIGHT_THRESHOLD 3

float getPinVoltage(int pin) {
  return 5 * (float)analogRead(pin) / 1024;
}

bool isLight(int pin) {
  float light = getPinVoltage(pin);
  Serial.println(light); // Use this line to test
  return (light > PHOTODIODE_LIGHT_THRESHOLD);
}

void setup() {
  Serial.begin(9600);
  pinMode(6, OUTPUT);
  pinMode(A1, INPUT);
}

void loop() {
  if (isLight(A1)) {
    digitalWrite(6, HIGH);
  } else {
    digitalWrite(6, LOW);
  }
}
