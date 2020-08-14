/*------------------------------------------------------------------------------------------
	Project			: RFM
	Description		: 

	Writer			: $Author: zlkit $
	Revision		: $Rev: 1891 $
	Date			: 2020. 08.
	Copyright		: Piranti Corp.	( sw@piranti.co.kr )
	 
	Revision History 
	1. 2020. 08.	: Created
-------------------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------------------
	[ 구현목록 정의 ]
	* RFM 공통함수 모음.
-------------------------------------------------------------------------------------------*/


//========================================================================
#ifndef RFM_H
#define RFM_H
//========================================================================

#include "RFMProtocol.h"		//

//========================================================================
// Define

enum eRFMMode
{
	RFMModeNormal	=	0,
	RFMModeTx		=	1,
	RFMModeRx		=	2,
};

//========================================================================

enum eRFMConfig
{
	DefaultSpkVol	=	1,
	MaxSpkVol		=	3,			//	볼륨 최대값.
	MaxTrainSet		=	10,			//	10편성.
};

extern int		g_nSpkLevel;		//  Default (2) - 0(Mute) / 1 / 2(Normal) / 3

extern int		g_idxTrainSet;		//  Train Set Index

//========================================================================
int		GetRFMMode		( void );
void	SetRFMMode		( int nRFMMode );
//========================================================================


int		GetDevID		( void );
void	SetDevID		( int nDevID );

int		GetTrainSetIdx	( void );
void	SetTrainSetIdx	( int idxTrainSet );

int		GetCarNo		( void );
void	SetCarNo		( int nCarNo );

int		GetSpkVol	    ( void );
void	SetSpkVol	    ( int nSpkVol );


//========================================================================
//	GPIO Setting

void	RF_Tx_Mode		( void );
void	RF_Rx_Mode		( void );

void	RFM_Spk			( int bOnOff );		//	1(On) / 0(Off)

void	RF_Ping			( void );
void	RF_RSSI			( void );

//========================================================================

void	PrintVerInfo	( void );

//========================================================================
//	Command

int		cmd_ch			( int argc, char * argv[] );
int		cmd_car			( int argc, char * argv[] );
int		cmd_info		( int argc, char * argv[] );

//========================================================================
//		RFM Procedure
//========================================================================

int		InitRFM			( void );
int		RFM_main		( void );
void	LoopProcRFM		( void );


//========================================================================
#endif
//========================================================================


