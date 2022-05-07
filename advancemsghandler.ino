#include <CustomSoftwareSerial.h>

const bool DEBUG=false;

// State flag
const byte SEEK_DELIMITER = 0x01;
const byte READ_COMMANDBYTE = 0x02;
const byte READ_CONTENTS = 0x03;

// Command byte
const byte INITIALIZE = 0x01;
const byte ENDING = 0x02;
const byte DATA = 0x03;
// STEP byte of INITIALIZE
const byte INIT_STEP0 = 0x01;
const byte INIT_STEP1 = 0x11;
const byte INIT_STEP2 = 0x21;
const byte INIT_STEP3 = 0x31;

#define SIZEM(a)  (sizeof((a)) / sizeof((a[0])))

void readSerialMain(CustomSoftwareSerial* ser)
{
  static byte state = SEEK_DELIMITER;
  static byte commandByte = 0x00;

  uint8_t serbufcnt = ser->available();
  if(serbufcnt > 0) {
    if(ser->overflow()) {
      // Blink 'L' LED connected to pin13
      digitalWrite(13, HIGH);
      digitalWrite(13, LOW);
      
      // Clear buffer.
      while(ser->read() >= 0);
      state = SEEK_DELIMITER;
      commandByte = 0x00;
      return;
    }
    else {
      switch(state)
      {
        case SEEK_DELIMITER:
          if(seekDelimiter(ser))
            state = READ_COMMANDBYTE;
          break;
        case READ_COMMANDBYTE:
          commandByte = ser->read();
          state = READ_CONTENTS;
          break;
        case READ_CONTENTS:
          if(readContents(ser, serbufcnt, commandByte))
            state=SEEK_DELIMITER;
          break;
      }
    }
  }
}

bool seekDelimiter(CustomSoftwareSerial* ser)
{
  static byte buf[] = {0, 0, 0};

  if(DEBUG)
    Serial.println("Finding delimiter");
  
    buf[0]=buf[1];
    buf[1]=buf[2];
    buf[2]=customSerial->read();

    if(buf[0] == 0x00 && buf[1] == 0xFF && buf[2] == 0xFF) {
      if(DEBUG)
        Serial.println("Delimiter is found.");
      return true;
    }
  else
    return false;
}

bool readContents(CustomSoftwareSerial* ser, uint8_t serbufcnt, byte comandByte)
{
  switch(comandByte) {
    case INITIALIZE:
      if(serbufcnt >= 22) {
          byte content[22];
          readData(ser, content, 22);
          handleInitializeFrame(ser, content);
          return true;
      }
      else
        return false;
      break;
    case ENDING:
      return true; // Do nothing on ENDING command.
      break;
    case DATA:
      if(serbufcnt >= 30) {
        byte content[30];
        readData(ser, content, 30);
        handleDataFrame(ser, content);
        Serial.println(ser->available());
        return true;
      }
      else
        return false;
  }
}

void handleInitializeFrame(CustomSoftwareSerial* ser, byte* frameContent)
{
  if(DEBUG)
    Serial.println("Processing INITIALIZE message(0x01)");

  byte initMode = frameContent[0];
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
        handleMeterTypeQuery(ser, frameContent);
      break;
    case INIT_STEP3:
      if(DEBUG)
        Serial.println("INITIALIZE_STEP3.");
      break;
  }
}

void handleMeterTypeQuery(CustomSoftwareSerial* ser, byte* frameContent)
{
  if(DEBUG)
    Serial.println("Handle meter type query.");

  byte meterID = frameContent[21];
  byte returnData[] = {0, 0xFF, 0xFF, 0x0A, 0x00, 0, 0};

  switch(meterID)
  {
    case 0x01:
      if(DEBUG)
        Serial.println("Return meter type ID 1 = 0x01(Boost).");

      returnData[4] = 0x01; // Boost
      sendData(ser, returnData, SIZEM(returnData));
      break;
    case 0x02:
      if(DEBUG)
        Serial.println("Return meter type ID 2 = 0x06(Rev).");

      returnData[4] = 0x06; // Rev
      sendData(ser, returnData, SIZEM(returnData));
      break;
    case 0x03:
      if(DEBUG)
        Serial.println("Return meter type ID 3 = 0x09(Oil pres).");

      returnData[4] = 0x09; // Oil pres
      sendData(ser, returnData, SIZEM(returnData));
      break;
    case 0x04:
      if(DEBUG)
        Serial.println("Return meter type ID 4 = 0x0B(Oil temp).");

      returnData[4] = 0x0B; // Oil temp
      sendData(ser, returnData, SIZEM(returnData));
      break;
    case 0x05:
      if(DEBUG)
        Serial.println("Return meter type ID 5 = 0x0c(Water temp).");

      returnData[4] = 0x0C; // Water temp
      sendData(ser, returnData, SIZEM(returnData));
      break;
    default:
       if(DEBUG)
        Serial.println("Meter query type is not implemented.");
      break;
  }
}

void handleDataFrame(CustomSoftwareSerial* ser, byte* frameContent) {
  uint16_t rev = (frameContent[11] << 8) + frameContent[10];
  uint16_t boost = (frameContent[13] << 8) + frameContent[12];
  uint16_t oilpres = (frameContent[15] << 8) + frameContent[14];
  uint16_t fuelpres = (frameContent[17] << 8) + frameContent[16];
  uint16_t oiltemp = (frameContent[19] << 8) + frameContent[18];
  uint16_t watertemp = (frameContent[21] << 8) + frameContent[20];
  uint16_t exttemp = (frameContent[23] << 8) + frameContent[22];
  sendDataFrameToHost('A', rev);
  sendDataFrameToHost('B', boost);
  sendDataFrameToHost('C', oilpres);
  sendDataFrameToHost('D', fuelpres);
  sendDataFrameToHost('E', oiltemp);
  sendDataFrameToHost('F', watertemp);
  sendDataFrameToHost('G', exttemp);
}

void sendDataFrameToHost(char header, uint16_t value)
{
  byte obuf[5];
  obuf[0] = header;
  for(uint8_t i = 0; i < 4; i++) {
    uint8_t shift = (3 - i) * 4;
    uint16_t mask = 0x000F << shift;
    byte outval = (value & mask) >> shift;
    if(outval < 10)
      obuf[i+1] = outval + 0x30;
    else
      obuf[i+1] = outval - 10 + 0x61;
  }
  Serial.write(obuf, 5);
}

void sendData(CustomSoftwareSerial* ser,  byte dat[], int siz) 
{
  for(int i = 0; i < siz; i++) {
    ser->write(dat[i]);
  }
}

void readData(CustomSoftwareSerial* ser, byte* dat, int siz)
{
  for(int i = 0; i < siz; i++)
    dat[i] = ser->read();
}
