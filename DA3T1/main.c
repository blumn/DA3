/*
 * HomeAuto.c
 *
 * Created: 22/02/2013 3:03:37 PM
 *  Author: PC
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#define F_CPU 16000000UL 
#include <util/delay.h>
#define ADC_CH 1;
#define USART_BAUDRATE 9600
#define BAUD_PRESCALE (((F_CPU/(USART_BAUDRATE*16UL)))-1)
uint16_t counter;
////////////////////////////////////////////////////////This is USART////////////////////////////////////////////////////////////
void USARTInit(unsigned int ubrr_value)
{
	/*Set baud rate */
UBRR0H = (unsigned char)(ubrr_value>>8); 
UBRR0L = (unsigned char)ubrr_value;
/* Set frame format: 8data, 2stop bit */
UCSR0C |= (1<<UCSZ00) | (1<<UCSZ01);

/*Enable receiver and transmitter */ 
UCSR0B = (1<<RXEN0)|(1<<TXEN0);
}

void USARTTransmitChar(unsigned char data)
{
	/* Wait for empty transmit buffer */ 
	while ( !( UCSR0A & (1<<UDRE0)) );
	/* Put data into buffer, sends the data */
	UDR0 = data;         
}
///////////////////////////////////////////////////////////////USART END/////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////ADC START//////////////////////////////////////////////////////////////
void adc_Init()
{
	//DDRC=0x00;
	ADMUX|=(1<<REFS0);
	ADCSRA|=(1<<ADEN)|(1<<ADPS0)|(1<<ADPS1)|(1<<ADPS2);

}

uint16_t read_adc(uint8_t ch)
{
	ADMUX = (ADMUX & 0xF0) | (ch & 0x0F);
	ADCSRA|=(1<<ADSC);
	while(!(ADCSRA&(1<<ADSC)));
	return ADC;
}
///////////////////////////////////////////////////////////////ADC END////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////TImer START//////////////////////////////////////////////////////////////////////////////
void timer_init(){
TCCR0A = 0x00;
TCCR0B = 0x05; //clk/1024
TIMSK0 = 0x01;	
}

ISR(TIMER0_OVF_vect){
	
	if(counter == 61)
	{
		counter = 0;
		uint8_t data = read_adc(0);
        volatile float f = (((float)data/1024.0f)*500.0f); 
		char ch[20];
		sprintf(ch,"%d",(uint8_t)f);
        USARTTransmitChar(ch[0]);
		USARTTransmitChar(ch[1]);
		USARTTransmitChar('\n');
	}
	counter++;
}

////////////////////////////////////////////////////////////Timer End////////////////////////////////////////////////////////////////////////////////
int main(void)
{
	USARTInit(BAUD_PRESCALE);
	USARTTransmitChar('g');
    adc_Init();	
    timer_init();
	sei();
	while(1){}
}