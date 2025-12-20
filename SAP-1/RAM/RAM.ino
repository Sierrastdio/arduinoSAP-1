/*
 * RAM: 메모리
 */
#include <NeoSWSerial.h>

#define RXC 3
#define TXC 4
#define RXB 5
#define TXB 6

NeoSWSerial busSerial(RXB, TXB);
NeoSWSerial ctrSerial(RXC, TXC);

byte ramMemory[16] = {  // 현재 LDA->LDB->ADD 실행구조 구현중. LDA는 전원이 켜지면 먼저 동작하는 기본 명령어다.
  0b00101100, //LDA에서 실행되는 부분이니까 상위 4비트가 LDB를 부르는 비트여야함.
  0b00111101,
  0b00100000,
  0b00000000, 
  0b00000000, 
  0b00000000, 
  0b00000000, 
  0b00000000,
  0b00000000, 
  0b00000000, 
  0b00000000, 
  0b00000000,
  0b00000101, //LDA에서 A에 저장될 값.
  0b00000011, //LDB에서 B에 저장될 값.
  0b00000000, 
  0b00000000
};      //즉 현재 목표는 LDB에서 ADD로 전환하는 것이다. 

#define CMD_RAM_TO_IR   0b00000100
#define CMD_RAM_TO_A    0b00000110
#define CMD_RAM_TO_B    0b00000111

byte dataToSend = 0b00000000;

void setup() {
  Serial.begin(19200);
  busSerial.begin(19200);
  ctrSerial.begin(19200);

  pinMode(13, OUTPUT);
  pinMode(TXC, INPUT);
  pinMode(RXB, INPUT);
  pinMode(TXB, INPUT);
  pinMode(0, INPUT);
  
  digitalWrite(13, LOW);
  ctrSerial.listen();
}

void loop() {
  if (Serial.available()) {
    byte address = Serial.read() & 0b00001111;
    
    if (address >= 16) address = 0;
    
    dataToSend = ramMemory[address];
    
    digitalWrite(13, HIGH);
    delay(200);
    digitalWrite(13, LOW);
  }
  
  if (ctrSerial.available()) {
    handleCommand(ctrSerial.read());
  }
}

void handleCommand(byte cmd) {
  switch(cmd) {
    case CMD_RAM_TO_IR:
    case CMD_RAM_TO_A:
    case CMD_RAM_TO_B:
      sendDataToBus();
      break;
      
    default:
      ctrSerial.listen();
      break;
  }
}

void sendDataToBus() {
  pinMode(TXB, OUTPUT);
  digitalWrite(TXB, LOW);
  delay(50);
  
  busSerial.listen();
  delay(10);
  
  busSerial.write(dataToSend);
  busSerial.flush();
  
  delay(15);
  
  digitalWrite(TXB, LOW);
  delay(5);
  pinMode(TXB, INPUT);
  
  ctrSerial.listen();
}
