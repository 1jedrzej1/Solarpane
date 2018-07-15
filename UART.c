/*
 * UART.c
 *
 * Created: 2013-12-11 11:06:35
 *  Author: PiotrP
 */ 
#include <avr/io.h>
#include <avr/interrupt.h>
#include <string.h>

#include "UART.h"
#include "UARTdef.h"
#include "defines.h"

volatile char tab[16];
volatile int idx = 0;
volatile int bReady = 0;
volatile uint8_t bReadFrame = 0;

void UART_Init( unsigned int ubrr )
{
	/* Set baud rate */
	UBRR1H = (unsigned char)(ubrr>>8);
	UBRR1L = (unsigned char)ubrr;
	/* Enable receiver and transmitter */
	UCSR1B = (1<<RXEN1)|(1<<TXEN1)|(1<<RXCIE1);
	/* Set frame format: 8data, 2stop bit */
	UCSR1C = (3<<UCSZ10);
}

//TRANSMIT
void UART_T( unsigned char data )
{
	/* Wait for empty transmit buffer */
	while ( !( UCSR1A & (1<<UDRE)) );
	/* Put data into buffer, sends the data */
	UDR1 = data;
}

void UART_T_Str( const char* psString )
{
	uint8_t c;
	while( c = *psString++ )
	{
		UART_T( c );
	}
}

void UART_T_Int( int iVal )
{
	char buffer[16];
	UART_T_Str( itoa( iVal, buffer, 10 ) );
}

void UART_T_IntN( const char* cVal, int iVal )
{
	UART_T_Str( cVal );
	UART_T_Int( iVal );
}

void UART_T_Int2( int iVal1, int iVal2 )
{
	UART_T_Int( iVal1 );
	UART_T_Str( "\t" );
	UART_T_Int( iVal2 );
	UART_T_NextLine();
}

void UART_T_NextLine()
{
	UART_T('\n');
	UART_T('\r');
}

void UART_T_Frame( char cPar, int uiVal )
{
	uint8_t acFrame[6];
	uint8_t buffer[3];
	
	acFrame[0] = UART_FBEGIN;
	acFrame[1] = cPar;
	if( uiVal < 0 )
		uiVal = 0;
	else if( uiVal >= 1000 )
		uiVal = 600;
	itoa( uiVal, buffer, 10 );
	if( uiVal >= 0 && uiVal < 10 )
	{
		acFrame[2] = '0';
		acFrame[3] = '0';
		acFrame[4] = buffer[0];
	}
	else if( uiVal >= 10 && uiVal < 100 )
	{
		acFrame[2] = '0';
		acFrame[3] = buffer[0];
		acFrame[4] = buffer[1];
	}
	else if( uiVal >= 100 && uiVal < 1000 )
	{
		acFrame[2] = buffer[0];
		acFrame[3] = buffer[1];
		acFrame[4] = buffer[2];
	}
	acFrame[5] = UART_FEND;
	
	for( uint8_t i = 0; i < 6; i++ )
	{
		UART_T( acFrame[i] );
	}
}

//RECEIVE
unsigned char UART_R( void )
{
	/* Wait for data to be received */
	while ( !(UCSR1A & (1<<RXC1)) );
	/* Get and return received data from buffer */
	return UDR1;
}

//GET
char* UART_GetFrame()
{
	return tab;
}

int UART_IsFrameRdy()
{
	return bReady;
}

//SET
void UART_SetRdyOff()
{
	bReady = 0;
}

ISR( USART1_RX_vect ) 
{
	char c = UART_R();
	if( c == UART_FBEGIN )
		bReadFrame = 1;
	if( bReady == 0 && bReadFrame == 1 )
		tab[idx++] = c;
	if( c == UART_FEND || idx > 5 )
	{
		idx = 0;
		bReady = 1;
		bReadFrame = 0;
// 		UCSR1B &= ~(1<<RXCIE1);
// 		UCSR1B &= ~(1<<RXEN1);
	}
}