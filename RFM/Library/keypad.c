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

#include "rfm.h"				//	g_nSpkLevel, ...

#include "keypad.h"

#include "Adafruit_SSD1306.h"	//	OLED Display

//#include "audio.h"				//	Audio Function

//========================================================================
// Define


//========================================================================
// Function

static int s_btnStat[9] = { 1, };	//	Default ( 1 - pull-up )

//========================================================================
int	GetKey( int eKey )
//========================================================================
{
	return !(s_btnStat[eKey]);	//	Pull-up ( Active Low )
}


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
	int 	btnStat[9];

	//    if( ( HAL_GetTick() - tickBase ) >= 1000 )
	if ( ( tickCurr - tickBase ) >= 100 )
	{
		//  Period : 100 ms

		//	Key 값 얻기.
		GetKeyStat( btnStat );

		if ( memcmp( s_btnStat, btnStat, sizeof( btnStat ) ) != 0 )
		{
			//	Key Value Changed.

			if ( s_btnStat[0] != btnStat[0] )	KeyMenu		( !btnStat[0] );	//	DOME1
			if ( s_btnStat[1] != btnStat[1] )	KeyUp		( !btnStat[1] );	//	DOME2
			if ( s_btnStat[2] != btnStat[2] )	KeyOK		( !btnStat[2] );	//	DOME3
			if ( s_btnStat[3] != btnStat[3] )	KeyLight	( !btnStat[3] );	//	DOME4
			if ( s_btnStat[4] != btnStat[4] )	KeyDown		( !btnStat[4] );	//	DOME5
			if ( s_btnStat[5] != btnStat[5] )	KeyVol		( !btnStat[5] );	//	DOME6
			if ( s_btnStat[6] != btnStat[6] )	KeyPtt		( !btnStat[6] );	//	PTT
			if ( s_btnStat[7] != btnStat[7] )	KeySos		( !btnStat[7] );	//	SOS
			if ( s_btnStat[8] != btnStat[8] )	KeyPwrOnOff	( !btnStat[8] );	//	ON/OFF

			//	값 저장.
			memcpy( s_btnStat, btnStat, sizeof( btnStat ) );
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
	//========================================================================
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
	//========================================================================
	//	bValue : 0(Up) / 1(Down)
	printf( "%s(%d)\n", __func__, __LINE__ );

	if ( bValue )
	{
		ProcBtnOK();
	}
}


//========================================================================
void KeyLight( int bValue )
//========================================================================
{
	//========================================================================
	//	bValue : 0(Up) / 1(Down)
	printf( "%s(%d)\n", __func__, __LINE__ );

	//========================================================================
	//	Light On/Off Toggle
	static int bOnOff = 0;

	//========================================================================
	//	GPIO제어.
	if ( bValue )
	{
		if ( bOnOff == 0 )
		{
			//	Flash Light On
			HAL_GPIO_WritePin( FLASH_ON_GPIO_Port, FLASH_ON_Pin, GPIO_PIN_SET );

			LCDLight( 1 );
			bOnOff = 1;
		}
		else
		{
			//	Flash Light Off
			HAL_GPIO_WritePin( FLASH_ON_GPIO_Port, FLASH_ON_Pin, GPIO_PIN_RESET );

			LCDLight( 0 );
			bOnOff = 0;
		}
	}
}

//========================================================================
void KeyUp( int bValue )
//========================================================================
{
	//========================================================================
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
	//========================================================================
	//	bValue : 0(Up) / 1(Down)
	printf( "%s(%d)\n", __func__, __LINE__ );

	if( bValue )
	{
		ProcBtnDown();
	}
}

//========================================================================
void KeyVol( int bValue )
//========================================================================
{
	//========================================================================
	//	bValue : 0(Up) / 1(Down)
	printf( "%s(%d)\n", __func__, __LINE__ );

	if( bValue )
	{

		g_nSpkLevel = ( g_nSpkLevel + 1 ) % 4;  //  0, 1, 2, 3

		//========================================================================
		SetSpkVol( g_nSpkLevel );
		//========================================================================

		if ( g_nSpkLevel )
		{
			//	RFM SPK On
//				HAL_GPIO_WritePin( SPK_ON_GPIO_Port, SPK_ON_Pin, GPIO_PIN_SET );
			//	RFM SPK Off
			HAL_GPIO_WritePin( SPK_ON_GPIO_Port, SPK_ON_Pin, GPIO_PIN_RESET );

			LCDSpeaker( g_nSpkLevel );
		}
		else
		{
			//	RFM SPK Off
			HAL_GPIO_WritePin( SPK_ON_GPIO_Port, SPK_ON_Pin, GPIO_PIN_RESET );

			LCDSpeaker( 0 );
		}
	}
}

//========================================================================
//		Side Button
//========================================================================


//========================================================================
void KeyPtt( int bValue )
//========================================================================
{
	//========================================================================
	//	bValue : 0(Up) / 1(Down)
	printf( "%s(%d)\n", __func__, __LINE__ );

	//	송신기 -> 송신기

	if ( bValue )
	{
		LCDSetCursor( 20, 13 );
		LCDPrintf( "방송중..." );

		SetRFMMode( RFMModeTx );

		//  Green LED On
		HAL_GPIO_WritePin ( LED_ON_A_GPIO_Port, LED_ON_A_Pin, GPIO_PIN_SET ); //  Green LED

//		//	방송 - 시작
//		SendPA( 1 );		//	SendRF - Send PA ( 송신기 -> 수신기 )
	}
	else
	{
		//========================================================================
		//  편성 : XXX
		UpdateLCDMain();

		//========================================================================
		RF_Rx_Mode();
		//========================================================================

		SetRFMMode( RFMModeNormal );

		//  Green LED Off
		HAL_GPIO_WritePin ( LED_ON_A_GPIO_Port, LED_ON_A_Pin, GPIO_PIN_RESET ); //  Green LED

//		// Start RX with radio packet length
//		vRadio_StartRX (
//			pRadioConfiguration->Radio_ChannelNumber,
//			pRadioConfiguration->Radio_PacketLength );

//		//	방송 - 종료
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
//		Power Button
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


//========================================================================
//		Power Button
//========================================================================


void	KeyTestLoopback( int bValue )
{

#if 0

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

		//	Codec Loopback On
//		AudioPlayDMASine();

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
