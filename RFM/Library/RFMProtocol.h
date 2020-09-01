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
	PktStat			=	0x01,		//	상태정보.
	PktPA			=	0x02,		//	방송. ( 송신기 -> 수신기 )
	PktCall			=	0x03,		//	통화. ( 송신기 -> 송신기 )
	PktLight		=	0x04,		//	조명제어. ( On/Off )
	PktLightOn		=	PktLight,
	PktLightOff		=	0x05,		//	조명제어. ( Off )
	PktAudioPA		=	0x12,		//	Audio Stream. ( 방송 )
	PktAudioCall	=	0x13,		//	Audio Stream. ( 통화 )
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

//==========================================================================
//	RFM Packet

#if defined(USE_HOPPING)

#define		RFPktHdrLen		4
#define		RFPktDataLen	58

typedef struct _RFMPktHdr
{
	uint16_t	nIDFlag;		//	Device ID Flag		( 0(Stat) : Hopping X )
	uint8_t		nSeq;			//	Pkt Sequence Number ( 0(Stat) : Hopping X / 1 ~ 255 : Hopping)
	uint8_t		nPktCmd;		//	Command에 따라 Data Length 구분.
} RFMPktHdr;

#else	//	Non - Hopping

#define		RFPktHdrLen		4
#define		RFPktDataLen	58

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
	uint8_t		nCh;			//	Channel		-	채널
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
	uint8_t		nSpare22[1];	//	Spare

} RFMPktStat;


//==========================================================================
//	RFM Packet - Command PA/Call - Start/Stop
typedef struct _RFMPktPACall
{
	uint8_t		nStartStop;		//	Start(1) / Stop(0)
	uint8_t		nSpare1[3];		//	Spare
} RFMPktPACall;


//==========================================================================
//	RFM Packet - Command Light On/Off
typedef struct _RFMPktLight
{
	uint8_t		nOnOff;			//	On(1) / Off(0)
	uint8_t		nSpare1[3];		//	Spare
} RFMPktLight;

//==========================================================================

typedef struct _RFMPkt
{
	RFMPktHdr	hdr;

	union
	{
		uint8_t			data[RFPktDataLen];	//	Data
		RFMPktStat		stat;				//	상태정보
		RFMPktLight		light;				//	Light On/Off정보.
		RFMPktPACall	pacall;				//	PA/Call Start/Stop
	} dat;
} RFMPkt;


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

//==========================================================================

void	SendStat			( void );
void	SendPA				( int nStartStop );
void	SendCall			( int nStartStop );
void	SendLight			( int nOnOff );

void	SendLightOn			( void );		//	조명제어 조명 On
void	SendLightOff		( void );		//	조명제어 조명 Off

//==========================================================================

int		ProcPktStat			( const RFMPkt *pPkt );
int		ProcPktPA			( const RFMPkt *pPkt );
int		ProcPktCall			( const RFMPkt *pPkt );
int		ProcPktLight		( const RFMPkt *pPkt );
int		ProcPktAudioPA		( const RFMPkt *pPkt );
int		ProcPktAudioCall	( const RFMPkt *pPkt );

//==========================================================================

void	ProcessPkt			( const uint8_t *pbuf, int length );

//==========================================================================
#ifdef __cplusplus
}
#endif
//==========================================================================


#endif	//	RFM_PROTOCOL_H

