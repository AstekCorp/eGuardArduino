#include <astekcrypto.h>

  ATCA_STATUS ret;

// the setup function runs once when you press reset or power the board
void setup() {
  char sw_rev[12];
  uint8_t sernum[ATCA_SERIAL_NUM_SIZE];
  OTPConfig config;
  uint8_t my_rand[ATCA_BLOCK_SIZE];
  uint8_t my_pubkey[ATCA_PUB_KEY_SIZE];
	
  // initialize digital pin 13 as an output and open serial port at 9600 bps.
  pinMode(13, OUTPUT);
  Serial.begin(9600);

  /************************************************************/
  // Read software revision and report results.
  /************************************************************/
  Serial.print(F("Getting Software Revision..."));
  ret = egGetRev(sw_rev);
  if (0 == ret) {
    Serial.print(F("Revision: "));
    Serial.println(sw_rev);
  } else {
    Serial.println(F("No revision data found"));
  }
  Serial.println(F("Getting Software Revision Complete"));
  
  /************************************************************/
  // Select eGuard device to use for authentication operations
  /************************************************************/
  Serial.print(F("Selecting eGuard Device..."));
  ret = egSelectDevice(cfg_eGuard);
  if (0 == ret) {
    Serial.println(F("device selected!"));
  } else {
    Serial.print(F("No device found. Returned: "));
    Serial.println(ret);
  }
  Serial.println(F("Selecting eGuard Complete"));

  /************************************************************/
  // Detect eGuard device
  /************************************************************/
  Serial.print(F("Detecting eGuard Device..."));
  ret = egDetectDevice();
  if (0 == ret) {
    Serial.println(F("device detected!"));
  } else {
    Serial.print(F("No device detected. Returned: "));
    Serial.println(ret);
  }
  Serial.println(F("Detecting eGuard Complete"));

  /************************************************************/
  // Get Serial Number from eGuard
  /************************************************************/
  Serial.print(F("Getting eGuard Serial Number..."));
  ret = egSerNum(sernum);
  if (0 == ret) {
    for (int i; i < ATCA_SERIAL_NUM_SIZE; i++ ) {
      Serial.print(sernum[i], HEX);
    }
    Serial.println("");
  } else {
    Serial.print(F("No device detected. Returned: "));
    Serial.println(ret);
  }
  Serial.println(F("Getting eGuard Serial Number Complete"));

  /************************************************************/
  // Get Configuration from eGuard
  /************************************************************/
  Serial.print(F("Getting eGuard Configuration..."));
  ret = egGetConfig((OTPConfig*)&config.OTPFormat);
  if (0 == ret) {
    for (int i; i < ATCA_BLOCK_SIZE; i++ ) {
      Serial.print((char)config.Description[i]);
    }
    Serial.println("");
  } else {
    Serial.print(F("No device detected. Returned: "));
    Serial.println(ret);
  }
  Serial.println(F("Getting eGuard Configuration Complete"));


  /************************************************************/
  // Generate Random number from eGuard
  /************************************************************/
  Serial.print(F("Generate Random Number from eGuard..."));
  ret = egGenRandom(&my_rand[0]);
  if (0 == ret) {
    for (int i; i < ATCA_BLOCK_SIZE; i++ ) {
      Serial.print(my_rand[i], HEX);
    }
    Serial.println("");
  } else {
    Serial.print(F("No device detected. Returned: "));
    Serial.println(ret);
  }
  Serial.println(F("Generate Random Number from eGuard Complete"));

  /************************************************************/
  // Get Public key for device
  /************************************************************/
  Serial.print(F("Get Public Key from eGuard..."));
  ret = egDevicePubKey(&my_pubkey[0]);
  if (0 == ret) {
    for (int i; i < ATCA_BLOCK_SIZE; i++ ) {
      Serial.print(my_pubkey[i], HEX);
    }
    Serial.println("");
  } else {
    Serial.print(F("No device detected. Returned: "));
    Serial.println(ret);
  }
  Serial.println(F("Get Public Key from eGuard Complete"));

  /************************************************************/
  // Perform symmetric Authentication
  /************************************************************/
  Serial.print(F("Perform Symmetric Authentication..."));
  ret = egAuthenticate(SYMMETRIC, NULL);
  if (0 == ret) {
    Serial.println("Authentication SUCCESS!");
  } else {
    Serial.print(F("No device detected or Authentication FAILED! Returned: "));
    Serial.println(ret);
  }
  Serial.println(F("Perform Symmetric Authentication Complete"));

}

// the loop function runs over and over again forever
void loop() {

  ret = egAuthenticate(SYMMETRIC, NULL);
  delay(100);              // wait for a 0.1 second

  Serial.println(ret, HEX);
  
  if (0 == ret) {
    digitalWrite(13, HIGH);   // turn the LED on if verification succeeds.
  } else {
    digitalWrite(13, LOW);   // turn the LED off if verification fails.
  }

  
}


