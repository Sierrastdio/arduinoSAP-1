/*
 * Output Register and 16x2 lcd(I2C)
 */
#include <avr/io.h>
#include <util/delay.h>
#include <NeoSWSerial.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define RXC  PD3
#define TXC  PD4
#define RXB  PD5
#define TXB  PD6

NeoSWSerial ctrSerial(RXC, TXC);
NeoSWSerial busSerial(RXB, TXB);

#define SB_SERIAL_HIGH_Z \
    DDRD  &= ~((1 << TXB) | (1 << RXB)); \
    PORTD &= ~((1 << TXB) | (1 << RXB));

#define SC_SERIAL_HIGH_Z \
    DDRD  &= ~((1 << TXC) | (1 << RXC)); \
    PORTD &= ~((1 << TXC) | (1 << RXC));


#define LED_OUTPUT DDRB |= (1 << 5);
#define LED_HIGH  PORTB |= (1 << 5);
#define LED_LOW   PORTB &= ~(1 << 5);

LiquidCrystal_I2C lcd(0x3F, 16, 2);

#define CMD_A_TO_OUT  0b00001011;

byte outValue = 0b00000000;

void setup() {
  ctrSerial.begin(19200);
  busSerial.begin(19200);

  LED_OUTPUT;
  SC_SERIAL_HIGH_Z;
  SB_SERIAL_HIGH_Z;
  LED_LOW;

  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("BIN: ");
  lcd.setCursor(0, 1);
  lcd.print("DEC: ");

  ctrSerial.listen();
}

void loop() {
  if (ctrSerial.available()) {
    handleCommand(ctrSerial.read());
  }
}

void handleCommand(byte cmd) {
  if (cmd == CMD_A_TO_OUT) {
    receiveFromBus();
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
      outValue = busSerial.read();

      LED_HIGH;

      updateDisplay();

      _delay_ms(200);
      LED_LOW;

      ctrSerial.listen();
      return;
    }
    _delay_ms(10);
  }

  ctrSerial.listen();
}

void updateDisplay() {
  lcd.setCursor(5, 0);
  lcd.print("        ");
  lcd.setCursor(5, 0);
  for(int i = 7; i >= 0; i--) {
    lcd.print((outValue >> i) & 1);
  }

  lcd.setCursor(5, 1);
  lcd.print("        ");
  lcd.setCursor(5, 1);
  lcd.print(outValue);
}
