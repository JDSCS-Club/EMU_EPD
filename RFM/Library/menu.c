
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

#include "audio.h"					//  g_bufAudioRFRx

#include "adc.h"					//  Adc_Power()

#include "version.h"				//  APP_VER

#include "cli.h"

#include "menu.h"

#include "rfm.h"

#include "radio.h"					//	pRadioConfiguration

#include "ProcPkt.h"				//	g_flagRspID


//========================================================================
//	Menu LightCtrl

char *_sLightCtrl[] = {
	"1. 조명소등",		//  조명소등
	"2. 조명점등"		//  조명점등
};

Menu_t	g_MenuLightCtrl = {
	_sLightCtrl,
	sizeof(_sLightCtrl)/sizeof(char *),		//	Item Count
	0,						// 	curr Idx
	ProcMenuLightCtrl		//	Callback Function
};

//========================================================================
//	Menu TrainSet

char *_sTrainSet[] = {
	" 100 편성",	//  100편성
	" 101 편성",	//  101편성
	" 102 편성",	//  102편성
	" 103 편성",	//  103편성
	" 104 편성",	//  104편성
	" 105 편성",	//  105편성
	" 106 편성",	//  106편성
	" 107 편성",	//  107편성
	" 108 편성",	//  108편성
	" 109 편성"	//  109편성
};

Menu_t	g_MenuTrainSet = {
	_sTrainSet,
	sizeof(_sTrainSet)/sizeof(char *),		//	Item Count
	0,						// 	curr Idx
	ProcMenuTrainSet		//	Callback Function
};

//========================================================================
//	Menu Diag

#if defined(USE_RFT_MENU_DIAG)
char *_sDiagList[] = {
	" AudioLoop:Off",	//  Loopback Off
	" AudioLoop:On",	//  Loopback On
	" AudioSine:On",	//  Sine Wave On
};

Menu_t	g_MenuDiagList = {
	_sDiagList,
	sizeof(_sDiagList)/sizeof(char *),		//	Item Count
	0,						// 	curr Idx
	ProcMenuDiag		//	Callback Function
};
#endif	//	defined(USE_RFT_MENU_DIAG)

//========================================================================
//	Main Version Info

#if defined(USE_RFT_MENU_DEV_VER)

char _sVerList[16][20] = {
	"RFM v----",		//  Self Device Version
//	"01:v1.1.X.X",
	"01:    --- ",	//	"01:v1.1.X.X",
	"02:    --- ",	//
	"03:    --- ",	//
	"04:    --- ",	//
	"05:    --- ",	//
	"06:    --- ",	//
	"07:    --- ",	//
	"08:    --- ",	//
	"09:    --- ",	//
	"10:    --- ",	//
	"11:    --- ",	//
	"12:    --- ",	//
	"13:    --- ",	//
	"14:    --- ",	//
	"15:    --- ",	//
};

char *_psVerList[] = {
	_sVerList[0],	_sVerList[1],	_sVerList[2],	_sVerList[3],
	_sVerList[4],	_sVerList[5],	_sVerList[6],	_sVerList[7],
	_sVerList[8],	_sVerList[9],	_sVerList[10],	_sVerList[11],
	_sVerList[12],	_sVerList[13],	_sVerList[14],	_sVerList[15],
};


Menu_t	g_MenuVerList = {
	_psVerList,
	sizeof(_psVerList)/sizeof(char *),		//	Item Count
	0,						// 	curr Idx
	ProcMenuVer				//	Callback Function
};

#endif	//	defined(USE_RFT_MENU_DEV_VER)
//========================================================================

//========================================================================
//	Menu Command

#if defined(USE_RFT_MENU_CMD)
char *_sCmdList[] = {
	" Reset",		//  Reset
	" DFU Mode",	//  DFU Mode
	" Upgrade",		//  Upgrade
};

Menu_t	g_MenuCmdList = {
	_sCmdList,
	sizeof(_sCmdList)/sizeof(char *),		//	Item Count
	0,						// 	curr Idx
	ProcMenuCmd				//	Callback Function
};
#endif	//	defined(USE_RFT_MENU_CMD)

//========================================================================
//	Main Menu

enum eMenuIdx
{
	eMenuIdxCtlLight = 0,	//	조명제어
	eMenuIdxSWVer,			//	S/W버전
	eMenuIdxTrainSet,		//	편성설정
#if defined(USE_RFT_MENU_DIAG)
	eMenuIdxDiag,			//	진단
#endif
#if defined(USE_RFT_MENU_STAT)
	eMenuIdxStat,			//	상태정보
#endif
#if defined(USE_RFT_MENU_CMD)
	eMenuIdxCmd,			//	명령전송
#endif
};

char *_sMainMenu[] = {
	"1. 조명제어",
	"2. S/W 버전",
	"3. 편성설정",
#if defined(USE_RFT_MENU_DIAG)
	"4. 진    단",
#endif	//	defined(USE_RFT_MENU_DIAG)
#if defined(USE_RFT_MENU_STAT)
	"5. 상태정보",
#endif	//	defined(USE_RFT_MENU_STAT)
#if defined(USE_RFT_MENU_CMD)
	"6. 명령전송",
#endif	//	defined(USE_RFT_MENU_CMD)
};


Menu_t	g_MenuMain = {
	_sMainMenu,
	sizeof(_sMainMenu)/sizeof(char *),		//	Item Count
	0,					// 	curr Idx
	ProcMenuMain		//	Callback Function
};


//========================================================================
Menu_t	*g_pActiveMenu	=	NULL;
//========================================================================

static int _bEnDispStat = 0;

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

	sprintf( sBuf, "편성 : %d", 100 + g_idxTrainSet );

//	LCDPrintf( "편성 : 100" );
	LCDPrintfXY( 20, 13, sBuf );
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

#if defined(USE_RFT_MENU_LOOP)

	LCDMenuUpDown( 3 ); //  3. Up/Down

#else

	if ( 0 < *pIdxMenu && *pIdxMenu < *pCntMenu - 1 )	LCDMenuUpDown( 3 ); //  3. Up/Down
	else if ( *pIdxMenu == 0 )							LCDMenuUpDown( 2 ); //  2. Down
	else if ( *pIdxMenu == *pCntMenu - 1 )				LCDMenuUpDown( 1 ); //  1. Up
	else												LCDMenuUpDown( 0 ); //  0. None

#endif

	LCDSetCursor( 20, 13 );
	LCDPrintf( sMenu[*pIdxMenu] );
}

//========================================================================
void	UpdateLCDMonitor( int nTick )
//========================================================================
{
	static int oldTick = 0;

	if ( nTick - oldTick > 1000 )
	{
		if( _bEnDispStat )
		{
			//	상태정보 현시.
			LCDSetCursor( 5, 13 );

			char sBuf[100];
			sprintf(sBuf, "Stat:%04X / %02d", g_flagRspID, g_nRSSI );
			LCDPrintf( sBuf );
		}
		oldTick = nTick;
	}
}


//========================================================================
void	ProcBtnUp( void )
//========================================================================
{
	if ( GetActiveMenu() == NULL ) return;

	//	Menu : Up Key Press

	g_pActiveMenu->currIdx--;

	if ( g_pActiveMenu->currIdx < 0 )
	{
#if defined(USE_RFT_MENU_LOOP)

		//	Menu Up/Down Loop
		g_pActiveMenu->currIdx = g_pActiveMenu->cntItem  - 1;

#else

		//	Menu Up/Down Limit
		g_pActiveMenu->currIdx = 0;

#endif
	}


	UpdateLCDMenu();   //  메뉴화면 Update
}

//========================================================================
void	ProcBtnDown( void )
//========================================================================
{
	if ( GetActiveMenu() == NULL ) return;

	//	Menu : Down Key Press
	g_pActiveMenu->currIdx++;

	if ( g_pActiveMenu->currIdx >= g_pActiveMenu->cntItem )
	{
#if defined(USE_RFT_MENU_LOOP)

		//	Menu Up/Down Loop
		g_pActiveMenu->currIdx = 0;

#else

		//	Menu Up/Down Limit
		g_pActiveMenu->currIdx = g_pActiveMenu->cntItem  - 1;

#endif
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

	SendLightOn();	 //  조명On 명령 전송.
	HAL_Delay( 200 );
	SendLightOn();	 //  조명On 명령 전송.
	HAL_Delay( 200 );
	SendLightOn();	 //  조명On 명령 전송.
	HAL_Delay( 200 );
	SendLightOn();	 //  조명On 명령 전송.
}

//========================================================================
void	ProcLightOff ( void )
//========================================================================
{
	LCDSetCursor( 20, 13 );
	LCDPrintf( "[조명 소등]" );

	SendLightOff();	 //  조명Off 명령 전송.
	HAL_Delay( 200 );
	SendLightOff();	 //  조명Off 명령 전송.
	HAL_Delay( 200 );
	SendLightOff();	 //  조명Off 명령 전송.
	HAL_Delay( 200 );
	SendLightOff();	 //  조명Off 명령 전송.
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
void	ProcDispStat ( void )
//========================================================================
{
	//========================================================================
	//	Display Status Enable
	_bEnDispStat		=	1;
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
void	ProcMenuVer( int idxItem  )
//========================================================================
{
	//	Upgrade Ok / Cancel

	//  메뉴 Exit
	SetActiveMenu( NULL );

	//	메인화면 갱신.
	UpdateLCDMain();
}


//========================================================================
void	ProcMenuCmd( int idxItem  )
//========================================================================
{
	LCDSetCursor( 20, 13 );
	LCDPrintf( "[명령전송]" );

	switch( idxItem )
	{
	case 0:		SendRFCmdReset();		break;		//	Reset 명령.
	case 1:		SendRFCmdDFUMode();		break;		//	DFU Mode 명령.
	case 2:		SendRFCmdUpgrade();		break;		//	Upgrade 명령.
	}

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
	case eMenuIdxCtlLight:	//	0:		 //  조명제어
		//	Menu

		SetActiveMenu( &g_MenuLightCtrl );
		GetActiveMenu()->currIdx = 0;	//	메뉴 Index초기화.

		UpdateLCDMenu();

		break;

	case eMenuIdxSWVer:		//	1:		 //  S/W 버전
		//  메뉴 Exit

#if defined(USE_RFT_MENU_DEV_VER)
		SetActiveMenu( &g_MenuVerList );	//	버전 List 현시.
		GetActiveMenu()->currIdx = 0;		//	메뉴 Index초기화.

		ProcDispVer();
//		UpdateLCDMenu();
#else
		SetActiveMenu( NULL );
		ProcDispVer();
#endif

		break;

	case eMenuIdxTrainSet:	//	2:		 //  편성설정.

		SetActiveMenu( &g_MenuTrainSet );
		GetActiveMenu()->currIdx = g_idxTrainSet;	//	메뉴 Index초기화.

		UpdateLCDMenu();
		break;

#if defined(USE_RFT_MENU_DIAG)

	case eMenuIdxDiag:		//	3:		 //  진단

		SetActiveMenu( &g_MenuDiagList );
		GetActiveMenu()->currIdx = 0;	//	메뉴 Index초기화.

		UpdateLCDMenu();
		break;

#endif	//	defined(USE_RFT_MENU_DIAG)


#if defined(USE_RFT_MENU_STAT)

	case eMenuIdxStat:		//	4:		 //  상태정보.

		SetActiveMenu( NULL );

		ProcDispStat();			//	상태정보 표출.
		break;

#endif	//	defined(USE_RFT_MENU_STAT)


#if defined(USE_RFT_MENU_CMD)

	case eMenuIdxCmd:		//	5:		 //  명령전송.

		SetActiveMenu( &g_MenuCmdList );
		GetActiveMenu()->currIdx = 0;	//	메뉴 Index초기화.

		UpdateLCDMenu();
		break;

#endif	//	defined(USE_RFT_MENU_CMD)

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
void 	ProcMenuDiag( int idxItem )
//========================================================================
{
	LCDMenuUpDown( 0 );

//	" AudioLoop:Off",	//  Loopback Off
//	" AudioLoop:On",	//  Loopback On
//	" AudioSine:On",	//  Sine Wave On

	switch( idxItem )
	{
	case 0:	//	Audio Loopback Off
		HAL_GPIO_WritePin( SPK_ON_GPIO_Port, SPK_ON_Pin, GPIO_PIN_RESET );

		//	Codec Loopback Off
		AudioStop();

		break;

	case 1:	//	Audio Loopback On
		printf( "%s(%d) - sine\n", __func__, __LINE__ );
		LCDPrintf( "Audio Loop" );

		HAL_GPIO_WritePin( SPK_ON_GPIO_Port, SPK_ON_Pin, GPIO_PIN_SET );

		AudioDMALoopback();
		break;

	case 2:	//	Audio Sine Wave On
		printf( "%s(%d) - sine\n", __func__, __LINE__ );
		LCDPrintf( "Audio Sine" );

		HAL_GPIO_WritePin( SPK_ON_GPIO_Port, SPK_ON_Pin, GPIO_PIN_SET );

		AudioPlayDMASine();
		break;

	default:
		printf("%s(%d) - invalid menu(%d)\n", __func__, __LINE__, idxItem);
		break;
	}

	//  1초후 Main화면 갱신.
//	HAL_Delay( 1000 );
//	UpdateLCDMain();

//	SetActiveMenu( NULL );
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

		//========================================================================
		//	Display Status Disable
		_bEnDispStat		=	0;

		return;
	}
	else if ( g_pActiveMenu->cbFunc != NULL )
	{
		//	Menu Procedure Function
		g_pActiveMenu->cbFunc( g_pActiveMenu->currIdx );
	}
}


//========================================================================


//#define	 USE_PTT_KEY_TOGGLE	  1	   //  PTT Toggle Key

