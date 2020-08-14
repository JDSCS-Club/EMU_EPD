/*------------------------------------------------------------------------------------------
	Project			: RFM
	Description		: ADC Battery Power

	Writer			: $Author: zlkit $
	Revision		: $Rev: 1891 $
	Date			: 2020. 08.
	Copyright		: Piranti Corp.	( sw@piranti.co.kr )
	 
	Revision History 
	1. 2020. 08.	: Created
-------------------------------------------------------------------------------------------*/

//========================================================================
// Header

#include "main.h"

#include "adc.h"

//========================================================================
// Define

#ifndef GetDbgLevel

#define GetDbgLevel( args ... )     (0)         //  GetDbgLevel이 정의되지 않으면 Default 0

#endif

//========================================================================
// Function

//========================================================================
void Adc_Power( void )
//========================================================================
{
    enum
    {
        VCCMax = 2550,
        VCCMin = 1000,
    };

    static int old_adc_pwr = 3100;
    int adc_pwr;

    static int  bOldIsCharge = 0;        //  충전중.
    int         bIsCharge;       //  충전중.

    HAL_ADC_Start( &hadc1 );
    HAL_ADC_PollForConversion( &hadc1, 100 );

    adc_pwr = HAL_ADC_GetValue( &hadc1 );

    //  충전중인지 검사.
    bIsCharge = ( HAL_GPIO_ReadPin( BAT_CHRG_GPIO_Port, BAT_CHRG_Pin ) ) ? 0 : 1;

    if( GetDbgLevel() > 0 )
    {
        printf( "%s(%d) - adc_pwr : %d / old_adc_pwr : %d / BAT_CHRG : %d\n",
            __func__, __LINE__,
            adc_pwr, old_adc_pwr,
            bIsCharge
        );
    }

    if ( bOldIsCharge != bIsCharge )
    {
        if ( bIsCharge == 0 )
        {
            //  충전 단자 제거시 -  LED Off
            HAL_GPIO_WritePin( LED_ON_A_GPIO_Port, LED_ON_A_Pin, GPIO_PIN_RESET );  //  Green
            HAL_GPIO_WritePin( LED_ON_B_GPIO_Port, LED_ON_B_Pin, GPIO_PIN_RESET );  //  Red
        }

        bOldIsCharge = bIsCharge;
    }

    if( bIsCharge )
    {
        //  충전중. - Charging Start.

        LCDBattery( 6 );

        //  LED 상태. - 충전중 Red / 충전완료 : Green

        if ( adc_pwr < 2550 )
        {
            //  충전중.
            //  Red LED
            HAL_GPIO_WritePin( LED_ON_A_GPIO_Port, LED_ON_A_Pin, GPIO_PIN_RESET );  //  Green
            HAL_GPIO_WritePin( LED_ON_B_GPIO_Port, LED_ON_B_Pin, GPIO_PIN_SET );    //  Red
        }
        else
        {
            //  충전완료.
            //  Green LED
            HAL_GPIO_WritePin( LED_ON_A_GPIO_Port, LED_ON_A_Pin, GPIO_PIN_SET );    //  Green
            HAL_GPIO_WritePin( LED_ON_B_GPIO_Port, LED_ON_B_Pin, GPIO_PIN_RESET );  //  Red
        }
    }
    else
    {
        if ( adc_pwr > 2450 )                           LCDBattery( 5 );
        else if ( 2400 < adc_pwr && adc_pwr <= 2450 )   LCDBattery( 4 );
        else if ( 2350 < adc_pwr && adc_pwr <= 2400 )   LCDBattery( 3 );
        else if ( 2200 < adc_pwr && adc_pwr <= 2350 )   LCDBattery( 2 );
        else if ( 1000 < adc_pwr && adc_pwr <= 2200 )   LCDBattery( 1 );
        else if ( adc_pwr <= 1000 )                     LCDBattery( 0 );
    }

    old_adc_pwr = adc_pwr;
}

//========================================================================
