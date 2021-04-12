/*------------------------------------------------------------------------------------------
	Project			: RFM ( RF-Module )
	Description		: RFM Protocol Header

	Writer			: $Author: zlkit $
	Revision		: $Rev: 1446 $
	Date			: 2020. 02.
	Copyright		: Piranti Corp.
	 
	Revision History 
	1. 2020. 02.	: Created
-------------------------------------------------------------------------------------------*/

#ifndef RFM_PROTOCOL_H
#define RFM_PROTOCOL_H

#include <stdint.h>			//	uint8_t, ...

#include "typedef.h"		//	char_t, int32_t, ...


//==========================================================================
//	Configuration

#include "rfm_config.h"		//	USE_HOPPING

//==========================================================================
//	Define


//==========================================================================
#pragma pack(1)		//	Byte Align : 1 Byte
//==========================================================================

//==========================================================================
//	Packet Define ( 64 Byte )
//
//		|----------------------------------------------------|
//		|            Header 4 Byte           |  Data 60 Byte |
//		|----------------------------------------------------|
//		| Src Addr | Dest Addr | Len | PktID |     Data      |
//		|----------------------------------------------------|
//	

//==========================================================================
//	Packet Define ( 64 Byte ) - Hopping
//		Header Type
//		|-----------------------------------------------------------------|
//		|                   Header 4 Byte                  | Data 60 Byte |
//		|-----------------------------------------------------------------|
//		| DevIDFlag(2Byte) | Seq. No(1Byte) | PktID(1Byte) |    Data      |
//		|-----------------------------------------------------------------|


//		Header Type #1
//		|-------------------------------------------------------------------------|
//		| HdrID |              Header 4 Byte                       | Data 60 Byte |
//		|-------------------------------------------------------------------------|
//		| 0 | 0 | DevIDFlag(14Bit) | Seq. No(1Byte) | PktID(1Byte) |     Data     |
//		|-------------------------------------------------------------------------|

//		Header Type #2
//		|-------------------------------------------------------------------------|
//		| HdrID |              Header 4 Byte                       | Data 60 Byte |
//		|-------------------------------------------------------------------------|
//		| 0 | 1 | DevIDFlag(14Bit) | Src Ch(1Byte)  | PktID(1Byte) |     Data     |
//		|-------------------------------------------------------------------------|

//==========================================================================

/*==========================================================================
 *	Hopping Algorithm
 *==========================================================================

	* Packet Structure

	1. Sequence Number
		|-------------------------------------------------------------|
		|  4 Byte |            Header 4 Byte           | Data 56 Byte |
		|-------------------------------------------------------------|
		| Seq. No | Src Addr | Dest Addr | Len | PktID |    Data      |
		|-------------------------------------------------------------|

		- 상위 Seq. No 수신시 동일한 Packet 송신 후 Seq. No 증가.

	2. ID Flag
		- 각 장치별 ID부여 ( #0 ~ #9호차 수신기 / 송신기 #10, #11 )
//		|-------------------------------------------------------------|
//		|  4 Byte |            Header 4 Byte           | Data 56 Byte |
//		|-------------------------------------------------------------|
//		| ID Flag | Src Addr | Dest Addr | Len | PktID |    Data      |
//		|-------------------------------------------------------------|

		|-------------------------------------------------------------|
		|  2 Byte | Header 2 Byte   |        Data 56 Byte             |
		|-------------------------------------------------------------|
//		| ID Flag |  Len  |  PktID  |            Data                 |
		| ID Flag |  Seq  |  PktID  |            Data                 |
		|-------------------------------------------------------------|

	2-1. ID Flag
		- Data 송신시 자신의 ID Flag Set하여 전송.
		- 자신의 ID가 Set된 Packet 수신 시 Skip.

	2-2. ID Flag ( Route )
		- 장치별 수신감도(RSSI) 확인.
		- Data 송신시 자신포함 주변장치 ID Flag Set하여 전송.
		- 수신된 ID Flag중에 포함이 안된 장치가 존재하는 경우 
		  해당장치 ID Flag Set하여 패킷 송신.

	3. ID Flag ( Src / Dest )
		- 각 장치별 ID부여 ( #0 ~ #9호차 수신기 / 송신기 #10, #11 )
		|---------------------------------------------------------------------------------|
		|  4 Byte                    |              Header 4 Byte         |  Data 56 Byte |
		|---------------------------------------------------------------------------------|
		| Src ID Flag | Dest ID Flag | Src Addr | Dest Addr | Len | PktID |     Data      |
		|---------------------------------------------------------------------------------|
		- 자신의 Src ID Flag Set하고 주변의 Device에 Dest ID Flag를 설정.
		- 주변 장치가 Deivce ID Flag와 Dest ID Flag를 Src ID Flag에 Set하고
		  상태정보 응답하는 타 장치로 Data 전송.

 *==========================================================================
 *	Frequency Define
 *==========================================================================

	*==========================================================================
	*	주파수 정의.
	*==========================================================================
	* Base Freq.		: 915 MHz
	* Channel spacing	: 0.25 MHz ( 250 kHz )
	* Center Freq.		: 931 MHz
	* Channel Number	: 64

	* Modulation type	: 2GFSK
	* Data rate			: 500 kbps
	* Packet Data Size	: 64 byte
	* CRC16				: Enable

	*==========================================================================
	*	채널 사용 정의.
	*==========================================================================
	* CH0 : 공통채널
	* CH1 : Upgrade 전용 채널 ( 수신기 - Target )
	* CH2 : Upgrade 전용 채널 ( 송신기 - Origin )
	* CH3 ~ 10 : Reserved
	* CH10 : RFT(송신기)
	* CH11 : 1편성 ( 1,3,5호차 )
	* CH12 : 1편성 ( 2,4,6호차 )
	* CH13 : 2편성 ( 1,3,5호차 )
	* CH14 : 2편성 ( 2,4,6호차 )
	*
	* 	...

==========================================================================*/


//==========================================================================
//	Define ( enum )


//==========================================================================
//	Device ID
enum eDevID
{
	DevNone			=	0x00,
	DevRF900M		=	0x01,       //  RF900M 수신기
	DevRF900T		=	0x02,       //  RF900T 송신기
};

//==========================================================================
//	Command
enum ePktID
{
	PktNone			=	0x00,		//	None

	//	Packet ID	: 0x00 ~ 0x7F
	//	|-----------------------------------------------------------------|
	//	|                   Header 4 Byte                  | Data 60 Byte |
	//	|-----------------------------------------------------------------|
	//	| DevIDFlag(2Byte) | Seq. No(1Byte) | PktID(1Byte) |    Data      |
	//	|-----------------------------------------------------------------|
	PktStat			=	0x01,		//	상태정보. ( nIDFlag(0) / nSeq(0) )
	PktPA			=	0x02,		//	방송스트림. ( 송신기 -> 수신기 ) / OccPa ( 송신기 -> 송신기 )
	PktCall			=	0x03,		//	통화스트림. ( 송신기 -> 송신기 )
	PktLight		=	0x04,		//	조명제어. ( On/Off )

	PktCtrlPaCall	=	0x08,		//	방송/통화 제어. ( 시작 / 종료 )

	//==========================================================================
	//	Request
	PktStatReq		=	0x11,		//	상태정보 요청. ( nIDFlag(0) / nSeq(0) )

	PktDevConn		=	0x19,		//	장치 노드 연결. ( Device Node Connection )

	//==========================================================================
	//	Command
	PktCmd			=	0x20,		//	Command ( nIDFlag(0) / nSeq(0) )
	PktCmdRsp		=	0x21,		//	Cmd 처리결과 응답.

	//==========================================================================
	//	Upgrade
	PktUpgr			=	0x40,		//	Upgrade ( nIDFlag(0) / nSeq(0) )
	PktUpgrStat		=	0x41,		//	Upgrade Status : Success / Failed


	//	Packet ID Extented : 0x80 ~ 0xFF
	//	|---------------------------------------------------------------------------|
	//	|                   Header 4 Byte                  | Ex 4Byte | Data 60 Byte|
	//	|---------------------------------------------------------------------------|
	//	| DevIDFlag(2Byte) | Seq. No(1Byte) | PktID(1Byte) |          |   Data      |
	//	|---------------------------------------------------------------------------|
//
//	PktExStatReq	=	0x81,		//	상태정보 요청. ( nIDFlag(0) / nSeq(0) )
//	PktExStatRsp	=	0x82,		//	상태정보 응답. ( nIDFlag(0) / nSeq(0) )
//
//	PktExConnTx		=	0x88,		//	수신기->송신기 : 등록 / 해제

};

enum eChannel
{
	ChNoUsed		=	0,			//	* CH0 : 사용안함. - 915 MHz
	ChCommon		=	1,			//	* CH1 : 공통채널

	ChUpgrDst		=	3,			//	* CH3 : Upgrade 전용 채널 ( 수신기 - Target )
	ChUpgrSrc		=	4,			//	* CH4 : Upgrade 전용 채널 ( 송신기 - Origin )

	ChResv			=	5,			//	* CH3 ~ 7 : Reserved

	ChGap			=	4,			//	호차별 채널 간격. ( 4 - 1 Mhz )

	ChTx_1			=	8,			//	* CH8 : 송신기#1 - (Car No : 11)
	ChTx_2			=	(ChTx_1+ChGap),		//	9,	//	* CH9 : 송신기#2 - (Car No : 12)

//DEL	ChRFT			=	10,			//	* 송신기 - 917.5 MHz

//	ChTS1_1
	ChTS_base		=	11,			//	* CH11 : 수신기 Base 채널.

	ChTS1_1			=	21,			//	* CH11 : 1편성 ( 1호차 ) - Offset ( + 10 )
	

//	ChTS1_1			=	11,			//	* CH11 : 1편성 ( 1호차 )
	// ChTS1_2			=	12,			//	* CH12 : 1편성 ( 2호차 )

	// //	...

	// ChTS1_10		=	20,			//	* CH20 : 1편성 ( 10호차 ) - 920 MHz

	// ChTS2_1			=	21,			//	* CH21 : 2편성 ( 1호차 )
	// ChTS2_2			=	22,			//	* CH22 : 2편성 ( 2호차 )
};

//==========================================================================
//	Device Status
enum eStatus
{
	StatNone		=	0x00,		//	Normal
	StatPATx		=	0x01,       //  방송 송신중. ( 송신기 -> 수신기 )
	StatPARx		=	0x02,       //  방송 수신중.
	StatCallTx		=	0x03,       //  통화 송신중. ( 송신기 -> 송신기 )
	StatCallRx		=	0x04,       //  통화 수신중.
};

enum eStartStop
{
	PACallStart 	=	0x01,		//	Start
	PACallStop		=	0x00,		//	Stop
};

enum eDevFlag
{
	DevFlagNone		=	0x00,		//	Normal
#if defined(USE_STAT_LIGHT)
	DevFlagLight	=	0x01,       //  점등/소등 상태.
#endif
};


//==========================================================================
//	RFM Packet

#if defined(USE_HOPPING)

#define		RFPktHdrLen		4
#define		RFPktDataLen	60

typedef struct _RFMPktHdr
{
	enum eHdrID
	{
		HdrID1	=	0x0,		//	4 Byte Header
//		HdrID2	=	0x1,		//
	};
	uint16_t	nIDFlag:14;		//	[13:0] Device ID Flag		( 0(Stat) : Hopping X )
	uint16_t	bHdrID:2;		//	[15:14] 00 : Hdr#1 / 01 : Hdr#2
	uint8_t		nSeq;			//	Hdr#1 : Pkt Sequence Number ( 0(Stat) : Hopping X / 1 ~ 255 : Hopping)
	uint8_t		nPktCmd;		//	Command에 따라 Data Length 구분.
} RFMPktHdr;

//#define		RFPktExHdrLen		8
//#define		RFPktExDataLen		56

#if defined(USE_CH_ISO_DEV)		//	장치별 채널분리.

typedef struct _RFMPktHdr2
{
	enum eHdrIDEx
	{
//		HdrID1	=	0x0,		//	4 Byte Header
		HdrID2	=	0x1,		//	채널 분리.
	};
	uint8_t		nTS:6;			//	[5:0] TrainSet
	uint8_t		bHdrID:2;		//	[7:6] 00 : Hdr#1 / 01 : Hdr#2
	uint8_t		nSrcCh:6;		//	Source Channel ( 0 ~ 64 )
	uint8_t		bRFT1:1;		//	송신기#1 전송 Flag
	uint8_t		bRFT2:1;		//	송신기#2 전송 Flag
	uint8_t		nSpare3;		//	Spare
	uint8_t		nPktCmd;		//	Command에 따라 Data Length 구분.
} RFMPktHdr2;

#endif	//	defined(USE_CH_ISO_DEV)		//	장치별 채널분리.

#else	//	Non - Hopping

#define		RFPktHdrLen		4
#define		RFPktDataLen	60

//==========================================================================
typedef struct _RFMPktHdr
{
	uint8_t		addrSrc;		//	Src Address
	uint8_t		addrDest;		//	Dest Address
	uint8_t		nLen;
	uint8_t		nPktCmd;
} RFMPktHdr;

//==========================================================================
#endif

//==========================================================================
//	RFM Packet - Status Data Request
typedef struct _RFMPktStatReq
{
	//	상태정보 요청.
	//--------------------------------------------------------------------------
	//	TEXT 0
	uint8_t		nSrcCh;			//	Source Channel
	uint8_t		nSpare1[3];		//	Spare

	uint8_t		nTrainNo;		//	편성번호.
	uint8_t		nCarNo;			//	열차번호.

} RFMPktStatReq;

//==========================================================================
//	RFM Packet - Status Data
typedef struct _RFMPktStat
{
	//--------------------------------------------------------------------------
	//	TEXT 0
	uint8_t		nStat;			//	Device Status. ( eStatus )
	uint8_t		nDevID;			//	Device ID

//	uint8_t		nSpare2[2];		//	Spare
	uint16_t	nMagicNum;		//	0xAA55
	//--------------------------------------------------------------------------
	//	TEXT 4
	uint32_t	upTime;			//	Device Uptime ( msec )
	//--------------------------------------------------------------------------
	//	TEXT 8
	uint16_t	nFrequncy;		//	Frequency	-	주파수
	uint8_t		nChRx;			//	Channel		-	수신채널
	uint8_t		nSpare11[1];	//	Spare
	//--------------------------------------------------------------------------
	//	TEXT 12
	uint8_t		nTrainNo;		//	편성번호.
	uint8_t		nCarNo;			//	열차번호.
	uint8_t		ver_main;		//	Version Info
	uint8_t		ver_sub;		//
	//--------------------------------------------------------------------------
	//	TEXT 16
	uint16_t	ver_det;		//	
	uint16_t	ver_build;		//	
	//--------------------------------------------------------------------------
	//	TEXT 20
	uint16_t	rspID;			//	응답 ID
	uint8_t		nManHop;		//	Manual Hopping 설정. ( 0:Default / 1:On / 2: Off )
	uint8_t		nSpare23[1];	//	Spare
	//--------------------------------------------------------------------------
	//	TEXT 24
	uint8_t		nDevFlag;		//	Device Flag ( DevFlagLight )

} RFMPktStat;


//==========================================================================
//	RFM Packet - Status Data
typedef struct _RFMDevStat
{
	//--------------------------------------------------------------------------
	RFMPktStat	stat;
	int			stampRx;	//	상태정보 수신 Timestamp
	int			nRSSI;		//	RSSI 값.
} RFMDevStat;


//==========================================================================
//	RFM Packet - Device Node Connection
typedef struct _RFMPktDevConn
{
	//	Device Node Connection
	//--------------------------------------------------------------------------
	//	TEXT 0

	enum eDevConnect
	{
		DevConnect			=	1,	//	Node 연결
		DevDisconnect		=	0,	//	Node 연결 해제
	};

	uint8_t		nDevType;		//	장치 Type DevRF900T(송신기) / DevRF900M(수신기)
	uint8_t		nSrcCh;			//	송신기 Channel 등록.
	uint8_t		nConnect;		//	Connect / Disconnect
	uint8_t		nSpare1[13];	//

} RFMPktDevConn;


//==========================================================================
//	RFM Packet - Command Data
typedef struct _RFMPktCmd
{
	//	RSSI 수신감도 측정.
	//		0.25 M : 247 ~
	//		0.50 M : 244
	//		0.75 M : 230
	//		1.00 M : 224
	//		1.25 M : 221
	//		1.50 M : 200 ~ 210
	//		3.50 M : 193

	int8_t		nRSSIOver;		//	0 	| RSSI값 확인 후 해당 범위 내에 있는 경우 명령 동작.
	int8_t		nRsp;			//	1	| 0:None / 1:처리결과 송신.
	int8_t		nSpare[2];		//	2	| Spare
	char		sCmd[50];		//	10	| CLI String Command
} RFMPktCmd;

//==========================================================================
//	RFM Packet - Upgrade Data ( 60 Byte )
typedef struct _RFMPktUpgr
{
	enum ePktUpgr
	{
		PktUpgrDataSize		=	48,		//	FIXME: 4Byte의 배수로 설정 해야함.

		MaxUpgrDataPacket	=	8192,	//	Max 0x60000( 384KB ) : 8192 Packet = 384 * 1024 / 48
	};

	enum ePktUpgrFlag
	{
		PktUpgrFlagRetry	=	0x1,
	};
	uint32_t	baseAddr;		//	0	| Base Address ( 0x08080000 ~ 0x080FFFFF )
	uint16_t	totPkt;			//	4	| Total Packet
	uint16_t	idxPkt;			//	6	| Index Packet
	uint8_t		nSize;			//	8	| Data Size ( 0 ~ 50 )
//	int8_t		nSpare;			//	9	| Spare
	uint8_t		bFlag;			//	9	| Retry ( 1 - Retry )
	char		data[PktUpgrDataSize];	//	48];		//	10	| Binary Data
#if ( 50 - PktUpgrDataSize )
	uint8_t		nSpare2[ 50 - PktUpgrDataSize ];	//	| Spare
#endif
} RFMPktUpgr;

//==========================================================================
//	RFM Packet - Upgrade Stat
typedef struct _RFMPktUpgrStat
{
	enum ePktUpgrStat
	{
//		PktUpgrDataSize = 50
		UpgrStatFailed	= 0,
		UpgrStatSuccess	= 1,
	};
	uint8_t		nResult;		//	0	| 1 ( Success ) / 0 ( Failed )
	uint8_t		nSpare1[3];		//	1	| Spare
	uint8_t		nTrainSet;		//	4	| Train Set
	uint8_t		nCarNo;			//	5	| Car Number
	uint8_t		nSpare6[2];		//	6	| Spare
	uint16_t	nTotPkt;		//	8	| Total Packet
	uint16_t	nRecvPkt;		//	10	| Recv Packet
} RFMPktUpgrStat;

//==========================================================================
//	RFM Packet - Command PA/Call - Start/Stop
typedef struct _RFMPktStrmPACall
{
	uint8_t		nStrmData[RFPktDataLen];	//	PA / Call Stream Data
} RFMPktStrmPACall;

//==========================================================================
//	RFM Packet - Command PA/Call - Start/Stop
typedef struct _RFMPktCtrlPACall
{
	enum eCtrlStartStop
	{
		//	nStart/Stop
		CtrlStart	=	1,
		CtrlStop	=	0,
	};

	enum eCtrlPACall
	{
		//	nStart/Stop
		CtrlPA		=	1,
		CtrlCall	=	2,
		CtrlOccPa	=	3,
	};
	uint8_t		nStartStop;		//	Start(1) / Stop(0)
	uint8_t		nTypePACall;	//	PA( PktPA - 2 ) / Call( PktCall - 3 )
	uint8_t		nSpare2[2];		//	Spare
} RFMPktCtrlPACall;


//==========================================================================
//	RFM Packet - Command Light On/Off
typedef struct _RFMPktLight
{
	uint8_t		nOnOff;			//	On(1) / Off(0)
	uint8_t		nSpare1[3];		//	Spare
} RFMPktLight;

//==========================================================================
//	Packet		:		Header( 4 Byte ) / Data( 60 Byte )
typedef struct _RFMPkt
{
	union
	{
		RFMPktHdr	hdr;
		RFMPktHdr2	hdr2;	//	Source Channel / Train Set 포함
	};

	union
	{
		uint8_t				data[RFPktDataLen];		//	Data
		RFMPktStat			stat;					//	Status Data
		RFMPktStatReq		statReq;				//	Status Data Request
		RFMPktLight			light;					//	Light On/Off
		RFMPktCtrlPACall	pacall;					//	PA/Call Start/Stop
		RFMPktStrmPACall	pacallStrm;				//	PA/Call Stream Data
		RFMPktCmd			cmd;					//	Remote RF Command
		RFMPktDevConn		devConn;				//	Device Node Connection
		RFMPktUpgr			upgr;					//	Upgrade Binary Data
		RFMPktUpgrStat		upgrStat;				//	Upgrade Status
	} dat;

	//	Tail : Src Channel ( 1 Byte )
} RFMPkt;

//==========================================================================
//	Packet Extend	:	Header( 8 Byte ) / Data( 56 Byte ) - 확장패킷 ( 헤더정보에 편성번호 / 소스채널 추가 )
//typedef struct _RFMPktEx
//{
//	RFMPktHdrEx	hdr;
//
//	union
//	{
//		uint8_t				data[RFPktExDataLen];	//	Data
//		RFMPktStat			stat;					//	Status Data
//		RFMPktStatReq		statReq;				//	Status Data Request
//		RFMPktLight			light;					//	Light On/Off
//		RFMPktCtrlPACall	pacall;					//	PA/Call Start/Stop
//		RFMPktStrmPACall	pacallStrm;				//	PA/Call Stream Data
//		RFMPktCmd			cmd;					//	Remote RF Command
//		RFMPktDevConn		devConn;				//	Device Node Connection
//	} dat;
//
//	//	Tail : Src Channel ( 1 Byte )
//} RFMPktEx;



//==========================================================================
#pragma pack()
//==========================================================================

//==========================================================================


//==========================================================================
#ifdef __cplusplus
extern "C" {
#endif

//==========================================================================

void	SetStatus			( int nStat );
int		GetStatus			( void );

void	_MakePktHdr			( RFMPkt *pPkt, int addrSrc, int addrDest, int nLen, int nPktCmd );

#if defined(USE_CH_ISO_DEV)		//	장치별 채널분리.
void	_MakePktHdr2		( RFMPkt *pPkt, int nPktCmd );
#endif	//	defined(USE_CH_ISO_DEV)		//	장치별 채널분리.

//==========================================================================

void	SendStat			( int nDestCh );
void 	SendStatReq			( int nDestCh );

void	SendPA				( int nStartStop );
void	SendCall			( int nStartStop );
void 	SendOCCPA			( int nStartStop );

void	SendLight			( int nOnOff );
void	SendLightOn			( void );		//	조명제어 조명 On
void	SendLightOff		( void );		//	조명제어 조명 Off

void	SendRFCmd			( char *sCmd, int nRSSI );	//	원격 Command 명령 전송.
void	SendRFCmdReset		( void );		//	Reset 명령 전송.
void	SendRFCmdDFUMode	( void );		//	DFU Mode 명령 전송.
void	SendRFCmdUpgrade	( int bRetry );		//	Upgrade 명령 전송.

void	SendUpgrData		( uint32_t nAddrTarget, int nPktTot, int nPktIdx, uint8_t *sBuf, int nSize );	//	Send Upgrade Data
void	SendUpgrStat		( int nUpgrResult );	//	Send Upgrade Data

//==========================================================================

int		ProcPktStat			( const RFMPkt *pRFPkt );
int		ProcPktStatReq		( const RFMPkt *pRFPkt );

int		ProcPktPA			( const RFMPkt *pRFPkt );
int		ProcPktCall			( const RFMPkt *pRFPkt );
int		ProcPktOccPa		( const RFMPkt *pRFPkt );

int		ProcPktCtrlPaCall	( const RFMPkt *pRFPkt );
int		ProcPktDevConn		( const RFMPkt *pRFPkt );
int		ProcPktLight		( const RFMPkt *pRFPkt );

int		ProcPktCmd			( const RFMPkt *pRFPkt );
int		ProcPktCmdRsp		( const RFMPkt *pRFPkt );
int		ProcPktUpgr			( const RFMPkt *pRFPkt );
int		ProcPktUpgrStat		( const RFMPkt *pRFPkt );

//==========================================================================
#ifdef __cplusplus
}
#endif
//==========================================================================


#endif	//	RFM_PROTOCOL_H

