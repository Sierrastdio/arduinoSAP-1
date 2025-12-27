/*
 *  A Register
 */
#include <NeoSWSerial.h>

int RXB = 5;
int TXB = 6;
int RXC = 3;
int TXC = 4;

NeoSWSerial busSerial(RXB, TXB);
NeoSWSerial ctrSerial(RXC, TXC);

const byte CMD_RAM_TO_A   0b00000110;
const byte CMD_A_TO_ALU   0b00011000;
const byte CMD_ALU_TO_A   0b00001100;
const byte CMD_A_TO_OUT   0b00001011;

byte aValue = 0b00000000;

void setup() {
  Serial.begin(19200);
  busSerial.begin(19200);
  ctrSerial.begin(19200);
  
  pinMode(13, OUTPUT);
  pinMode(RXB, INPUT);
  pinMode(TXB, INPUT);
  pinMode(TXC, INPUT);
  pinMode(1, INPUT);
  pinMode(0, INPUT);
  
  digitalWrite(13, LOW);
  ctrSerial.listen();
}

void loop() {
  if (ctrSerial.available()) {
    handleCommand(ctrSerial.read());
  }
}

void handleCommand(byte cmd) {
  switch(cmd) {
    case CMD_RAM_TO_A:
      receiveFromBus();
      break;
      
    case CMD_ALU_TO_A:
      receiveFromALU();
      break;
      
    case CMD_A_TO_ALU:
      sendToALU();
      break;
      
    case CMD_A_TO_OUT:
      sendToBus();
      break;
  }
  
  ctrSerial.listen();
}

void receiveFromBus() {
  while(busSerial.available()) {
    busSerial.read();
  }
  
  busSerial.listen();
  
  for(int i = 0; i < 300; i++) {
    if (busSerial.available()) {
      aValue = busSerial.read();
      
      if (aValue == 5) {
        digitalWrite(13, HIGH);
        delay(100);
        digitalWrite(13, LOW);
      } else if (aValue == 3) {
        for(int j=0; j<2; j++) {
          digitalWrite(13, HIGH);
          delay(100);
          digitalWrite(13, LOW);
          delay(100);
        }
      } else if (aValue == 8) {
        digitalWrite(13, HIGH);
        delay(1000);
        digitalWrite(13, LOW);
      } else {
        byte blinkCount = aValue;
        if (blinkCount > 5) blinkCount = 5;
        for(int j=0; j<blinkCount; j++) {
          digitalWrite(13, HIGH);
          delay(100);
          digitalWrite(13, LOW);
          delay(100);
        }
      }
      
      ctrSerial.listen();
      return;
    }
    delay(10);
  }
  
  ctrSerial.listen();
}

void receiveFromALU() {
  while(Serial.available()) {
    Serial.read();
  }
  
  for(int i = 0; i < 300; i++) {
    if (Serial.available()) {
      aValue = Serial.read();
      
      digitalWrite(13, HIGH);
      delay(200);
      digitalWrite(13, LOW);
      
      return;
    }
    delay(10);
  }
}

void sendToALU() {
  pinMode(1, OUTPUT);
  Serial.write(aValue);
  Serial.flush();
  
  delay(50);
  pinMode(1, INPUT);
}

void sendToBus() {
  pinMode(TXB, OUTPUT);
  digitalWrite(TXB, LOW);
  delay(5);
  
  busSerial.listen();
  delay(10);
  
  busSerial.write(aValue);
  busSerial.flush();
  
  delay(15);
  
  digitalWrite(TXB, LOW);
  delay(5);
  pinMode(TXB, INPUT);
}