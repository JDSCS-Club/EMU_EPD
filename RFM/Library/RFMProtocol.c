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

//#include "diag.h"				//	GetDbgLevel()

#include "Adafruit_SSD1306.h"	//	LCDPrintf()

#include "main.h"				//	FLASH_ON_Pin / GPIO_PIN_SET / ...

#include "ProcPkt.h"			//	SendPacekt()

//==========================================================================
//	Define


int		g_nDevStatus;


//==========================================================================
//	Function


//==========================================================================
//		API
//==========================================================================


//==========================================================================
void	SetStatus				( int nStat )
//==========================================================================
{
	g_nDevStatus = nStat;
}

//==========================================================================
int		GetStatus				( void )
//==========================================================================
{
	return g_nDevStatus;
}


//==========================================================================
//		Send Packet
//==========================================================================

//========================================================================
void _MakePktHdr	( RFMPkt *pPkt, int addrSrc, int addrDest, int nLen, int nPktCmd )
//========================================================================
{
#if	defined(USE_HOPPING)


	if ( nPktCmd == PktStat )
	{
		//	상태정보의 경우 Seq / ID : 0x00
		pPkt->hdr.nSeq			=	0x00;		//	Sequence
		pPkt->hdr.nIDFlag		=	0x00;		//	ID Flag
	}
	else
	{
		g_nPktSeq++;
		if ( g_nPktSeq == 0 )	g_nPktSeq++;
		pPkt->hdr.nSeq			=	g_nPktSeq;			//
		pPkt->hdr.nIDFlag		=	g_flagRspID;		//
	}

#else
	pPkt->hdr.addrSrc		=	addrSrc;		//	Src Address
	pPkt->hdr.addrDest		=	addrDest;		//	Broadcast
	pPkt->hdr.nLen			=	nLen;			//	Length
#endif
	pPkt->hdr.nPktCmd		=	nPktCmd;		//	Status
}


//========================================================================
void SendStat( void )
//========================================================================
{
	//	RF Mode가 RFMModeNormal인 경우.
	//		상태정보 전송.

	if ( GetDbgLevel() > 0 )
	{
		printf( "%s(%d)\n", __func__, __LINE__ );
	}

	RFMPkt		stPkt;
	RFMPktStat	*pStat;

	memset( &stPkt, 0, sizeof( stPkt ) );
	pStat = (RFMPktStat *)&stPkt.dat.stat;

	//========================================================================
	//	Packet Header
	_MakePktHdr( &stPkt, GetDevID(), 0xFF, sizeof( RFMPktStat ), PktStat );

	//========================================================================
	//	Status Data
	pStat->ver_main		=	1;
	pStat->ver_sub		=	0;

	pStat->nCarNo		=	GetCarNo();

	//========================================================================
	//	Send RF
#if 1
	SendPacket( (U8 *)&stPkt, (U8)sizeof( RFMPktHdr ) + sizeof( RFMPktStat ) );

#else
	vRadio_StartTx_Variable_Packet( (unsigned char)pRadioConfiguration->Radio_ChannelNumber, \
									(unsigned char *)&stPkt, \
									pRadioConfiguration->Radio_PacketLength );
#endif
	//64 );
//		(unsigned char)(sizeof( RFMPktHdr ) + sizeof(RFMPktStat)) );

	//========================================================================
}


//========================================================================
void SendPA( int nStartStop )
//========================================================================
{
	printf( "%s(%d)\n", __func__, __LINE__ );

	RFMPkt			stPkt;
	RFMPktPACall	*pPACall;

	memset( &stPkt, 0, sizeof( stPkt ) );
	pPACall = (RFMPktPACall *)&stPkt.dat.pacall;

	//========================================================================
	//	Packet Header
	_MakePktHdr( &stPkt, GetDevID(), 0xFF, sizeof( RFMPktPACall ), PktPA );

	//========================================================================
	//	Status Data
	pPACall->nStartStop	=	nStartStop;

	//========================================================================
	//	Send RF

#if 1
	SendPacket( (U8 *)&stPkt, (U8)sizeof( RFMPktHdr ) + sizeof( RFMPktPACall ) );

#else
	vRadio_StartTx_Variable_Packet ( (U8)pRadioConfiguration->Radio_ChannelNumber,
		(U8 *)&stPkt,
		(U8)sizeof( RFMPktHdr ) + sizeof( RFMPktPACall ) );// pRadioConfiguration->Radio_PacketLength );
#endif

	//========================================================================
}


//========================================================================
void SendCall( int nStartStop )
//========================================================================
{
	printf( "%s(%d)\n", __func__, __LINE__ );

	RFMPkt			stPkt;
	RFMPktPACall	*pPACall;

	memset( &stPkt, 0, sizeof( stPkt ) );
	pPACall = (RFMPktPACall *)&stPkt.dat.pacall;

	//========================================================================
	//	Packet Header
	_MakePktHdr( &stPkt, GetDevID(), 0xFF, sizeof( RFMPktPACall ), PktCall );

	//========================================================================
	//	Status Data
	pPACall->nStartStop = nStartStop;

	//========================================================================
	//	Send RF

#if 1
	SendPacket( (U8 *)&stPkt, (U8)sizeof( RFMPktHdr ) + sizeof( RFMPktPACall ) );
#else
	vRadio_StartTx_Variable_Packet ( (U8)pRadioConfiguration->Radio_ChannelNumber,
		(U8 *)&stPkt,
		(U8)(sizeof( RFMPktHdr ) + sizeof( RFMPktPACall )) );// pRadioConfiguration->Radio_PacketLength );
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
	_MakePktHdr( &stPkt, GetDevID(), 0xFF, sizeof( RFMPktLight ), PktLight );

	//========================================================================
	//	Status Data
	pLight->nOnOff = nOnOff;

	//========================================================================
	//	Send RF
#if 1
	SendPacket( (U8 *)&stPkt, (U8)sizeof( RFMPktHdr ) + sizeof( RFMPktLight ) );
#else

	vRadio_StartTx_Variable_Packet ( (U8)pRadioConfiguration->Radio_ChannelNumber,
		(U8 *)&stPkt,
		(U8)sizeof( RFMPktHdr ) + sizeof( RFMPktLight ) );// pRadioConfiguration->Radio_PacketLength );
#endif

	//========================================================================
}


//==========================================================================
void SendLightOn( void )
//==========================================================================
{
	RFMPkt			stPkt;

	memset( &stPkt, 0, sizeof( stPkt ) );

	_MakePktHdr( &stPkt, GetDevID(), DevRF900M, sizeof( RFMPktLight ), PktLightOn );

#if 1
	SendPacket( (U8 *)&stPkt, (U8)sizeof( RFMPktHdr ) + sizeof( RFMPktLight ) );
#else
	vRadio_StartTx_Variable_Packet ( (U8)pRadioConfiguration->Radio_ChannelNumber,
		(U8 *)&stPkt,
//		(U8)sizeof( RFMPktHdr ) + sizeof( RFMPktLight ) );// 
		pRadioConfiguration->Radio_PacketLength );
#endif
}

//==========================================================================
void SendLightOff( void )
//==========================================================================
{
	RFMPkt			stPkt;

	memset( &stPkt, 0, sizeof( stPkt ) );

	_MakePktHdr( &stPkt, GetDevID(), DevRF900M, sizeof( RFMPktLight ), PktLightOff );

#if 1
	SendPacket( (U8 *)&stPkt, (U8)sizeof( RFMPktHdr ) + sizeof( RFMPktLight ) );
#else
	vRadio_StartTx_Variable_Packet ( (U8)pRadioConfiguration->Radio_ChannelNumber,
		(U8 *)&stPkt,
//		(U8)sizeof( RFMPktHdr ) + sizeof( RFMPktLight ) );// 
		pRadioConfiguration->Radio_PacketLength );
#endif
}

//==========================================================================
//		Process Packet
//==========================================================================

//========================================================================
int	ProcPktStat			( const RFMPkt *pPkt )
//========================================================================
{
	if ( GetDbgLevel() > 0 )
		printf( "%s(%d)\n", __func__, __LINE__ );
}

//========================================================================
int	ProcPktPA			( const RFMPkt *pPkt )
//========================================================================
{
	if ( GetDbgLevel() > 0 )
		printf( "%s(%d)\n", __func__, __LINE__ );

	//========================================================================
	const RFMPktHdr		*pHdr = (const RFMPktHdr *)&pPkt->hdr;
	const RFMPktPACall	*pPACall = (const RFMPktPACall *)&pPkt->dat.pacall;
	//========================================================================

	//========================================================================
	//	Packet Parsing
	if ( GetDbgLevel() > 0 )
	{
		printf( "%s(%d) - OnOff( %d )\n", __func__, __LINE__, pPACall->nStartStop );
	}

	if ( pPACall->nStartStop )
	{
		//	수신중
		LCDSetCursor( 20, 13 );
		LCDPrintf( "수신중..." );
	}
	else
	{
		//    편성 : 100
		LCDSetCursor( 20, 13 );
		LCDPrintf( "편성 : 100" );
	}
}

//========================================================================
int	ProcPktCall			( const RFMPkt *pPkt )
//========================================================================
{
	if ( GetDbgLevel() > 0 )
		printf( "%s(%d)\n", __func__, __LINE__ );

	//========================================================================
	const RFMPktHdr		*pHdr = (const RFMPktHdr *)&pPkt->hdr;
	const RFMPktPACall	*pPACall = (const RFMPktPACall *)&pPkt->dat.pacall;
	//========================================================================

	//========================================================================
	//	Packet Parsing
	if ( GetDbgLevel() > 0 )
	{
		printf( "%s(%d) - OnOff( %d )\n", __func__, __LINE__,
			pPACall->nStartStop
		);
	}

	if ( pPACall->nStartStop )
	{
		//	수신중
		LCDSetCursor( 20, 13 );
		LCDPrintf( "수신중..." );
	}
	else
	{
		//    편성 : 100
		LCDSetCursor( 20, 13 );
		LCDPrintf( "편성 : 100" );
	}
}

//========================================================================
int	ProcPktLight		( const RFMPkt *pPkt )
//========================================================================
{
	if ( GetDbgLevel() > 0 )
		printf( "%s(%d)\n", __func__, __LINE__ );

	//========================================================================
	const RFMPktHdr		*pHdr	= (const RFMPktHdr *)&pPkt->hdr;
	const RFMPktLight	*pLight = (const RFMPktLight *)&pPkt->dat.light;
	//========================================================================

	//========================================================================
	//	Packet Parsing
	if ( GetDbgLevel() > 0 )
	{
		printf( "%s(%d) - OnOff( %d )\n", __func__, __LINE__,
			pLight->nOnOff
		);
	}

	if ( pLight->nOnOff )
	{
		//	송신기 Flash - On
		HAL_GPIO_WritePin( FLASH_ON_GPIO_Port, FLASH_ON_Pin, GPIO_PIN_SET );

		//	수신기 Light(조명) - On
		HAL_GPIO_WritePin( LIGHT_ON_GPIO_Port, LIGHT_ON_Pin, GPIO_PIN_SET );
	}
	else
	{
		//	송신기 Flash - Off
		HAL_GPIO_WritePin( FLASH_ON_GPIO_Port, FLASH_ON_Pin, GPIO_PIN_RESET );

		//	수신기 Light(조명) - Off
		HAL_GPIO_WritePin( LIGHT_ON_GPIO_Port, LIGHT_ON_Pin, GPIO_PIN_RESET );
	}
}

//========================================================================
int	ProcPktAudioPA		( const RFMPkt *pPkt )
//========================================================================
{
	if ( GetDbgLevel() > 0 )
		printf( "%s(%d)\n", __func__, __LINE__ );
}

//========================================================================
int	ProcPktAudioCall	( const RFMPkt *pPkt )
//========================================================================
{
	if ( GetDbgLevel() > 0 )
		printf( "%s(%d)\n", __func__, __LINE__ );
}


//========================================================================
void ProcessPkt			( const uint8_t *pbuf, int length )
//========================================================================
{
	if ( GetDbgLevel() > 0 )
		printf( "%s(%d)\n", __func__, __LINE__ );

	//========================================================================
	const RFMPkt	*pPkt = (const RFMPkt *)pbuf;
	const RFMPktHdr	*pHdr = (const RFMPktHdr *)&pPkt->hdr;
	//========================================================================

	//========================================================================
	//	Packet Parsing
	if ( GetDbgLevel() > 0 )
	{
#if defined(USE_HOPPING)
		printf( "%s(%d) - ID( 0x%02X ) / Seq ( %d ) / PktCmd ( %d )\n", __func__, __LINE__,
			pHdr->nIDFlag, pHdr->nSeq, pHdr->nPktCmd
		);
#else
		printf( "%s(%d) - Src Addr( 0x%02X ) / Dest Addr( 0x%02X ) / Len ( %d ) / PktCmd ( %d )\n", __func__, __LINE__,
			pHdr->addrSrc, pHdr->addrDest, pHdr->nLen, pHdr->nPktCmd
		);
#endif
	}

	switch ( pHdr->nPktCmd )
	{
	case PktStat:		ProcPktStat			( pPkt );		break;
	case PktPA:			ProcPktPA			( pPkt );		break;
	case PktCall:		ProcPktCall			( pPkt );		break;
	case PktLight:		ProcPktLight		( pPkt );		break;
	case PktAudioPA:	ProcPktAudioPA		( pPkt );		break;
	case PktAudioCall:	ProcPktAudioCall	( pPkt );		break;
	default:
		printf( "%s(%d) - Invalid Value(%d)\n", __func__, __LINE__, pHdr->nPktCmd );
		break;
	}
}


