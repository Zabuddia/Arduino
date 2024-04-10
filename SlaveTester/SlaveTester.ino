#include <SoftwareSerial.h>

SoftwareSerial BTSerial(0, 1); // RX | TX

void setup() {
  Serial.begin(9600);
  
  BTSerial.begin(38400); // Set the baud rate to match the HC-05 setting
}

void loop() {
  if (BTSerial.available()) {
    Serial.write(BTSerial.read());
  }
}
