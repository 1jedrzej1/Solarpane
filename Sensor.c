/*
 * Sensor.c
 *
 * Created: 2014-01-04 12:42:23
 *  Author: PiotrP
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "Sensor.h"
#include "defines.h"

uint8_t uiLastSensor_Left = -1, uiCurSensor_Left = -1, uiLastSensor_Right = -1, uiCurSensor_Right = -1;	//to prevent trigerring detect edge when sensor detect sth during init

uint8_t Sensor_IsOn( uint8_t uiSensorSel )
{
	uint8_t bReturn = 0;
	
	if( BIT_UNCHECK( PINE, uiSensorSel ) )
	{
		bReturn = 1;
	}	
	
	return bReturn;
}

uint8_t Sensor_DetectEdge_Left()
{
	uint8_t uiReturn = 0;
	
	if( Sensor_IsOn( SENSOR_LEFT ) == 1 )
	{
		uiCurSensor_Left = 1;
	}
	if( Sensor_IsOn( SENSOR_LEFT ) == 0 )
	{
		uiCurSensor_Left = 0;
	}
			
	if( uiCurSensor_Left == 1 && uiLastSensor_Left == 0 )
	{
		//positive edge
		uiReturn = 1;
	}
	if( uiCurSensor_Left == 0 && uiLastSensor_Left == 1 )
	{
		//negative edge
		uiReturn = 2;
	}
	
	uiLastSensor_Left = uiCurSensor_Left;
	
	return uiReturn;	
}

uint8_t Sensor_DetectEdge_Right()
{
	uint8_t uiReturn = 0;
	
	if( Sensor_IsOn( SENSOR_RIGHT ) == 1 )
	{
		uiCurSensor_Right = 1;
	}
	if( Sensor_IsOn( SENSOR_RIGHT ) == 0 )
	{
		uiCurSensor_Right = 0;
	}
			
	if( uiCurSensor_Right == 1 && uiLastSensor_Right == 0 )
	{
		//positive edge
		uiReturn = 1;
	}
	if( uiCurSensor_Right == 0 && uiLastSensor_Right == 1 )
	{
		//negative edge
		uiReturn = 2;
	}
	
	uiLastSensor_Right = uiCurSensor_Right;
	
	return uiReturn;	
}

uint8_t Sensor_DetectSmart()
{
	uint8_t detectR = 0, detectL = 0, trigL = 0, trigR = 0, uiReturn = 0;
	
	while( 1 )
	{
		detectR = Sensor_DetectEdge_Right();
		detectL = Sensor_DetectEdge_Left();
		if( detectL == 1 )
		{
			trigL = 1;
		}
		if( detectR == 1 )
		{
			trigR = 1;
		}
		//protect from only falling edge 
		if( (trigL == 1 && trigR == 1) || (detectL == 2 && trigL == 1) || (detectR == 2 && trigR == 1 ) )	
		{
			break;
		}
	}
	
	if( trigL == 1 && trigR == 1 )
	{
		//both detected
		uiReturn = DETECT_BOTH;
	}
	else if( trigL == 1 && trigR == 0 )
	{
		//only left detected
		uiReturn = DETECT_LEFT;
	}
	else if( trigL == 0 && trigR == 1 )
	{
		//only right detected
		uiReturn = DETECT_RIGHT;
	}
	else 
	{
		//X996
	}
	
	return uiReturn;
}