/*
 * ADC.c
 *
 * Created: 2013-12-11 11:08:57
 *  Author: PiotrP
 */ 

#include "ADC.h"
#include "defines.h"
#include <avr/io.h>

void ADCintern_Init()
{
	ADMUX = 0x00;
	ADMUX |= (1<<REFS0);	//AVCC with external capacitor at AREF pin
	ADCSRA |= (1<<ADEN) | (1<<ADPS1) | (1<<ADPS2);	//enable ADC; prescaler to 64
}

uint8_t ADCintern_Getproc( uint8_t uiChannel )
{
	return ADCintern_Get8bit( uiChannel ) * 100 / 255;	//0-255 to 0-100
}

uint8_t ADCintern_Get8bit( uint8_t uiChannel )
{
	ADMUX |= (1<<ADLAR);	//make ADC 8-bit
	
	ADMUX &= 0xF8;
	ADMUX |= uiChannel;
	
	ADCSRA |= (1<<ADSC);
	
	while( ADCSRA & (1<<ADSC) );
	
	return ADCH;	//0-255 to 0-100
}

uint16_t ADCintern_Get10bit( uint8_t uiChannel )
{
	ADMUX &= ~(1<<ADLAR);	//make ADC 10-bit
	
	ADMUX &= 0xF8;
	ADMUX |= uiChannel;
	
	ADCSRA |= (1<<ADSC);
	
	while( ADCSRA & (1<<ADSC) );
	
	return ADC;
}

uint16_t ADCintern_Get10bitSUM( uint8_t uiChannel, uint8_t iter )
{
	unsigned long sum = 0;
	
	for( uint8_t i = 0; i < iter; i++ )
	{
		sum += ADCintern_Get10bit( uiChannel );
	}
	
	return (sum / iter);
}

uint8_t ADCextern_Getproc( int bCS )
{
	uint8_t uiReturn = 0;
	uint16_t temp = 0;
	//0-CS_LAD, 1-CS_ROZL; wybór, który ADC bêdzie odczytywany
 	if( !bCS )
 	{
			BIT_TO_ZERO( PORTE, BIT_ADC_CS );	//wybór przetwornika
			
			BIT_TO_ZERO( PORTD, BIT_ADC_WR );	//rozpoczêscie konwersji
			
			while( BIT_CHECK( PINE, BIT_ADC_INTR_LAD ) );
	}
	else
	{
			BIT_TO_ONE( PORTE, BIT_ADC_CS );	//wybór przetwornika
			
			BIT_TO_ZERO( PORTD, BIT_ADC_WR );	//rozpoczêscie konwersji
			
			while( BIT_CHECK( PINE, BIT_ADC_INTR_ROZL ) );
	}
	
	BIT_TO_ONE( PORTD, BIT_ADC_WR );	//zakoñczenie konwersji

	BIT_TO_ZERO( PORTD, BIT_ADC_RD );	//rozpoczêcie zczytywania wyniku
			
	uiReturn = PINC;
			
	BIT_TO_ONE( PORTD, BIT_ADC_RD );	//zakoñczenie zczytywania wyniku
	
	return uiReturn * 100 / 255;
}