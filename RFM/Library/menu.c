
#include "main.h"

#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdbool.h>

#include "QBuf.h"

#include "Adafruit_SSD1306.h"		//  LCD Display

#include "RFMProtocol.h"			//  SendStat()

#include "serial.h"					//  SerialInit()

#include "audio.h"					//  g_bufAudioRFRx

#if defined(_WIN32)
#else
#include "radio.h"					//  bRadio_Check_Tx_RX()
#endif

#include "adc.h"					//  Adc_Power()

#include "version.h"				//  APP_VER

#include "cli.h"

#include "menu.h"

static int stampRx;


//========================================================================
//	Menu LightCtrl
Menu_t	g_MenuLightCtrl = {
	{
		"1. 조명소등", //  조명소등
		"2. 조명점등", //  조명점등
		NULL,
	},
	2,		//	Item Count
	0,		// 	curr Idx
	ProcMenuLightCtrl		//	Callback Function
};

//========================================================================
//	Menu TrainSet
Menu_t	g_MenuTrainSet = {
	{	" 100 편성",  //  100편성
		" 101 편성",  //  101편성
		" 102 편성",  //  102편성
		" 103 편성",  //  103편성
		" 104 편성",  //  104편성
		" 105 편성",  //  105편성
		" 106 편성",  //  106편성
		" 107 편성",  //  107편성
		" 108 편성",  //  108편성
		" 109 편성",  //  109편성
		NULL,
	},
	10,		//	Item Count
	0,		// 	curr Idx
	ProcMenuTrainSet		//	Callback Function
};

//========================================================================
//	Main Menu
Menu_t	g_MenuMain = {
	{
		"1. 조명제어",
		"2. S/W 버전",
		"3. 편성설정",
		NULL,
	},
	3,		//	Item Count
	0,		// 	curr Idx
	ProcMenuMain		//	Callback Function
};

int	 g_idxTrainSet = 0;	  //  Train Set Index


//========================================================================
Menu_t	*g_pActiveMenu	=	NULL;
//========================================================================

//========================================================================
void	SetActiveMenu( Menu_t *pActiveMenu )
//========================================================================
{
	g_pActiveMenu = pActiveMenu;
}

//========================================================================
Menu_t	*GetActiveMenu( void )
//========================================================================
{
	return g_pActiveMenu;
}

//========================================================================
void	UpdateLCDMain( void )
//========================================================================
{
	char sBuf[30];

	//  Main화면 Clear
	LCDClearMain();

	//	편성 : 100
	LCDSetCursor( 20, 13 );

	sprintf( sBuf, "편성 : %d", 100 + g_idxTrainSet );
//	LCDPrintf( "편성 : 100" );
	LCDPrintf( sBuf );
}

//========================================================================
void	UpdateLCDMenu( void )
//========================================================================
{
	if ( GetActiveMenu() == NULL )
	{
		UpdateLCDMain();
		return;
	}

	char	**sMenu;
	int		*pIdxMenu, *pCntMenu;

	sMenu		=	g_pActiveMenu->sItem;
	pIdxMenu	=	&g_pActiveMenu->currIdx;
	pCntMenu	=	&g_pActiveMenu->cntItem;

	//  Main화면 Clear
	LCDClearMain();

	if ( 0 < *pIdxMenu && *pIdxMenu < *pCntMenu - 1 )	LCDMenuUpDown( 3 ); //  3. Up/Down
	else if ( *pIdxMenu == 0 )							LCDMenuUpDown( 2 ); //  2. Down
	else if ( *pIdxMenu == *pCntMenu - 1 )				LCDMenuUpDown( 1 ); //  1. Up
	else												LCDMenuUpDown( 0 ); //  0. None

	LCDSetCursor( 20, 13 );
	LCDPrintf( sMenu[*pIdxMenu] );
}


//========================================================================
void	ProcBtnUp( void )
//========================================================================
{
	if ( GetActiveMenu() == NULL ) return;

	//	Menu
	g_pActiveMenu->currIdx--;

	if ( g_pActiveMenu->currIdx < 0 )
	{
		g_pActiveMenu->currIdx = 0;
	}

	UpdateLCDMenu();   //  메뉴화면 Update
}

//========================================================================
void	ProcBtnDown( void )
//========================================================================
{
	if ( GetActiveMenu() == NULL ) return;

	//	Menu

	g_pActiveMenu->currIdx++;

	if ( g_pActiveMenu->currIdx >= g_pActiveMenu->cntItem )
	{
		g_pActiveMenu->currIdx = g_pActiveMenu->cntItem  - 1;
	}

	UpdateLCDMenu();   //  메뉴화면 Update
}

//========================================================================
void	ProcBtnMenu( void )
//========================================================================
{
	//	Menu

	if ( GetActiveMenu() == NULL )
	{
		//  메뉴모드가 아닌경우 메뉴로 진입.

		//	Set Main Menu
		SetActiveMenu( &g_MenuMain );
		GetActiveMenu()->currIdx = 0;	//	메뉴 Index초기화.
	}
	else
	{
		//  메뉴 진입상태 -> 메인화면으로 이동.

		//	Set Main Menu
		SetActiveMenu( NULL );
	}

	UpdateLCDMenu();	//	메뉴화면 Update
}

//========================================================================
void	ProcLightOn ( void )
//========================================================================
{
	LCDSetCursor( 20, 13 );
	LCDPrintf( "[조명 점등]" );

	SendLightOn();	 //  조명Off명령 전송.
	SendLightOn();	 //  조명Off명령 전송.
	SendLightOn();	 //  조명Off명령 전송.
}

//========================================================================
void	ProcLightOff ( void )
//========================================================================
{
	LCDSetCursor( 20, 13 );
	LCDPrintf( "[조명 소등]" );

	SendLightOff();	 //  조명Off명령 전송.
	SendLightOff();	 //  조명Off명령 전송.
	SendLightOff();	 //  조명Off명령 전송.
}

//========================================================================
void	ProcDispVer ( void )
//========================================================================
{
	//  S/W 버전
	LCDSetCursor( 5, 13 );
	LCDPrintf( "RFM v" APP_VER );
}

//========================================================================
void	ProcMenuTrainSet( int idxItem  )
//========================================================================
{
	LCDSetCursor( 20, 13 );
	LCDPrintf( "[편성설정]" );
	g_idxTrainSet = idxItem;  //  메뉴 Index값으로 설정.
	SetTrainSetIdx( g_idxTrainSet );

	//  Radio Channel 설정.
	pRadioConfiguration->Radio_ChannelNumber = g_idxTrainSet;

	//  1초후 Main화면 갱신.
	HAL_Delay( 1000 );
	UpdateLCDMain();

	//  메뉴 Exit
	SetActiveMenu( NULL );
}

//========================================================================
void 	ProcMenuMain( int idxItem )
//========================================================================
{
	switch ( idxItem )
	{
	case 0:		 //  조명제어
		//	Menu

		SetActiveMenu( &g_MenuLightCtrl );
		GetActiveMenu()->currIdx = 0;	//	메뉴 Index초기화.

		UpdateLCDMenu();

		break;

	case 1:		 //  S/W 버전

		//  메뉴 Exit
		SetActiveMenu( NULL );

		ProcDispVer();

		break;

	case 2:		 //  편성설정.

		SetActiveMenu( &g_MenuTrainSet );
		GetActiveMenu()->currIdx = g_idxTrainSet;	//	메뉴 Index초기화.

		UpdateLCDMenu();
		break;

#if defined(USE_ENV_TEST)
	case 3:		 //  RF 출력
		//	RF Tx시작.
		SetLoopRFTx( 1 );

		LCDSetCursor( 5, 13 );
		LCDPrintf( "[ RF Tx ]" );

		//  메뉴 Exit
		SetActiveMenu( NULL );

		break;
#endif	//	defined(USE_ENV_TEST)
	}
}


//========================================================================
void 	ProcMenuLightCtrl( int idxItem )
//========================================================================
{
	//  Tx모드
	RF_Tx_Mode();

	LCDMenuUpDown( 0 );

	if ( idxItem == 0 )
	{
		ProcLightOff();
	}
	else
	{
		ProcLightOn();
	}

	//  1초후 Main화면 갱신.
	HAL_Delay( 1000 );
	UpdateLCDMain();

	SetActiveMenu( NULL );

	//  Rx모드
	RF_Rx_Mode();
}

//========================================================================
void	ProcBtnOK( void )
//========================================================================
{
	if ( GetActiveMenu() == NULL )
	{
		LCDClearMain();
		//========================================================================
		//  편성 : XXX
		UpdateLCDMain();

		return;
	}
	else if ( g_pActiveMenu->cbFunc != NULL )
	{
		//	Menu Procedure Function
		g_pActiveMenu->cbFunc( g_pActiveMenu->currIdx );
	}
}


#if defined(USE_ENV_TEST)

static int	s_bEnLoopRFTx = 0;

//========================================================================
void	SetLoopRFTx		( int bEnable )	//	RF Data 전송 유지..
//========================================================================
{
	printf( "%s(%d) - Loop RF Tx : %d\n", __func__, __LINE__, bEnable );

	if ( bEnable )
	{
		RF_Tx_Mode();
	}
	else
	{
		RF_Rx_Mode();
	}

	s_bEnLoopRFTx = bEnable;
}


//========================================================================
int cmd_rftx      ( int argc, char * argv[] )
//========================================================================
{
	//	bEnable ( 1 / 0 )
	int bEnable = 0;
	int nCh = 0;	//	Channel
	uint8_t nPA = 0x7F;	//	Power Amplifier
	char *sRFType;

	switch ( argc )
	{
	case 5:		sscanf( argv[4], "%d", &nPA );		//	Power Amplifier
	case 4:		sscanf( argv[3], "%d", &nCh );		//	Channel
	case 3:		sRFType = argv[2];					//	RF Type [ u (unmodulated) / m(modulated) ]
	case 2:		sscanf( argv[1], "%d", &bEnable );	//	cmd [Enable]
//	case 2:		sText = argv[1];					//	sscanf( argv[1], "%s", sText );		//	cmd [Text] 
		break;
	}

	printf( "%s(%d) - En(%d) / Type(%s) / Ch(%d) / PA(%d)\n", __func__, __LINE__,
			bEnable, sRFType, nCh, nPA );

	//========================================================================
	//	채널 설정.
	pRadioConfiguration->Radio_ChannelNumber = nCh;

	//========================================================================
	//	RF Type
	if ( strcmp( sRFType, "u" ) == 0 )
	{
		SetLoopRFTx( 0 );	//	RF Loop 종료.
		//	Continuous Wave ( 무변조방식 )
		vRadio_Init_cw();	//	무변조방식 설정.

		vRadio_RF_Pwr( nPA );

//		vRadio_StartTx( nCh, NULL );
		vRadio_StartTx( pRadioConfiguration->Radio_ChannelNumber, NULL );

		return 0;
	}
	else if ( strcmp( sRFType, "m" ) == 0 )
	{
		vRadio_Init();	//	변조방식 설정.
		//========================================================================
		//	RF출력 설정.
		vRadio_RF_Pwr( nPA );
	}

	if ( argc >= 2 )
	{
		if ( strcmp( argv[1], "1" ) == 0 )			bEnable = 1;
		else if ( strcmp( argv[1], "0" ) == 0 )		bEnable = 0;
	}

	printf( "%s(%d) - Loop RF Tx : %d\n", __func__, __LINE__, bEnable );

	SetLoopRFTx( bEnable );
}

#endif	//	defined(USE_ENV_TEST)


//========================================================================


//#define	 USE_PTT_KEY_TOGGLE	  1	   //  PTT Toggle Key

#if 0

//========================================================================
void RF_PA_loop ( void )
//========================================================================
{
	static int idx = 0;

	int i;

	static U8 lPktSending = 0u;
	HAL_StatusTypeDef res;

	RFMPkt	bufRFTx;

	U8 bMain_IT_Status;

	int nTick;

	nTick = HAL_GetTick();  //  TickCount

#if defined(USE_ENV_TEST)

	if ( s_bEnLoopRFTx )
	{
		//	RF 전송모드

		vRadio_StartTx_Variable_Packet (
			pRadioConfiguration->Radio_ChannelNumber,
			&bufRFTx,
			pRadioConfiguration->Radio_PacketLength );

	}

#endif	//	defined(USE_ENV_TEST)

	//========================================================================
	//  Transmit - 송신기
	if ( GetDevID() == DevRF900T )
	{
		if( GetRFMMode() != RFMModeRx )
		{
			//  수신중이 아닌경우 PTT / SOS키 누를시 음성송출.
			static int bPTTOnOff = 0;	   //  PTT On/Off상태
			static int bOldPTT_Key = 1;
			int bPTT_Key;

			bPTT_Key = HAL_GPIO_ReadPin( PTT_KEY_GPIO_Port, PTT_KEY_Pin );

			if( bOldPTT_Key != bPTT_Key )
			{

#if defined(USE_PTT_KEY_TOGGLE)
				if ( bPTT_Key == 1 )
				{
					bPTTOnOff = !( bPTTOnOff );	 //  Toggle Key
				}
#else
				bPTTOnOff = !(bPTT_Key);			//  Press(On) / Release(Off)
#endif // USE_PTT_KEY_TOGGLE

				//  PTT_KEY ON Check
				if( bPTTOnOff )
				{				//	송신중
					LCDSetCursor( 20, 13 );
					LCDPrintf( "방송중..." );

					SetRFMMode( RFMModeTx );

					//  Green LED On
					HAL_GPIO_WritePin ( LED_ON_A_GPIO_Port, LED_ON_A_Pin, GPIO_PIN_SET ); //  Green LED
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

					// Start RX with radio packet length
					vRadio_StartRX (
						pRadioConfiguration->Radio_ChannelNumber,
						pRadioConfiguration->Radio_PacketLength );

				}

				bOldPTT_Key = bPTT_Key;
			}

			static int bSOSOnOff = 0;	   //  SOS On/Off상태
			static int bOldSOS_Key = 1;
			int bSOS_Key;

			bSOS_Key = HAL_GPIO_ReadPin( SOS_KEY_GPIO_Port, SOS_KEY_Pin );

			if( bOldSOS_Key != bSOS_Key )
			{
				bSOSOnOff = !( bSOS_Key );

				//  PTT_KEY ON Check
				if( bSOSOnOff )
				{				//	송신중
					LCDSetCursor( 20, 13 );
					LCDPrintf( "통화중..." );

					SetRFMMode( RFMModeTx );

					//  Green LED On
					HAL_GPIO_WritePin ( LED_ON_A_GPIO_Port, LED_ON_A_Pin, GPIO_PIN_SET ); //  Green LED
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

					// Start RX with radio packet length
					vRadio_StartRX (
						pRadioConfiguration->Radio_ChannelNumber,
						pRadioConfiguration->Radio_PacketLength );

				}

				bOldSOS_Key = bSOS_Key;
			}

			if( bPTTOnOff || bSOSOnOff )
			{
				//========================================================================
				RF_Tx_Mode();
				//========================================================================

				// PTT Key가 눌려있을시 전송.
				if( qBufCnt( &g_qBufAudioRFTx ) >= ( I2S_DMA_LOOP_SIZE * 2 ) )
				{
					//		printf ( "G" );
					qBufGet( &g_qBufAudioRFTx, (uint8_t*)bufRFTx.dat.data, ( I2S_DMA_LOOP_SIZE * 2 ) );

					//  송신기 -> 수신기
					bufRFTx.hdr.addrSrc = GetDevID();		//  Src : 송신기 DevRF900T

					if( bPTTOnOff )
					{
						//  송신기 -> 수신기
						bufRFTx.hdr.addrDest	=	DevRF900M;		 //  Dest : 수신기
						bufRFTx.hdr.nPktID		=	PktPA;
					}
					else
					{
						//  송신기 -> 송신기
						bufRFTx.hdr.addrDest	=	DevRF900T;		 //  Dest : 수신기
						bufRFTx.hdr.nPktID		=	PktCall;
					}

					vRadio_StartTx_Variable_Packet (
						pRadioConfiguration->Radio_ChannelNumber,
						(uint8_t *)&bufRFTx,
						pRadioConfiguration->Radio_PacketLength );
				}
			}
		}

		//========================================================================
		static int nOldRFMMode = 0;

		int nRFMMode = GetRFMMode();
		if( nRFMMode != nOldRFMMode )
		{
			switch ( nRFMMode )
			{
			case RFMModeRx:
				LCDSetCursor( 20, 13 );
				LCDPrintf( "수신중..." );
				break;

			case RFMModeNormal:
				//========================================================================
				//  편성 : XXX
				UpdateLCDMain();
				break;
			}

			nOldRFMMode = nRFMMode;
		}

		//========================================================================
		//  On/Off

		static int bOldOnOff;
		int bOnOff;

		bOnOff = HAL_GPIO_ReadPin( ON_OFF_KEY_GPIO_Port, ON_OFF_KEY_Pin );

		if( bOldOnOff != bOnOff )
		{
			if( bOnOff == 0 )
			{
				// On/Off 버튼 눌렀다 뗄경우 Off

				//	Power Off
				HAL_GPIO_WritePin( ON_OFF_EN_GPIO_Port, ON_OFF_EN_Pin, GPIO_PIN_RESET );
			}

			bOldOnOff = bOnOff;
		}

		//========================================================================
		//  Flash Light
		static int bOldKeyLight = 1;
		static int bLightOnOff; //  toggle
		int bKeyLight;

		bKeyLight = HAL_GPIO_ReadPin( DOME4_GPIO_Port, DOME4_Pin );

		if( bOldKeyLight != bKeyLight )
		{
			if( bKeyLight == 1 )
			{
				bLightOnOff = !(bLightOnOff); //  toggle
			}

			if( bLightOnOff )
			{
				//	Flash Light On
				HAL_GPIO_WritePin( FLASH_ON_GPIO_Port, FLASH_ON_Pin, GPIO_PIN_SET );

				LCDLight( 1 );
			}
			else
			{
				//	Flash Light Off
				HAL_GPIO_WritePin( FLASH_ON_GPIO_Port, FLASH_ON_Pin, GPIO_PIN_RESET );

				LCDLight( 0 );
			}


			bOldKeyLight = bKeyLight;
		}

		//========================================================================
		//  Spk
		static int bOldKeySpk = 1;
//		static int bSpkOnOff = 1; //  toggle
		int bKeySpk;

		bKeySpk = HAL_GPIO_ReadPin( DOME6_GPIO_Port, DOME6_Pin );

		if ( bOldKeySpk != bKeySpk )
		{
			if ( bKeySpk == 1 )
			{
//				bSpkOnOff = !(bSpkOnOff); //  toggle
				g_nSpkLevel = ( g_nSpkLevel + 1 ) % 4;  //  0, 1, 2, 3

				//========================================================================
				SetSpkVol( g_nSpkLevel );
				//========================================================================
			}

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

			bOldKeySpk = bKeySpk;
		}


		//========================================================================
		//  Menu Key
		static int bOldKeyMenu = 1;
		int bKeyMenu;

		bKeyMenu = HAL_GPIO_ReadPin( DOME1_GPIO_Port, DOME1_Pin );

		if ( bOldKeyMenu != bKeyMenu )
		{
			if ( bKeyMenu )
			{
			}
			else
			{
				//	Menu
				ProcBtnMenu();
			}

			bOldKeyMenu = bKeyMenu;
		}

		//========================================================================
		//  OK Key
		static int bOldKeyOK = 1;
		int bKeyOK;

		bKeyOK = HAL_GPIO_ReadPin( DOME3_GPIO_Port, DOME3_Pin );

		if ( bOldKeyOK != bKeyOK )
		{
			if ( bKeyOK )
			{
			}
			else
			{
				ProcBtnOK();
			}

			bOldKeyOK = bKeyOK;
		}

		//========================================================================
		//  Up Key
		static int bOldKeyUp = 1;
		int bKeyUp;

		bKeyUp = HAL_GPIO_ReadPin( DOME2_GPIO_Port, DOME2_Pin );

		if ( bOldKeyUp != bKeyUp )
		{
			if ( bKeyUp )
			{
			}
			else
			{
				ProcBtnUp();
			}

			bOldKeyUp = bKeyUp;
		}

		//========================================================================
		//  Down Key
		static int bOldKeyDown = 1;
		int bKeyDown;

		bKeyDown = HAL_GPIO_ReadPin( DOME5_GPIO_Port, DOME5_Pin );

		if ( bOldKeyDown != bKeyDown )
		{
			if ( bKeyDown )
			{
			}
			else
			{
				ProcBtnDown();
			}

			bOldKeyDown = bKeyDown;
		}

		//========================================================================
		//  ADC_Power
		//  Normal Mode 일때 Battery 체크.
		//	RSSI 수신감도 체크.
		static int oldTick = 0;

		if ( nTick - oldTick > 1000 )
		{
			//  Period : 1 sec
			if ( GetRFMMode() == RFMModeNormal )
			{
				Adc_Power();

#if defined(USE_RSSI)
				//	RSSI Ping
				RF_RSSI();	//	주기적으로 상태정보 전송.
#endif	//	defined(USE_RSSI)
			}

			oldTick = nTick;
		}

		//========================================================================
	}
	else
	{
		//========================================================================
		//  수신기.
		//  Buffering
		if ( bRxBuffering == 1 && 
			qBufCnt( &g_qBufAudioRFRx ) > ( ( I2S_DMA_LOOP_SIZE * 2 ) * 3 ) )
		{
			//  버퍼링 완료시점.
			//	  I2S 구동 재계.
			HAL_I2SEx_TxRxCpltCallback( &hi2s3 );
		}

		//========================================================================
		//	RSSI 수신감도 체크.
		static int oldTick = 0;

		if ( nTick - oldTick > 1000 )
		{
			//  Period : 1 sec
			if ( GetRFMMode() == RFMModeNormal )
			{
#if defined(USE_RSSI)
				//	RSSI Ping
				RF_RSSI();	//	주기적으로 상태정보 전송.
#endif	//	defined(USE_RSSI)
			}

			oldTick = nTick;
		}
	}
	//========================================================================

	bMain_IT_Status = bRadio_Check_Tx_RX();

	switch ( bMain_IT_Status )
	{
	//========================================================================
	//  Transmit
	case SI446X_CMD_GET_INT_STATUS_REP_PH_PEND_PACKET_SENT_PEND_BIT:

		HAL_GPIO_TogglePin ( LED_ST_GPIO_Port, LED_ST_Pin );
//		HAL_GPIO_TogglePin ( LED_ON_B_GPIO_Port, LED_ON_B_Pin );

		idx++;

		if ( idx % 250 == 0 )
		{
			printf ( "T" );

			//for ( i = 0; i < 64; i++ )	printf( "%02X ", bufRFTx[i] );
			//printf( "\n" );
		}
		/* Clear Packet Sending flag */
		lPktSending = 0u;
		break;

	//========================================================================
	//  Receive
	case SI446X_CMD_GET_INT_STATUS_REP_PH_PEND_PACKET_RX_PEND_BIT:

		HAL_GPIO_TogglePin ( LED_ST_GPIO_Port, LED_ST_Pin );

		//  Queue Buffer Put
//		printf ( "P" );

		RFMPkt	*pRFPkt = (RFMPkt *)&customRadioPacket[0];

		if( GetDevID() == DevRF900T && pRFPkt->hdr.nPktID == PktCall )
		{
			//  송신기
			uint16_t	 *pAudioBuf = (uint16_t*)pRFPkt->dat.data;

			stampRx = nTick;
			SetRFMMode( RFMModeRx );

#if defined(USE_RFT_ONLY_RX_SPK_ON)
			//  송신기 : 수신중인 경우 SPK ON
			HAL_GPIO_WritePin( SPK_ON_GPIO_Port, SPK_ON_Pin, GPIO_PIN_SET );
#endif

			//  Red LED On
			HAL_GPIO_WritePin ( LED_ON_B_GPIO_Port, LED_ON_B_Pin, GPIO_PIN_SET ); //  RED LED

			//  통화 : 송신기 -> 송신기
			qBufPut( &g_qBufAudioRFRx, (uint8_t*)pAudioBuf, ( I2S_DMA_LOOP_SIZE * 2 ) );
		}
		else if (	pRFPkt->hdr.addrSrc == DevRF900T
					&& pRFPkt->hdr.addrDest == DevRF900M
					&& pRFPkt->hdr.nPktID == PktPA 
				)
		{
			if ( GetDevID() == DevRF900M )
			{
				//  수신기
				uint16_t	 *pAudioBuf = (uint16_t*)pRFPkt->dat.data;

				//  방송 : 송신기 -> 수신기
				qBufPut( &g_qBufAudioRFRx, (uint8_t*)pAudioBuf, ( I2S_DMA_LOOP_SIZE * 2 ) );

				// 조명 On
				HAL_GPIO_WritePin ( LIGHT_ON_GPIO_Port, LIGHT_ON_Pin, GPIO_PIN_SET );

				stampRx = nTick;
				SetRFMMode( RFMModeRx );

				//  수신기 Spk Relay On
				HAL_GPIO_WritePin( AUDIO_ON_GPIO_Port, AUDIO_ON_Pin, GPIO_PIN_SET );
			}
			else
			{
				//========================================================================
				uint16_t	 *pAudioBuf = (uint16_t*)pRFPkt->dat.data;

#if defined(USE_RFT_ONLY_RX_SPK_ON)
				//  송신기 : 수신중인 경우 SPK ON
				HAL_GPIO_WritePin( SPK_ON_GPIO_Port, SPK_ON_Pin, GPIO_PIN_SET );
#endif

				//  방송 : 송신기 -> 수신기
				qBufPut( &g_qBufAudioRFRx, (uint8_t*)pAudioBuf, ( I2S_DMA_LOOP_SIZE * 2 ) );
				//========================================================================

				//  송신기
				stampRx = nTick;
				SetRFMMode( RFMModeRx );

				//  Red LED On
				HAL_GPIO_WritePin ( LED_ON_B_GPIO_Port, LED_ON_B_Pin, GPIO_PIN_SET ); //  RED LED
			}
		}

		if ( GetDbgLevel() > 0 )
		{
			for ( i = 0; i < 64; i++ )	printf( "%02X ", customRadioPacket[i] );
			printf( "\n" );
		}

		//========================================================================
		//  Status Data
		if ( pRFPkt->hdr.nPktID == PktStat )
		{
			//	상태정보 수신.
			printf ( "[Stat] Car:%d\n", pRFPkt->dat.stat.nCarNo );
		}

		if ( GetDevID() == DevRF900M )
		{
			//  수신기 조명제어.

			if ( pRFPkt->hdr.nPktID == PktLightOff )
			{
				// 조명 Off 명령 수신시.
				HAL_GPIO_WritePin ( LIGHT_ON_GPIO_Port, LIGHT_ON_Pin, GPIO_PIN_RESET );
			}
			else if ( pRFPkt->hdr.nPktID == PktLightOn )
			{
				// 조명 Off 명령 수신시.
				HAL_GPIO_WritePin ( LIGHT_ON_GPIO_Port, LIGHT_ON_Pin, GPIO_PIN_SET );
			}
		}

		idx++;

		if ( idx % 250 == 0 )
		{
			//  1초간격
			printf ( "R" );
			//for ( i = 0; i < 64; i++ )	printf( "%02X ", customRadioPacket[i] );
			//printf( "\n" );
		}

		if ( GetRFMMode() != RFMModeTx )
		{
			//  송신모드가 아닌경우 수신 Start
			// Start RX with radio packet length
			vRadio_StartRX (
				pRadioConfiguration->Radio_ChannelNumber,
				pRadioConfiguration->Radio_PacketLength );
		}

		break;

	default:
		break;
	}

	//  수신중 해제
	if( nTick - stampRx > 500 && GetRFMMode() == RFMModeRx )
	{
		// Rx 패킷이 500 ms 없을 경우 수신모드 해제
		SetRFMMode( RFMModeNormal );

		//  수신기 Spk Relay Off
		HAL_GPIO_WritePin( AUDIO_ON_GPIO_Port, AUDIO_ON_Pin, GPIO_PIN_RESET );

#if defined(USE_RFT_ONLY_RX_SPK_ON)
		if ( GetDevID() == DevRF900T )
		{
			//  송신기 : 수신중이 아닌경우 SPK OFF
			HAL_GPIO_WritePin( SPK_ON_GPIO_Port, SPK_ON_Pin, GPIO_PIN_RESET );
		}
#endif

		//  Red LED Off
		HAL_GPIO_WritePin ( LED_ON_B_GPIO_Port, LED_ON_B_Pin, GPIO_PIN_RESET ); //  RED LED
	}

	//  수신기 Standby GPIO 제어 : 모듈동작시 1초간격 Blink
	static int s_nTickStandby;

	if ( nTick - s_nTickStandby >= 1000 )
	{
		//  Standby GPIO Toggle
		if ( GetRFMMode() == RFMModeRx )
		{
			//  수신기:수신중 LED On ( Standby LED On )
			HAL_GPIO_WritePin( STANDBY_GPIO_Port, STANDBY_Pin, GPIO_PIN_SET );
		}
		else
		{
			//  Standby LED Toggle
			HAL_GPIO_TogglePin( STANDBY_GPIO_Port, STANDBY_Pin );
		}

		s_nTickStandby = nTick;
	}
}

#endif

#if 0

/**
  * @brief  The application entry point.
  * @retval int
  */
//========================================================================
int RF_PA_Init( void )
//========================================================================
{
	printf( "%s(%d)\n", __func__, __LINE__ );

	//========================================================================
	I2C_BusScan( &hi2c1 );
	I2C_BusScan( &hi2c3 );

	if ( HAL_OK == HAL_I2C_IsDeviceReady( &hi2c1, (uint16_t)( 0x50 << 1 ), 2, 2 ) )
	{
		printf( "%s(%d) - EEPROM OK\n", __func__, __LINE__ );
		TestEEPROM( &hi2c1 ); //  Test EEPROM

		//========================================================================
		//  Read TrainSet
		uint8_t	 idxTrainSet;
		idxTrainSet = GetTrainSetIdx();

		if ( idxTrainSet < 0 || idxTrainSet >= g_cntTrainSet )
		{
			idxTrainSet = 0;

			SetTrainSetIdx( idxTrainSet );
		}

		g_idxTrainSet = idxTrainSet;

		//========================================================================
		//  Radio Channel 설정.
		pRadioConfiguration->Radio_ChannelNumber = g_idxTrainSet;
	}

	//========================================================================
	//	Codec MAX9860ETG+
	if ( HAL_OK == HAL_I2C_IsDeviceReady( &hi2c1, (uint16_t)( 0x10 << 1 ), 2, 2 ) )
	{
		//  Read Rev.
		char buf[10];
		int cntRetry;

		cntRetry = 0;

		memset( buf, 0, sizeof( buf ) );
		while ( HAL_I2C_Mem_Read( &hi2c1, (uint16_t)( 0x10 << 1 ), (uint16_t)0xFF, I2C_MEMADD_SIZE_8BIT, buf, (uint16_t)1, 1000 ) != HAL_OK && cntRetry < 10 ) cntRetry++;

		printf( "%s(%d) - Codec ( MAX9860ETG+ ) / Rev. 0x%02X\n", __func__, __LINE__, buf[0] );

		//  Codec 초기화.

		InitCodecMAX9860();
	}

	//========================================================================
	//	OLED
	if ( HAL_OK == HAL_I2C_IsDeviceReady( &hi2c3, (uint16_t)( SSD1306_I2C_ADDRESS ), 2, 2 ) )
	{
		//========================================================================
		//	OLED가 연결되어있음.
		SetDevID( DevRF900T );	 //  송신기.

		//	LCD Init
		LCDInit();

		LCDClear();
		LCDDrawRect( 0, 0, 128, 32, 1 );

		HAL_Delay( 500 );

		//  LCD : 메뉴 화면.
		LCDMenu();

		//  LCD :   편성 : 100
		//========================================================================
		//  편성 : XXX
		UpdateLCDMain();

		//  LCD : Light Icon
		LCDLight( 0 );

		//========================================================================
		//  Spk Vol Set
		//  Default (1) - 0(Mute) / 1 / 2(Normal) / 3

		int	 nSpkVol;
		nSpkVol = GetSpkVol();

		if ( nSpkVol < 0 || nSpkVol > MAX_SPK_VOL )
		{
			nSpkVol = DEFAULT_SPK_VOL;
			SetSpkVol( nSpkVol );
		}

		g_nSpkLevel = nSpkVol;

		//  LCD : Spk Icon
		LCDSpeaker( g_nSpkLevel );		//  Spk Icon : Spk On
		//========================================================================
	}
	else
	{
		//========================================================================
		//	OLED가 없으면 -> 수신기
		SetDevID( DevRF900M );	 //  수신기.

		//  수신기 스피커 레벨 변경. 
		WriteI2CCodec( 0x09, 0x12 );	//  0x12 ( -6 )
//		WriteI2CCodec( 0x09, 0x1A );	//  0x1A ( -10 )
	}
	//========================================================================

	//========================================================================
	//  Default 수신모드.
	RF_Rx_Mode();

	//========================================================================

#if defined(USE_RFT_ONLY_RX_SPK_ON)

	if ( GetDevID() == DevRF900T )
	{
		//  송신기 : 수신중이 아닌경우 SPK OFF
		HAL_GPIO_WritePin( SPK_ON_GPIO_Port, SPK_ON_Pin, GPIO_PIN_RESET );
	}

#endif

	radio_spi_init( &hspi1 );


#if defined(USE_RADIO_SI4463)

	//========================================================================
	//	Drivers/si4463
	RF_Init();
	RF_Info();

#else

	//========================================================================
	//	Drivers/radio
	vRadio_Init ();
	si446x_part_info ();

	printf ( "=========================\n" );
	printf ( "%08x\n", Si446xCmd.PART_INFO.CHIPREV );
	printf ( "%08x\n", Si446xCmd.PART_INFO.PART );
	printf ( "%08x\n", Si446xCmd.PART_INFO.PBUILD );
	printf ( "%08x\n", Si446xCmd.PART_INFO.ID );
	printf ( "%08x\n", Si446xCmd.PART_INFO.CUSTOMER );
	printf ( "%08x\n", Si446xCmd.PART_INFO.ROMID );
	printf ( "-------------------------\n" );

#endif

	if ( GetDevID() == DevRF900T )
	{
		for ( int i = 0; i < pRadioConfiguration->Radio_PacketLength; i++ )
			customRadioPacket[i] = i;

		vRadio_StartTx_Variable_Packet ( pRadioConfiguration->Radio_ChannelNumber,
										 &customRadioPacket[0],
										 pRadioConfiguration->Radio_PacketLength );

		//  RF 수신 Start
		vRadio_StartRX(
			pRadioConfiguration->Radio_ChannelNumber,
			pRadioConfiguration->Radio_PacketLength );
	}
	else
	{
		vRadio_StartRX(
			pRadioConfiguration->Radio_ChannelNumber,
			pRadioConfiguration->Radio_PacketLength );
	}

	//========================================================================
	//	Init RF Audio Rx Buffer
	qBufInit( &g_qBufAudioRFRx, (uint8_t *)g_bufAudioRFRx, ( I2S_DMA_LOOP_SIZE * 2 ) * I2S_DMA_LOOP_QCNT );
	qBufInit( &g_qBufAudioRFTx, (uint8_t *)g_bufAudioRFTx, ( I2S_DMA_LOOP_SIZE * 2 ) * I2S_DMA_LOOP_QCNT );
	//========================================================================

	//	 res = HAL_I2S_Receive_IT(&hi2s3, r_audio_buff, 32u);
	HAL_I2SEx_TransmitReceive_DMA ( &hi2s3, t_audio_buff, r_audio_buff, I2S_DMA_LOOP_SIZE ); // 32byte
}


#endif

/**
  * @brief  The application entry point.
  * @retval int
  */
//========================================================================
int RF_PA_main( void )
//========================================================================
{
	printf( "%s(%d)\n", __func__, __LINE__ );
	RF_PA_Init();

	/* Infinite main loop */
	while ( 1 )
	{
		RF_PA_loop();

		//	Loop Process ( CLI )
		LoopProcCLI();
	}
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
