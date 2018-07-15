/*
 * defines.h
 *
 * Created: 2013-11-26 13:32:03
 *  Author: PiotrP
 */ 


#ifndef DEFINES_H_
#define DEFINES_H_

// typedef int bool;
// #define true 1
// #define false 0

#define TRUE	1
#define FALSE	0

#define BIT_TO_ONE( reg, bit )		( reg |= (1<<bit) )
#define BIT_TO_ZERO( reg, bit )		( reg &= ~(1<<bit) )

#define BIT_TO( val, reg, bit )		( reg |= (val<<bit) )

#define BIT_CHECK( reg, bit )		( reg & (1<<bit) )
#define BIT_UNCHECK( reg, bit )		!( reg & (1<<bit) )

#define TRYB_AUTO			1
#define TRYB_MANUAL			2

#define MAX_NOERROR_VAL		997

#define COUNTER_MAX			12
#define COUNTER_MIN			-12

#define SLEEPMODE_DURATION	60

//PWM
#define PWM_S12				1
#define PWM_S3				2

#define ENGINE_S12			PWM_S12
#define ENGINE_S3			PWM_S3
#define DIR_RIGHT			0
#define DIR_LEFT			1
#define DIR_UP				DIR_RIGHT	//if cables: black red red black !!!
#define DIR_DOWN			DIR_LEFT	//if cables: black red red black !!!

//ADC
#define ADC_CH_VI_LAD		0
#define ADC_CH_VI_ROZL		1
#define ADC_CH_V_5			2
#define ADC_CH_V_12			3
#define ADC_CH_4			4
#define ADC_CH_5			5
#define ADC_CH_6			6
#define ADC_CH_7			7
#define ADC_CH_FOTOL		ADC_CH_4
#define ADC_CH_FOTOD		ADC_CH_5
#define ADC_CH_FOTOR		ADC_CH_6
#define ADC_CH_POTEN		ADC_CH_7

//PORTS
//PORTA
#define BIT_JP_DI_0			PA0
#define BIT_JP_DI_1			PA1
#define BIT_JP_DI_2			PA2
#define BIT_JP_DI_3			PA3
#define BIT_JP_DI_4			PA4
#define BIT_JP_DI_5			PA5
#define BIT_JP_DI_6			PA6
#define BIT_JP_DI_7			PA7

//PORTB
#define BIT_ENS3			PB4
#define BIT_INS3			PB5	//1-INS3_1, 0-INS3_2; wybór kierunku silnika
#define BIT_INS12			PB6	//1-INS1,2_1, 0-INS1,2_2; wybór kierunku si³owników
#define BIT_ENS12			PB7

//PORTC
#define BIT_ADC_TO_UC_0		PC0
#define BIT_ADC_TO_UC_1		PC1
#define BIT_ADC_TO_UC_2		PC2
#define BIT_ADC_TO_UC_3		PC3
#define BIT_ADC_TO_UC_4		PC4
#define BIT_ADC_TO_UC_5		PC5
#define BIT_ADC_TO_UC_6		PC6
#define BIT_ADC_TO_UC_7		PC7

//PORTD
#define BIT_INT0			PD0
#define BIT_INT1			PD1
#define BIT_UC_RXD			PD2
#define BIT_UC_TXD			PD3
#define BIT_ADC_INTR_ROZL	PD4
#define BIT_ADC_INTR_LAD	PD5
#define BIT_ADC_WR			PD6
#define BIT_ADC_RD			PD7

//PORTE
#define BIT_ADC_CS			PE2	//0-CS_LAD, 1-CS_ROZL; wybór, który ADC bêdzie odczytywany
#define BIT_DI8				PE4
#define BIT_DI9				PE5
#define BIT_DI10			PE6

//PORTF
#define BIT_VI_LAD			PF0
#define BIT_VI_ROZL			PF1
#define BIT_VI_ZAS			PF2
#define BIT_VI_SIL			PF3
#define BIT_ADC4			PF4
#define BIT_ADC5			PF5
#define BIT_ADC6			PF6
#define BIT_ADC7			PF7


//Sensor
#define SENSOR_LEFT			BIT_DI8
#define SENSOR_RIGHT		BIT_DI9

#define DETECT_BOTH			3
#define DETECT_RIGHT		1
#define DETECT_LEFT			2

//ADC
#define VI10BIT_MIDDLE		511

#endif /* DEFINES_H_ */