/*
 * Solarpane.h
 *
 * Created: 2013-11-26 13:30:54
 *  Author: PiotrP
 */ 


#ifndef SOLARPANE_H_
#define SOLARPANE_H_

//GPIO
void SetGPIODirection();

//Timer
void TimersInit();
void TimerStart( uint8_t seconds );

//Frame
void Frame_Interpret( char* pcFrame );
uint8_t Frame_GetVal( char* tab );
char Frame_GetPar( char* tab );
uint8_t Frame_IsCorrect( char* pcFrame );
void UART_WaitForInit();

//Foto
void Foto_GetVal( uint8_t* uiFotoL, uint8_t* uiFotoR, uint8_t* uiFotoD );

//Solar tracking
void SolarTracking( uint8_t uiFotoL, uint8_t uiFotoR, uint8_t uiFotoD );
void SolarSleepMode( uint8_t seconds );

#endif /* SOLARPANE_H_ */