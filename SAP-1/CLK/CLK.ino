/*
* Clock Generator
*/

int CLK_PIN = 2;
int clkDelay = 101;

void setup() {
  pinMode(CLK_PIN, OUTPUT);
  pinMode(13, OUTPUT);
  delay(3000); //다른모듈 안정화 대기
}

void loop() {
  digitalWrite(CLK_PIN, HIGH);
  digitalWrite(13, HIGH);
  delay(clkDelay);

  digitalWrite(CLK_PIN, LOW);
  digitalWrite(13, LOW);
  delay(clkDelay);
}
