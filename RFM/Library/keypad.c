/*------------------------------------------------------------------------------------------
	Project			: RFM
	Description		:

	Writer			: $Author: zlkit $
	Revision		: $Rev: 1891 $
	Date			: 2019. 10.
	Copyright		: Piranti Corp.	( sw@piranti.co.kr )
	 
	Revision History 
	1. 2020. 02.	: Created
-------------------------------------------------------------------------------------------*/

//========================================================================
// Header
#include <stdio.h>

#include "main.h"				//	DOME_GPIO_Port, ...
#include "keypad.h"

#include "Adafruit_SSD1306.h"	//	OLED Display

//#include "audio.h"				//	Audio Function

//========================================================================
// Define




//========================================================================
// Function




//========================================================================
void GetKeyStat( int *btnStat )
//========================================================================
{
	btnStat[0] = HAL_GPIO_ReadPin( DOME1_GPIO_Port, DOME1_Pin );
	btnStat[1] = HAL_GPIO_ReadPin( DOME2_GPIO_Port, DOME2_Pin );
	btnStat[2] = HAL_GPIO_ReadPin( DOME3_GPIO_Port, DOME3_Pin );
	btnStat[3] = HAL_GPIO_ReadPin( DOME4_GPIO_Port, DOME4_Pin );
	btnStat[4] = HAL_GPIO_ReadPin( DOME5_GPIO_Port, DOME5_Pin );
	btnStat[5] = HAL_GPIO_ReadPin( DOME6_GPIO_Port, DOME6_Pin );
	btnStat[6] = HAL_GPIO_ReadPin( PTT_KEY_GPIO_Port, PTT_KEY_Pin );
	btnStat[7] = HAL_GPIO_ReadPin( SOS_KEY_GPIO_Port, SOS_KEY_Pin );
	btnStat[8] = HAL_GPIO_ReadPin( ON_OFF_KEY_GPIO_Port, ON_OFF_KEY_Pin );
}

#include <string.h>		//	memcmp()

//========================================================================
void LoopProcKey ( uint32_t tickCurr )
//========================================================================
{
	static uint32_t tickBase = 0;
	static int btnStatOld[9] = { 1, };	//	Default ( 1 - pull-up )
	static int btnStat[9];

	//    if( ( HAL_GetTick() - tickBase ) >= 1000 )
	if ( ( tickCurr - tickBase ) >= 100 )
	{
		//  Period : 100 ms

		//	Key 값 얻기.
		GetKeyStat( btnStat );

		if ( memcmp( btnStatOld, btnStat, sizeof( btnStat ) ) != 0 )
		{
			//	Key Value Changed.

			if ( btnStatOld[0] != btnStat[0] )	KeyMenu		( !btnStat[0] );	//	DOME1
			if ( btnStatOld[1] != btnStat[1] )	KeyUp		( !btnStat[1] );	//	DOME2
			if ( btnStatOld[2] != btnStat[2] )	KeyOK		( !btnStat[2] );	//	DOME3
			if ( btnStatOld[3] != btnStat[3] )	KeyLight	( !btnStat[3] );	//	DOME4
			if ( btnStatOld[4] != btnStat[4] )	KeyDown		( !btnStat[4] );	//	DOME5
			if ( btnStatOld[5] != btnStat[5] )	KeyVol		( !btnStat[5] );	//	DOME6
			if ( btnStatOld[6] != btnStat[6] )	KeyPtt		( !btnStat[6] );	//	PTT
			if ( btnStatOld[7] != btnStat[7] )	KeySos		( !btnStat[7] );	//	SOS
			if ( btnStatOld[8] != btnStat[8] )	KeyPwrOnOff	( !btnStat[8] );	//	ON/OFF

			//	값 저장.
			memcpy( btnStatOld, btnStat, sizeof( btnStat ) );
		}

		tickBase = tickCurr;
	}

#if defined(USE_FREERTOS)
	osDelay( 1 );		//	1 msec
#else
//        HAL_Delay( 1 );
#endif
}



//========================================================================
//		KeyPad ( Dome Button )
//========================================================================


//========================================================================
void KeyMenu( int bValue )
//========================================================================
{
	//	bValue : 0(Up) / 1(Down)
	printf( "%s(%d)\n", __func__, __LINE__ );

	if ( bValue )
	{
//		LCDMenu();
		ProcBtnMenu();
	}
}

//========================================================================
void KeyOK( int bValue )
//========================================================================
{
	//	bValue : 0(Up) / 1(Down)
	printf( "%s(%d)\n", __func__, __LINE__ );

#if 1

	if ( bValue )
	{
		ProcBtnOK();
	}

#else

	//	Speex Loopback Test
	//	Audio Loopback Proc

	if ( bValue )
	{
		//	송신중
		LCDSetCursor( 10, 13 );
		LCDPrintf( "Audio Speex" );

		//	Spk On
		LCDSpeaker( 1 );

		HAL_GPIO_WritePin( SPK_ON_GPIO_Port, SPK_ON_Pin, GPIO_PIN_SET );

		//	Codec Loopback On
//		AudioRxTxLoop();
//		AudioLoopbackDMA();

//DEL		AudioLoopbackDMASpeex();
		//	-> Speex는 성능문제로 사용 X

		AudioLoopbackDMACompress();
	}
	else
	{
		//    편성 : 100
		LCDSetCursor( 20, 13 );
		LCDPrintf( "편성 : 100" );

		//	Spk Off
		LCDSpeaker( 0 );

		HAL_GPIO_WritePin( SPK_ON_GPIO_Port, SPK_ON_Pin, GPIO_PIN_RESET );

		//	Codec Loopback Off
		AudioStop();
	}
#endif

}


//========================================================================
void KeyLight( int bValue )
//========================================================================
{
	//	bValue : 0(Up) / 1(Down)
	printf( "%s(%d)\n", __func__, __LINE__ );

	//	Light On/Off Toggle
	static int bOnOff = 0;

	//	GPIO제어.
	if ( bValue )
	{
		if ( bOnOff == 0 )
		{
			//	Flash Light On
			HAL_GPIO_WritePin( FLASH_ON_GPIO_Port, FLASH_ON_Pin, GPIO_PIN_SET );

			LCDLight( 1 );

//			SendLight( 1 );		//	SendRF - Light On

			bOnOff = 1;
		}
		else
		{
			//	Flash Light Off
			HAL_GPIO_WritePin( FLASH_ON_GPIO_Port, FLASH_ON_Pin, GPIO_PIN_RESET );

			LCDLight( 0 );

//			SendLight( 0 );		//	SendRF - Light Off

			bOnOff = 0;
		}
	}
}

//========================================================================
void KeyUp( int bValue )
//========================================================================
{
	//	bValue : 0(Up) / 1(Down)
	printf( "%s(%d)\n", __func__, __LINE__ );

	if( bValue )
	{
		ProcBtnUp();
	}
}


//========================================================================
void KeyDown( int bValue )
//========================================================================
{
#if 1
	printf( "%s(%d)\n", __func__, __LINE__ );

	if( bValue )
	{
		ProcBtnDown();
	}

#else
	//	bValue : 0(Up) / 1(Down)
	printf( "%s(%d)\n", __func__, __LINE__ );

	if ( bValue )
	{
		//	sine
		LCDSetCursor( 10, 13 );
		LCDPrintf( "sine wave" );

		//	Spk On
		LCDSpeaker( 1 );

		HAL_GPIO_WritePin( SPK_ON_GPIO_Port, SPK_ON_Pin, GPIO_PIN_SET );

		//	Codec Loopback On
		AudioPlayDMASine();

	}
	else
	{
		//	편성 : 100
		LCDSetCursor( 20, 13 );
		LCDPrintf( "편성 : 100" );

		//	Spk Off
		LCDSpeaker( 0 );

		HAL_GPIO_WritePin( SPK_ON_GPIO_Port, SPK_ON_Pin, GPIO_PIN_RESET );

		//	Codec Loopback Off
		AudioStop();
	}
#endif
}

//========================================================================
void KeyVol( int bValue )
//========================================================================
{
	//	bValue : 0(Up) / 1(Down)
	printf( "%s(%d)\n", __func__, __LINE__ );

	//	Audio Loopback Proc

	if ( bValue )
	{
		//	송신중
		LCDSetCursor( 10, 13 );
		LCDPrintf( "Audio Loopback" );

		//	Spk On
		LCDSpeaker( 1 );

		HAL_GPIO_WritePin( SPK_ON_GPIO_Port, SPK_ON_Pin, GPIO_PIN_SET );

		//	Codec Loopback On
		AudioRxTxLoop();
		//AudioLoopbackDMA();
	}
	else
	{
		//    편성 : 100
		LCDSetCursor( 20, 13 );
		LCDPrintf( "편성 : 100" );

		//	Spk Off
		LCDSpeaker( 0 );

		HAL_GPIO_WritePin( SPK_ON_GPIO_Port, SPK_ON_Pin, GPIO_PIN_RESET );

		//	Codec Loopback Off
		AudioStop();
	}
}

//========================================================================
//		Side Button
//========================================================================


//========================================================================
void KeyPtt( int bValue )
//========================================================================
{
	//	bValue : 0(Up) / 1(Down)
	printf( "%s(%d)\n", __func__, __LINE__ );

	//	송신기 -> 송신기

	if ( bValue )
	{
		//	송신중
		LCDSetCursor( 20, 13 );
		LCDPrintf( "송신중..." );

		//	방송 - 시작
//		SendPA( 1 );		//	SendRF - Send PA ( 송신기 -> 수신기 )
	}
	else
	{
		//    편성 : 100
		LCDSetCursor( 20, 13 );
		LCDPrintf( "편성 : 100" );

		//	방송 - 종료
//		SendPA( 0 );		//	SendRF - Send PA ( 송신기 -> 수신기 )
	}

}


//========================================================================
void KeySos( int bValue )
//========================================================================
{
	//	bValue : 0(Up) / 1(Down)
	printf( "%s(%d)\n", __func__, __LINE__ );

	//	송신기 -> 수신기

	if ( bValue )
	{
		//	송신중
		LCDSetCursor( 20, 13 );
		LCDPrintf( "통화중..." );

		//	통화 - 시작
//		SendCall( 1 );		//	SendRF - Send Call ( 송신기 -> 송신기 )
	}
	else
	{
		//    편성 : 100
		LCDSetCursor( 20, 13 );
		LCDPrintf( "편성 : 100" );

		//	통화 - 종료
//		SendCall( 0 );		//	SendRF - Send Call ( 송신기 -> 송신기 )
	}
}


//========================================================================
//		Side Button
//========================================================================

//========================================================================
void	KeyPwrOnOff		( int bValue )
//========================================================================
{
	//	bValue : 0(Up) / 1(Down)
	printf( "%s(%d)\n", __func__, __LINE__ );


	if ( bValue )
	{
		//========================================================================
		//	Power Off

		HAL_GPIO_WritePin( ON_OFF_EN_GPIO_Port, ON_OFF_EN_Pin, GPIO_PIN_RESET );
	}
}

//========================================================================
