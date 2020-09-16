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
//	Configuration

#include "rfm_config.h"

//========================================================================
// Define

enum eRFMMode
{
	RFMModeNormal	=	0,	//	Rx/Tx
	RFMModeTx		=	1,	//	Tx Only
	RFMModeRx		=	2,
};

//========================================================================

enum eRFMConfig
{
	DefaultSpkVol	=	1,
	MaxSpkVol		=	3,			//	볼륨 최대값.
	MaxTrainSet		=	10,			//	100 ~ 109 편성 ( 10개 편성 )
//	MaxCarNo		=	12,			//	1 ~ 10호차 / 11(수신기#1) / 12(수신기#2)
	MaxCarNo		=	15,			//	1 ~ 10호차 / 11(수신기#1) / 12(수신기#2) / 13~15(Test)
};

extern int		g_nSpkLevel;		//  Default (2) - 0(Mute) / 1 / 2(Normal) / 3

extern int		g_idxTrainSet;		//  Train Set Index

extern int		g_offsetCA;			//	충돌회피 Offset ( msec )

extern int		g_nStampCallPa;		//	방송/통화 Stamp

extern int		g_nRSSI;			//	RSSI Value

#if defined(USE_HOP_MANUAL)
extern int		g_nManHopping;		//	On(1) / Off(2) / Unused(0 : Other)
#endif	//	defined(USE_HOP_MANUAL)

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
int		LoadCarNo		( void );			//	I2C에서 Loading

int		GetSpkVol	    ( void );
void	SetSpkVol	    ( int nSpkVol );

#if defined(USE_HOP_MANUAL)
int		GetManHop	( void );
void	SetManHop	( int nManHop );
#endif	//	defined(USE_HOP_MANUAL)


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

int 	cmd_ts      	( int argc, char * argv[] );
int		cmd_ch			( int argc, char * argv[] );
int		cmd_car			( int argc, char * argv[] );
int		cmd_info		( int argc, char * argv[] );
int 	cmd_rfstat		( int argc, char * argv[] );
int		cmd_rspid		( int argc, char * argv[] );

int		cmd_hop			( int argc, char * argv[] );	//	Hopping

#if defined(USE_ENV_TEST)

void	SetLoopRFTx		( int bEnable );	//	RF Data 전송 유지..
int		cmd_rftx		( int argc, char * argv[] );

#endif	//	defined(USE_ENV_TEST)

//========================================================================
//		RFM Procedure
//========================================================================

int		InitRFM			( void );
int		RFM_main		( void );
void	LoopProcRFM		( int nTick );


//========================================================================
#endif
//========================================================================


