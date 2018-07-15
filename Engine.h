/*
 * Engine.h
 *
 * Created: 2014-01-16 14:41:40
 *  Author: PiotrP
 */ 


#ifndef ENGINE_H_
#define ENGINE_H_


//Engine
void Engine_Init();
void Engine_SetDir( uint8_t uiDir );
void Engine_DirSwap();
void Engine_SetSpeedProc( uint8_t uiProcVal );
void Engine_Stop();
int Engine_Move( uint8_t uiDir );

//Actuator
void Actuator_SetDir( uint8_t uiDir );
void Actuator_SetSpeedProc( uint8_t uiProcVal );
void Actuator_SetPos( uint8_t uiProcPos, uint8_t uiProcSpeed );
void Actuator_Stop();

#endif /* ENGINE_H_ */