/*
 * ALU: 산술 논리 연산 장치
 */
#include <NeoSWSerial.h>

// 핀 정의
#define RXC 3
#define TXC 4
#define RXB 7
#define TXB 8

NeoSWSerial ctrSerial(RXC, TXC);
NeoSWSerial bSerial(RXB, TXB);

// 명령어 정의
#define CMD_A_TO_ALU   0b00011000
#define CMD_B_TO_ALU   0b00011100
#define CMD_ALU_ADD    0b00001001
#define CMD_ALU_TO_A   0b00001100

byte aluA = 0b00000000;
byte aluB = 0b00000000;
byte aluResult = 0b00000000;

void setup() {
  Serial.begin(19200);
  ctrSerial.begin(19200);
  bSerial.begin(19200);
  
  pinMode(13, OUTPUT);
  pinMode(TXC, INPUT);
  pinMode(RXB, INPUT);
  pinMode(TXB, INPUT);
  pinMode(0, INPUT);
  pinMode(1, INPUT);
  
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
    case CMD_A_TO_ALU:
      receiveFromA();
      break;
      
    case CMD_B_TO_ALU:
      receiveFromB();
      break;
      
    case CMD_ALU_ADD:
      executeAdd();
      break;
      
    case CMD_ALU_TO_A:
      sendToA();
      break;
  }
  
  ctrSerial.listen();
}

void receiveFromA() {
  while(Serial.available()) {
    Serial.read();
  }
  
  unsigned long waitStart = millis();
  while (!Serial.available() && (millis() - waitStart < 500)) {}
  
  if (Serial.available()) {
    aluA = Serial.read();
    
    digitalWrite(13, HIGH);
    delay(100);
    digitalWrite(13, LOW);
  }
}

void receiveFromB() {
  while(bSerial.available()) {
    bSerial.read();
  }
  
  bSerial.listen();
  
  unsigned long waitStart = millis();
  while (!bSerial.available() && (millis() - waitStart < 500)) {}
  
  if (bSerial.available()) {
    aluB = bSerial.read();
    
    digitalWrite(13, HIGH);
    delay(100);
    digitalWrite(13, LOW);
  }
  
  ctrSerial.listen();
}

void executeAdd() {
  aluResult = aluA + aluB;
  
  digitalWrite(13, HIGH);
  delay(200);
  digitalWrite(13, LOW);
}

void sendToA() {
  pinMode(1, OUTPUT);
  Serial.write(aluResult);
  Serial.flush();
  
  delay(50);
  pinMode(1, INPUT);
}