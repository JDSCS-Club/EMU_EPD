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
	RFMModeRx		=	2,	//	Rx Only
	RFMModeOcc		=	3,	//	OCC Mode
	RFMModeUpgr		=	4,	//	Upgrade Mode
};

//========================================================================

enum eRFMConfig
{
	DefaultSpkVol	=	1,
	MaxSpkVol		=	3,			//	Spk 볼륨 최대값.

	DefaultMicVol	=	5,
	MaxMicVol		=	9,			//	Mic 볼륨 최대값.

//DEL	MaxTrainSet		=	10,			//	100 ~ 109 편성 ( 10개 편성 )
	MaxTrainSet		=	20,			//	100 ~ 119 편성 ( 20개 편성 )
//	MaxCarNo		=	12,			//	1 ~ 10호차 / 11(수신기#1) / 12(수신기#2)
//	MaxCarNo		=	15,			//	1 ~ 10호차 / 11(수신기#1) / 12(수신기#2) / 13~15(Test)

	RFTCarNo1		=	11,			//	송신기 #1 ( 11호차 )
	RFTCarNo2		=	12,			//	송신기 #2 ( 12호차 )

	MaxRFMNo		=	10,			//	1 ~ 10호차

	MaxCarNo		=	13,			//	1 ~ 10호차 / 11(수신기#1) / 12(수신기#2)
};

//	Device Status


extern int			g_nDevID;			//  Device ID ( 1 : RF900M / 2 : RF900T )

extern int			g_nDevFlag;			//  Device Flag

extern int			g_nSpkLevel;		//  Default (2) - 0(Mute) / 1 / 2(Normal) / 3

extern int			g_idxTrainSet;		//  Train Set Index
extern int			g_nCarNo;			//  Car Number

extern int			g_nChRx;			//  RF Rx Channel

extern int			g_offsetCA;			//	충돌회피 Offset ( msec )

extern int			g_nStampRxPkt;		//	방송/통화 Stamp

extern int			g_nRSSI;			//	RSSI Value

extern RFMDevStat	g_devStat[];		//	Device Status

#if defined(USE_RF_COMM_MODE)
extern int			g_nRFMode;			//	RF Mode 무선 중계 모드. eRFMode ( RFMode1 / RFMode2 )
#endif

#if defined(USE_HOP_MANUAL)
extern int			g_nManHopping;		//	On(1) / Off(2) / Unused(0 : Other)
#endif	//	defined(USE_HOP_MANUAL)

//========================================================================
int		GetRFMMode		( void );
void	SetRFMMode		( int nRFMMode );

char	*StrRFMMode		( int nRFMMode );
//========================================================================

int 	GetChRx			( void );

int		GetChOtherRFT	( void );			//	타 송신기 채널.

int		GetChNearRFT	( int nMinRSSI );			//	가장 가까운 송신기 채널.
int		GetChNearRFM	( void );			//	가장 가까운 수신기 채널.

#if defined(USE_ROUTE_REQ)

extern int		g_nStampRouteRsp;		//	Route Rsp TimeStamp
extern int		g_nIdxRouteFindNext;	//	Next호차 Find Index / Route Rsp 수신시 Reset

void	SetChRFMUp		( int nCh );	//	가까운 수신기 채널 ( Up )
int		GetChRFMUp		( void );		//	가까운 수신기 채널 ( Up )

void	SetChRFMDown	( int nCh );	//	가까운 수신기 채널 ( Down )
int		GetChRFMDown	( void );		//	가까운 수신기 채널 ( Down )

#endif

void	SetChPA			( int nCh );		//	수신기로 송신할 방송채널 설정.
int		GetChPA			( void );

int		GetCh2Car		( int nCh );		//	채널 -> 호차정보 변환.

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
int		GetManHop		( void );
void	SetManHop		( int nManHop );
#endif	//	defined(USE_HOP_MANUAL)

#if defined(USE_RF_COMM_MODE)
void	SetRFMode		( int nRFMode );
int		GetRFMode		( void );
#endif	//	defined(USE_RF_COMM_MODE)


//========================================================================
//	GPIO Setting

void	RFM_Spk			( int bOnOff );		//	1(On) / 0(Off)

void	RF_RSSI			( void );

//========================================================================

void	PrintVerInfo	( void );


void ReflashStat( int nTick );
void ReloadStampStat( void );

//========================================================================
//	Command

int 	cmd_ts      	( int argc, char * argv[] );
int		cmd_ch			( int argc, char * argv[] );
int		cmd_car			( int argc, char * argv[] );
int 	cmd_swinfo    	( int argc, char * argv[] );
int		cmd_info		( int argc, char * argv[] );
int 	cmd_rfstat		( int argc, char * argv[] );

int 	cmd_rftx    	( int argc, char * argv[] );	//	RF Tx Test

int 	cmd_txpwr		( int argc, char * argv[] );	//	Tx Pwr Setting

int		cmd_rspid		( int argc, char * argv[] );

int		cmd_OccPa		( int argc, char * argv[] );	//	대승객방송 시작/종료

int		cmd_hop			( int argc, char * argv[] );	//	Hopping

int		cmd_rfmod		( int argc, char * argv[] );	//	RFMode


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


