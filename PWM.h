/*
 * PWM.h
 *
 * Created: 2013-12-11 11:10:49
 *  Author: PiotrP
 */ 

#ifndef PWM_H_
#define PWM_H_

#include <avr/io.h>

uint8_t Conv8bitproc( uint8_t uiPWMproc );
uint8_t Conv8bitprocRange( uint8_t uiPWMproc, uint8_t uiLowerRange );

void PWM_Stop( uint8_t uiEngine );
void PWM_Setproc( uint8_t uiEngine, uint8_t uiPWMproc );
void PWM_SetprocRange( uint8_t uiEngine, uint8_t uiPWMproc, uint8_t uiLowerRange );

void CheckAndCorrectProcValue( uint8_t* uiValue );

#endif /* PWM_H_ */