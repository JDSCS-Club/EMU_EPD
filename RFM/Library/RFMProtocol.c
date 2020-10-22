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

	if ( nPktCmd == PktStat
		|| nPktCmd == PktCmd
		|| nPktCmd == PktUpgr
		)
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
	pStat->ver_main		=	APP_MAIN_VER;		//	1;
	pStat->ver_sub		=	APP_SUB_VER;		//	0;
	pStat->ver_det		=	APP_DETAIL_VER;		//	0;
	pStat->ver_build	=	APP_BUILD_VER;		//	0;

	pStat->nMagicNum	=	0xAA55;				//	패킷 요류검출용.

	pStat->nCarNo		=	GetCarNo();
	pStat->nDevID		=	GetDevID();

	pStat->upTime		=	HAL_GetTick();

	pStat->rspID		=	g_flagRspID;		//	Rsp ID Flag

	pStat->nManHop		=	g_nManHopping;		//	Manual Hopping Setting

	pStat->nDevFlag		=	g_nDevFlag;			//	Device Flag : 조명 상태 등.

	//========================================================================
	//	Send RF
	SendPacket( (U8 *)&stPkt, (U8)sizeof( RFMPktHdr ) + sizeof( RFMPktStat ) );
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

	SendPacket( (U8 *)&stPkt, (U8)sizeof( RFMPktHdr ) + sizeof( RFMPktPACall ) );

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

	SendPacket( (U8 *)&stPkt, (U8)sizeof( RFMPktHdr ) + sizeof( RFMPktPACall ) );

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
	SendPacket( (U8 *)&stPkt, (U8)sizeof( RFMPktHdr ) + sizeof( RFMPktLight ) );

	//========================================================================
}

//==========================================================================
void SendLightOn( void )
//==========================================================================
{
#if OLD
	RFMPkt			stPkt;

	memset( &stPkt, 0, sizeof( stPkt ) );

	_MakePktHdr( &stPkt, GetDevID(), DevRF900M, sizeof( RFMPktLight ), PktLightOn );

	SendPacket( (U8 *)&stPkt, (U8)sizeof( RFMPktHdr ) + sizeof( RFMPktLight ) );
#else
	SendLight( 1 );		//	Light On
#endif
}

//==========================================================================
void SendLightOff( void )
//==========================================================================
{
#if OLD
	RFMPkt			stPkt;

	memset( &stPkt, 0, sizeof( stPkt ) );

	_MakePktHdr( &stPkt, GetDevID(), DevRF900M, sizeof( RFMPktLight ), PktLightOff );

	SendPacket( (U8 *)&stPkt, (U8)sizeof( RFMPktHdr ) + sizeof( RFMPktLight ) );
#else
	SendLight( 0 );		//	Light Off
#endif
}

//==========================================================================
void SendRFCmd( char *sCmd, int nRSSI )
//==========================================================================
{
	printf( "%s(%d)\n", __func__, __LINE__ );

	RFMPkt			stPkt;
	memset( &stPkt, 0, sizeof( stPkt ) );
//	pLight = (RFMPktLight *)&stPkt.dat.light;

	//========================================================================
	//	Packet Header
	_MakePktHdr( &stPkt, GetDevID(), 0xFF, RFPktDataLen, PktCmd );

	//========================================================================
	//	Command
	stPkt.dat.cmd.nRSSIOver = nRSSI;		//	명령 동작 RSSI 범위.
	strcpy( stPkt.dat.cmd.sCmd, sCmd );		//	명령 전송.

	//========================================================================
	//	Send RF
	SendPacket( (U8 *)&stPkt, (U8)sizeof( RFMPktHdr ) + RFPktDataLen );

	//========================================================================
}

//==========================================================================
void SendRFCmdReset( void )
//==========================================================================
{
	printf( "%s(%d)\n", __func__, __LINE__ );
	SendRFCmd( "reset", 190 );
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
void SendRFCmdUpgrade( void )
//==========================================================================
{
	//==========================================================================
	//	송신기에서 Upgrade 명령 전송.
	//==========================================================================

	printf( "%s(%d)\n", __func__, __LINE__ );
	//==========================================================================
	//	Upgrade Start Command
	SendRFCmd( "upgrade 1", 200 );	//	DFU모드의 경우 근접(RSSI-200)하지 않으면 동작하지 않도록 한다!!!
	//==========================================================================

	HAL_Delay( 500 );		//	sleep 200 msec

	//========================================================================
	//	Upgrade Image 전송.
	UpgrSendImageApp();
	//========================================================================

	//==========================================================================
	//	Upgrade End Command
	SendRFCmd( "upgrade 0", 200 );	//	DFU모드의 경우 근접(RSSI-200)하지 않으면 동작하지 않도록 한다!!!
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
	memcpy( pUpgr->data, sBuf, nSize );

	//========================================================================
	//	Send RF
	SendPktCh( ChUpgrade, (U8 *)&stPkt, (U8)sizeof( RFMPktHdr ) + RFPktDataLen );

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
	int nCh = ChTS1_1 + g_idxTrainSet * 2 + ((g_nCarNo) % 2);	// 현재 호차 채널
	SendPktCh( nCh, (U8 *)&stPkt, (U8)sizeof( RFMPktHdr ) + RFPktDataLen );

	HAL_Delay(5);	//	재전송 전 Delay

	nCh = ChTS1_1 + g_idxTrainSet * 2 + ((g_nCarNo+1) % 2);	// 현재 호차 채널
	SendPktCh( nCh, (U8 *)&stPkt, (U8)sizeof( RFMPktHdr ) + RFPktDataLen );

	//========================================================================
}

//==========================================================================
//		Process Packet
//==========================================================================

//========================================================================
int	ProcPktStat			( const RFMPkt *pRFPkt )
//========================================================================
{
	if ( GetDbgLevel() > 0 )
		printf( "%s(%d)\n", __func__, __LINE__ );

	int nRspID = pRFPkt->dat.stat.nCarNo;
	RFMPktStat *pStat = &pRFPkt->dat.stat;
	//	상태정보 수신.
//		printf ( "[Stat] Car:%d\n", pRFPkt->dat.stat.nCarNo );

	if( nRspID < 16
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
int	ProcPktPA			( const RFMPkt *pRFPkt )
//========================================================================
{
	if ( GetDbgLevel() > 0 )
		printf( "%s(%d)\n", __func__, __LINE__ );

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
	if ( GetDbgLevel() > 0 )
		printf( "%s(%d)\n", __func__, __LINE__ );

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
int	ProcPktLight		( const RFMPkt *pRFPkt )
//========================================================================
{
	if ( GetDbgLevel() > 0 )
		printf( "%s(%d)\n", __func__, __LINE__ );

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
	if ( GetDbgLevel() > 0 )
		printf( "%s(%d)\n", __func__, __LINE__ );

	//	RSSI Check

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
	if ( GetDbgLevel() > 0 )
		printf( "%s(%d)\n", __func__, __LINE__ );

	//	Command 처리결과 응답.

	//	ToDo
}


//========================================================================
int	ProcPktUpgr			( const RFMPkt *pRFPkt )
//========================================================================
{
	//========================================================================
	//	Upgrade Flash Image
	//========================================================================

	if ( GetDbgLevel() > 0 )
		printf( "%s(%d)\n", __func__, __LINE__ );

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

		s_rxPkt = 1;
	}
	else if ( pUpgr->idxPkt == ( pUpgr->totPkt - 1 ) )
	{
		//	End Upgrade
		s_rxPkt++;

		printf("%s(%d) - End Upgrade ( rxPkt : %d / totPkt : %d )\n", __func__, __LINE__,
					s_rxPkt, pUpgr->totPkt );

		//========================================================================
		if ( s_rxPkt == pUpgr->totPkt )
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
	else
	{
		s_rxPkt++;
	}

	//========================================================================
	//	Write Upgrade Image Data

	//    if ( FLASH_If_Write( flashdestination, (uint32_t*)ramsource, packet_length / 4 ) == FLASHIF_OK )
	if ( FLASH_If_Write( pUpgr->baseAddr + (pUpgr->idxPkt * PktUpgrDataSize),
						(uint32_t *)pUpgr->data,
						pUpgr->nSize / 4 ) == FLASHIF_OK )
	{
//        flashdestination += packet_length;
	}
	else /* An error occurred while writing to Flash memory */
	{
		/* End session */
		printf("%s(%d) - Error idx(%d)\n", __func__, __LINE__, pUpgr->idxPkt );
	}


	g_nStampRxPkt = HAL_GetTick();		//	Rx Pkt Stamp
	//========================================================================
}


//========================================================================
int	ProcPktUpgrStat			( const RFMPkt *pRFPkt )
//========================================================================
{
	//========================================================================
	//	Upgrade Status
	//========================================================================

	//	Upgrade 결과 수신후 수신기 LCD창에 표시.
	if ( GetDbgLevel() > 0 )
		printf( "%s(%d)\n", __func__, __LINE__ );

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



#if OLD
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
	default:
		printf( "%s(%d) - Invalid Value(%d)\n", __func__, __LINE__, pHdr->nPktCmd );
		break;
	}
}

#endif

