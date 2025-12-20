/*
 * B: B 레지스터
 */
#include <NeoSWSerial.h>

#define RXB 5
#define TXB 6
#define RXC 3
#define TXC 4
#define RXA 7
#define TXA 8

NeoSWSerial busSerial(RXB, TXB);
NeoSWSerial ctrSerial(RXC, TXC);
NeoSWSerial bSerial(RXA, TXA);

#define CMD_RAM_TO_B   0b00000111
#define CMD_B_TO_ALU   0b00011100

byte bValue = 0b00000000;

void setup() {
  busSerial.begin(19200);
  ctrSerial.begin(19200);
  bSerial.begin(19200);
  
  pinMode(13, OUTPUT);
  pinMode(RXB, INPUT);
  pinMode(TXB, INPUT);
  pinMode(TXC, INPUT);
  pinMode(RXA, INPUT);
  pinMode(TXA, INPUT);
  
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
    case CMD_RAM_TO_B:
      receiveFromBus();
      break;
      
    case CMD_B_TO_ALU:
      sendToALU();
      break;
  }
}

void receiveFromBus() {
  while(busSerial.available()) {
    busSerial.read();
  }
  
  busSerial.listen();
  delay(500);
  
  for(int i = 0; i < 300; i++) {
    if (busSerial.available()) {
      bValue = busSerial.read();
      
      if(bValue = 0b00000011) {
        digitalWrite(13, HIGH);
        delay(200);
        digitalWrite(13, LOW);
      }
      
      ctrSerial.listen();
      return;
    }
    delay(10);
  }
  
  ctrSerial.listen();
}

void sendToALU() {
  pinMode(TXA, OUTPUT);
  digitalWrite(TXA, LOW);
  delay(50);
  
  bSerial.listen();
  delay(10);
  
  bSerial.write(bValue);
  bSerial.flush();
  digitalWrite(13, HIGH);
  
  delay(15);
  
  digitalWrite(TXA, LOW);
  delay(5);
  pinMode(TXA, INPUT);
  
  ctrSerial.listen();
  delay(200);
  digitalWrite(13, LOW);
}
