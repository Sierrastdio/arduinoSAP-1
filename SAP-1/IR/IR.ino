/*
 * IR: Instruction Register
 */
#include <avr/io.h>
#include <util/delay.h>
#include <NeoSWSerial.h>

#define RXB PD5
#define TXB PD6
#define RXC PD3
#define TXC PD4

NeoSWSerial busSerial(RXB, TXB);
NeoSWSerial ctrSerial(RXC, TXC);

#define SB_SERIAL_HIGH_Z \
    DDRB  &= ~((1 << TXB) | (1 << RXB)); \
    PORTB &= ~((1 << TXB) | (1 << RXB));

#define SB_SERIAL_OUTPUT \
    DDRD  |= ((1 << TXB) | (1 << RXB)); \
    PORTD &= ~((1 << TXB) | (1 << RXB));

#define SB_SERIAL_TXB_LOW PORTD &= ~(1 << TXB);

#define SC_SERIAL_HIGH_Z \
    DDRD  &= ~((1 << TXC) | (1 << RXC)); \
    PORTD &= ~((1 << TXC) | (1 << RXC));

#define SC_SERIAL_OUTPUT \
    DDRD  |= ((1 << TXC) | (1 << RXC)); \
    PORTD &= ~((1 << TXC) | (1 << RXC));

#define H_SERIAL_HIGH_Z \
    DDRD  &= ~((1 << PD1) | (1 << PD0)); \
    PORTD &= ~((1 << PD1) | (1 << PD0));

#define H_SERIAL_OUTPUT \
    DDRD  |= ((1 << PD1) | (1 << PD0)); \
    PORTD &= ~((1 << PD1) | (1 << PD0));

#define LED_OUTPUT DDRB |= (1 << 5);
#define LED_HIGH  PORTB |= (1 << 5);
#define LED_LOW   PORTB &= ~(1 << 5);

#define CMD_RAM_TO_IR 0b00000100
#define CMD_IR_TO_MAR 0b00000101

byte instruction = 0b00000000;
byte opcode = 0b00000000;
byte address = 0b00000000;
byte lastInstruction = 0b11111111;  // 이전 instruction 값 저장

void setup() {
  Serial.begin(19200);
  busSerial.begin(19200);
  ctrSerial.begin(19200);

  LED_OUTPUT;
  SB_SERIAL_HIGH_Z;
  SC_SERIAL_HIGH_Z;
  H_SERIAL_HIGH_Z;

  LED_LOW;
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
        LED_HIGH;
      }

      opcode = (instruction >> 4) & 0b00001111;
      address = instruction & 0b00001111;

      // CMD_RAM_TO_IR 받을 때마다 무조건 CTR에 전송
      sendOpcodeToCTR();
      lastInstruction = instruction;

      ctrSerial.listen();
      return;
    }
    _delay_ms(10);
  }

  ctrSerial.listen();
}

void sendToMAR() {
  SB_SERIAL_OUTPUT;
  SB_SERIAL_LOW;
  _delay_ms(50);

  busSerial.listen();
  _delay_ms(10);

  busSerial.write(address);
  busSerial.flush();

  _delay_ms(15);

  SB_SERIAL_LOW;
  _delay_ms(5);
  SB_SERIAL_HIGH_Z;

  ctrSerial.listen();
}

void sendOpcodeToCTR() {
  byte dataToSend = (opcode << 4);
  Serial.write(dataToSend);
  Serial.flush();
}
