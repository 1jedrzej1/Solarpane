/*
 * Sensor.h
 *
 * Created: 2014-01-04 12:42:14
 *  Author: PiotrP
 */ 


#ifndef SENSOR_H_
#define SENSOR_H_

uint8_t Sensor_IsOn( uint8_t uiSensorSel );
uint8_t Sensor_DetectSmart();	//1-right, 2-left, 3-both
uint8_t Sensor_DetectEdge_Left();	//1-rising edge, 2-falling edge
uint8_t Sensor_DetectEdge_Right();	//1-rising edge, 2-falling edge

#endif /* SENSOR_H_ */