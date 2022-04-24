#include <CustomSoftwareSerial.h>

const bool DEBUG=true;

const byte INITIALIZE = 0x01;
const byte ENDING = 0x02;
const byte DATA = 0x03;

const byte INIT_STEP0 = 0x01;
const byte INIT_STEP1 = 0x11;
const byte INIT_STEP2 = 0x21;
const byte INIT_STEP3 = 0x31;


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
