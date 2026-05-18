#include <avr/io.h>
#include <util/delay.h>


#define CLK_PIN 2
#define clkDelay 101

#define LED_OUTPUT DDRB |= (1 << 5);
#define LED_HIGH  PORTB |= (1 << 5);
#define LED_LOW   PORTB &= ~(1 << 5);

#define CLK_OUTPUT DDRB |= (1 << CLK_PIN);
#define CLK_HIGH   PORTB |= (1 << CLK_PIN);
#define CLK_LOW    PORTB &= ~(1 << CLK_PIN);

void setup() {
  LED_OUTPUT();
  LED_LOW();
  _delay_ms(3000); //다른모듈 안정화 대기
}

void loop() {
  CLK_HIGH();
  LED_HIGH();
  _delay_ms(clkDelay);

  CLK_LOW();
  LED_LOW();
  _delay_ms(clkDelay);
}
