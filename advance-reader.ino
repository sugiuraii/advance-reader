#include <CustomSoftwareSerial.h>

const bool DEBUG=true;

const byte INITIALIZE = 0x01;
const byte ENDING = 0x02;
const byte DATA = 0x03;

const byte INIT_STEP0 = 0x01;
const byte INIT_STEP1 = 0x11;
const byte INIT_STEP2 = 0x21;
const byte INIT_STEP3 = 0x31;

#define SIZEM(a)  (sizeof((a)) / sizeof((a[0])))
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

  }

}

void skipRead(CustomSoftwareSerial* customSerial, int bytes)
{
  for(int i = 0; i < bytes; i++)
    customSerial->read();
}

void sendData(byte dat[], int siz) 
{
  for(int i = 0; i < siz; i++) 
  {
    customSerial->write(dat[i]);
  }
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

void handleInitializeFrame(CustomSoftwareSerial* customSerial)
{
  if(DEBUG)
    Serial.println("Processing INITIALIZE message(0x01)");

  byte initMode = customSerial->read();
  switch(initMode)
  {
    case INIT_STEP0:
      if(DEBUG)
        Serial.println("INITIALIZE_STEP0.");
      break;
    case INIT_STEP1:
      if(DEBUG)
        Serial.println("INITIALIZE_STEP1.");
      break;
    case INIT_STEP2:
      if(DEBUG)
        Serial.println("INITIALIZE_STEP2.");
      break;
    case INIT_STEP3:
      if(DEBUG)
        Serial.println("INITIALIZE_STEP3.");
      break;
  }
}

void handleMeterTypeQuery(CustomSoftwareSerial* customSerial)
{
  if(DEBUG)
    Serial.println("Handle meter type query.");

  skipRead(customSerial, 20);
  byte meterID = customSerial->read();
  byte[] returnData = {0, 0xFF, 0xFF, 0x0A, 0x00, 0, 0};

  switch(meterID)
  {
    case 0x01:
      if(DEBUG)
        Serial.println("Return meter type ID 1 = 0x01(Boost).");

      retunData[4] = 0x01; // Boost
      sendData(retunData, SIZEM(retunData));
      break;
    case 0x02:
      if(DEBUG)
        Serial.println("Return meter type ID 2 = 0x06(Rev).");

      retunData[4] = 0x06; // Rev
      sendData(retunData, SIZEM(retunData));
      break;
    case 0x03:
      if(DEBUG)
        Serial.println("Return meter type ID 3 = 0x09(Oil pres).");

      retunData[4] = 0x09; // Oil pres
      sendData(retunData, SIZEM(retunData));
      break;
    case 0x04:
      if(DEBUG)
        Serial.println("Return meter type ID 4 = 0x0B(Oil temp).");

      retunData[4] = 0x0B; // Oil temp
      sendData(retunData, SIZEM(retunData));
      break;
    case 0x05:
      if(DEBUG)
        Serial.println("Return meter type ID 5 = 0x0c(Water temp).");

      retunData[4] = 0x0C; // Water temp
      sendData(retunData, SIZEM(retunData));
      break;
    default;
       if(DEBUG)
        Serial.println("Meter query type is not implemented.");
      break;
  }
}