/*------------------------------------------------------------------------------------------
	Project			: It Library
	Description		: 

	Writer			: $Author: zlkit $
	Revision		: $Rev: 1891 $
	Date			: 2019. 10.
	Copyright		: Piranti Corp.	( sw@piranti.co.kr )
	 
	Revision History 
	1. 2019. 10.	: Created
-------------------------------------------------------------------------------------------*/

//========================================================================
#ifndef MENU_H
#define MENU_H
//========================================================================

#include "RFMProtocol.h"		//	

//========================================================================
//	Define


//========================================================================
//	Menu Structure

typedef struct _Menu
{
	char 		**sItem;						//	Item Name
	int			cntItem;						//	Menu Item Count
	int			currIdx;						//	Current Item Index
	void		(*cbFunc)( int idxItem );		//	Callback Func
} Menu_t;

int 	IsMenuMaint			( void );			//	MainMenu Maintenace Menu활성화.
void 	EnableMenuMaint		( int bEnable );	//	MainMenu Maintenace Menu활성화.

extern Menu_t	g_MenuMain;						//	Main Menu
extern Menu_t	g_MenuLightCtrl;				//	Light Ctrl
extern Menu_t	g_MenuTrainSet;					//	TrainSet

extern char 	_sVerList[16][20];				//	Version Info

//========================================================================
//	Menu Procedure

void 	ProcMenuMain		( int idxItem );	//	Main
void	ProcMenuTrainSet	( int idxItem );	//	TrainSet
void 	ProcMenuLightCtrl	( int idxItem );	//	LightCtrl

void 	ProcMenuRFTID		( int idxItem );	//	송신기 ID설정.

//========================================================================
void 	ProcMenuSelfTest	( int idxItem );	//	자가진단 ( 호차별 상태정보 점검 )

void 	ProcMenuSetting		( int idxItem );	//	설정

void 	ProcMenuSetMic		( int idxItem );
void 	ProcMenuSetSpk		( int idxItem );
void 	ProcMenuSetTxPwr	( int idxItem );
void 	ProcMenuSetRFMode	( int idxItem );

//========================================================================

void 	ProcMenuDiag		( int idxItem );	//	Diag
void	ProcMenuVer			( int idxItem );	//	Menu Version	while( ( HAL_GetTick() - nTxStamp) <= 4 )	;	//	Tx 시작 후 완료까지 : 3msec

void	ProcMenuCmd			( int idxItem );	//	Command

void	ProcMenuSetCmdTS	( int idxItem  );
void	ProcMenuSetCmdCar	( int idxItem  );
void 	ProcMenuSetCmdRFMode( int idxItem );

//========================================================================

void	ProcLightOn			( void );
void	ProcLightOff		( void );
void	ProcDispVer			( void );
void	ProcDispStat 		( void );

//========================================================================
//	Menu LCD Disp

void    UpdateLCDMain		( void );
void    UpdateLCDMenu		( void );

void	UpdateLCDMonitor	( int nTick );		//	LCD : 모니터링 상태 Update

//========================================================================
//	Menu Key Proc

void    ProcBtnUp			( void );
void    ProcBtnDown			( void );

void    ProcBtnMenu			( void );
void    ProcBtnOK			( void );

//========================================================================
#endif
//========================================================================


