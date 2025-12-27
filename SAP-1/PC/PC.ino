/*
 * Program Counter
 */
#include <NeoSWSerial.h>

int RXC = 3;
int TXC = 4;
int RXB = 5;
int TXB = 6;

NeoSWSerial busSerial(RXB, TXB);
NeoSWSerial ctrSerial(RXC, TXC);

const byte CMD_PC_INC     0b00000001;
const byte CMD_PC_TO_MAR  0b00000010;

byte pcValue = 0b0000;

void setup() {
  busSerial.begin(19200);
  ctrSerial.begin(19200);
  
  pinMode(TXB, INPUT);
  pinMode(13, OUTPUT);
  
  ctrSerial.listen();
  digitalWrite(13, LOW);
}

void loop() {
  if (ctrSerial.available()) {
    byte cmd = ctrSerial.read();
    
    if (cmd == CMD_PC_INC) {
      pcValue++;
      digitalWrite(13, HIGH);
      delay(200);
      digitalWrite(13, LOW);
      
    } else if (cmd == CMD_PC_TO_MAR) {
      
      
      pinMode(TXB, OUTPUT);
      busSerial.listen();
      delay(10);
      
      busSerial.write(pcValue);
      busSerial.flush();
      delay(10);
      
      pinMode(TXB, INPUT);
      ctrSerial.listen();
      
      if(pcValue == 0b0001) {
        digitalWrite(13, HIGH);
        delay(100);
        digitalWrite(13, LOW);
      }
    }
  }
}
