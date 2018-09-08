/*
 * TestUARTecho.c
 *
 * 
 * Author : Nikita Evsyukov
 */ 

#include "main.h"

char buff[100];
unsigned int count = 0;

void clearAll(){
	for(int i = 0; i < count; i++){
		buff[i] = '\0';
	}
	count = 0;
}

void initUART(){
	/* Config pins for UART */
	DDRD &= (~(1<<pinRX));
	DDRD |= (1<<pinTX);
	/* Config UART speed */
	UBRR0L = (unsigned char) BAUDRATE;
	UBRR0H = (unsigned char) (BAUDRATE >> 8);
	/* Config UART */
	UCSR0B =  (1<<RXEN0) | (1<<TXEN0);
	UCSR0C =  (1<<USBS0) | (3<<UCSZ00);
}

unsigned volatile long millis = 0;
unsigned long lastMillis = 0;
unsigned long countMeasure = 0;

ISR (TIMER1_COMPA_vect){
	millis++;
}

/* Init debug timer on 1Ghz frequency */

void timer_init(){
	TCCR1B |= (1<<WGM12) | (1<<CS11) | (1<<CS10); // CTC mode | Prescaler: 64 (CS11 = 1, CS10 = 1)
	TIMSK1 |= (1<<OCIE1A); // Interrupt when compare with OCR1A
	/* OCR1A = 250 */
	OCR1AH = 0b00000000;
	OCR1AL = 0b11111010;
	sei();
}

void debugMode(){
	lcd_initPort();
	lcd_init();
	timer_init();
	lcd_setPos(0,1);
	lcd_sendStr("Start");
	DDRB |= (1<<PINB0);
	PORTB |= (1<<PINB0);
	_delay_ms(500);
	PORTB &= ~(1<<PINB0);
	_delay_ms(500);
	lcd_clear();
}

void debugData(){
	lcd_clear();
	lcd_setPos(0,1);
	lcd_sendStr("val: ");
	lcd_sendStr(buff);
}


unsigned char giveUART(){
	while (!(UCSR0A & (1<<RXC0)));	return UDR0;
}



int main(void){
	DDRD &=(~(1<<PIND0)); // RX - input
	initUART();
	debugMode();
    while (1){
		char input = giveUART();
		if(input == '\n'){
			countMeasure++;
			debugData();
			clearAll();
		} else {
			buff[count] = input;
			count++;
		}
		/* Send time to LCD of giving 1000 package from UART */
		if(countMeasure >= 1000){
			lcd_clear();
			lcd_setPos(0,0);
			lcd_sendStr("val: ");
			lcd_sendInt(millis - lastMillis);
			lastMillis = millis;
			countMeasure = 0;
		}
    }
}

