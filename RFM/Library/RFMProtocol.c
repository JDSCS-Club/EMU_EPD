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

#include <stdio.h>				//	printf()
#include <stdint.h>				//	uint8_t, ...

#include <string.h>				//	memset()

#include "typedef.h"			//	char_t, int32_t, ...

#include "RFMProtocol.h"		//	RFM Protocol

#include "radio.h"				//	vRadio_StartTx_Variable_Packet()

#include "Adafruit_SSD1306.h"	//	LCDPrintf()

#include "main.h"				//	FLASH_ON_Pin / GPIO_PIN_SET / ...

#include "ProcPkt.h"			//	SendPacekt()

#include "version.h"			//	Version Info

#include "rfm.h"				//	g_nManHopping

#include "audio.h"				//	I2S_DMA_LOOP_SIZE

#include "flash_if.h"			//	FLASH_If_Write()

#include "eeprom.h"				//	AddrEEPUpgrMGN1


//==========================================================================
//	Define


//==========================================================================
//	Function


//==========================================================================
//		API
//==========================================================================



//==========================================================================
//		Send Packet
//==========================================================================

//========================================================================
void _MakePktHdr	( RFMPkt *pPkt, int addrSrc, int addrDest, int nLen, int nPktCmd )
//========================================================================
{
#if	defined(USE_HOPPING)

	if ( nPktCmd == PktStat
		|| nPktCmd == PktCmd
		|| nPktCmd == PktUpgr
		)
	{
		//	상태정보의 경우 Seq / ID : 0x00
		pPkt->hdr.bHdrID		=	HdrID1;		//	Header ID #1
		pPkt->hdr.nSeq			=	0x00;		//	Sequence
		pPkt->hdr.nIDFlag		=	0x00;		//	ID Flag
	}
	else
	{
		g_nPktSeq++;
		if ( g_nPktSeq == 0 )	g_nPktSeq++;
		pPkt->hdr.nSeq			=	g_nPktSeq;			//

		if ( GetDevID() == DevRF900T )
		{
			//	송신기.
			//		송신기의 송/수신반경과 수신기의 송/수신반경의 차이로 
			//		송신기가 상태정보를 수신후에 수신기에 송신을 할때 범위에 도달하지 못하는 경우가 있으므로, 
			//		수신기에서 중계를 할 수 있도록 송신기 ID만 설정하여 송신함.
			pPkt->hdr.nIDFlag		=	(0x1 << GetCarNo());	//	송신기는 자신의 ID만 Set하여 전송.
		}
		else
		{
			//	수신기.
			pPkt->hdr.nIDFlag		=	g_flagRspID;		//
		}
	}

#else

	pPkt->hdr.addrSrc		=	addrSrc;		//	Src Address
	pPkt->hdr.addrDest		=	addrDest;		//	Broadcast
	pPkt->hdr.nLen			=	nLen;			//	Length

#endif

	pPkt->hdr.nPktCmd		=	nPktCmd;		//	Status
}

#if defined(USE_CH_ISO_DEV)		//	장치별 채널분리.

//========================================================================
void _MakePktHdr2	( RFMPkt *pPkt, int nPktCmd )
//========================================================================
{
	//	상태정보의 경우 Seq / ID : 0x00
	pPkt->hdr2.bHdrID		=	HdrID2;				//	Header ID #1
	pPkt->hdr2.nSrcCh		=	GetChRx();			//	Source Channel
	pPkt->hdr2.nTS			=	GetTrainSetIdx();	//	ID Flag
	pPkt->hdr2.nPktCmd		=	nPktCmd;			//	Status

//	CLEAR_BIT(pPkt->hdr2.nTS, (0x1<<7));
//	SET_BIT(pPkt->hdr2.nTS, (0x1<<6));

#if defined( USE_RFT_REG_TO_RFM )
	//	자신의 송신기 ID Flag 설정.	-	재수신 받지 않음.
	if( GetRFTID() == 1 )
	{
		pPkt->hdr2.bRFT1	=	1;	//	SET_BIT( pPkt->hdr2.nSrcCh, (0x1<<6) );	//
		if( GetChPARFT() != 0 )	pPkt->hdr2.bRFT2	=	1;	//	송신기에 직접 전송하는경우.
	}
	else if( GetRFTID() == 2 )
	{
		pPkt->hdr2.bRFT2	=	1;	//	SET_BIT( pPkt->hdr2.nSrcCh, (0x1<<7) );//
		if( GetChPARFT() != 0 )	pPkt->hdr2.bRFT1	=	1;	//	송신기에 직접 전송하는경우.
	}

#endif
}

#endif	//	defined(USE_CH_ISO_DEV)		//	장치별 채널분리.

//==========================================================================
void _MakeRFCmd( RFMPkt	*pPkt, char *sCmd, int nRSSI )
//==========================================================================
{
	printf( "%s(%d)\n", __func__, __LINE__ );

	memset( pPkt, 0, sizeof( RFMPkt ) );

	//========================================================================
	//	Packet Header
	_MakePktHdr( pPkt, GetDevID(), 0xFF, RFPktDataLen, PktCmd );

	//========================================================================
	//	Command
	pPkt->dat.cmd.nRSSIOver = nRSSI;		//	명령 동작 RSSI 범위.
	strcpy( pPkt->dat.cmd.sCmd, sCmd );		//	명령 전송.

	//========================================================================
}


//========================================================================
void SendStatReq( int nDestCh )
//========================================================================
{
	if ( GetDbg() )	printf( "%s(%d)\n", __func__, __LINE__ );

	RFMPkt			stPkt;
	RFMPktStatReq	*pStatReq;

	memset( &stPkt, 0, sizeof( stPkt ) );
	pStatReq = (RFMPktStatReq *)&stPkt.dat.statReq;

	//========================================================================
	//	Packet Header
	_MakePktHdr( &stPkt, GetDevID(), 0xFF, sizeof( RFMPktStatReq ), PktStatReq );

	//========================================================================
	//	Packet Body
	pStatReq->nSrcCh	=	GetChRx();	//	수신받을 채널

	pStatReq->nCarNo	=	GetCarNo();			//	호차번호.
	pStatReq->nTrainNo	=	GetTrainSetIdx();	//	편성번호.

	//========================================================================
	//	Send RF
	SendPktCh( nDestCh, (U8 *)&stPkt, (U8)sizeof( RFMPktHdr ) + sizeof( RFMPktStatReq ) );
}

//========================================================================
void SendStat( int nDestCh )
//========================================================================
{
	//	RF Mode가 RFMModeNormal인 경우.
	//		상태정보 전송.

	if ( GetDbg() )	printf( "%s(%d)\n", __func__, __LINE__ );

	RFMPkt		stPkt;
	RFMPktStat	*pStat;

	memset( &stPkt, 0, sizeof( stPkt ) );
	pStat = (RFMPktStat *)&stPkt.dat.stat;

	//========================================================================
	//	Packet Header
	_MakePktHdr( &stPkt, GetDevID(), 0xFF, sizeof( RFMPktStat ), PktStat );

	//========================================================================
	//	Status Data
	pStat->ver_main		=	APP_MAIN_VER;		//	1;
	pStat->ver_sub		=	APP_SUB_VER;		//	0;
	pStat->ver_det		=	APP_DETAIL_VER;		//	0;
	pStat->ver_build	=	APP_BUILD_VER;		//	0;

	pStat->nMagicNum	=	0xAA55;				//	패킷 요류검출용.

	pStat->nCarNo		=	GetCarNo();
	pStat->nDevID		=	GetDevID();

	pStat->nTrainNo		=	GetTrainSetIdx();	//	편성번호.

	pStat->upTime		=	HAL_GetTick();

	pStat->rspID		=	g_flagRspID;		//	Rsp ID Flag

	pStat->nManHop		=	g_nManHopping;		//	Manual Hopping Setting

	pStat->nDevFlag		=	g_nDevFlag;			//	Device Flag : 조명 상태 등.

	pStat->nChRx		=	GetChRx();			//	수신 채널

	//========================================================================
	//	Send RF
#if defined(USE_CH_ISO_DEV)
	//	상태정보는 송신기로 전송.
	int nCh;

	//========================================================================
	if ( nDestCh == 0 )	nCh = ChTx_1;
	else				nCh = nDestCh;
	//========================================================================

	SendPktCh(nCh, (U8 *)&stPkt, (U8)sizeof( RFMPktHdr ) + sizeof( RFMPktStat ) );
#else
	SendPacket( (U8 *)&stPkt, (U8)sizeof( RFMPktHdr ) + sizeof( RFMPktStat ) );
#endif
	//========================================================================
}


//========================================================================
void SendPA( int nStartStop )
//========================================================================
{
	printf( "%s(%d)\n", __func__, __LINE__ );

	RFMPkt			stPkt;
	RFMPktCtrlPACall	*pPACall;

	memset( &stPkt, 0, sizeof( stPkt ) );
	pPACall = (RFMPktCtrlPACall *)&stPkt.dat.pacall;

	//========================================================================
	//	Packet Header
#if defined(USE_CH_ISO_DEV)
	_MakePktHdr2( &stPkt, PktCtrlPaCall );
#else
	_MakePktHdr( &stPkt, GetDevID(), 0xFF, sizeof( RFMPktCtrlPACall ), PktCtrlPaCall );
#endif

	//========================================================================
	//	Status Data
	pPACall->nStartStop		=	nStartStop;

	pPACall->nTypePACall	=	CtrlPA;

	//========================================================================
	//	Send RF

#if defined(USE_CH_ISO_DEV)

	if ( GetChPARFT() != 0 )
	{
		//	송신기에 전송.
		SendPktCh( GetChPARFT(), (uint8_t *)&stPkt,
			(U8)sizeof( RFMPktHdr ) + sizeof( RFMPktCtrlPACall ) );
	}

	//	수신기에 전송
	SendPktCh( GetChPA(), (uint8_t *)&stPkt,
		(U8)sizeof( RFMPktHdr ) + sizeof( RFMPktCtrlPACall ) );

#else
	SendPacket( (U8 *)&stPkt, (U8)sizeof( RFMPktHdr ) + sizeof( RFMPktCtrlPACall ) );
#endif

	//========================================================================
}


//========================================================================
void SendCall( int nStartStop )
//========================================================================
{
	printf( "%s(%d)\n", __func__, __LINE__ );

	RFMPkt			stPkt;
	RFMPktCtrlPACall	*pPACall;

	memset( &stPkt, 0, sizeof( stPkt ) );
	pPACall = (RFMPktCtrlPACall *)&stPkt.dat.pacall;

	//========================================================================
	//	Packet Header
#if defined(USE_CH_ISO_DEV)
	_MakePktHdr2( &stPkt, PktCtrlPaCall );
#else
	_MakePktHdr( &stPkt, GetDevID(), 0xFF, sizeof( RFMPktCtrlPACall ), PktCtrlPaCall );
#endif

	//========================================================================
	//	Status Data
	pPACall->nStartStop		=	nStartStop;

	pPACall->nTypePACall	=	CtrlCall;

	//========================================================================
	//	Send RF

#if defined(USE_CH_ISO_DEV)

	if ( GetChPARFT() != 0 )
	{
		//	송신기에 직접 전송.
		SendPktCh( GetChPARFT(), (uint8_t *)&stPkt,
			(U8)sizeof( RFMPktHdr ) + sizeof( RFMPktCtrlPACall ) );
	}
	else
	{
		//	수신기를 통해 전송.
		SendPktCh( GetChPA(),	//GetChOtherRFT(),
				(uint8_t *)&stPkt,
				(U8)sizeof( RFMPktHdr ) + sizeof( RFMPktCtrlPACall ) );
	}

#else

	SendPacket( (U8 *)&stPkt, (U8)sizeof( RFMPktHdr ) + sizeof( RFMPktCtrlPACall ) );

#endif

	//========================================================================
}


//========================================================================
void SendLight( int nOnOff )
//========================================================================
{
	printf( "%s(%d)\n", __func__, __LINE__ );

	RFMPkt			stPkt;
	RFMPktLight		*pLight;

	memset( &stPkt, 0, sizeof( stPkt ) );
	pLight = (RFMPktLight *)&stPkt.dat.light;

	//========================================================================
	//	Packet Header
#if defined(USE_CH_ISO_DEV)

	_MakePktHdr2( &stPkt, PktLight );

#else

	_MakePktHdr( &stPkt, GetDevID(), 0xFF, sizeof( RFMPktLight ), PktLight );

#endif

	//========================================================================
	//	Status Data
	pLight->nOnOff = nOnOff;

	//========================================================================
	//	Send RF
#if defined(USE_CH_ISO_DEV)

	SendPktCh( GetChPA(), (uint8_t *)&stPkt,
		(U8)sizeof( RFMPktHdr ) + sizeof( RFMPktLight ) );

#else

	SendPacket( (U8 *)&stPkt, (U8)sizeof( RFMPktHdr ) + sizeof( RFMPktLight ) );

#endif

	//========================================================================
}

//==========================================================================
void SendLightOn( void )
//==========================================================================
{
	SendLight( 1 );		//	Light On
}

//==========================================================================
void SendLightOff( void )
//==========================================================================
{
	SendLight( 0 );		//	Light Off
}

//==========================================================================
void SendRFCmdCh( int nCh, char *sCmd, int nRSSI )
//==========================================================================
{
	printf( "%s(%d)\n", __func__, __LINE__ );
	//========================================================================
	RFMPkt			stPkt;
	_MakeRFCmd( &stPkt, sCmd, nRSSI );

	//========================================================================
	//	Send RF
	SendPktCh( nCh, (U8 *)&stPkt, (U8)sizeof( RFMPktHdr ) + RFPktDataLen );
	//========================================================================
}

//==========================================================================
void SendRFCmd( char *sCmd, int nRSSI )
//==========================================================================
{
	printf( "%s(%d)\n", __func__, __LINE__ );
	//========================================================================
	RFMPkt			stPkt;
	_MakeRFCmd( &stPkt, sCmd, nRSSI );

	//========================================================================
	//	Send RF
//	SendPacket( (U8 *)&stPkt, (U8)sizeof( RFMPktHdr ) + RFPktDataLen );

	//========================================================================
	//	모든 수신기로 전송.
	for ( int nCh = ChTS1_1; nCh < ChTS1_1 + 10; nCh++ )
	{
		SendPktCh( nCh, (U8 *)&stPkt, (U8)sizeof( RFMPktHdr ) + RFPktDataLen );

		HAL_Delay(3);	//	Tx Delay
	}
	//========================================================================
}

//==========================================================================
void SendRFCmdReset( void )
//==========================================================================
{
	printf( "%s(%d)\n", __func__, __LINE__ );

	SendRFCmd( "reset", 190 );

	//========================================================================
	//	Reset시 Upgrade채널도 reset
	RFMPkt			stPkt;
	_MakeRFCmd( &stPkt, "reset", 190 );
	SendPktCh( ChUpgrDst, (U8 *)&stPkt, (U8)sizeof( RFMPktHdr ) + RFPktDataLen );
	//========================================================================
}

//==========================================================================
void SendRFCmdDFUMode( void )
//==========================================================================
{
	printf( "%s(%d)\n", __func__, __LINE__ );
	//==========================================================================
	SendRFCmd( "stboot", 247 );	//	DFU모드의 경우 아주 근접(RSSI-247)하지 않으면 동작하지 않도록 한다!!!
	//==========================================================================
}

//==========================================================================
void SendRFCmdUpgrade( int bRetry )
//==========================================================================
{
	//==========================================================================
	//	송신기에서 Upgrade 명령 전송.
	//==========================================================================

	printf( "%s(%d)\n", __func__, __LINE__ );
	//==========================================================================
	//	Upgrade Start Command

	if ( bRetry )
	{
		//	Upgrade 채널로 전송
		//	Upgrade Mode로 변경.
		SendRFCmdCh( ChUpgrDst, "upgrade 1", 200 );	//	DFU모드의 경우 근접(RSSI-200)하지 않으면 동작하지 않도록 한다!!!
	}
	else
	{
		//	Upgrade Mode로 변경.
		SendRFCmd( "upgrade 1", 200 );	//	DFU모드의 경우 근접(RSSI-200)하지 않으면 동작하지 않도록 한다!!!
	}
	//==========================================================================

	SetUpgrReTry( bRetry );			//	Retry 설정.

	HAL_Delay( 500 );	//	sleep 200 msec

	//========================================================================
	//	Upgrade Image 전송.
	UpgrSendImageApp();
	//========================================================================

	//==========================================================================
	//	Upgrade End Command
//DEL	SendRFCmd( "upgrade 0", 200 );	//	DFU모드의 경우 근접(RSSI-200)하지 않으면 동작하지 않도록 한다!!!
	//==========================================================================
}

//==========================================================================
void	SendUpgrData		( uint32_t nAddrTarget, int nPktTot, int nPktIdx, uint8_t *sBuf, int nSize )	//	Send Upgrade Data
//==========================================================================
{
//	printf( "%s(%d)\n", __func__, __LINE__ );

	RFMPkt			stPkt;
	memset( &stPkt, 0, sizeof( stPkt ) );

	//========================================================================
	//	Packet Header
	_MakePktHdr( &stPkt, GetDevID(), 0xFF, RFPktDataLen, PktUpgr );

	//========================================================================
	//	Packet Body
	RFMPktUpgr	*pUpgr = (RFMPktUpgr *)&stPkt.dat.upgr;

	pUpgr->baseAddr		=	nAddrTarget;
	pUpgr->totPkt		=	nPktTot;
	pUpgr->idxPkt		=	nPktIdx;
	pUpgr->nSize		=	nSize;

	if( GetUpgrReTry() ) pUpgr->bFlag	|=	PktUpgrFlagRetry;	//	Retry Flag

	memcpy( pUpgr->data, sBuf, nSize );

	//========================================================================
	//	Send RF
	SendPktCh( ChUpgrDst, (U8 *)&stPkt, (U8)sizeof( RFMPktHdr ) + RFPktDataLen );

	//========================================================================
}

//==========================================================================
void	SendUpgrStat		( int nUpgrResult )	//	Send Upgrade Data
//==========================================================================
{
	printf( "%s(%d) - %d\n", __func__, __LINE__, nUpgrResult );

	RFMPkt			stPkt;
	memset( &stPkt, 0, sizeof( stPkt ) );

	//========================================================================
	//	Packet Header
	_MakePktHdr( &stPkt, GetDevID(), 0xFF, RFPktDataLen, PktUpgrStat );

	//========================================================================
	//	Packet Body
	RFMPktUpgrStat	*pUpgrStat = (RFMPktUpgrStat *)&stPkt.dat.upgrStat;

	pUpgrStat->nResult		=	nUpgrResult;
	pUpgrStat->nTrainSet	=	g_idxTrainSet;
	pUpgrStat->nCarNo 		=	g_nCarNo;

	//========================================================================
	//	Send RF
#if defined( USE_CH_ISO_DEV )

	//	송신기#1
	SendPktCh( ChTx_1, (U8 *)&stPkt, (U8)sizeof( RFMPktHdr ) + RFPktDataLen );

	HAL_Delay(3);	//	재전송 전 Delay

	//	송신기#2
	SendPktCh( ChTx_2, (U8 *)&stPkt, (U8)sizeof( RFMPktHdr ) + RFPktDataLen );

#else
	int nCh;
	nCh = ChTS1_1 + g_idxTrainSet * 2 + ((g_nCarNo) % 2);	// 현재 호차 채널
	SendPktCh( nCh, (U8 *)&stPkt, (U8)sizeof( RFMPktHdr ) + RFPktDataLen );

	HAL_Delay(5);	//	재전송 전 Delay

	nCh = ChTS1_1 + g_idxTrainSet * 2 + ((g_nCarNo+1) % 2);	// 현재 호차 채널
	SendPktCh( nCh, (U8 *)&stPkt, (U8)sizeof( RFMPktHdr ) + RFPktDataLen );
#endif

	//========================================================================
}

//==========================================================================
//		Process Packet
//==========================================================================

//========================================================================
int	ProcPktStat			( const RFMPkt *pRFPkt )
//========================================================================
{
	if ( GetDbg() )		printf( "%s(%d)\n", __func__, __LINE__ );

	int nRspID = pRFPkt->dat.stat.nCarNo;
	RFMPktStat *pStat = &pRFPkt->dat.stat;
	//	상태정보 수신.
//		printf ( "[Stat] Car:%d\n", pRFPkt->dat.stat.nCarNo );

	if( nRspID < MaxCarNo	//	MaxCarNo(13)
		&& ( pStat->nDevID == DevRF900M || pStat->nDevID == DevRF900T )
		&& pStat->nMagicNum == 0xAA55
		&& g_bSetRspIDManual == 0		//	수동설정모드가 아닌경우.
		)
	{
		//	장치 응답 Flag 설정.
		SetStat( nRspID );		//	상태정보 설정.

		UpdateStat( pStat );	//	상태정보 Update. ( 버전정보 갱신 등 )
	}

	//========================================================================
	//	송신기 조명상태에 따른 조명 제어.
#if defined(USE_STAT_LIGHT)
	if( pStat->nDevID == DevRF900T )
	{
		//	조명상태 저장 및 제어.
		if( pStat->nDevFlag & DevFlagLight )
		{
			// 조명 On 명령 수신시.
			g_nDevFlag |= DevFlagLight;
			HAL_GPIO_WritePin ( LIGHT_ON_GPIO_Port, LIGHT_ON_Pin, GPIO_PIN_SET );
		}
		else
		{
			// 조명 Off
			g_nDevFlag &= ~DevFlagLight;
			HAL_GPIO_WritePin ( LIGHT_ON_GPIO_Port, LIGHT_ON_Pin, GPIO_PIN_RESET );
		}
	}
#endif
	//========================================================================
}

//========================================================================
int	ProcPktStatReq		( const RFMPkt *pRFPkt )
//========================================================================
{
	if ( GetDbg() )		printf( "%s(%d)\n", __func__, __LINE__ );

	RFMPktStatReq *pStatReq = &pRFPkt->dat.statReq;

	//	수신기의 경우 송신기 상태정보 갱신.
	//		-> 상태정보 요청한 송신기의 상태정보를 갱신한다.
	if ( GetDevID() == DevRF900M )
	{
		int idx = pStatReq->nCarNo;

		//========================================================================
		//	장치 응답 Flag 설정.
		SetStat( idx );		//	상태정보 설정.

		//========================================================================
		//	RSSI 갱신
		g_devStat[idx].nRSSI = g_nRSSI;
	}

	//	Source Channel로 상태정보 송신.
	SendStat( pStatReq->nSrcCh );
}


//========================================================================
int	ProcPktCtrlPaCall	( const RFMPkt *pRFPkt )
//========================================================================
{
	if ( GetDbg() )		printf( "%s(%d)\n", __func__, __LINE__ );

	//	방송/통화 시작 종료 명령.
	RFMPktCtrlPACall	*pCtrl = &pRFPkt->dat.pacall;

	switch ( pCtrl->nStartStop )
	{
	case CtrlStart:
		printf("[Start]");
		SetRFMMode( RFMModeRx );
		break;
	case CtrlStop:
		printf("[Stop]");
		SetRFMMode( RFMModeNormal );
		break;
	default:			printf("%s:Invalid\n", __func__);	return 0;
	}

	switch ( pCtrl->nTypePACall )
	{
	case CtrlPA:		printf("[PA]");						break;
	case CtrlCall:		printf("[Call]");					break;
	default:			printf("%s:Invalid\n", __func__);	return 0;
	}

	printf("\n");

	g_nStampRxPkt = HAL_GetTick();		//	방송/통화 Stamp
}

//========================================================================
int	ProcPktPA			( const RFMPkt *pRFPkt )
//========================================================================
{
	if ( GetDbg() )		printf( "%s(%d)\n", __func__, __LINE__ );

	//========================================================================
	//  방송 ( PTT )
	if ( GetDevID() == DevRF900M )
	{
		//  수신기
		uint16_t	 *pAudioBuf = (uint16_t*)pRFPkt->dat.data;

		//  방송 : 송신기 -> 수신기
		qBufPut( &g_qBufAudioRx, (uint8_t*)pAudioBuf, ( I2S_DMA_LOOP_SIZE * 2 ) );

		// 조명 On
		HAL_GPIO_WritePin ( LIGHT_ON_GPIO_Port, LIGHT_ON_Pin, GPIO_PIN_SET );

		SetRFMMode( RFMModeRx );

		//	Codec Speaker On
		RFM_Spk(1);
		//  수신기 Spk Relay On
		HAL_GPIO_WritePin( AUDIO_ON_GPIO_Port, AUDIO_ON_Pin, GPIO_PIN_SET );
	}
	else
	{
		//========================================================================
		uint16_t	 *pAudioBuf = (uint16_t*)pRFPkt->dat.data;

#if defined(USE_RFT_ONLY_RX_SPK_ON)
		//  송신기 : 수신중인 경우 SPK ON
//			HAL_GPIO_WritePin( SPK_ON_GPIO_Port, SPK_ON_Pin, GPIO_PIN_SET );
		RFM_Spk(1);
#endif

		//  방송 : 송신기 -> 수신기
		qBufPut( &g_qBufAudioRx, (uint8_t*)pAudioBuf, ( I2S_DMA_LOOP_SIZE * 2 ) );
		//========================================================================

		//  송신기
		SetRFMMode( RFMModeRx );

		//  Red LED On
		HAL_GPIO_WritePin ( LED_ON_B_GPIO_Port, LED_ON_B_Pin, GPIO_PIN_SET ); //  RED LED
	}
	g_nStampRxPkt = HAL_GetTick();		//	방송/통화 Stamp
}

//========================================================================
int	ProcPktCall			( const RFMPkt *pRFPkt )
//========================================================================
{
	if ( GetDbg() )		printf( "%s(%d)\n", __func__, __LINE__ );

	if( GetDevID() == DevRF900T )
	{
		//	송신기

		//========================================================================
		//  통화 ( SOS )
		uint16_t	 *pAudioBuf = (uint16_t*)pRFPkt->dat.data;

		SetRFMMode( RFMModeRx );

#if defined(USE_RFT_ONLY_RX_SPK_ON)
		//  송신기 : 수신중인 경우 SPK ON
//		HAL_GPIO_WritePin( SPK_ON_GPIO_Port, SPK_ON_Pin, GPIO_PIN_SET );
		RFM_Spk(1);
#endif

		//  Red LED On
		HAL_GPIO_WritePin ( LED_ON_B_GPIO_Port, LED_ON_B_Pin, GPIO_PIN_SET ); //  RED LED

		//  통화 : 송신기 -> 송신기

		qBufPut( &g_qBufAudioRx, (uint8_t*)pAudioBuf, ( I2S_DMA_LOOP_SIZE * 2 ) );
	}
	else if ( GetDevID() == DevRF900M )
	{
		//	수신기

		SetRFMMode( RFMModeRx );
	}

	g_nStampRxPkt = HAL_GetTick();		//	방송/통화 Stamp
}


//========================================================================
int	ProcPktDevConn			( const RFMPkt *pRFPkt )
//========================================================================
{
	//	Device Node Connection
	if ( GetDbg() )		printf( "%s(%d)\n", __func__, __LINE__ );

	RFMPktDevConn	*pConn = &pRFPkt->dat.devConn;

	if( GetDevID() == DevRF900T )
	{
		//========================================================================
		//	송신기
		SetDevConn( pConn->nSrcCh, pConn->nDevType, pConn->nConnect );
	}
	else if ( GetDevID() == DevRF900M )
	{
		//========================================================================
		//	수신기
		SetDevConn( pConn->nSrcCh, pConn->nDevType, pConn->nConnect );
	}
}

//========================================================================
int	ProcPktLight		( const RFMPkt *pRFPkt )
//========================================================================
{
	if ( GetDbg() )		printf( "%s(%d)\n", __func__, __LINE__ );

	if ( GetDevID() == DevRF900M && pRFPkt->hdr.nPktCmd == PktLight )
	{
		//  수신기 조명제어.
		if ( pRFPkt->dat.light.nOnOff == 0 )
		{
			// 조명 Off 명령 수신시.
			HAL_GPIO_WritePin ( LIGHT_ON_GPIO_Port, LIGHT_ON_Pin, GPIO_PIN_RESET );
		}
		else if ( pRFPkt->dat.light.nOnOff == 1 )
		{
			// 조명 On 명령 수신시.
			HAL_GPIO_WritePin ( LIGHT_ON_GPIO_Port, LIGHT_ON_Pin, GPIO_PIN_SET );
		}
	}
}


//========================================================================
int	ProcPktCmd			( const RFMPkt *pRFPkt )
//========================================================================
{
	if ( GetDbg() )		printf( "%s(%d)\n", __func__, __LINE__ );

	//	RSSI Check
	printf( "%s(%d) - %s\n", __func__, __LINE__, pRFPkt->dat.cmd.sCmd );

	if ( g_nRSSI >= pRFPkt->dat.cmd.nRSSIOver )
	{
		if ( pRFPkt->dat.cmd.nRsp == 1 )
		{
			//	명령 처리 동작 결과 응답.

			//	ToDo
		}

		//	RSSI값 확인 후 해당 범위 내에 있는 경우 명령 동작.
		ProcessCommand( pRFPkt->dat.cmd.sCmd );
	}
}


//========================================================================
int	ProcPktCmdRsp		( const RFMPkt *pRFPkt )
//========================================================================
{
	if ( GetDbg() )		printf( "%s(%d)\n", __func__, __LINE__ );

	//	Command 처리결과 응답.

	//	ToDo
}

//========================================================================
uint8_t	s_bUpgrDataValid[(MaxUpgrDataPacket + 7) / 8];	//	4000];	//	Upgrade Data Valid Check
//========================================================================

//========================================================================
int	ProcPktUpgr			( const RFMPkt *pRFPkt )
//========================================================================
{
	//========================================================================
	//	Upgrade Flash Image
	//========================================================================

	if ( GetDbg() )		printf( "%s(%d)\n", __func__, __LINE__ );

	//========================================================================
	//	Data Flash영역에 Write
	RFMPktUpgr	*pUpgr = &pRFPkt->dat.upgr;

	if ( pUpgr->baseAddr < 0x08080000 || 0x080FFFFF < pUpgr->baseAddr )
	{
		printf("%s(%d) - Out Of Range - baseAddr(0x%08X)\n", __func__, __LINE__, pUpgr->baseAddr );
		return 0;
	}

	//========================================================================
	//	RFM Packet
//	RFMPktUpgr	*pUpgr = (RFMPktUpgr *)pRFPkt->dat.upgr;
//	pUpgr->baseAddr		=	nAddrTarget;
//	pUpgr->totPkt		=	nPktTot;
//	pUpgr->idxPkt		=	nPktIdx;
//	pUpgr->nSize		=	nSize;
//	memcpy( pUpgr->data, sBuf, nSize );

	//========================================================================
	static int	s_rxPkt;

	if ( pUpgr->idxPkt == 0 )
	{
		//	Start Uprade
		printf("%s(%d) - Start Upgrade\n", __func__, __LINE__ );

		if( pUpgr->bFlag & PktUpgrFlagRetry )
		{
			//	No Erase
			printf( "[%08d] Flash Erase - Skip ( Retry - Upgrade )\n", HAL_GetTick() );
		}
		else
		{
			//========================================================================
			//	Flash Erase
	//		 __HAL_RCC_DBGMCU_CLK_ENABLE();
			 __HAL_DBGMCU_FREEZE_IWDG();
	//		MX_IWDG_Disable();		//	Disable Watchdog
			printf( "[%08d] Flash Erase - Start\n", HAL_GetTick() );
	//        FLASH_If_Erase( ADDR_FLASH_IMGAPP );
			FLASH_If_Erase( ADDR_FLASH_IMGBOOT );		//	0x08080000
			printf( "[%08d] Flash Erase - End\n", HAL_GetTick() );
	    	//========================================================================

			memset( s_bUpgrDataValid, 0, sizeof(s_bUpgrDataValid) );
		}

		s_rxPkt = 0;
	}

	s_rxPkt++;

	//========================================================================
	//	Write Upgrade Image Data

	if ( FLASH_If_Write( pUpgr->baseAddr + (pUpgr->idxPkt * PktUpgrDataSize),
						(uint32_t *)pUpgr->data,
						pUpgr->nSize / 4 ) == FLASHIF_OK )
	{
		//	Valid Check Data
		SET_BIT( s_bUpgrDataValid[pUpgr->idxPkt/8], (0x1 << (pUpgr->idxPkt % 8)) );
	}
	else /* An error occurred while writing to Flash memory */
	{
		/* End session */
		printf("%s(%d) - Error idx(%d)\n", __func__, __LINE__, pUpgr->idxPkt );
		CLEAR_BIT( s_bUpgrDataValid[pUpgr->idxPkt/8], (0x1 << (pUpgr->idxPkt % 8)) );	//	Clear
	}

	g_nStampRxPkt = HAL_GetTick();		//	Rx Pkt Stamp
	//========================================================================

	//========================================================================
	if ( pUpgr->idxPkt == (pUpgr->totPkt - 1) )
	{
		//	End Upgrade
//		s_rxPkt++;

		printf("%s(%d) - End Upgrade ( rxPkt : %d / totPkt : %d )\n", __func__, __LINE__,
					s_rxPkt, pUpgr->totPkt );

		int result = UpgrStatSuccess;
		for( int i = 0; i < pUpgr->totPkt; i++ )
		{
			if( READ_BIT( s_bUpgrDataValid[i / 8], (0x1 << (i % 8)) ) == 0 )
			{
				result = UpgrStatFailed;
				break;
			}
		}

		//========================================================================
		if ( result == UpgrStatSuccess )//( s_rxPkt == pUpgr->totPkt )
		{
			//	Upgrade Success

			char buf[10];
			int filesize = pUpgr->totPkt * PktUpgrDataSize;

			printf("%s(%d) - Upgrade Success ( Size : %d )\n", __func__, __LINE__, filesize );

			memset( buf, 0, sizeof( buf ) );

			buf[0] = 0xaa;
			buf[1] = 0x55;
			buf[2] = ( filesize >> 16 ) & 0xFF;
			buf[3] = ( filesize >> 8 ) & 0xFF;
			buf[4] = ( filesize >> 0 ) & 0xFF;

		    M24_HAL_WriteBytes( &hi2c1, 0xA0, AddrEEPUpgrMGN1, (uint8_t *)buf, 5 );

			memset( buf, 0, sizeof( buf ) );

		    M24_HAL_ReadBytes( &hi2c1, 0xA0, AddrEEPUpgrMGN1, (uint8_t *)buf, 5 );

		    filesize = ( buf[2] << 16 ) | ( buf[3] << 8 ) | ( buf[4] );

			printf( "%s(%d) - EEPROM [0,1] : 0x%02X 0x%02X / ( bin size : %d )\n", __func__, __LINE__, buf[0], buf[1], filesize );

			HAL_Delay( 2000 );	//	Upgrade 결과전송 전 Delay

			//===========================================================================
			//	Send Upgrade Result
			SendUpgrStat( UpgrStatSuccess );	//	Upgrade 결과전송.
			HAL_Delay( 100 );	//	Upgrade 결과전송 전 Delay
			SendUpgrStat( UpgrStatSuccess );	//	Upgrade 결과전송.
			HAL_Delay( 100 );	//	Upgrade 결과전송 전 Delay
			SendUpgrStat( UpgrStatSuccess );	//	Upgrade 결과전송.

			//===========================================================================
			//	Reset
			cmd_reset(0, 0);
			//===========================================================================
		}
		else
		{
			HAL_Delay( 2000 );	//	Upgrade 결과전송 전 Delay

			//===========================================================================
			//	Send Upgrade Result
			SendUpgrStat( UpgrStatFailed );		//	Upgrade 결과전송.
			HAL_Delay( 100 );	//	Upgrade 결과전송 전 Delay
			SendUpgrStat( UpgrStatFailed );	//	Upgrade 결과전송.
			HAL_Delay( 100 );	//	Upgrade 결과전송 전 Delay
			SendUpgrStat( UpgrStatFailed );	//	Upgrade 결과전송.

			//	Upgrade Failed
			printf("%s(%d) - Upgrade Failed\n", __func__, __LINE__ );
		}
		//========================================================================

		//========================================================================
		SetRFMMode( RFMModeNormal );	//	Normal 모드로 설정.
		//========================================================================
	}
}


//========================================================================
int	ProcPktUpgrStat			( const RFMPkt *pRFPkt )
//========================================================================
{
	//========================================================================
	//	Upgrade Status
	//========================================================================

	//	Upgrade 결과 수신후 수신기 LCD창에 표시.
	if ( GetDbg() )		printf( "%s(%d)\n", __func__, __LINE__ );

	char sBuf[50];

	//========================================================================
	//	Data Flash영역에 Write
	RFMPktUpgrStat	*pUpgrStat = (RFMPktUpgrStat *)&pRFPkt->dat.upgrStat;

	printf( "%s(%d) - %d\n", __func__, __LINE__, pUpgrStat->nResult );

	if ( pUpgrStat->nResult == UpgrStatSuccess )
	{
		//	송신기 : Upgr Success

		sprintf(sBuf, "Success(%d/%d)", pUpgrStat->nTrainSet, pUpgrStat->nCarNo);
		LCDPrintfXY( 0, 13, sBuf );

//		HAL_Delay(2000);
	}
	else if( pUpgrStat->nResult == UpgrStatFailed )
	{
		//	송신기 : Upgr Failed

		sprintf(sBuf, "Failed(%d/%d)", pUpgrStat->nTrainSet, pUpgrStat->nCarNo);
		LCDPrintfXY( 0, 13, sBuf );

//		HAL_Delay(2000);
	}

	return 1;
}

