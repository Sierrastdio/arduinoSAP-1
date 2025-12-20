/*
 * MAR: 메모리 주소 레지스터 (수정본)
 */
#include <NeoSWSerial.h>

#define RXC 3
#define TXC 4
#define RXB 5
#define TXB 6

NeoSWSerial busSerial(RXB, TXB);
NeoSWSerial ctrSerial(RXC, TXC);

#define CMD_PC_TO_MAR         0b00000010
#define CMD_IR_TO_MAR         0b00000101

byte marValue = 0b00000000;

void setup() {
  Serial.begin(19200); 
  
  busSerial.begin(19200);
  ctrSerial.begin(19200);

  pinMode(13, OUTPUT);
  
  pinMode(TXC, INPUT);
  pinMode(TXB, INPUT);
  
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
    case CMD_PC_TO_MAR:
    case CMD_IR_TO_MAR:
      receiveFromBus();
      break;
      
    default:
      ctrSerial.listen();
      break;
  }
}

void receiveFromBus() {
  busSerial.listen();
  
  // 버퍼 비우기 (이전 쓰레기 값 제거)
  while(busSerial.available()) {
    busSerial.read();
  }
  
  // 데이터 수신 대기 (타임아웃 루프)
  for(int i = 0; i < 500; i++) {
    if (busSerial.available()) {
      marValue = busSerial.read() & 0b00001111; // 4비트 마스킹
      
  
      if(marValue == 0b0001) {
        digitalWrite(13, HIGH);
        
        sendToRAM();
        
        delay(500); 
        digitalWrite(13, LOW);
      } else {
        // 0b0001이 아니면 그냥 전송만 하고 LED는 짧게 (혹은 안 킴)
        sendToRAM();
      }
      
      ctrSerial.listen();
      return;
    }
    delay(2);
  }
  
  // 타임아웃 시 복귀
  ctrSerial.listen();
}

void sendToRAM() {
  Serial.write(marValue);
  Serial.flush();
}
