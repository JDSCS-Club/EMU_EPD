
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
//	Main Version Info

#if defined(USE_RFT_MENU_DEV_VER)

char _sVerList[16][20] = {
	"RFM v" APP_VER,		//  Self Device Version
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
//	Menu RFTID

#if defined(USE_RFT_MENU_RFTID)
char *_sRFTIDList[] = {
	" 송신기#1",		//  11
	" 송신기#2",		//  12
};

Menu_t	g_MenuRFTIDList = {
	_sRFTIDList,
	sizeof(_sRFTIDList)/sizeof(char *),		//	Item Count
	0,						// 	curr Idx
	ProcMenuRFTID			//	Callback Function
};
#endif	//	defined(USE_RFT_MENU_RFTID)

//========================================================================
//	Menu SelfTest - 자가진단

#if defined(USE_RFT_MENU_SELFTEST)
char *_sSelfTestList[] = {
	" 1 호차 : OK",	//
	" 2 호차 : OK",	//
	" 3 호차 : OK",	//
	" 4 호차 : N/A",	//
	" 5 호차 : N/A",	//
	" 6 호차 : OK",	//
};

Menu_t	g_MenuSelfTestList = {
	_sSelfTestList,
	sizeof(_sSelfTestList)/sizeof(char *),		//	Item Count
	0,						// 	curr Idx
	ProcMenuSelfTest		//	Callback Function
};
#endif	//	defined(USE_RFT_MENU_SELFTEST)

//========================================================================
//	Menu Diag - 진단

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
//	Menu Setting

#if defined(USE_RFT_MENU_SETTING)
char *_sSettingList[] = {
	" Mic Vol",		//	Mic Volume
	" Spk Vol",		//	Spk Volume
	" RF Tx Pwr",	//	RF Tx Power
};

Menu_t	g_MenuSettingList = {
	_sSettingList,
	sizeof(_sSettingList)/sizeof(char *),		//	Item Count
	0,						// 	curr Idx
	ProcMenuSetting		//	Callback Function
};

char *_sSetMicList[] = { " Mic : 0",
						" Mic : 1",
						" Mic : 2",
						" Mic : 3",
						" Mic : 4",
						" Mic : 5",
						" Mic : 6",
						" Mic : 7",
						" Mic : 8",
						" Mic : 9",
						};

Menu_t	g_MenuSetMicList = {
	_sSetMicList,
	sizeof(_sSetMicList)/sizeof(char *),		//	Item Count
	0,						// 	curr Idx
	ProcMenuSetMic			//	Callback Function
};

char *_sSetSpkList[] = { " Spk : 0",
						" Spk : 1",
						" Spk : 2",
						" Spk : 3",
						};

Menu_t	g_MenuSetSpkList = {
	_sSetSpkList,
	sizeof(_sSetSpkList)/sizeof(char *),		//	Item Count
	0,						// 	curr Idx
	ProcMenuSetSpk			//	Callback Function
};

//	[0(-38dBm)~7(0dBm)~12(5dBm)~22(10dBm)~40(15dBm)~127(20dBm)]",
char *_sSetTxPwrList[] = { 	" Pwr : -38 dBm",	//	0
							" Pwr : -20 dBm",	//	1
							" Pwr : -5 dBm",	//	4
							" Pwr : 0 dBm",		//	7
							" Pwr : 5 dBm",		//	12
							" Pwr : 10 dBm",	//	22
							" Pwr : 15 dBm",	//	40
							" Pwr : 20 dBm",	//	127
						};

Menu_t	g_MenuSetTxPwrList = {
	_sSetTxPwrList,
	sizeof(_sSetTxPwrList)/sizeof(char *),		//	Item Count
	0,							// 	curr Idx
	ProcMenuSetTxPwr			//	Callback Function
};


#endif	//	defined(USE_RFT_MENU_SETTING)

//========================================================================
//	Menu Command

#if defined(USE_RFT_MENU_CMD)
char *_sCmdList[] = {
	" Reset",			//  Reset
	" DFU Mode",		//  DFU Mode
	" Upgrade ReTry",	//  Upgrade Re-Try
	" Upgrade",			//  Upgrade
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
#if defined(USE_RFT_MENU_RFTID)
	eMenuIdxRFTID,			//	송신기 ID
#endif
#if defined(USE_RFT_MENU_SELFTEST)
	eMenuIdxSelfTest,		//	자가 진단
#endif
#if defined(USE_RFT_MENU_DIAG)
	eMenuIdxDiag,			//	진단
#endif
#if defined(USE_RFT_MENU_SETTING)
	eMenuIdxSetting,		//	설정
#endif
#if defined(USE_RFT_MENU_STAT)
	eMenuIdxStat,			//	상태정보
#endif
#if defined(USE_RFT_MENU_CMD)
	eMenuIdxCmd,			//	명령전송
#endif
};

//========================================================================
int	g_bEnMenuMaint	= 	0;	//	MainMenu Maintenace Menu활성화.

int 	IsMenuMaint			( void )			//	MainMenu Maintenace Menu활성화.
{
	return g_bEnMenuMaint;
}

//========================================================================
void EnableMenuMaint( int bEnable )
//========================================================================
{
	g_bEnMenuMaint = bEnable;

	if ( bEnable )
	{
		//	LCD [Menu Maint.]
		LCDPrintfXY( 10, 13, "[Maint. Menu]" );
	}
}

//========================================================================
//	Main Menu - Base 기본메뉴
char *_sMainMenuBase[] = {
	"1. 조명제어",
	"2. S/W 버전",
};

Menu_t	g_MenuMainBase = {
	_sMainMenuBase,
	sizeof(_sMainMenuBase)/sizeof(char *),		//	Item Count
	0,					// 	curr Idx
	ProcMenuMain		//	Callback Function
};

//========================================================================
//	Main Menu -	Maintenance Menu 활성화.
char *_sMainMenuMaint[] = {
	"1. 조명제어",
	"2. S/W 버전",
	"3. 편성설정",
#if defined(USE_RFT_MENU_RFTID)
	"4. 송신기ID",			//	송신기 ID
#endif
#if defined(USE_RFT_MENU_SELFTEST)
	"5. 자가진단",
#endif	//	defined(USE_RFT_MENU_SELFTEST)
#if defined(USE_RFT_MENU_DIAG)
	"6. 진    단",
#endif	//	defined(USE_RFT_MENU_DIAG)
#if defined(USE_RFT_MENU_SETTING)
	"7. 설    정",
#endif	//	defined(USE_RFT_MENU_SETTING)
#if defined(USE_RFT_MENU_STAT)
	"8. 상태정보",
#endif	//	defined(USE_RFT_MENU_STAT)
#if defined(USE_RFT_MENU_CMD)
	"9. 명령전송",
#endif	//	defined(USE_RFT_MENU_CMD)
};

Menu_t	g_MenuMainMaint = {
	_sMainMenuMaint,
	sizeof(_sMainMenuMaint)/sizeof(char *),		//	Item Count
	0,					// 	curr Idx
	ProcMenuMain		//	Callback Function
};

//========================================================================

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

		if ( g_bEnMenuMaint )
		{
			//	Maintenance 유지보수 메뉴 활성화.
			SetActiveMenu( &g_MenuMainMaint );
		}
		else
		{
			//	Base 기본메뉴.
			SetActiveMenu( &g_MenuMainBase );
		}
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

	//========================================================================
	//	숭신기와 가까운 수신기 설정.
	SetChPA( GetChNearRFM() );
	//========================================================================

	SendLightOn();	 //  조명On 명령 전송.
	HAL_Delay( 200 );
	SendLightOn();	 //  조명On 명령 전송.
	HAL_Delay( 200 );
	SendLightOn();	 //  조명On 명령 전송.
	HAL_Delay( 200 );
	SendLightOn();	 //  조명On 명령 전송.

#if defined(USE_STAT_LIGHT)
	g_nDevFlag |= DevFlagLight;
#endif
}

//========================================================================
void	ProcLightOff ( void )
//========================================================================
{
	LCDSetCursor( 20, 13 );
	LCDPrintf( "[조명 소등]" );

	//========================================================================
	//	숭신기와 가까운 수신기 설정.
	SetChPA( GetChNearRFM() );
	//========================================================================

	SendLightOff();	 //  조명Off 명령 전송.
	HAL_Delay( 200 );
	SendLightOff();	 //  조명Off 명령 전송.
	HAL_Delay( 200 );
	SendLightOff();	 //  조명Off 명령 전송.
	HAL_Delay( 200 );
	SendLightOff();	 //  조명Off 명령 전송.

#if defined(USE_STAT_LIGHT)
	g_nDevFlag &= ~(DevFlagLight);
#endif
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
	g_idxTrainSet = idxItem;	//	메뉴 Index값으로 설정.
	SetTrainSetIdx( g_idxTrainSet );

#if defined(USE_CH_ISO_DEV)		//	장치별로 채널 구분.
	//	각 장치별로 채널 구분.
	//	편성이 변경되어도 채널 고정.
#else
	//	편성별 채널 구분.
	//  Radio Channel 설정.
	pRadioConfiguration->Radio_ChannelNumber = g_idxTrainSet;
#endif

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
	case 2:		SendRFCmdUpgrade(1);	break;		//	Upgrade(Re-Try 명령.
	case 3:		SendRFCmdUpgrade(0);	break;		//	Upgrade 명령.
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

		if ( IsMenuMaint() )
		{
			SetActiveMenu( &g_MenuVerList );	//	버전 List 현시.
			GetActiveMenu()->currIdx = 0;		//	메뉴 Index초기화.
		}
		else
		{
			SetActiveMenu( NULL );
		}

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

#if defined(USE_RFT_MENU_RFTID)

	case eMenuIdxRFTID:		//	3:		 //  송신기 ID

		SetActiveMenu( &g_MenuRFTIDList );
		GetActiveMenu()->currIdx = ( ( GetCarNo() + 1 ) % 2 );	//	메뉴 Index초기화.

		UpdateLCDMenu();
		break;

#endif	//	defined(USE_RFT_MENU_DIAG)

#if defined(USE_RFT_MENU_RFTID)

	case eMenuIdxSelfTest:	//	4:		 //  송신기 ID

		SetActiveMenu( &g_MenuSelfTestList );
		GetActiveMenu()->currIdx = 0;	//	메뉴 Index초기화.

		UpdateLCDMenu();
		break;

#endif	//	defined(USE_RFT_MENU_DIAG)


#if defined(USE_RFT_MENU_DIAG)

	case eMenuIdxDiag:		//	5:		 //  진단

		SetActiveMenu( &g_MenuDiagList );
		GetActiveMenu()->currIdx = 0;	//	메뉴 Index초기화.

		UpdateLCDMenu();
		break;

#endif	//	defined(USE_RFT_MENU_DIAG)

#if defined(USE_RFT_MENU_SETTING)

	case eMenuIdxSetting:	//	6:		 //  설정

		SetActiveMenu( &g_MenuSettingList );
		GetActiveMenu()->currIdx = 0;	//	메뉴 Index초기화.

		UpdateLCDMenu();
		break;

#endif	//	defined(USE_RFT_MENU_DIAG)

#if defined(USE_RFT_MENU_STAT)

	case eMenuIdxStat:		//	7:		 //  상태정보.

		SetActiveMenu( NULL );

		ProcDispStat();			//	상태정보 표출.
		break;

#endif	//	defined(USE_RFT_MENU_STAT)


#if defined(USE_RFT_MENU_CMD)

	case eMenuIdxCmd:		//	8:		 //  명령전송.

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
}

//========================================================================
void 	ProcMenuRFTID( int idxItem )
//========================================================================
{
	LCDMenuUpDown( 0 );	//	UpDown Off

	switch( idxItem )
	{
	case 0:	//	송신기 #1
		SetCarNo( RFTCarNo1 );	//	CarNo ( 11 )
		LCDPrintf( "Set RFT#1" );
		break;

	case 1:	//	송신기 #2
		SetCarNo( RFTCarNo2 );	//	CarNo ( 12 )
		LCDPrintf( "Set RFT#2" );
		break;

	default:
		printf("%s(%d) - invalid menu(%d)\n", __func__, __LINE__, idxItem);
		break;
	}

	//  1초후 Main화면 갱신.
	HAL_Delay( 1000 );
	UpdateLCDMain();

	SetActiveMenu( NULL );
}

//========================================================================
void 	ProcMenuSelfTest( int idxItem )
//========================================================================
{
	LCDMenuUpDown( 0 );

	//  1초후 Main화면 갱신.
	HAL_Delay( 1000 );
	UpdateLCDMain();

	SetActiveMenu( NULL );
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
void 	ProcMenuSetting( int idxItem )
//========================================================================
{
	LCDMenuUpDown( 0 );

//	" Mic Vol",		//	Mic Volume
//	" Spk Vol",		//	Spk Volume
//	" RF Tx Pwr",	//	RF Tx Power

	switch( idxItem )
	{
	case 0:		//	Mic Volume
		SetActiveMenu( &g_MenuSetMicList );
		GetActiveMenu()->currIdx = GetMicVol();	//	메뉴 Index초기화.

		UpdateLCDMenu();

		break;

	case 1:		//	Spk Volume
		SetActiveMenu( &g_MenuSetSpkList );
		GetActiveMenu()->currIdx = GetSpkVol();	//	메뉴 Index초기화.

		UpdateLCDMenu();
		break;

	case 2:		//	RF Tx Power
		SetActiveMenu( &g_MenuSetTxPwrList );
		GetActiveMenu()->currIdx = 7;	//	메뉴 Index초기화.

		UpdateLCDMenu();
		break;

	default:
		printf("%s(%d) - invalid menu(%d)\n", __func__, __LINE__, idxItem);
		break;
	}
}

//========================================================================
void 	ProcMenuSetMic( int idxItem )
//========================================================================
{
	LCDMenuUpDown( 0 );

	//	Mic : 0 ~ 9

	switch( idxItem )
	{
	case 0:
	case 1:
	case 2:
	case 3:
	case 4:
	case 5:
	case 6:
	case 7:
	case 8:
	case 9:
		SetMicVol(idxItem);
		break;
	default:
		printf("%s(%d) - invalid menu(%d)\n", __func__, __LINE__, idxItem);
		break;
	}
	//  1초후 Main화면 갱신.
	HAL_Delay( 1000 );
	UpdateLCDMain();

	SetActiveMenu( NULL );
}

//========================================================================
void 	ProcMenuSetSpk( int idxItem )
//========================================================================
{
	LCDMenuUpDown( 0 );

	//	Spk : 0 ~ 3

	switch( idxItem )
	{
	case 0:
	case 1:
	case 2:
	case 3:
		SetSpkVol(idxItem);
		break;
	default:
		printf("%s(%d) - invalid menu(%d)\n", __func__, __LINE__, idxItem);
		break;
	}
	//  1초후 Main화면 갱신.
	HAL_Delay( 1000 );
	UpdateLCDMain();

	SetActiveMenu( NULL );
}


//========================================================================
void 	ProcMenuSetTxPwr( int idxItem )
//========================================================================
{
	LCDMenuUpDown( 0 );

//	char *_sSetTxPwrList[] = { 	" Pwr : -38 dBm",	//	0
//								" Pwr : -20 dBm",	//	1
//								" Pwr : -5 dBm",	//	4
//								" Pwr : 0 dBm",		//	7
//								" Pwr : 5 dBm",		//	12
//								" Pwr : 10 dBm",	//	22
//								" Pwr : 15 dBm",	//	40
//								" Pwr : 20 dBm",	//	127
//							};

	switch( idxItem )
	{
	case 0:	vRadio_Set_TxPower(0);		break;
	case 1:	vRadio_Set_TxPower(1);		break;
	case 2:	vRadio_Set_TxPower(4);		break;
	case 3:	vRadio_Set_TxPower(7);		break;
	case 4:	vRadio_Set_TxPower(12);		break;
	case 5:	vRadio_Set_TxPower(40);		break;
	case 6:	vRadio_Set_TxPower(127);	break;
	default:
		printf("%s(%d) - invalid menu(%d)\n", __func__, __LINE__, idxItem);
		break;
	}
	//  1초후 Main화면 갱신.
	HAL_Delay( 1000 );
	UpdateLCDMain();

	SetActiveMenu( NULL );
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

