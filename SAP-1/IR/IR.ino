/*
 * Instruction Register
 */
#include <NeoSWSerial.h>

int RXB = 5;
int TXB = 6;
int RXC = 3;
int TXC = 4;

NeoSWSerial busSerial(RXB, TXB);
NeoSWSerial ctrSerial(RXC, TXC);

const byte CMD_RAM_TO_IR 0b00000100;
const byte CMD_IR_TO_MAR 0b00000101;

byte instruction = 0b00000000;
byte opcode = 0b00000000;
byte address = 0b00000000;
byte lastInstruction = 0b11111111;  // 이전 instruction 값 저장

void setup() {
  Serial.begin(19200);
  busSerial.begin(19200);
  ctrSerial.begin(19200);
  
  pinMode(13, OUTPUT);
  pinMode(TXB, INPUT);
  pinMode(TXC, INPUT);
  pinMode(RXB, INPUT);
  pinMode(1, INPUT);
  
  digitalWrite(13, LOW);
  ctrSerial.listen();
}

void loop() {
  // instruction 값이 변경되면 즉시 CTR에 opcode 전송
  if (instruction != lastInstruction) {
    sendOpcodeToCTR();
    lastInstruction = instruction;
  }
  
  if (ctrSerial.available()) {
    handleCommand(ctrSerial.read());
  }
}

void handleCommand(byte cmd) {
  switch(cmd) {
    case CMD_RAM_TO_IR:
      receiveFromBus();
      break;
      
    case CMD_IR_TO_MAR:
      sendToMAR();
      break;
      
    default:
      ctrSerial.listen();
      break;
  }
}

void receiveFromBus() {
  while(busSerial.available()) {
    busSerial.read();
  }
  
  busSerial.listen();
  
  for(int i = 0; i < 300; i++) {
    if (busSerial.available()) {
      instruction = busSerial.read();
      if(instruction == 0b00111101){
        digitalWrite(13, HIGH);
      }
      
      opcode = (instruction >> 4) & 0b00001111;
      address = instruction & 0b00001111;
      
      // CMD_RAM_TO_IR 받을 때마다 무조건 CTR에 전송
      sendOpcodeToCTR();
      lastInstruction = instruction;
      
      ctrSerial.listen();
      return;
    }
    delay(10);
  }
  
  ctrSerial.listen();
}

void sendToMAR() {
  pinMode(TXB, OUTPUT);
  digitalWrite(TXB, LOW);
  delay(50);
  
  busSerial.listen();
  delay(10);
  
  busSerial.write(address);
  busSerial.flush();
  
  delay(15);
  
  digitalWrite(TXB, LOW);
  delay(5);
  pinMode(TXB, INPUT);
  
  ctrSerial.listen();
}

void sendOpcodeToCTR() {
  byte dataToSend = (opcode << 4);
  Serial.write(dataToSend);
  Serial.flush();
}
