/*
 * ADC.h
 *
 * Created: 2013-12-11 11:08:48
 *  Author: PiotrP
 */ 


#ifndef ADC_H_
#define ADC_H_

#include <avr/io.h>

void ADCintern_Init();
uint8_t ADCintern_Getproc( uint8_t uiChannel );	//uiChannel from 0 to 7
uint8_t ADCintern_Get8bit( uint8_t uiChannel );
uint16_t ADCintern_Get10bit( uint8_t uiChannel );	//uiChannel from 0 to 7
uint16_t ADCintern_Get10bitSUM( uint8_t uiChannel, uint8_t iter );

uint8_t ADCextern_Getproc( int bCS );	//0-CS_LAD, 1-CS_ROZL; wybór, który ADC bêdzie odczytywany

#endif /* ADC_H_ */