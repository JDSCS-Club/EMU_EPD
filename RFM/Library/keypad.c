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

#include "menu.h"				//	g_bEnMenuMaint

//#include "audio.h"				//	Audio Function

//========================================================================
// Define


//========================================================================
// Function

static int s_btnStat[9] = { 0, };	//	Default ( 1 - pull-up )

//========================================================================
int	GetKey( int eKey )
//========================================================================
{
	return (s_btnStat[eKey]);
}


//========================================================================
void GetKeyStat( int *btnStat )
//========================================================================
{
	//	Pull-Up ( Active Low )
	btnStat[0] = !( HAL_GPIO_ReadPin( DOME1_GPIO_Port, DOME1_Pin ) );	//	Menu
	btnStat[1] = !( HAL_GPIO_ReadPin( DOME2_GPIO_Port, DOME2_Pin ) );	//	Up
	btnStat[2] = !( HAL_GPIO_ReadPin( DOME3_GPIO_Port, DOME3_Pin ) );	//	OK
	btnStat[3] = !( HAL_GPIO_ReadPin( DOME4_GPIO_Port, DOME4_Pin ) );	//	Light
	btnStat[4] = !( HAL_GPIO_ReadPin( DOME5_GPIO_Port, DOME5_Pin ) );	//	Down
	btnStat[5] = !( HAL_GPIO_ReadPin( DOME6_GPIO_Port, DOME6_Pin ) );	//	Vol
	btnStat[6] = !( HAL_GPIO_ReadPin( PTT_KEY_GPIO_Port, PTT_KEY_Pin ) );
	btnStat[7] = !( HAL_GPIO_ReadPin( SOS_KEY_GPIO_Port, SOS_KEY_Pin ) );

	//	Pull-Down ( Active High )
	btnStat[8] = ( HAL_GPIO_ReadPin( ON_OFF_KEY_GPIO_Port, ON_OFF_KEY_Pin ) );
}

#include <string.h>		//	memcmp()

//========================================================================
void LoopProcKey ( uint32_t tickCurr )
//========================================================================
{
	//========================================================================
	static uint32_t tickBase = 0;
	int 	btnStat[9];

	//    if( ( HAL_GetTick() - tickBase ) >= 1000 )
	if ( ( tickCurr - tickBase ) >= 100 )
	{
		//========================================================================
		//  Period : 100 ms

		//========================================================================
		//	Key 값 얻기.
		GetKeyStat( btnStat );

		if ( memcmp( s_btnStat, btnStat, sizeof( btnStat ) ) != 0 )
		{
			//	Key Value Changed.
//			int i;
//			printf("[s_btnStat] ");
//			for( i = 0; i < 9; i++ ) printf("%d ", s_btnStat[i]);
//			printf("\n");
//			printf("[btnStat] ");
//			for( i = 0; i < 9; i++ ) printf("%d ", btnStat[i]);
//			printf("\n");

			if ( s_btnStat[0] != btnStat[0] )	KeyMenu		( btnStat[0] );	//	DOME1
			if ( s_btnStat[1] != btnStat[1] )	KeyUp		( btnStat[1] );	//	DOME2
			if ( s_btnStat[2] != btnStat[2] )	KeyOK		( btnStat[2] );	//	DOME3
			if ( s_btnStat[3] != btnStat[3] )	KeyLight	( btnStat[3] );	//	DOME4
			if ( s_btnStat[4] != btnStat[4] )	KeyDown		( btnStat[4] );	//	DOME5
			if ( s_btnStat[5] != btnStat[5] )	KeyVol		( btnStat[5] );	//	DOME6
			if ( s_btnStat[6] != btnStat[6] )	KeyPtt		( btnStat[6] );	//	PTT
			if ( s_btnStat[7] != btnStat[7] )	KeySos		( btnStat[7] );	//	SOS
			if ( s_btnStat[8] != btnStat[8] )	KeyPwrOnOff	( btnStat[8] );	//	ON/OFF

			//	값 저장.
			memcpy( s_btnStat, btnStat, sizeof( btnStat ) );
		}

		tickBase = tickCurr;
	}

	//========================================================================
	//	* [송신기][MENU] : 유지보수 메뉴 진입 기능 추가.
	//		- [OK]버튼을 누른상태에서 [MENU]버튼을 3초간 누를 경우 Maintance모드로 진입함.

	static uint32_t tickBase2 = 0;

	if ( ( tickCurr - tickBase2 ) >= 1000 && IsMenuMaint() == 0 )
	{
		//	Period : 1 sec
		static int s_cntKeyOkMenu = 0;
		if ( s_btnStat[eKeyOk] && s_btnStat[eKeyMenu] )
		{
			s_cntKeyOkMenu++;
		}
		else
		{
			s_cntKeyOkMenu = 0;
		}

		if ( s_cntKeyOkMenu > 3 )	//	3초 이상 누를 경우.
		{
			EnableMenuMaint( 1 );
		}

		tickBase2 = tickCurr;
	}

	//========================================================================


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
	printf( "%s(%d) - %d\n", __func__, __LINE__, bValue );

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
	printf( "%s(%d) - %d\n", __func__, __LINE__, bValue );

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
	printf( "%s(%d) - %d\n", __func__, __LINE__, bValue );

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
	printf( "%s(%d) - %d\n", __func__, __LINE__, bValue );

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
	printf( "%s(%d) - %d\n", __func__, __LINE__, bValue );

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
	printf( "%s(%d) - %d\n", __func__, __LINE__, bValue );

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
//			HAL_GPIO_WritePin( SPK_ON_GPIO_Port, SPK_ON_Pin, GPIO_PIN_RESET );
			RFM_Spk(0);

			LCDSpeaker( g_nSpkLevel );
		}
		else
		{
			//	RFM SPK Off
//			HAL_GPIO_WritePin( SPK_ON_GPIO_Port, SPK_ON_Pin, GPIO_PIN_RESET );
			RFM_Spk(0);

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
#if defined( USE_TEST_PTT_AUDIO_LOOP )

	//	Audio Loopback Test
	KeyTestLoopback( bValue );

	return ;
#endif
	//========================================================================

	//========================================================================
	//	bValue : 0(Up) / 1(Down)
	printf( "%s(%d) - %d\n", __func__, __LINE__, bValue );

	//	송신기 -> 수신기	:	방송

	if ( bValue )
	{
#if defined(USE_CH_ISO_DEV)
		SetChPA( GetChNearRFM() );		//	가장 가까운 수신기 설정.

		SetChPARFT( GetChNearRFT( 190 ) );	//	가까운 송신기 설정.
#endif

		LCDSetCursor( 20, 13 );

		if ( IsMenuMaint() )
		{
			//	Maint Mode : 송신채널 표시.
			char sBuf[20];
			sprintf( sBuf, "방송중...(%d)", GetChPA() );
			LCDPrintf( sBuf );
		}
		else
		{
			LCDPrintf( "방송중..." );
		}

		SetRFMMode( RFMModeTx );

		//  Green LED On
		HAL_GPIO_WritePin ( LED_ON_A_GPIO_Port, LED_ON_A_Pin, GPIO_PIN_SET ); //  Green LED

		//	방송 - 시작
		SendPA( 1 );		//	SendRF - Send PA ( 송신기 -> 수신기 )
	}
	else
	{
		//========================================================================
		//  편성 : XXX
		UpdateLCDMain();

		SetRFMMode( RFMModeNormal );

#if defined(USE_CH_ISO_DEV)
		SetChPA( GetChNearRFM() );		//	가장 가까운 수신기 설정.

		SetChPARFT( GetChNearRFT( 190 ) );	//	가까운 송신기 설정.
#endif

		//  Green LED Off
		HAL_GPIO_WritePin ( LED_ON_A_GPIO_Port, LED_ON_A_Pin, GPIO_PIN_RESET ); //  Green LED

		//	방송 - 종료
		SendPA( 0 );		//	SendRF - Send PA ( 송신기 -> 수신기 )
	}
}


//========================================================================
void KeySos( int bValue )
//========================================================================
{
	//	bValue : 0(Up) / 1(Down)
	printf( "%s(%d) - %d\n", __func__, __LINE__, bValue );

	//	송신기 -> 송신기	:	통화

	if ( bValue )
	{
#if defined(USE_CH_ISO_DEV)
		SetChPARFT( GetChNearRFT( 190 ) );	//	가까운 송신기 설정.

		SetChPA( GetChNearRFM() );		//	가장 가까운 수신기 설정.
#endif

		//	송신중
		LCDSetCursor( 20, 13 );
		LCDPrintf( "통화중..." );

		SetRFMMode( RFMModeTx );

		//	통화 - 시작
		SendCall( CtrlStart );		//	SendRF - Send Call ( 송신기 -> 송신기 )
	}
	else
	{
#if defined(USE_CH_ISO_DEV)
		SetChPARFT( GetChNearRFT( 190 ) );	//	가까운 송신기 설정.

		SetChPA( GetChNearRFM() );		//	가장 가까운 수신기 설정.
#endif

		//    편성 : 100
		UpdateLCDMain();
//		LCDSetCursor( 20, 13 );
//		LCDPrintf( "편성 : 100" );

		SetRFMMode( RFMModeNormal );

		//	통화 - 종료
		SendCall( CtrlStop );		//	SendRF - Send Call ( 송신기 -> 송신기 )
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
	printf( "%s(%d) - %d\n", __func__, __LINE__, bValue );

	static int bPowerOn = 0;

	if ( bValue )
	{
		bPowerOn = 1;
	}
	else
	{
		if( bPowerOn )
		{
			//========================================================================
			//	Power Off
			printf( "%s(%d) - Power Off\n", __func__, __LINE__  );

			HAL_GPIO_WritePin( ON_OFF_EN_GPIO_Port, ON_OFF_EN_Pin, GPIO_PIN_RESET );
		}
	}
}

//========================================================================


//========================================================================
//		Power Button
//========================================================================


void	KeyTestLoopback( int bValue )
{

#if 1

	//	Speex Loopback Test
	//	Audio Loopback Proc

	if ( bValue )
	{
		//	송신중
		LCDSetCursor( 10, 13 );
		LCDPrintf( "Audio Loop" );

		//	Spk On
//		LCDSpeaker( 1 );
//		LCDSpeaker( 1 );

		HAL_GPIO_WritePin( SPK_ON_GPIO_Port, SPK_ON_Pin, GPIO_PIN_SET );

		//	Codec Loopback On
//		AudioRxTxLoop();
		AudioDMALoopback();

		//	Codec Loopback On
//		AudioPlayDMASine();

//DEL		AudioLoopbackDMASpeex();
		//	-> Speex는 성능문제로 사용 X
//		AudioLoopbackDMACompress();
	}
	else
	{
		//    편성 : 100
		UpdateLCDMain();
//		LCDSetCursor( 20, 13 );
//		LCDPrintf( "편성 : 100" );

		//	Spk Off
//		LCDSpeaker( 0 );

		HAL_GPIO_WritePin( SPK_ON_GPIO_Port, SPK_ON_Pin, GPIO_PIN_RESET );

		//	Codec Loopback Off
		AudioStop();
	}
#endif

}


//========================================================================
