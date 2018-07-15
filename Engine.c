/*
 * Engine.c
 *
 * Created: 2014-01-16 14:41:26
 *  Author: PiotrP
 */ 

#define F_CPU 12000000

#include <avr/delay.h>
#include "PWM.h"
#include "ADC.h"
#include "UART.h"
#include "defines.h"
#include "Engine.h"
#include "Sensor.h"

uint8_t uiEngDirGLOB = -1;
int iCounterGLOB = 0;

void Engine_Init()
{
	uint8_t senL = 0, senR = 0, bothOn = 0;
	uint8_t trigR = 0, trigL = 0;
	uint8_t uiSpeedProc1 = 15, uiSpeedProc2 = 8;
	uint8_t uiDetectSmart = 0;
	uint8_t uiDelay1 = 500;
	
	//M001
	
	Engine_SetDir( DIR_RIGHT );
	Engine_SetSpeedProc( uiSpeedProc1 );
	
	_delay_ms( 200 );
	
	uiDetectSmart = Sensor_DetectSmart();
	
	if( uiDetectSmart == DETECT_BOTH )
	{
		//punkt charakterystyczny
		uiDetectSmart = Sensor_DetectSmart();
	
		if( uiDetectSmart == DETECT_BOTH )
		{
			//pozycja 90st
			Engine_Stop();
			_delay_ms( 100 );
			Engine_DirSwap();
			Engine_SetSpeedProc( uiSpeedProc1 );
			while(1)
			{
				uiDetectSmart = Sensor_DetectSmart();
				if( uiDetectSmart == DETECT_BOTH )
					break;
			}
			while(1)
			{
				uiDetectSmart = Sensor_DetectSmart();
				if( uiDetectSmart == DETECT_BOTH )
					break;
			}
			Engine_Stop();
		}
		else if( uiDetectSmart == DETECT_RIGHT )
		{
			//byla to pozycja 90st i jedziemy do pozycji init
			Engine_Stop();
			_delay_ms( 100 );
			Engine_DirSwap();
			Engine_SetSpeedProc( uiSpeedProc1 );
			while(1)
			{
				uiDetectSmart = Sensor_DetectSmart();
				if( uiDetectSmart == DETECT_BOTH )
					break;
			}
			Engine_Stop();
		}
		else if( uiDetectSmart == DETECT_LEFT )
		{
			Engine_Stop();
			_delay_ms( 100 );
			Engine_SetSpeedProc( uiSpeedProc1 );
			while(1)
			{
				uiDetectSmart = Sensor_DetectSmart();
				if( uiDetectSmart == DETECT_BOTH )
					break;
			}
			Engine_Stop();
		}
		else
		{
			//X997
			Engine_Stop();
		}
	}
	else if( uiDetectSmart == DETECT_RIGHT )
	{
		//zmiana kierunek na lewo i oczekiwanie na oba sensory jednoczesnie
		Engine_Stop();
		_delay_ms( uiDelay1 );
		Engine_DirSwap();
		Engine_SetSpeedProc( uiSpeedProc2 );
		while(1)
		{
			uiDetectSmart = Sensor_DetectSmart();
			if( uiDetectSmart == DETECT_BOTH )
				break;
		}
		Engine_Stop();
	}
	//else if( trigR == 0 && trigL == 1 )
	else if( uiDetectSmart == DETECT_LEFT )
	{
		//bez zmiany kierunku i oczekiwanie na oba sensory jednoczesnie
		Engine_Stop();
		_delay_ms( uiDelay1 );
		Engine_SetSpeedProc( uiSpeedProc2 );
		while(1)
		{
			uiDetectSmart = Sensor_DetectSmart();
			if( uiDetectSmart == DETECT_BOTH )
				break;
		}
		Engine_Stop();
	}
	else
	{
		//X997
		Engine_Stop();
	}

	//ostatnia poprawka pozycji z najmniejsza predkoscia
	_delay_ms( 1500 );	//poczekaj aby zniwelowaæ dzia³anie bezw³adnoœci silnika
	senL = Sensor_IsOn( SENSOR_LEFT );
	senR = Sensor_IsOn( SENSOR_RIGHT );
	if( (senL == 1 && senR == 0)
		|| (senL == 0 && senR == 1) 
		|| (senL == 0 && senR == 0)  )
	{
		//M002
		Engine_DirSwap();
		Engine_SetSpeedProc(2);
		if( senL == 0 && senR == 1 )	//only right sensor detects
		{
			while(1)
			{
				senL = Sensor_DetectEdge_Left();
				if( senL == 1 )
					break;
			}
		}
		else if( senL == 1 && senR == 0 )
		{
			while(1)
			{
				senR = Sensor_DetectEdge_Right();
				if( senR == 1 )
					break;
			}
		}
		else 
		{
			while(1)
			{
				uiDetectSmart = Sensor_DetectSmart();
				if( uiDetectSmart == DETECT_BOTH )
					break;
			}
		}
	}
	
	Engine_Stop();
	iCounterGLOB = 0;
	//M003
}

void Engine_SetDir( uint8_t uiDir )
{
	uiEngDirGLOB = uiDir;
	
	if( uiDir == DIR_RIGHT )
	{
		BIT_TO_ZERO( PORTB, BIT_INS3 );
	}
	else
	{
		BIT_TO_ONE( PORTB, BIT_INS3 );
	}
}

void Engine_DirSwap()
{
	if( uiEngDirGLOB == DIR_RIGHT )
	{
		Engine_SetDir( DIR_LEFT );
	}
	else
	{
		Engine_SetDir( DIR_RIGHT );
	}
}

void Engine_SetSpeedProc( uint8_t uiProcVal )
{
	uint8_t uiLowerRange = 20;
	
	PWM_SetprocRange( PWM_S3, uiProcVal, uiLowerRange );
};

void Engine_Stop()
{
	PWM_Stop( PWM_S3 );
}

int Engine_Move( uint8_t uiDir )
{
	uint8_t uiDetect = 0, Move = 0;
	const int ciCounterMax = COUNTER_MAX, ciCounterMin = COUNTER_MIN;
	int iNewCounterVal = 0;
	
	if( uiDir == DIR_RIGHT && iCounterGLOB < ciCounterMax )
	{
		Move = 1;
		iNewCounterVal = iCounterGLOB + 1;
	}
	else if( uiDir == DIR_LEFT && iCounterGLOB > ciCounterMin )
	{
		Move = 1;
		iNewCounterVal = iCounterGLOB - 1;
	}
	
	if( Move == 1 )
	{
		Engine_SetDir( uiDir );
		Engine_SetSpeedProc( 15 );
	
		uiDetect = Sensor_DetectSmart();
		if( uiDetect == DETECT_RIGHT || uiDetect == DETECT_LEFT )
		{
			iCounterGLOB = iNewCounterVal;
		}
		else if( uiDetect == DETECT_BOTH )
		{
			if( iCounterGLOB > (ciCounterMin / 2) && iCounterGLOB < (ciCounterMax / 2) )
			{
				iCounterGLOB = 0;
				//M004
			}
			else if( iCounterGLOB > (ciCounterMax / 2) )
			{
				iCounterGLOB = ciCounterMax;
				//M005
			}
			else if( iCounterGLOB < (ciCounterMin / 2) )
			{
				iCounterGLOB = ciCounterMin;
				//M006
			}
		}
	}
	
	Engine_Stop();
	
	return iCounterGLOB;	
}

void Actuator_SetDir( uint8_t uiDir )
{
	if( uiDir == DIR_UP )
	{
		BIT_TO_ZERO( PORTB, BIT_INS12 );
	}
	else if( uiDir == DIR_DOWN )
	{
		BIT_TO_ONE( PORTB, BIT_INS12 );
	}
}

void Actuator_SetSpeedProc( uint8_t uiProcVal )
{
	uint8_t uiLowerRange = 55;
	
	PWM_SetprocRange( PWM_S12, uiProcVal, uiLowerRange );	
}

void Actuator_Stop()
{
	PWM_Stop( PWM_S12 );
}

void Actuator_SetPos( uint8_t uiProcPos, uint8_t uiProcSpeed )
{
	uint8_t uiADCproc = ADCintern_Getproc( ADC_CH_7 );
	
	if( uiADCproc > uiProcPos )
	{
		Actuator_SetDir( DIR_DOWN );
		while( uiADCproc > uiProcPos )
		{
			uiADCproc = ADCintern_Getproc( ADC_CH_7 );
			Actuator_SetSpeedProc( uiProcSpeed );
		}
	}
	else
	{
		Actuator_SetDir( DIR_UP );
		while( uiADCproc < uiProcPos )
		{
			uiADCproc = ADCintern_Getproc( ADC_CH_7 );
			Actuator_SetSpeedProc( uiProcSpeed );
		}
	}
	
	if( uiADCproc == 0 || uiADCproc == 100 )
		_delay_ms( 1000 );
	Actuator_Stop();
}