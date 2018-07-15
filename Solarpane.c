/*
 * Solarpane.c
 *
 * Created: 2013-11-26 13:30:02
 *  Author: PiotrP
 */ 

#define F_CPU 12000000
#define BAUD 9600
#define MYUBRR (F_CPU/16/BAUD-1)

#include <avr/io.h>
#include <avr/interrupt.h> 
#include <util/delay.h>

#include "defines.h"
#include "Solarpane.h"
#include "UART.h"
#include "ADC.h"
#include "UARTdef.h"
#include "Sensor.h"
#include "Engine.h"

uint8_t TimerLimit = 0, TimerSec = 1, TimerEnded = 0, SleepMode = 0;
uint8_t tryb_pracy = TRYB_AUTO;
int iCounter = 0;

int main(void)
{
	//Variables
	char cFrame[6];
	char* pcFrame = cFrame;
	uint8_t uiFotoL = 0, uiFotoR = 0, uiFotoD = 0;
	uint8_t SleepModeLimit = 15;
	
	//Initialization
 	SetGPIODirection();
 	UART_Init( MYUBRR );
 	ADCintern_Init();
	TimersInit();
	
	Actuator_SetPos( 0, 100 );
	Engine_Init();

	sei();
	
	UART_WaitForInit();
	
	while(1)
    {
  		if( UART_IsFrameRdy() == 1 )
		{
			pcFrame = UART_GetFrame();
			//UART_T_Str( pcFrame );
 			if( Frame_IsCorrect( pcFrame ) == 1 )
 			{
 				Frame_Interpret( pcFrame );
 			}
			UART_SetRdyOff();
// 			UCSR1B |= (1<<RXCIE1);
// 			UCSR1B |= (1<<RXEN1);
		}
		else if( tryb_pracy == TRYB_AUTO )
		{
			Foto_GetVal( &uiFotoL, &uiFotoR, &uiFotoD );
			
			if( uiFotoL <= SleepModeLimit && uiFotoR <= SleepModeLimit && uiFotoD <= SleepModeLimit && SleepMode == 0 )
			{
				SolarSleepMode( SLEEPMODE_DURATION );
			}
			else if( SleepMode == 0 )
			{
				SolarTracking( uiFotoL, uiFotoR, uiFotoD );
			}
		}
    }	//while end
}

void UART_WaitForInit()
{
	char b[6];
	char* pcFrame = b;
	uint8_t uiInitOK = 0;
	
	while( !uiInitOK )
	{
		if( UART_IsFrameRdy() )
		{
			pcFrame = UART_GetFrame();
			if( Frame_IsCorrect( pcFrame ) == 1 )
			{
				uint8_t uiFrameVal = Frame_GetVal( pcFrame );
				char cPar = Frame_GetPar( pcFrame );
				if( cPar == UART_F_INIT )
				{
					if( uiFrameVal == 1 )
					{
						tryb_pracy = TRYB_AUTO;
						UART_T_Frame( UART_F_INIT, 1 );	//inicjalizacja udana w trybie auto
						break;
					}
					else if( uiFrameVal == 2 )
					{
						tryb_pracy = TRYB_MANUAL;
						Actuator_Stop();
						UART_T_Frame( UART_F_INIT, 2 );	//inicjalizacja udana w trybie manual
						break;	
					}
					else
					{
						UART_T_Frame( UART_F_INIT, UART_ERR_1 );	//wrong val
						break;
					}
				}
			}
			UART_SetRdyOff();
		}
	}
}

void SolarTracking( uint8_t uiFotoL, uint8_t uiFotoR, uint8_t uiFotoD )
{
	uint8_t uiErrorEngine = 2, uiErrorActuator = 5;
	uint8_t UpAverage = 0, potenVal = 0;
	uint8_t bPotenEnd = FALSE;
	
	Foto_GetVal( &uiFotoL, &uiFotoR, &uiFotoD );
		
	if( (uiFotoR > uiFotoL + uiErrorEngine || uiFotoR < uiFotoL - uiErrorEngine) )
	{
		Actuator_Stop();
		if( uiFotoR > uiFotoL )
		{
			iCounter = Engine_Move( DIR_RIGHT );
		}
		else
		{
			iCounter = Engine_Move( DIR_LEFT );
		}
		_delay_ms( 500 );
	}
	else
	{
		UpAverage = (uiFotoL + uiFotoR) / 2;
 		if( UpAverage > uiFotoD + uiErrorActuator || UpAverage < uiFotoD - uiErrorActuator )
 		{
			potenVal = ADCintern_Getproc( ADC_CH_POTEN );
			if( UpAverage > uiFotoD && potenVal > 0 )
 			{
 				Actuator_SetDir( DIR_DOWN );
				Actuator_SetSpeedProc( 30 );
				bPotenEnd = FALSE;
 			}
			else if( UpAverage < uiFotoD && potenVal < 100 )
			{
				Actuator_SetDir( DIR_UP );
				Actuator_SetSpeedProc( 30 );
				bPotenEnd = FALSE;
			}
			else if( bPotenEnd == TRUE )
			{
				Actuator_Stop();
			}
			else 
			{
				_delay_ms( 1000 );
				Actuator_Stop();
				bPotenEnd = TRUE;
			}
 		}
		else
		{
			Actuator_Stop();
		}
	}
}

void SolarSleepMode( uint8_t seconds )
{
	SleepMode = 1;
	Actuator_Stop();
	Engine_Stop();
	Engine_Init();
	Actuator_SetPos( 0, 100 );
	TimerStart( seconds );
}

ISR(TIMER1_COMPA_vect)
{
	if( TimerSec < TimerLimit )
	{
		TimerSec++;
	}
	else
	{
		TIMSK &= ~(1<<OCIE1A);
		TimerSec = 1;
		SleepMode = 0;
	}
}

void TimerStart( uint8_t seconds )
{
	TIMSK |= (1<<OCIE1A);	//Timer/Counter1, Output Compare A Match Interrupt Enable
	
	TimerLimit = seconds;
}

uint8_t Frame_IsCorrect( char* cFrame )
{
	if( cFrame[0] == UART_FBEGIN && cFrame[5] == UART_FEND )
	{
		return 1;
	}
	else
	{
		//wrong frame format error
		UART_T_Frame( UART_F_ERROR, UART_ERR_FRAME_FORMAT );
	}
	
	return 0;
}

void Frame_Interpret( char* cFrame )
{
	uint8_t ValToSend = 0;
	uint8_t uiFrameVal;
	char cPar;
	
	uiFrameVal = Frame_GetVal( cFrame );
	cPar = Frame_GetPar( cFrame );
	
	if( cPar == UART_F_INIT )
	{
		if( uiFrameVal == 1 )
		{
			tryb_pracy = TRYB_AUTO;
			UART_T_Frame( UART_F_INIT, 1 );	//inicjalizacja udana w trybie auto
		}
		else if( uiFrameVal == 2 )
		{
			tryb_pracy = TRYB_MANUAL;
			Actuator_Stop();
			//stop and reset timer1
			TIMSK &= ~(1<<OCIE1A);
			TimerSec = 1;
			SleepMode = 0;	//disable sleep mode
			UART_T_Frame( UART_F_INIT, 2 );	//inicjalizacja udana w trybie manual
		}
		else
		{
			UART_T_Frame( UART_F_INIT, UART_ERR_1 ); //wrong val
		}
	}
	else if( cPar == UART_F_SILOWNIK )
	{
		if( (tryb_pracy == TRYB_AUTO) && (uiFrameVal == 1) )
		{
			ValToSend = ADCintern_Getproc( ADC_CH_POTEN );
			UART_T_Frame( UART_F_SILOWNIK, ValToSend );
		}
		else if( tryb_pracy == TRYB_MANUAL )
		{
			if( uiFrameVal == 1 )
			{
				ValToSend = ADCintern_Getproc( ADC_CH_POTEN );
				UART_T_Frame( UART_F_SILOWNIK, ValToSend );
			}
			else if( uiFrameVal == 101 )
			{
				Actuator_SetDir( DIR_UP );
				Actuator_SetSpeedProc( 30 );
			}
			else if( uiFrameVal == 102 )
			{
				Actuator_SetDir( DIR_DOWN );
				Actuator_SetSpeedProc( 30 );
			}
			else if( uiFrameVal == 103 )
			{
				Actuator_Stop();
				ValToSend = ADCintern_Getproc( ADC_CH_POTEN );
				UART_T_Frame( UART_F_SILOWNIK, ValToSend );
			}
			else
			{
				UART_T_Frame( UART_F_SILOWNIK, UART_ERR_1 ); //wrong val
			}
		}
		else
		{
			UART_T_Frame( UART_F_SILOWNIK, UART_ERR_1 ); //out of range oraz wrong val
		}
	}
	else if( cPar == UART_F_SILNIK )
	{
		if( (tryb_pracy == TRYB_AUTO) && (uiFrameVal == 1) )
		{
			UART_T_Frame( UART_F_SILNIK, iCounter + 15 );
		}
		else if( tryb_pracy == TRYB_MANUAL )
		{
			if( uiFrameVal == 1 )
			{
				UART_T_Frame( UART_F_SILNIK, iCounter + 15 );
			}
			else if( uiFrameVal == 2 )
			{
				iCounter = Engine_Move( DIR_RIGHT );
				UART_T_Frame( UART_F_SILNIK, iCounter + 15 );
			}
			else if( uiFrameVal == 3 )
			{
				iCounter = Engine_Move( DIR_LEFT );
				UART_T_Frame( UART_F_SILNIK, iCounter + 15 );
			}
			else
			{
				UART_T_Frame( UART_F_SILNIK, UART_ERR_2 );
			}
		}
		else
		{
			UART_T_Frame( UART_F_SILNIK, UART_ERR_1 ); //wrong val
		}
	}
	else if( cPar == UART_F_I12 )
	{
		if( uiFrameVal == 1 )
		{
			//pomiar pradu zasilania 12V
			uint16_t adc = ADCintern_Get10bitSUM( ADC_CH_V_12, 10 );
			adc = adc - 512;
			UART_T_Frame( UART_F_I12, adc );
		}
		else
		{
			UART_T_Frame( UART_F_I12, UART_ERR_1 );
		}
	}
	else if( cPar == UART_F_ILAD )
	{
		if( uiFrameVal == 1 )
		{
			//pomiar pradu ladowania
			uint16_t adc = ADCintern_Get10bitSUM( ADC_CH_VI_LAD, 10 );
			adc = adc - 512;
			UART_T_Frame( UART_F_ILAD, adc );
		}
		else
		{
			UART_T_Frame( UART_F_ILAD, UART_ERR_1 );
		}
	}
	else if( cPar == UART_F_IROZL )
	{
		if( uiFrameVal == 1 )
		{
			//pomiar pradu rozladowania
			uint16_t adc = ADCintern_Get10bitSUM( ADC_CH_VI_ROZL, 10 );
			adc = adc - 512;
			UART_T_Frame( UART_F_IROZL, adc );
		}
		else
		{
			UART_T_Frame( UART_F_IROZL, UART_ERR_1 );
		}
	}
	else if( cPar == UART_F_VLAD )
	{
		if( uiFrameVal == 1 )
		{
			//pomiar napiecia ladowania
		}
		else 
		{
			UART_T_Frame( UART_F_VLAD, UART_ERR_1 );
		}
	}
	else if( cPar == UART_F_VROZL )
	{
		if( uiFrameVal == 1 )
		{
			//pomiar napiecia rozladowania
			//wyslanie wyniku
		}
		else
		{
			UART_T_Frame( UART_F_VROZL, UART_ERR_1 );
		}
	}
	else if( cPar == UART_F_FOTOR && tryb_pracy == TRYB_MANUAL )
	{
		if( uiFrameVal == 1 )
		{
			ValToSend = ADCintern_Getproc( ADC_CH_FOTOR );
			UART_T_Frame( UART_F_FOTOR, ValToSend );
		}
		else
		{
			UART_T_Frame( UART_F_FOTOR, UART_ERR_1 );
		}
	}
	else if( cPar == UART_F_FOTOL && tryb_pracy == TRYB_MANUAL )
	{
		if( uiFrameVal == 1 )
		{
			ValToSend = ADCintern_Getproc( ADC_CH_FOTOL );
			UART_T_Frame( UART_F_FOTOL, ValToSend );
		}
		else
		{
			UART_T_Frame( UART_F_FOTOL, UART_ERR_1 );
		}
	}
	else if( cPar == UART_F_FOTOD && tryb_pracy == TRYB_MANUAL )
	{
		if( uiFrameVal == 1 )
		{
			ValToSend = ADCintern_Getproc( ADC_CH_FOTOD );
			UART_T_Frame( UART_F_FOTOD, ValToSend );
		}
		else
		{
			UART_T_Frame( UART_F_FOTOD, UART_ERR_1 );
		}
	}
	else
	{
		//wrong parameter error
		UART_T_Frame( UART_F_ERROR, UART_ERR_2 );
	}		
}

uint8_t Frame_GetVal( char* tab )
{	
	char temp[3];
	
	//check if there are only digits
	temp[0] = tab[2];
	temp[1] = tab[3];
	temp[2] = tab[4];

	return atoi( temp );
}

char Frame_GetPar( char* tab )
{
	return tab[1];
}

void Foto_GetVal( uint8_t* uiFotoL, uint8_t* uiFotoR, uint8_t* uiFotoD )
{
	uint16_t sumL = 0, sumR = 0, sumD = 0;
	uint8_t iter = 100;
	
	for( uint8_t i = 0; i < iter; i++ )
	{
		sumL += ADCintern_Getproc( ADC_CH_FOTOL );
		sumD += ADCintern_Getproc( ADC_CH_FOTOD );
		sumR += ADCintern_Getproc( ADC_CH_FOTOR );
	}
	
	*uiFotoL = sumL / iter;
	*uiFotoD = sumD / iter;
	*uiFotoR = sumR / iter;
}

void TimersInit()
{
//////////////////////////////////////////////////////////////////////////
//PWM
//////////////////////////////////////////////////////////////////////////
//TIMER0 - fastPWM
// 	TCCR0 |= (1<<WGM01) | (1<<WGM00);	//fast PWM
// 	TCCR0 |= (1<<COM01);		//compare match; Clear OC0 on compare match, set OC0 at BOTTOM, (non-inverting mode)
// 	TCCR0 |= (1<<CS00) | (1<<CS01) | (1<<CS02);		//prescaler 1024
//	TCCR0 = 0x6F;
	
//TIMER2 - fastPWM
// 	TCCR2 |= (1<<WGM21) | (1<<WGM20);	//fast PWM mode
// 	TCCR2 |= (1<<COM21);	//Clear OC2 on compare match, set OC2 at BOTTOM, (non-inverting mode)
// 	TCCR2 |= (1<<CS20) | (1<<CS22);		//prescaler 1024
//	TCCR2 = 0x6D;
	
//////////////////////////////////////////////////////////////////////////
//TIMER1 - timer counting
	TCCR1B |= (1<<WGM12);	//CTC mode
	TCCR1B |= (1<<CS12);	//prescaler: 256, freq: 46875 Hz
	OCR1A = 46875;
}

void SetGPIODirection()
{
//GPIO Direction
//Outputs
	//DDRB
	BIT_TO_ONE( DDRB, BIT_ENS3 );
	BIT_TO_ONE( DDRB, BIT_INS3 );
	BIT_TO_ONE( DDRB, BIT_ENS12 );
	BIT_TO_ONE( DDRB, BIT_INS12 );
	//DDRD
	BIT_TO_ONE( DDRD, BIT_ADC_WR );
	BIT_TO_ONE( DDRD, BIT_ADC_RD );
	//DDRE
	BIT_TO_ONE( DDRE, BIT_ADC_CS );
	//BIT_TO_ONE( DDRE, BIT_DI10 );	//tymaczasowa dioda LED
	
//Inputs
	//DDRA
 	BIT_TO_ZERO( DDRA, BIT_JP_DI_0 );
 	BIT_TO_ZERO( DDRA, BIT_JP_DI_1 );
 	BIT_TO_ZERO( DDRA, BIT_JP_DI_2 );
 	BIT_TO_ZERO( DDRA, BIT_JP_DI_3 );
 	BIT_TO_ZERO( DDRA, BIT_JP_DI_4 );
 	BIT_TO_ZERO( DDRA, BIT_JP_DI_5 );
 	BIT_TO_ZERO( DDRA, BIT_JP_DI_6 );
 	BIT_TO_ZERO( DDRA, BIT_JP_DI_7 );
	//DDRC
	BIT_TO_ZERO( DDRC, BIT_ADC_TO_UC_0 );
	BIT_TO_ZERO( DDRC, BIT_ADC_TO_UC_1 );
	BIT_TO_ZERO( DDRC, BIT_ADC_TO_UC_2 );
	BIT_TO_ZERO( DDRC, BIT_ADC_TO_UC_3 );
	BIT_TO_ZERO( DDRC, BIT_ADC_TO_UC_4 );
	BIT_TO_ZERO( DDRC, BIT_ADC_TO_UC_5 );
	BIT_TO_ZERO( DDRC, BIT_ADC_TO_UC_6 );
	BIT_TO_ZERO( DDRC, BIT_ADC_TO_UC_7 );
	//DDRD
	BIT_TO_ZERO( DDRD, BIT_INT0 );
	BIT_TO_ZERO( DDRD, BIT_INT1 );
	BIT_TO_ZERO( DDRD, BIT_ADC_INTR_ROZL );
	BIT_TO_ZERO( DDRD, BIT_ADC_INTR_LAD );
	//DDRE
	BIT_TO_ZERO( DDRE, BIT_DI8 );
	BIT_TO_ZERO( DDRE, BIT_DI9 );
	BIT_TO_ZERO( DDRE, BIT_DI10 );
	//DDRF
	BIT_TO_ZERO( DDRF, BIT_VI_LAD );
	BIT_TO_ZERO( DDRF, BIT_VI_ROZL );
	BIT_TO_ZERO( DDRF, BIT_VI_ZAS );
	BIT_TO_ZERO( DDRF, BIT_VI_SIL );
	BIT_TO_ZERO( DDRF, BIT_ADC4 );
	BIT_TO_ZERO( DDRF, BIT_ADC5 );
	BIT_TO_ZERO( DDRF, BIT_ADC6 );
	BIT_TO_ZERO( DDRF, BIT_ADC7 );
}