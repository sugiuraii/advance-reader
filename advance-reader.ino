#include <CustomSoftwareSerial.h>
#include "advancemsghandler.h"

CustomSoftwareSerial* customSerial;  


void setup() {
  Serial.begin(19200);
  delay(10);
  
  Serial.println("Start Setup");

  // For overflow indicator
  pinMode(13,OUTPUT);
  digitalWrite(13, LOW);

  customSerial = new CustomSoftwareSerial(7, 6); // rx, tx
  customSerial->begin(19200, CSERIAL_8E1);         // Baud rate: 19200
  delay(100);
}

void loop() {
  readSerialMain(customSerial);
}
