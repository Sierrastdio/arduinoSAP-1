/*
 * Output Register and 16x2 lcd(I2C)
 */
#include <NeoSWSerial.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

int RXC = 3;  
int TXC = 4;
int RXB = 5;
int TXB = 6;

NeoSWSerial ctrSerial(RXC, TXC);
NeoSWSerial busSerial(RXB, TXB);

LiquidCrystal_I2C lcd(0x3F, 16, 2);

const byte CMD_A_TO_OUT  0b00001011;

byte outValue = 0b00000000;

void setup() {
  ctrSerial.begin(19200);
  busSerial.begin(19200);
  
  pinMode(13, OUTPUT);
  pinMode(TXC, INPUT);
  pinMode(RXB, INPUT);
  pinMode(TXB, INPUT);
  
  digitalWrite(13, LOW);
  
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
      
      digitalWrite(13, HIGH);
      
      updateDisplay();
      
      delay(200);
      digitalWrite(13, LOW);
      
      ctrSerial.listen();
      return;
    }
    delay(10);
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
