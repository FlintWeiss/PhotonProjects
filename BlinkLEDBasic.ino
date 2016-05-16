int led = D7;
int loopCount = 0;

void setup() {
   pinMode(led, OUTPUT);
   Serial.begin(9600);
}

void loop() {
   
   int i; 
   loopCount++;
   for(i=0; i<=3; i++) {
      digitalWrite(led, HIGH);
      delay(1000);
      digitalWrite(led, LOW);
      delay(1000);
   }
   
   blinkLED(led, 10);
   Serial.print("end of loop: ");
   Serial.println(loopCount);
}// end loop



//==========================================================================
void blinkLED(int pin, int nTimes) {
  digitalWrite(pin, LOW);
  for(int i=0; i<=nTimes; i++) {
    digitalWrite(pin, HIGH);
    delay(300);
    digitalWrite(pin, LOW);
    delay(300);
  }

} // end blinkLED
