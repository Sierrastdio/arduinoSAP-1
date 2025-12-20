/*
 * CTR: 클럭 상승엣지를 감지하고
 * 각 스텝에 맞는 8비트 제어 코드를 전송한다.
 */
#include <NeoSWSerial.h>

#define CLK_PIN 2
#define RXC 3
#define TXC 4

NeoSWSerial ctrSerial(RXC, TXC);

#define CMD_PC_CLEAR              0b00000000
#define CMD_PC_INC                0b00000001
#define CMD_PC_TO_MAR             0b00000010
#define CMD_MAR_TO_RAM            0b00000011
#define CMD_RAM_TO_IR             0b00000100
#define CMD_IR_TO_MAR             0b00000101
#define CMD_RAM_TO_A              0b00000110
#define CMD_RAM_TO_B              0b00000111
#define CMD_ALU_ADD               0b00001001
#define CMD_A_TO_OUT              0b00001011
#define CMD_ALU_TO_A              0b00001100
#define CMD_A_TO_ALU              0b00011000
#define CMD_B_TO_ALU              0b00011100

#define CYCLE_LDA 0b0001
#define CYCLE_LDB 0b0010
#define CYCLE_ADD 0b0011

byte stp = 0;
bool lastCLK = LOW;
byte currentCycle = CYCLE_LDA;
byte nextCycle = CYCLE_LDA;

void setup() {
  Serial.begin(19200);
  pinMode(CLK_PIN, INPUT);
  pinMode(TXC, INPUT);
  pinMode(13, OUTPUT);
  pinMode(0, INPUT);
  digitalWrite(13, LOW);

  ctrSerial.begin(19200);
  
  delay(100);
  lastCLK = digitalRead(CLK_PIN);
}

void loop() {
  if (Serial.available()) {
    byte opcode = Serial.read();
    nextCycle = (opcode >> 4) & 0b00001111;
    
    if(opcode == 0b0011){
      digitalWrite(13, HIGH);
      delay(100);
      digitalWrite(13, LOW);
    }
  }
  
  bool currentCLK = digitalRead(CLK_PIN);
  if (lastCLK == LOW && currentCLK == HIGH) {
    stp++;
    bool isCycleComplete = false;
    
    switch(currentCycle) {
      case CYCLE_LDA:
        isCycleComplete = LDAcycle(stp);
        break;
      case CYCLE_LDB:
        isCycleComplete = LDBcycle(stp);
        break;
      case CYCLE_ADD:
        isCycleComplete = ADDcycle(stp);
        break;
    }

    if (isCycleComplete) {
      stp = 0;
      currentCycle = nextCycle;
    }
  }
  lastCLK = currentCLK;
}

bool LDAcycle(byte step) {
  switch (step) {
    case 1: sendCommand(CMD_PC_TO_MAR); break;
    case 2: sendCommand(CMD_PC_INC); break;
    case 3: sendCommand(CMD_RAM_TO_IR); break;        //case5와 case6 사이에 옵코드 전송이 있음.
    case 4: sendCommand(CMD_IR_TO_MAR); break;
    case 5: sendCommand(CMD_MAR_TO_RAM); break;
    case 6: sendCommand(CMD_RAM_TO_A); break;         // 마지막 명령 실행
    case 7: return true;                               // idle 클럭 후 사이클 완료
    default: break;
  }
  return false;
}

bool LDBcycle(byte step) {
  switch (step) {
    case 1: sendCommand(CMD_PC_TO_MAR); break;
    case 2: sendCommand(CMD_PC_INC); break;
    case 3: sendCommand(CMD_RAM_TO_IR); break;      //case4와 case5 사이
    case 4: sendCommand(CMD_IR_TO_MAR); break;      
    case 5: sendCommand(CMD_MAR_TO_RAM); break;
    case 6: sendCommand(CMD_RAM_TO_B); break;        // 마지막 명령 실행
    case 7: return true;                             // idle 클럭 후 사이클 완료
    default: break;
  }
  return false;
}

bool ADDcycle(byte step) {
  switch (step) {
    case 1: sendCommand(CMD_A_TO_ALU); break;
    case 2: sendCommand(CMD_B_TO_ALU); break;
    case 3: sendCommand(CMD_ALU_ADD); break;
    case 4: sendCommand(CMD_ALU_TO_A); break;
    case 5: sendCommand(CMD_PC_TO_MAR); break;
    case 7: sendCommand(CMD_PC_INC); break;
    case 8: sendCommand(CMD_RAM_TO_IR); break;
    case 9: sendCommand(CMD_A_TO_OUT); break;       // 마지막 명령 실행
    case 10: return true;                             // idle 클럭 후 사이클 완료
    default: break;
  }
  return false;
}

void sendCommand(byte cmd) {
  pinMode(TXC, OUTPUT);
  digitalWrite(TXC, LOW);
  delay(5);
  
  ctrSerial.listen();
  delay(10);
  
  ctrSerial.write(cmd);
  ctrSerial.flush();
  
  delay(15);
  
  digitalWrite(TXC, LOW);
  delay(5);
  pinMode(TXC, INPUT);
}
