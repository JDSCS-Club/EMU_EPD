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

//typedef struct _MenuItem
//{
//	char 	*sItem;							//	Item Name
//	void	*pNextMenu;						//	Next Menu
//} MenuItem_t;

//#define	MAX_MENU_ITEM	20

typedef struct _Menu
{
	enum
	{
		MaxMenuItem = 20,
	};

//	MenuItem_t 	*listItem;					//	Menu Item List
//	char 		*sItem[];					//	Item Name
	char 		*sItem[MaxMenuItem];		//	Item Name
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

#define		RF_CMD_SIZE				RFPktHdrLen			//	[4Byte Commnad]

//========================================================================

extern int		g_nSpkLevel;                //  Default (2) - 0(Mute) / 1 / 2(Normal) / 3

extern char		*g_sTrainSet[];
extern int		g_cntTrainSet;

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

void	RF_PA_loop		( void );

int		RF_PA_Init		( void );

int		RF_PA_main		( void );

void	RF_PA_I2SEx_TxRxCpltCallback	( I2S_HandleTypeDef* hi2s );


//========================================================================
#endif
//========================================================================


