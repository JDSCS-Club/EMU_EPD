/*
 * naranja_boron.c
 *
 *  Created on: 2019. 12. 24.
 *      Author: RobertKim
 */
//--------------------------------------------------------------------------------------------//
//
//--------------------------------------------------------------------------------------------//
#include "main.h"
#include "stdbool.h"
#include "naranja_boron.h"
#include "adc.h"
//--------------------------------------------------------------------------------------------//
//
//--------------------------------------------------------------------------------------------//
typedef enum
{
	CHARGE_RATE_UNKNOWN = 0u,
	CHARGE_RATE_75_UNDER,
	CHARGE_RATE_100_UNDER,
	CHARGE_RATE_100
} ChargeRateState;
//--------------------------------------------------------------------------------------------//
//
//--------------------------------------------------------------------------------------------//
uint8_t u8RS485Id = false;
bool bCurLedCtr = false;
bool bAmpOnOff = false;
bool bCurAmpOnOff = false;
bool bAmpStateChange = false;

ChargeRateState curChargeState = CHARGE_RATE_UNKNOWN;
ChargeRateState viaChargeState = CHARGE_RATE_UNKNOWN;
uint16_t u16LedChangeTick = 0;
uint16_t u16Led75UnderFlickerTick = 0;
uint16_t u16Led100UnderFlickerTick = 0;
uint16_t u16AmpSettingTick = 0;
bool bAmpSettingDetected = false;
uint16_t u16BatVol = 0;
uint16_t u16CntBatVol = 0;
uint32_t u16BatVolSum = 0;
//--------------------------------------------------------------------------------------------//
//
//--------------------------------------------------------------------------------------------//
/**
  * @brief  75% LED
  */
bool getLed75(void){ return HAL_GPIO_ReadPin(LED_75_GPIO_Port, LED_75_Pin); }
void setLed75(bool state){ HAL_GPIO_WritePin(LED_75_GPIO_Port, LED_75_Pin, state); }
void toggleLed75(void){ HAL_GPIO_TogglePin(LED_75_GPIO_Port, LED_75_Pin); }
void led75On(void){ HAL_GPIO_WritePin(LED_75_GPIO_Port, LED_75_Pin, false); }
void led75Off(void){ HAL_GPIO_WritePin(LED_75_GPIO_Port, LED_75_Pin, true); }
//--------------------------------------------------------------------------------------------//
//
//--------------------------------------------------------------------------------------------//
/**
  * @brief  100% Red LED
  */
bool getLed100Red(void){ return HAL_GPIO_ReadPin(LED_100_RED_GPIO_Port, LED_100_RED_Pin); }
void setLed100Red(bool state){ HAL_GPIO_WritePin(LED_100_RED_GPIO_Port, LED_100_RED_Pin, state); }
void toggleLed100Red(void){ HAL_GPIO_TogglePin(LED_100_RED_GPIO_Port, LED_100_RED_Pin); }
void led100RedOn(void){ HAL_GPIO_WritePin(LED_100_RED_GPIO_Port, LED_100_RED_Pin, false); }
void led100RedOff(void){ HAL_GPIO_WritePin(LED_100_RED_GPIO_Port, LED_100_RED_Pin, true); }
//--------------------------------------------------------------------------------------------//
//
//--------------------------------------------------------------------------------------------//
/**
  * @brief  100% Green LED
  */
bool getLed100Green(void){ return HAL_GPIO_ReadPin(LED_100_GREEN_GPIO_Port, LED_100_GREEN_Pin); }
void setLed100Green(bool state){ HAL_GPIO_WritePin(LED_100_GREEN_GPIO_Port, LED_100_GREEN_Pin, state); }
void led100GreenOn(void){ HAL_GPIO_WritePin(LED_100_GREEN_GPIO_Port, LED_100_GREEN_Pin, false); }
void led100GreenOff(void){ HAL_GPIO_WritePin(LED_100_GREEN_GPIO_Port, LED_100_GREEN_Pin, true); }
//--------------------------------------------------------------------------------------------//
//
//--------------------------------------------------------------------------------------------//
/**
  * @brief  get Charger detection
  */
bool getChargerDet(void){ return HAL_GPIO_ReadPin(CHARGER_DET_GPIO_Port, CHARGER_DET_Pin); }
//--------------------------------------------------------------------------------------------//
//
//--------------------------------------------------------------------------------------------//
/**
  * @brief  get Charge rate
  */
uint8_t getChargeRate(void){ return (Get_Adc1_Value() * 100) / ADC_MAX_DATA; }
//--------------------------------------------------------------------------------------------//
//
//--------------------------------------------------------------------------------------------//
/**
  * @brief  get VCC input
  */
bool getVccIn(void){ return HAL_GPIO_ReadPin(VCC_IN_GPIO_Port, VCC_IN_Pin); }
//--------------------------------------------------------------------------------------------//
//
//--------------------------------------------------------------------------------------------//
/**
  * @brief  get Radio module operation state
  */
bool getStandBy(void){ return HAL_GPIO_ReadPin(ST_BY_GPIO_Port, ST_BY_Pin); }
//--------------------------------------------------------------------------------------------//
//
//--------------------------------------------------------------------------------------------//
/**
  * @brief  get Light state
  */
bool getLightOn(void){ return HAL_GPIO_ReadPin(LIGHT_ON_GPIO_Port, LIGHT_ON_Pin); }
//--------------------------------------------------------------------------------------------//
//
//--------------------------------------------------------------------------------------------//
/**
  * @brief  get Master input
  */
bool getMasterIn(void){ return HAL_GPIO_ReadPin(MASTER_IN_GPIO_Port, MASTER_IN_Pin); }
//--------------------------------------------------------------------------------------------//
//
//--------------------------------------------------------------------------------------------//
/**
  * @brief  Override On
  */
bool getOverrideOn(void){ return HAL_GPIO_ReadPin(OVERRIDE_GPIO_Port, OVERRIDE_Pin); }
void setOverrideOn(GPIO_PinState state){ HAL_GPIO_WritePin(OVERRIDE_GPIO_Port, OVERRIDE_Pin, state); }
//--------------------------------------------------------------------------------------------//
//
//--------------------------------------------------------------------------------------------//
/**
  * @brief  set RF LED
  */
void setRfLed(GPIO_PinState state){ HAL_GPIO_WritePin(RF_LED_GPIO_Port, RF_LED_Pin, state); }
void rfLedOn(void){ HAL_GPIO_WritePin(RF_LED_GPIO_Port, RF_LED_Pin, false); }
void rfLedOff(void){ HAL_GPIO_WritePin(RF_LED_GPIO_Port, RF_LED_Pin, true); }
//--------------------------------------------------------------------------------------------//
//
//--------------------------------------------------------------------------------------------//
/**
  * @brief  set RS485 inactive state
  */
void setRS485Inactive(GPIO_PinState state){ HAL_GPIO_WritePin(RE_GPIO_Port, RE_Pin, state); }
//--------------------------------------------------------------------------------------------//
//
//--------------------------------------------------------------------------------------------//
/**
  * @brief  set LED CTR pin
  */
void setLedCtr(bool state){ return HAL_GPIO_WritePin(LED_CTL_GPIO_Port, LED_CTL_Pin, state); }
void ledCtrOn(void){ return HAL_GPIO_WritePin(LED_CTL_GPIO_Port, LED_CTL_Pin, true); }
void ledCtrOff(void){ return HAL_GPIO_WritePin(LED_CTL_GPIO_Port, LED_CTL_Pin, false); }
//--------------------------------------------------------------------------------------------//
//
//--------------------------------------------------------------------------------------------//
/**
  * @brief  get Audio state
  */
bool getAudioOn(void){ return HAL_GPIO_ReadPin(AUDIO_ON_GPIO_Port, AUDIO_ON_Pin); }
//--------------------------------------------------------------------------------------------//
//
//--------------------------------------------------------------------------------------------//
/**
  * @brief  set Amp mute
  */
void setAmpMute(bool state){ HAL_GPIO_WritePin(MUTE_GPIO_Port, MUTE_Pin, state); }
void ampMuteOn(void){ HAL_GPIO_WritePin(MUTE_GPIO_Port, MUTE_Pin, false); }
void ampMuteOff(void){ HAL_GPIO_WritePin(MUTE_GPIO_Port, MUTE_Pin, true); }
//--------------------------------------------------------------------------------------------//
//
//--------------------------------------------------------------------------------------------//
/**
  * @brief  set Amp SD
  */
void setAmpSd(bool state){ HAL_GPIO_WritePin(SD_GPIO_Port, SD_Pin, state); }
//--------------------------------------------------------------------------------------------//
//
//--------------------------------------------------------------------------------------------//
/**
  * @brief  get RS485 ID
  */
uint8_t getRS485Id(void)
{
	uint8_t id = HAL_GPIO_ReadPin(DEV_ID0_GPIO_Port, DEV_ID0_Pin);
	id |= HAL_GPIO_ReadPin(DEV_ID1_GPIO_Port, DEV_ID1_Pin) << 1;
	id |= HAL_GPIO_ReadPin(DEV_ID2_GPIO_Port, DEV_ID2_Pin) << 2;
	id |= HAL_GPIO_ReadPin(DEV_ID3_GPIO_Port, DEV_ID3_Pin) << 3;
	return id;
}
//--------------------------------------------------------------------------------------------//
//
//--------------------------------------------------------------------------------------------//
/**
  * @brief  get Charge rate state
  */
ChargeRateState getChargeRateState(void)
{
	if(getChargerDet())
		return CHARGE_RATE_100;

	if(u16BatVol < VOLTAGE_BAT_75)
		return CHARGE_RATE_75_UNDER;
	else if(u16BatVol >= VOLTAGE_BAT_75 && u16BatVol < VOLTAGE_BAT_100)
		return CHARGE_RATE_100_UNDER;
	else if(u16BatVol >= VOLTAGE_BAT_100)
		return CHARGE_RATE_100_UNDER;
	else
		return CHARGE_RATE_UNKNOWN;
}
//--------------------------------------------------------------------------------------------//
//
//--------------------------------------------------------------------------------------------//
/**
  * @brief	process Override output
  */
void processOverrideOn(void)
{
	setOverrideOn(getMasterIn());
}
//--------------------------------------------------------------------------------------------//
//
//--------------------------------------------------------------------------------------------//
/**
  * @brief	process RF LED
  */
void processRfLed(void)
{
	if(getStandBy())
		rfLedOn();
	else
		rfLedOff();
}
//--------------------------------------------------------------------------------------------//
//
//--------------------------------------------------------------------------------------------//
/**
  * @brief	process Charge LED
  */
void processChargeLed(void)
{
	ChargeRateState chargeRateState = getChargeRateState();
	if(viaChargeState == CHARGE_RATE_UNKNOWN)
	{
		viaChargeState = chargeRateState;
		curChargeState = chargeRateState;
	}
	else if(viaChargeState != chargeRateState)
	{
		viaChargeState = chargeRateState;
		u16LedChangeTick = 5000;
	}

	if(curChargeState != viaChargeState && u16LedChangeTick == 0)
		curChargeState = viaChargeState;

	switch(curChargeState)
	{
		case CHARGE_RATE_75_UNDER:
		{
			if(u16Led75UnderFlickerTick == 0)
			{
				u16Led75UnderFlickerTick = 500;
				toggleLed75();
				led100RedOff();
				led100GreenOff();
			}
			break;
		}
		case CHARGE_RATE_100_UNDER:
		{
			if(u16Led100UnderFlickerTick == 0)
			{
				u16Led100UnderFlickerTick = 500;
				led75On();
				toggleLed100Red();
				led100GreenOff();
			}
			break;
		}
		case CHARGE_RATE_100:
		{
			led75Off();
			led100RedOff();
			led100GreenOn();
			break;
		}
		default:
		{
			curChargeState = CHARGE_RATE_UNKNOWN;
			led75Off();
			led100RedOff();
			led100GreenOff();
			break;
		}
	}
}
//--------------------------------------------------------------------------------------------//
//
//--------------------------------------------------------------------------------------------//
/**
  * @brief	process Light LED
  */
void processLightLed(void)
{
	if(getMasterIn())
	{
		bool bLightOn = getLightOn();
		if(bLightOn)
			ledCtrOn();
		else
			ledCtrOff();
		bCurLedCtr = bLightOn;
	}
	else
	{
		ledCtrOn();
		bCurLedCtr = true;
	}
}
//--------------------------------------------------------------------------------------------//
//
//--------------------------------------------------------------------------------------------//
/**
  * @brief	process audio amp
  */
void processAudioAmpProcess(void)
{
	if(getAudioOn())
	{
		bAmpOnOff = true;
		if(bCurAmpOnOff != bAmpOnOff)
		{
			bCurAmpOnOff = bAmpOnOff;
			setAmpSd(true);
			u16AmpSettingTick = 500;
		}

		if(bAmpSettingDetected)
		{
			bAmpSettingDetected = false;
			ampMuteOff();
		}
	}
	else
	{
		bAmpOnOff = false;
		if(bCurAmpOnOff != bAmpOnOff)
		{
			bCurAmpOnOff = bAmpOnOff;
			ampMuteOn();
			u16AmpSettingTick = 500;
		}

		if(bAmpSettingDetected)
		{
			bAmpSettingDetected = false;
			setAmpSd(false);
		}
	}
}
//--------------------------------------------------------------------------------------------//
//
//--------------------------------------------------------------------------------------------//
/**
  * @brief	process get battery voltage
  */
void processGetBatVol(void)
{
	u16CntBatVol++;
	uint16_t batVol = getAdc1Vol();
	if(getVccIn())
	{
		batVol-=VOLTAGE_CHARGE_BAT;
	}
	else
	{
		if(bCurLedCtr)
			batVol += VOLTAGE_LOAD_LED;
		if(getAudioOn())
			batVol += VOLTAGE_LOAD_AUDIO;
	}
	u16BatVolSum += batVol;
	if(u16CntBatVol >= 1000)
	{
		u16BatVol = u16BatVolSum / u16CntBatVol;
		u16CntBatVol = 0;
		u16BatVolSum = 0;
	}
}
//--------------------------------------------------------------------------------------------//
//
//--------------------------------------------------------------------------------------------//
/**
  * @brief	process Test Debug printf
  */
void processTestDebug(void)
{
	if(u16CntBatVol == 1)	// cycle 1 sec
	{
		PrintfUart1(printf("getChargerDet():%d\n", getChargerDet()));
		PrintfUart1(printf("u16BatVol:%d\n", u16BatVol));
		PrintfUart1(printf("getVccIn():%d\n", getVccIn()));
		PrintfUart1(printf("getRS485Id():%d\n", getRS485Id()));
		PrintfUart1(printf("getMasterIn():%d\n", getMasterIn()));
		PrintfUart1(printf("getOverrideOn():%d\n", getOverrideOn()));
		PrintfUart1(printf("getAudioOn():%d\n", getAudioOn()));
		PrintfUart1(printf("getStandBy():%d\n\n", getStandBy()));
	}
}
//--------------------------------------------------------------------------------------------//
//
//--------------------------------------------------------------------------------------------//
