/*
 * UART.h
 *
 * Created: 2013-12-11 11:06:25
 *  Author: PiotrP
 */ 


#ifndef UART_H_
#define UART_H_

#include "defines.h"

void UART_Init( unsigned int ubrr );
unsigned char UART_R( void );
void UART_T( unsigned char data );
void UART_T_Str( const char* psString );
void UART_T_Int( int iVal );
void UART_T_IntN( const char* cVal, int iVal );
void UART_T_NextLine();
void UART_T_Frame( char cPar, int uiVal );
void UART_T_Int2( int iVal1, int iVal2 );

char* UART_GetFrame();
int UART_IsFrameRdy();
void UART_SetRdyOff();

#endif /* UART_H_ */