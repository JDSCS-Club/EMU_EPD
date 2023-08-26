/*
 * naranja_boron.h
 *
 *  Created on: Dec 30, 2019
 *      Author: RobertKim
 */

#ifndef INC_NARANJA_BORON_H_
#define INC_NARANJA_BORON_H_

#include "main.h"
#include "stdbool.h"
//--------------------------------------------------------------------------------------------//
//
//--------------------------------------------------------------------------------------------//
#define PrintfUart1(x)          //x // debug for uart1
//--------------------------------------------------------------------------------------------//
//
//--------------------------------------------------------------------------------------------//
#define VOLTAGE_LOAD_LED					75	// When bat_vcc is 2700mV
#define VOLTAGE_LOAD_AUDIO					0

#define VOLTAGE_CHARGE_BAT					85

#define VOLTAGE_BAT_75			2700	// 2700mV
#define VOLTAGE_BAT_90			2900	// 2900mV
#define VOLTAGE_BAT_100			3000	// 3000mV

//uint8_t getChargeRate(void);		//	Battery 0 ~ 100 %
//--------------------------------------------------------------------------------------------//
//
//--------------------------------------------------------------------------------------------//
extern uint16_t u16LedChangeTick;
extern uint16_t u16Led75UnderFlickerTick;
extern uint16_t u16Led100UnderFlickerTick;
extern uint16_t u16AmpSettingTick;

extern uint8_t uDI_getMasterIn;
extern uint8_t uSpk_Stat;
extern uint8_t uRssi_NgFlag;

extern bool bAmpSettingDetected;
extern uint8_t uAudioPlayFlag;
extern bool bCurLedCtr;

extern int		g_bRFMOk;		//	무정전RFM모듈 동작상태.		-	OK(1) / Fault(0)

//--------------------------------------------------------------------------------------------//
//
//--------------------------------------------------------------------------------------------//
extern void processOverrideOn(void);
extern void processRfLed(void);
extern void processChargeLed(void);
extern void processLightLed(void);
extern void processAudioAmpProcess(void);
extern void processGetBatVol(void);
extern void processTestDebug(void);
//--------------------------------------------------------------------------------------------//

extern void processCurrentVal(void);
//--------------------------------------------------------------------------------------------//
extern bool getVccRfIn(void);
extern bool getAmpFault(void);
extern bool getVccLedIn(void);
extern bool getVccAudioIn(void);

extern void ONTD(uint8_t IN,uint8_t *OUT,uint8_t MS,int *CLK );

extern void ONTD_Function(void);

extern uint8_t AMP_SPK_CHECK(void);

//--------------------------------------------------------------------------------------------//
//
//--------------------------------------------------------------------------------------------//

#endif /* INC_NARANJA_BORON_H_ */
