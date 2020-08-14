
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
//	Main Menu

char *_sMainMenu[] = {
	"1. 조명제어",
	"2. S/W 버전",
	"3. 편성설정",
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


//========================================================================


//#define	 USE_PTT_KEY_TOGGLE	  1	   //  PTT Toggle Key

