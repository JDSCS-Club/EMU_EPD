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
//	Menu LCD Disp

void    UpdateLCDMain	( void );
void    UpdateLCDMenu	( void );

//========================================================================
//	Menu Key Proc

void    ProcBtnUp		( void );
void    ProcBtnDown		( void );

void    ProcBtnMenu		( void );
void    ProcBtnOK		( void );

//========================================================================
#endif
//========================================================================


