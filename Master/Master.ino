#include <SoftwareSerial.h>

SoftwareSerial BTSerial(2, 3); // RX | TX

#define BUTTON_1 A1
#define BUTTON_2 A2
#define BUTTON_3 A3
#define BUTTON_4 A4
#define BUTTON_5 A5

#define BUTTON_THRESHOLD 2.5

void setup() {
  Serial.begin(9600);
  BTSerial.begin(38400); // Set the baud rate to match the HC-05 setting

  pinMode(BUTTON_1, INPUT);
  pinMode(BUTTON_2, INPUT);
  pinMode(BUTTON_3, INPUT);
  pinMode(BUTTON_4, INPUT);
  pinMode(BUTTON_5, INPUT);
}

void loop() {
  static int lastButtonState = -1; // Track the last button state
  int currentButtonState = getButtonState();

  if (currentButtonState != lastButtonState) {
    lastButtonState = currentButtonState;
    BTSerial.write('0' + currentButtonState); // Converts button state to corresponding character
    // Note: '0' + 0 = '0', '0' + 1 = '1', ... this maps int to char directly based on ASCII values
  }
}

int getButtonState() {
  if (isButtonPushed(BUTTON_1)) return 1;
  if (isButtonPushed(BUTTON_2)) return 2;
  if (isButtonPushed(BUTTON_3)) return 3;
  if (isButtonPushed(BUTTON_4)) return 4;
  if (isButtonPushed(BUTTON_5)) return 5;
  return 0; // No button pushed
}


bool isButtonPushed(int button_pin) {
  if (getPinVoltage(button_pin) >= BUTTON_THRESHOLD) {
    return(true);
  }
  else {
    return (false);
  }
}

float getPinVoltage(int pin) {
  return 5 * (float)analogRead(pin) / 1024;
}
