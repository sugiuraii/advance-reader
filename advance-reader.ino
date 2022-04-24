#include <CustomSoftwareSerial.h>

const bool DEBUG=true;

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
  // put your main code here, to run repeatedly:

}

void findDelimiter(CustomSoftwareSerial* customSerial)
{
  if(DEBUG)
    Serial.println("Finding delimiter");
  byte buf[] = {0,0,0};
  
  while(buf[0] == 0x00 && buf[0] == 0xFF && buf[0] == 0xFF)
  {
    buf[0]=buf[1];
    buf[1]=buf[2];
    buf[2]=customSerial->read();  
  }
  if(DEBUG)
    Serial.println("Delimiter is found.");
}
