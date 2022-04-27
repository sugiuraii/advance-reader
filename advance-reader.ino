#include <CustomSoftwareSerial.h>

CustomSoftwareSerial* customSerial;  


void setup() {
  Serial.begin(19200);
  delay(10);
  
  Serial.println("Start Setup");

  customSerial = new CustomSoftwareSerial(7, 6); // rx, tx
  customSerial->begin(19200, CSERIAL_8E1);         // Baud rate: 19200
  delay(100);
}

void loop() {
  // Scan until finding delimiter.
  findDelimiter(customSerial);
  byte command = customSerial->read();
  switch(command)
  {
    case INITIALIZE:
      handleInitializeFrame(customSerial);
      break;
  }

}

