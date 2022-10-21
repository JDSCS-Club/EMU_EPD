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

uint8_t uDI_getMasterIn;
uint8_t uSpk_Stat;
uint8_t uRssi_NgFlag;

uint8_t uAudioPlayFlag = false;
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
#if defined( MASTER_IN_OUT_GPIO_Port )

//	MASTER_IN 신호 Output으로 수정.
bool getMasterIn(void){ return 1; }
#else
bool getMasterIn(void){ return HAL_GPIO_ReadPin(MASTER_IN_GPIO_Port, MASTER_IN_Pin); }
#endif
//--------------------------------------------------------------------------------------------//
//
//--------------------------------------------------------------------------------------------//
/**
  * @brief  get Digital input
  */
bool getDICtl(void){ return HAL_GPIO_ReadPin(DI_CTL_GPIO_Port, DI_CTL_Pin); }
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

bool getVccRfIn(void){ return HAL_GPIO_ReadPin(VCC_RF_IN_GPIO_Port, VCC_RF_IN_Pin); }
bool getAmpFault(void){ return HAL_GPIO_ReadPin(AMP_FAULT_GPIO_Port, AMP_FAULT_Pin); }
bool getVccLedIn(void){ return HAL_GPIO_ReadPin(VCC_LED_IN_GPIO_Port, VCC_LED_IN_Pin); }
bool getVccAudioIn(void){ return HAL_GPIO_ReadPin(VCC_AUDIO_IN_GPIO_Port, VCC_AUDIO_IN_Pin); }
//--------------------------------------------------------------------------------------------//
//
//--------------------------------------------------------------------------------------------//
/**
  * @brief  set LED CTR pin
  */
void setLedCtr(void ){  HAL_GPIO_WritePin(LED_CTL_GPIO_Port, LED_CTL_Pin, true); }
void ledCtrOn(void){  HAL_GPIO_WritePin(LED_CTL_GPIO_Port, LED_CTL_Pin, true); }
void ledCtrOff(void){  HAL_GPIO_WritePin(LED_CTL_GPIO_Port, LED_CTL_Pin, false); }
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
void setAmpMute(bool state)
{ 
    HAL_GPIO_WritePin(MUTE_GPIO_Port, MUTE_Pin, state); 
    HAL_GPIO_WritePin(MUTE_GPIO_Port, MUTE_Pin, state); 
}


void ampMuteOn(void)
{
	uint8_t     nTbuf[10];
	uint8_t     nRbuf[10];

	HAL_StatusTypeDef result;

	//HAL_GPIO_WritePin(MUTE_GPIO_Port, MUTE_Pin, false);
	result = HAL_I2C_IsDeviceReady( &hi2c2, (uint16_t)(0xD8), 2, 2 );

	if ( HAL_OK == result  )
	{
		////////////////////////////////////////////////////////////////////////
		nTbuf[0] = 0X1F;


		if(MB85_HAL_WriteBytes(&hi2c2,0xD8,0x0C,(uint8_t *)nTbuf,1))
		{
			printf(" ampMuteOn OK \n");
		}
		else
		{
			printf( "ampMuteOn NG \n" );
		}

	}


}


void ampMuteOff(void)
{
	uint8_t     nTbuf[10];
	uint8_t     nRbuf[10];


	HAL_StatusTypeDef result;

	//HAL_GPIO_WritePin(MUTE_GPIO_Port, MUTE_Pin, true);

	result = HAL_I2C_IsDeviceReady( &hi2c2, (uint16_t)(0xD8), 2, 2 );

	if ( HAL_OK == result  )
	{

		////////////////////////////////////////////////////////////////////////


        nTbuf[0] = 0x00;
            
		if(MB85_HAL_WriteBytes(&hi2c2,0xD8,0x0C,(uint8_t *)nTbuf,1))
		{
			printf("-Mute OFF Write Test(0xD8-0x%02X) OK \n",nTbuf[0] );
		}
		else
		{
			printf( "-Mute OFF Write Test(0xD8-0x0C) NG \n" );
		}


        HAL_Delay(50);
		////////////////////////////////////////////////////////////////////////
		nRbuf[0] = 0xFF;

		if(MB85_HAL_ReadBytes(&hi2c2,0xD8,0x06,(uint8_t *)nRbuf,1))
		{
			printf("-Play and Mute Mode Read Test(0xD8-0x%02X) OK \n",nRbuf[0] );
		}
		else
		{
			printf( "-0x00 Read Test(0xD8-0x06) NG \n" );
		}

	}

}
//--------------------------------------------------------------------------------------------//
//
//--------------------------------------------------------------------------------------------//
/**
  * @brief  set Amp SD
  */
void setAmpSd(bool state)
{
	  uint8_t     nTbuf[10];
	  uint8_t     nRbuf[10];

	  HAL_StatusTypeDef result;

    // AUDIO
	HAL_GPIO_WritePin(SD_GPIO_Port, SD_Pin, true); //
    HAL_GPIO_WritePin(SD_GPIO_Port, SD_Pin, true); //
    
        
    //HAL_GPIO_WritePin(LED_CTL_GPIO_Port, LED_CTL_Pin, state); //
    //HAL_GPIO_WritePin(LED_CTL_GPIO_Port, LED_CTL_Pin, state); //
    
        
    setAmpMute(true);

	if(state == true)
	{


		result = HAL_I2C_IsDeviceReady( &hi2c2, (uint16_t)(0xD8), 2, 2 );

		if ( HAL_OK == result  )
		{
				////////////////////////////////////////////////////////////////////////


				////////////////////////////////////////////////////////////////////////
				//nTbuf[0] = 0xBE;

                nTbuf[0] = 0xAA;

				if(MB85_HAL_WriteBytes(&hi2c2,0xD8,0x08,(uint8_t *)nTbuf,1))
				{
					printf("-Gain Select (0xD8-0x%02X) OK \n",nTbuf[0] );
				}
				else
				{
					printf( "-Gain Select (0xD8-0x08) NG \n" );
				}


				////////////////////////////////////////////////////////////////////////
//				nTbuf[0] = 0x0C;
                nTbuf[0] = 0x0C;


				if(MB85_HAL_WriteBytes(&hi2c2,0xD8,0x0A,(uint8_t *)nTbuf,1))
				{
					printf("- Clip_OTW Configuration (0xD8-0x%02X) OK \n",nTbuf[0] );
				}
				else
				{
					printf( "- Clip_OTW Configuration (0xD8-0x0A) NG \n" );
				}
				////////////////////////////////////////////////////////////////////////
				//nTbuf[0] = 0xD0;
                nTbuf[0] = 0xD6;

				if(MB85_HAL_WriteBytes(&hi2c2,0xD8,0x0B,(uint8_t *)nTbuf,1))
				{
					printf("-Load Diagnostics (0xD8-0x%02X) OK \n",nTbuf[0] );
				}
				else
				{
					printf( "-Load Diagnostics (0xD8-0x0B) NG \n" );
				}



                HAL_Delay(30);

				////////////////////////////////////////////////////////////////////////
//				nTbuf[0] = 0x00;
//
//				if(MB85_HAL_WriteBytes(&hi2c2,0xD8,0x0D,(uint8_t *)nTbuf,1))
//				{
//					printf("-Low-low state disabled Write (0xD8-0x%02X) OK \n",nTbuf[0] );
//				}
//				else
//				{
//					printf( "-Low-low state disabled Write (0xD8-0x0D) NG \n" );
//				}
                
                ////////////////////////////////////////////////////////
//				nRbuf[0] = 0xFF;
//
//				if(MB85_HAL_ReadBytes(&hi2c2,0xD8,0x00,(uint8_t *)nRbuf,1))
//				{
//					printf("-0xD8-0x00 Read Test(0x00-0x%02X) OK \n",nRbuf[0] );
//				}
//				else
//				{
//					printf( "-0x00 Read Test(0x00-0x00) NG \n" );
//				}
                    
                 
//                if(MB85_HAL_ReadBytes(&hi2c2,0xD8,0x00,(uint8_t *)nRbuf,1))
//				{
//					printf("-0xD8-0x00 Read Test(0x00-0x%02X) OK \n",nRbuf[0] );
//				}
//				else
//				{
//					printf( "-0x00 Read Test(0x00-0x00) NG \n" );
//				}

            ////////////////////////////////////////////////////////////////////////
				nTbuf[0] = 0x09;
                HAL_Delay(30);
				if(MB85_HAL_WriteBytes(&hi2c2,0xD8,0x0C,(uint8_t *)nTbuf,1))
				{
					printf("-play mode (0xD8-0x%02X) OK \n",nTbuf[0] );
				}
				else
				{
					printf( "-play mode (0xD8-0x0C) NG \n" );
				}

		}


	}
        
     HAL_Delay(150);
    
   // setAmpMute(false);

}
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
    static int sDataVal = 0;
    static int sDateValRe = 0;

    static int sCnt = 0;

	setOverrideOn(uDI_getMasterIn);


    sDataVal = uDI_getMasterIn;

    if(sDataVal != sDateValRe)
    {

      sCnt++;

      if(sCnt > 5 )
      {
        sCnt = 0;
        sDateValRe = sDataVal;

      }

       cmd_occ(2);

    }
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
    static int sCnnt = 0;

	if(uDI_getMasterIn)
	{
         sCnnt++;

		//	방공등 Off
		bool bLightOn = getLightOn();

		if(bLightOn)
        {
			ledCtrOn();

//             if(!(sCnnt%5000))
//            {
//                printf("---ledCtrOn \n\r" );
//            }
        }
		else
        {
			ledCtrOff();

//             if(!(sCnnt%5000))
//            {
//                printf("---ledCtrOff \n\r" );
//            }

        }

		bCurLedCtr = bLightOn;
	}
	else
	{
		//	방공등 On
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

			HAL_GPIO_WritePin(SD_GPIO_Port, SD_Pin, true); // SD

			u16AmpSettingTick = 500;
		}

		if(bAmpSettingDetected)
		{
			bAmpSettingDetected = false;
			ampMuteOff();
                
            uAudioPlayFlag = true;
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
                
            uAudioPlayFlag = false;
		}

		if(bAmpSettingDetected)
		{
			bAmpSettingDetected = false;
			//setAmpSd(false);

			//HAL_GPIO_WritePin(SD_GPIO_Port, SD_Pin, false); // SD
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
		PrintfUart1(printf("getDICtl():%d\n", getDICtl()));
		PrintfUart1(printf("getOverrideOn():%d\n", getOverrideOn()));
		PrintfUart1(printf("getAudioOn():%d\n", getAudioOn()));
		PrintfUart1(printf("getStandBy():%d\n\n", getStandBy()));
	}
}
//--------------------------------------------------------------------------------------------//
// 전류 센서 측정 하는 부분.
// 최종 기능 업데이트시 적용 필요.   ( LED가 동작무 부하일때 측정중지하는 부분 필요 -  무부하일때 측정값 틀어짐 확인)
//--------------------------------------------------------------------------------------------//

void processCurrentVal(void)
{
       uint8_t     nRbuf[10];
       int sData_val = 0;



    if(bCurLedCtr) // 전류 센서 값은 LED 동작 중일 때만 동작 한다.
    {
        nRbuf[0] = 0x41;
        nRbuf[1] = 0x27;

        if(MB85_HAL_WriteBytes(&hi2c2,0x80,0x00,(uint8_t *)nRbuf,2))
        {
            printf("++++++++++++ Write Calibration Register  ++++++++++++ \n\r" );
        }
        else
        {
            printf( "++Write Test NG \n\r" );
        }


        nRbuf[0] = 0xFF;
        nRbuf[1] = 0xFF;
         if(MB85_HAL_ReadBytes(&hi2c2,0x80,0x01,(uint8_t *)nRbuf,2))
        {
            //printf("++ INA226 OK \n\r");

             printf( "++ Read Shunt Register (%02X,%02X) \n\r",nRbuf[0],nRbuf[1] );
        }
        else
        {
            printf( "++ INA226 NG \n\r" );
        }



         if(MB85_HAL_ReadBytes(&hi2c2,0x80,0x02,(uint8_t *)nRbuf,2))
        {
            //printf("++ INA226 OK \n\r");

             sData_val = ((nRbuf[0] << 8) | nRbuf[1]) / 800;

             printf( "++ Read Bus Voltage Register (%d) n\r",sData_val );
        }
        else
        {
            printf( "++ INA226 NG \n\r" );
        }



        /////////////////////////////////////IC  상태를 전류 측정 상태로 변경 하는 값.
            nRbuf[0] = 0x0A;
            nRbuf[1] = 0x00;

            if(MB85_HAL_WriteBytes(&hi2c2,0x80,0x05,(uint8_t *)nRbuf,2))
            {
                printf("++ Write V-C Change \n\r" );
            }
            else
            {
                printf( "++Write Calibration NG \n\r" );
            }
      /////////////////////////////////////

         if(MB85_HAL_ReadBytes(&hi2c2,0x80,0x04,(uint8_t *)nRbuf,2))
        {

             sData_val = ((nRbuf[0] << 8) | nRbuf[1]) / 33;

            printf( "++ Read Current Register(%2d) \n\r",sData_val );
        }
        else
        {
            printf( "++ INA226 Read NG \n\r");
        }




        if(MB85_HAL_ReadBytes(&hi2c2,0x80,0x03,(uint8_t *)nRbuf,2))
        {
            //printf("++ INA226 OK \n\r");

             printf( "++ Read Power Register(%02X,%02X) \n\r",nRbuf[0],nRbuf[1] );
        }
        else
        {
            printf( "++ INA226 NG \n\r" );

        }



    }



}


/*****************************************************************************
* @brief -
* @param -
* @retval-
******************************************************************************/

uint8_t AMP_SPK_CHECK(void)
{

    uint8_t     nRbuf_1[2];
    uint8_t     nRbuf_2[2];

    uint8_t     nTbuf[2];


    uint8_t sSpkData;

    setAmpMute(true);
HAL_Delay(100);


    HAL_GPIO_WritePin(SD_GPIO_Port, SD_Pin, true); // SD

    HAL_GPIO_WritePin(SD_GPIO_Port, SD_Pin, true); // SD
    
    

    setAmpSd(true);
        
//
//    nTbuf[0] = 0x09; // Play Mode 설정
//   MB85_HAL_WriteBytes(&hi2c2, 0xD8, 0x0C, (uint8_t *)nTbuf, 1);
//   HAL_Delay(50);
//
//
//
//      ampMuteOff();  
//   // HAL_Delay(100);   
//    nTbuf[0] = 0xD6; //0x46
//    if(MB85_HAL_WriteBytes(&hi2c2,0xD8,0x0B,(uint8_t *)nTbuf,1))
//	{
//		printf("-Load Diagnostics (0xD8-0x%02X) OK \n",nTbuf[0] );
//	}
////    
//    ampMuteOn();
     
    HAL_Delay(100);



//    nRbuf_1[0] = 0xFF;
//    nRbuf_2[0] = 0xFF;
//
//    MB85_HAL_ReadBytes(&hi2c2, 0xD8, 0x07, (uint8_t *)nRbuf_1, 1);
//    printf("+++++ D8  getAmp1 Spk read :%02X\r\n", (nRbuf_1[0]));



	MB85_HAL_ReadBytes(&hi2c2,0xD8,0x02,(uint8_t *)nRbuf_1,1);
    
	MB85_HAL_ReadBytes(&hi2c2,0xD8,0x03,(uint8_t *)nRbuf_2,1);



    printf("+++++ getAmp1 Spk read :%02X - %02X \r\n", (nRbuf_1[0]), (nRbuf_2[0]));

    sSpkData  = (nRbuf_1[0] & 0x80) == 0x80 ? 1 : 0;
    
    sSpkData  |= (nRbuf_2[0] & 0x08) == 0x08 ? (0x01<<1) : 0;

    
    printf("+++++ getAmp1 Spk Retun : %02X \n",sSpkData);
    
HAL_Delay(50);

//    ampMuteOn();

     
         
    //HAL_GPIO_WritePin(SD_GPIO_Port, SD_Pin, false); // SD
    //setAmpMute(false);

    //HAL_GPIO_WritePin(SD_GPIO_Port, SD_Pin, false); // SD
     return sSpkData;

}


//--------------------------------------------------------------------------------------------//
//
//--------------------------------------------------------------------------------------------//
/*****************************************************************************
* @brief -
* @param -
* @retval-
******************************************************************************/


void ONTD_Function(void)
{
    static int sMasterCnt = 0;

    ONTD(getMasterIn(),&uDI_getMasterIn,3,&sMasterCnt);

}



/*****************************************************************************
* @brief -
* @param -
* @retval-
******************************************************************************/
void ONTD(uint8_t IN,uint8_t *OUT,uint8_t MS,int *CLK )
{
	uint8_t CE;

	if(!IN){*CLK = 0; *OUT = 0;}
	CE = NOT(*OUT) && IN;
	if(CE)
	{
		if(MS == *CLK) {*OUT = 1;}
		else *CLK = *CLK+1;
	}
}
