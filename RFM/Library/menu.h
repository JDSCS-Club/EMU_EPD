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

/*------------------------------------------------------------------------------------------
	[ 구현목록 정의 ]
	* Func1 구현. 
		- Func1 기능 세부내용.

	* Func2 구현. 
-------------------------------------------------------------------------------------------*/

//========================================================================
#ifndef MENU_H
#define MENU_H
//========================================================================

#include "RFMProtocol.h"		//	

//========================================================================
//	Define

#define		USE_RFT_MENU_TRAINSET		1		//	송신기 편성설정.

#define		USE_RFT_ONLY_RX_SPK_ON		1		//	송신기 수신중일때 만 Spk On.

//========================================================================
//	Menu Structure

typedef struct _Menu
{
	char 		**sItem;					//	Item Name
	int			cntItem;					//	Menu Item Count
	int			currIdx;					//	Current Item Index
	void		(*cbFunc)( int idxItem );	//	Callback Func
} Menu_t;

extern Menu_t	g_MenuMain;			//	Main Menu
extern Menu_t	g_MenuLightCtrl;	//	Light Ctrl
extern Menu_t	g_MenuTrainSet;		//	TrainSet

//========================================================================
//	Menu Procedure

void 	ProcMenuMain		( int idxItem );	//	Main
void	ProcMenuTrainSet	( int idxItem );	//	TrainSet
void 	ProcMenuLightCtrl	( int idxItem );	//	LightCtrl

//========================================================================

void	ProcLightOn		( void );
void	ProcLightOff	( void );
void	ProcDispVer		( void );

//========================================================================

void    UpdateLCDMain	( void );
void    UpdateLCDMenu	( void );

void    ProcBtnUp		( void );
void    ProcBtnDown		( void );

void    ProcBtnMenu		( void );
void    ProcBtnOK		( void );

#if defined(USE_ENV_TEST)

void	SetLoopRFTx		( int bEnable );	//	RF Data 전송 유지..

int		cmd_rftx		( int argc, char * argv[] );

#endif	//	defined(USE_ENV_TEST)

//========================================================================
#endif
//========================================================================


