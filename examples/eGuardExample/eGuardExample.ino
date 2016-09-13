#include <astekcrypto.h>


// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin 13 as an output.
  pinMode(13, OUTPUT);
  crypto_init();
  Serial.begin(9600);      // open the serial port at 9600 bps:
}

// the loop function runs over and over again forever
void loop() {
  int ret = 0;
  
  ret = crypto_boreclean(&cfg_508a_borecleaner);
  Serial.println("test");
  Serial.print(ret);
  
  if (0 != ret) {
    digitalWrite(13, HIGH);   // turn the LED on (HIGH is the voltage level)
    delay(1000);              // wait for a second
    digitalWrite(13, LOW);    // turn the LED off by making the voltage LOW
    delay(1000);              // wait for a second
  }
//  crypto_authenticate_sw(&cfg_508a_host);
  
}
