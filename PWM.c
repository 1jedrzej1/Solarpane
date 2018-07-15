/*
 * PWM.c
 *
 * Created: 2013-12-11 11:10:59
 *  Author: PiotrP
 */ 

#define F_CPU 12000000
#include "PWM.h"
#include "defines.h"

#define TIMER0_START	TCCR0 = 0x6F
#define TIMER0_STOP		TCCR0 = 0x00
#define TIMER2_START	TCCR2 = 0x6D
#define TIMER2_STOP		TCCR2 = 0x00

uint8_t Conv8bitproc( uint8_t uiPWMproc )
{
	CheckAndCorrectProcValue( &uiPWMproc );

	return uiPWMproc * 255 / 100;
}

uint8_t Conv8bitprocRange( uint8_t uiPWMproc, uint8_t uiLowerRange )
{
	CheckAndCorrectProcValue( &uiPWMproc );
	CheckAndCorrectProcValue( &uiLowerRange );
	
	return ( uiLowerRange + ( uiPWMproc * (100 - uiLowerRange ) / 100 ) ) * 255 / 100;
}

void PWM_Stop( uint8_t uiEngine )
{
	if( uiEngine == PWM_S12 )
	{
		TIMER2_STOP;
		OCR2 = 0x00;
	}
	else if( uiEngine == PWM_S3 )
	{
		TIMER0_STOP;
		OCR0 = 0x00;
	}
}

void PWM_Setproc( uint8_t uiEngine, uint8_t uiPWMproc )
{
	if( uiEngine == PWM_S12 )
	{
		TIMER2_START;
		OCR2 = Conv8bitproc( uiPWMproc );
	}
	else if( uiEngine == PWM_S3 )
	{
		TIMER0_START;
		OCR0 = Conv8bitproc( uiPWMproc );
	}
}

void PWM_SetprocRange( uint8_t uiEngine, uint8_t uiPWMproc, uint8_t uiLowerRange )
{
	if( uiEngine == PWM_S12 )
	{
		TIMER2_START;
		OCR2 = Conv8bitprocRange( uiPWMproc, uiLowerRange );
	}
	else if( uiEngine == PWM_S3 )
	{
		TIMER0_START;
		OCR0 = Conv8bitprocRange( uiPWMproc, uiLowerRange );
	}
}

void CheckAndCorrectProcValue( uint8_t *uiValue )
{
	uint8_t uiPWMprocTemp = 0;
	
	if( *uiValue < 0 )
		uiPWMprocTemp = 0;
	else if( *uiValue > 100 )
		uiPWMprocTemp = 100;
	else
		uiPWMprocTemp = *uiValue;
		
	*uiValue = uiPWMprocTemp;
}
