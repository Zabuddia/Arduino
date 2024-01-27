#define LEDPIN 13
void setup()
{
  pinMode(LEDPIN, OUTPUT);
}

void loop()
{
  digitalWrite(LEDPIN, HIGH);   // Turn on the LED
  delay(1000);              // Wait for one second
  digitalWrite(LEDPIN, LOW);    // Turn off the LED  
  delay(1000); 
  digitalWrite(LEDPIN, HIGH);   // Turn on the LED
  delay(1000);              // Wait for one second
  digitalWrite(LEDPIN, LOW);    // Turn off the LED  
  delay(1000); // Wait for one second
}
