/*
클럭 생성기 아두이노
- 일정 주기(예: 1Hz)로 클럭 신호 생성
- 디지털 2번 핀(CLK)을 통해 출력

-나중에 토글스위치와 연결해서 클락속도를 바꾸거나 클락을 멈추는 등의(일시정지) 기능을 추가할 예정.
-사용자에게 입력을 받아 클락 속도를 제어할수 있도록 코드 수정 고려중.
*/

#define CLK_PIN 2


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
