/*! @file main.c
 * @brief The main.c file of the 802.15.4g bidirectional
 * for Si446X devices.
 *
 * Contains the initialization of the MCU & the radio.
 * @n The main loop controls the program flow & the radio.
 *
 * @b CREATED
 * @n Silicon Laboratories Ltd.
 *
 * @b COPYRIGHT
 * @n Silicon Laboratories Confidential
 * @n Copyright 2014 Silicon Laboratories, Inc.
 * @n http://www.silabs.com
 *
 */

//#include "..\bsp.h"

#include "typedef.h"			//	uint32_t, ...

#include "compiler_defs.h"		//	U8,
#include "si446x_defs.h"

#include "radio.h"				//	pRadioConfiguration

#include "ProcPkt.h"			//	TestProcPkt

#include "main.h"

#include "RFMProtocol.h"		//	RFMPkt
#include "rfm.h"				//	eRFMMode
#include "audio.h"				//	I2S_DMA_LOOP_SIZE

/*------------------------------------------------------------------------*/
/*                          Global variables                              */
/*------------------------------------------------------------------------*/
SEGMENT_VARIABLE(bMain_IT_Status, U8, SEG_XDATA);

int nTxPkt = 0;
int nRxPkt = 0;

int nHopPkt = 0;		//	Hopping Packet Count
int nDropPkt = 0;		//	Drop Packet Count ( 처리된 Packet을 다시 받는 경우. )

int nRxErr = 0;			//	Error Packet Count
int nCrcErr = 0;

int nTxStamp = 0;
int nRxStamp = 0;

//========================================================================
int			g_bSetRspIDManual	=	0;				//  RspID Flag 수동설정. ( 디버깅용 )
//========================================================================

uint16_t	g_flagRspID 	=	0x00;				//  범위 안의 Device ID Flag ( 0 ~ 15 bit )
uint8_t	 	g_nPktSeq 		=	0x00;				//  Packet Sequence

static int	s_bShowPkt		=	DEFAULT_EN_SHOW_PKT;	//	1;

/*------------------------------------------------------------------------*/
/*                              Defines                                   */
/*------------------------------------------------------------------------*/
//#define PACKET_SEND_INTERVAL 2000u
//#define PACKET_SEND_INTERVAL 500u
#define PACKET_SEND_INTERVAL		50u

#define PHR_CRC16_DW_DIS			0x10
#define PHR_CRC16_DW_EN				0x18
#define PHR_CRC32_DW_DIS			0x00
#define PHR_CRC32_DW_EN				0x08

#define MOD_TYPE_2GFSK				0x03
#define MOD_TYPE_4GFSK				0x05

#define COMPARE_PHR_AND_PAYLOAD_FULL			0
#define COMPARE_PHR_LENGTH_AND_PAYLOAD			1

#define RADIO_CONFIGURATION_DATA_ACK_MAC_PAYLOAD_LENGTH     0x0003
#define RADIO_CONFIGURATION_DATA_ACK_PAYLOAD				{ 0x00, 0x00, 0x41, 0x43, 0x4B}

/*------------------------------------------------------------------------*/
/*                             Enumeration                                */
/*------------------------------------------------------------------------*/

/*------------------------------------------------------------------------*/
/*                             Typedefs                                   */
/*------------------------------------------------------------------------*/

/*------------------------------------------------------------------------*/
/*                          Local variables                               */
/*------------------------------------------------------------------------*/

SEGMENT_VARIABLE(bPropValue1, U8, SEG_XDATA);
SEGMENT_VARIABLE(bPropValue2, U8, SEG_XDATA);
SEGMENT_VARIABLE(bModulationType, U8, SEG_XDATA);
SEGMENT_VARIABLE(bPktConfig1ForRx, U8, SEG_XDATA);

/*------------------------------------------------------------------------*/
/*                      Local function prototypes                         */
/*------------------------------------------------------------------------*/

// Reverse bit order
U8 bBitOrderReverse(U8 bByteToReverse);

// Extracts PHY payload length from PHR
U16 wPayloadLenghtFromPhr(U8* pbPhrMsb);


/*------------------------------------------------------------------------*/
/*                          Function implementations                      */
/*------------------------------------------------------------------------*/

//========================================================================
void Dump( const char *sTitle, const char *sBuf, int nSize )
//========================================================================
{
	if ( GetDbg() < 2 )	return;

	printf( "%s : ", sTitle );

	int i;

	for( i = 0; i < nSize; i++ )
	{
		printf("%02X ", sBuf[i]);
	}

	printf("\n");
}


//========================================================================
void TestProcPkt(void)
//========================================================================
{
	InitProcPkt();

	static uint32_t s_nTick;

	uint32_t currTick;

	s_nTick = HAL_GetTick();

	while (TRUE)
	{
		currTick = HAL_GetTick();

		// Demo Application Poll-Handler function
		LoopProcPkt( currTick );
	}
}

//========================================================================
int	InitProcPkt ( void )
//========================================================================
{

#if defined( USE_IEEE802_15_4G )

	// Find out wheather it is 2GFSK or 4GFSK. PKT_CONFIG1 will have to be configured accordingly
	bRadio_FindProperty(pRadioConfiguration->Radio_ConfigurationArray, SI446X_PROP_GRP_ID_MODEM, SI446X_PROP_GRP_INDEX_MODEM_MOD_TYPE, &bModulationType);

	// Configure PH field split, CRC endian, bit order for RX
	if (bModulationType == MOD_TYPE_2GFSK)
	{
		bPktConfig1ForRx = SI446X_PROP_PKT_CONFIG1_PH_FIELD_SPLIT_BIT | SI446X_PROP_PKT_CONFIG1_CRC_ENDIAN_BIT | SI446X_PROP_PKT_CONFIG1_BIT_ORDER_BIT;
	}
	else if (bModulationType == MOD_TYPE_4GFSK)
	{
		bPktConfig1ForRx = SI446X_PROP_PKT_CONFIG1_PH_FIELD_SPLIT_BIT | SI446X_PROP_PKT_CONFIG1_4FSK_EN_BIT | SI446X_PROP_PKT_CONFIG1_CRC_ENDIAN_BIT | SI446X_PROP_PKT_CONFIG1_BIT_ORDER_BIT;
	}

	// Configure PKT_CONFIG1 for RX
	si446x_set_property(SI446X_PROP_GRP_ID_PKT, 1, SI446X_PROP_GRP_INDEX_PKT_CONFIG1, bPktConfig1ForRx);
	// Start RX with Packet handler settings
	vRadio_StartRX(pRadioConfiguration->Radio_ChannelNumber,0u);

#else

	//  RF 수신 Start
	g_nChRx = GetChRx();	//	ChTS1_1 + g_idxTrainSet * 2 + ((g_nCarNo+1) % 2);	// 현재 호차 채널

	vRadio_StartRX(
		g_nChRx,	//g_idxTrainSet,	//		pRadioConfiguration->Radio_ChannelNumber,
		pRadioConfiguration->Radio_PacketLength );

#endif

	return TRUE;
}

//========================================================================
int _ChkDropPktSeq( uint8_t _nRxSeq, uint8_t _currSeq )
//========================================================================
{
	//	현재 받은 Packet Sequence가 새로운 패킷인지 확인.
	//		-> 이전 Packet인 경우 Drop

	if ( _nRxSeq == _currSeq )		//	Seq가 같은 Packet 수신시 Drop
	{
		return 1;	//	Pkt Drop
	}

	//	Rx Packet이 currPkt보다 1크면 처리.
	uint8_t currSeq = _currSeq;
	if ( ++currSeq == 0 )	currSeq++;
	if ( _nRxSeq == currSeq )		//	Seq가 같은 Packet 수신시 Drop
	{
		return 0;	//	Valid Pkt
	}

	//	현재 패킷보다 이전에 받은 5개 패킷은 Drop
	for( int i = 0; i < 5; i++ )
	{
		if ( ++_nRxSeq == 0 )	_nRxSeq++;
		if ( _nRxSeq == _currSeq )		//	Seq가 같은 Packet 수신시 Drop
		{
			return 1;	//	Pkt Drop
		}
	}

	return 0;	//	Valid Pkt
}

//========================================================================
void CallbackRecvPacket( const char *pData, int nSize )
//========================================================================
{
	const RFMPkt	*pRFPkt = (const RFMPkt *)pData;

#if defined(USE_HOPPING)

	//========================================================================
	//	Header ID
	switch( pRFPkt->hdr.bHdrID )
	{
	case HdrID1:		//	Header ID #1
//		if(GetDbg()) printf("H1");
		break;
	case HdrID2:		//	Header ID #2
		if(GetDbg()) printf("H2");
		break;
	}

	//========================================================================
	//	Packet Filtering
	//		- Pkt 처리 여부 확인.
	if	(	pRFPkt->hdr.nSeq != 0 &&
			(
//				(pRFPkt->hdr.nSeq == g_nPktSeq)		//	Seq가 같은 Packet 수신시 Drop
				_ChkDropPktSeq(pRFPkt->hdr.nSeq, g_nPktSeq)		//	Seq가 같은 Packet 수신시 Drop
				|| GetRFMMode() == RFMModeTx		//	송신모드에서는 Packet Drop
			)
		)
	{
		//	이미 처리된 Packet Skip.
		nDropPkt++;
		return ;
	}

	//========================================================================
	//	Hopping
	uint16_t flagID = g_flagRspID &	(~(0x1 << GetCarNo()));		//	자신의 ID Flag를 제외한 값.

#if defined(USE_HOP_MANUAL)
	if ( ( pRFPkt->hdr.nSeq != 0 && pRFPkt->hdr.nIDFlag != 0
				&& (GetDevID() == DevRF900M) )		//	수신기만 중계함.
#if defined(USE_HOP_FORCE)
			//========================================================================
			//	강제 중계 설정.
			//========================================================================
#else
			&& ( ( ( (g_nManHopping == 0) && (((~pRFPkt->hdr.nIDFlag)&flagID) != 0) )	//	Default
				|| ( g_nManHopping == 1 ) )		//	Hopping On
				&& !( g_nManHopping == 2 ) )	//	Hopping Off
#endif
		)
#else
	if ( pRFPkt->hdr.nSeq != 0 && pRFPkt->hdr.nIDFlag != 0
			&& ( ( (~pRFPkt->hdr.nIDFlag) & flagID ) != 0 )
		)
#endif
	{
		//	전송 범위 밖의 Device가 수신된 경우.
		//	Rsp Flag 설정 후에 전송.
		nHopPkt++;
		char buf[64];
		memcpy( buf, pData, 64 );
		RFMPkt	*pSendPkt = (RFMPkt *)buf;
		pSendPkt->hdr.nIDFlag |= g_flagRspID;

#if defined(USE_CH_ISO_DEV)

		//	수신채널 분리.
		int nCh = GetChRx() + 1;	//	Test : Hopping 시 Rx + 1 Channel로 전송.

		//	1 - 2 - 3 - 4 - 5 - 6
		//	  <- ->	2번 수신시 1, 3으로 전송.
		//

		//==========================================================================
		//	Tx #1
		SendPktCh( nCh, buf, nSize );

		//==========================================================================
		//	Tx #2
		HAL_Delay(2);		//	2 msec
		SendPktCh( nCh + 10, buf, nSize );
		//==========================================================================

#elif defined(USE_HOP_CH)

		int nCh = ChTS1_1 + g_idxTrainSet * 2 + ( (g_nCarNo) % 2);	//	타채널
		SendPktCh( nCh, buf, nSize );

#else

		SendPacket( buf, nSize );

#endif
		//
	}

	if ( pRFPkt->hdr.nSeq != 0 )
	{
		//	Seq No. 가 0이 아닌경우 Seq 갱신.
		g_nPktSeq = pRFPkt->hdr.nSeq;	//	Packet Seq 갱신.
	}

	//	Device ID Flag 확인.

#endif	//	defined(USE_HOPPING)
	//========================================================================

	switch ( pRFPkt->hdr.nPktCmd )
	{
	case PktCall:		ProcPktCall		( pRFPkt );		break;
	case PktPA:			ProcPktPA		( pRFPkt );		break;
	case PktStat:		ProcPktStat		( pRFPkt );		break;
	case PktStatReq:	ProcPktStatReq	( pRFPkt );		break;
	case PktLight:		ProcPktLight	( pRFPkt );		break;
	case PktCmd:		ProcPktCmd		( pRFPkt );		break;
	case PktUpgr:		ProcPktUpgr		( pRFPkt );		break;
	case PktUpgrStat:	ProcPktUpgrStat	( pRFPkt );		break;
	default:
//		printf( "%s(%d) - Invalid Value(%d)\n", __func__, __LINE__, pRFPkt->hdr.nPktCmd );
		printf( "E\n" );	//	Packet Error
		break;
	}
}

/**
 *  Demo Application Poll-Handler
 *
 *  @note This function must be called periodically.
 *
 */
//========================================================================
void LoopProcPkt( int nTick )
//========================================================================
{
	bMain_IT_Status = bRadio_Check_Tx_RX();

#if defined( USE_IEEE802_15_4G )

#if OLD


	switch (bMain_IT_Status)
	{
	case SI446X_CMD_GET_INT_STATUS_REP_PH_PEND_PACKET_SENT_PEND_BIT:

		// Configure PKT_CONFIG1 for RX
		si446x_set_property(SI446X_PROP_GRP_ID_PKT, 1, SI446X_PROP_GRP_INDEX_PKT_CONFIG1, bPktConfig1ForRx);
		// Start RX with Packet handler settings
		vRadio_StartRX(pRadioConfiguration->Radio_ChannelNumber,
			pRadioConfiguration->Radio_PacketLength);

		HAL_GPIO_TogglePin ( LED_ST_GPIO_Port, LED_ST_Pin );

		nTxPkt++;
		nTxStamp = HAL_GetTick();

		// Custom message sent successfully
		if ( nTxPkt % 250 == 0 )
		{
			printf ( "T" );
		}

		break;

	case SI446X_CMD_GET_INT_STATUS_REP_PH_PEND_PACKET_RX_PEND_BIT:

		HAL_GPIO_TogglePin ( LED_ST_GPIO_Port, LED_ST_Pin );

		// PHR was put in the RX FIFO in an LSB order, Payload in an MSB order. Store both in MSB in customRadioPacket[]
		customRadioPacket[0u] = bBitOrderReverse(customRadioPacket[0u]);
		customRadioPacket[1u] = bBitOrderReverse(customRadioPacket[1u]);

		nRxPkt++;
		nRxStamp = HAL_GetTick();

		Dump("Rx", customRadioPacket, 0x40);
		if ( nRxPkt % 250 == 0 )
		{
			printf ( "R" );
		}

//		CallbackRecvPacket( customRadioPacket, 0x40 );
		CallbackRecvPacket( &customRadioPacket[2], (0x40 - 2) );

		// Configure PKT_CONFIG1 for RX
		si446x_set_property(SI446X_PROP_GRP_ID_PKT, 1, SI446X_PROP_GRP_INDEX_PKT_CONFIG1, bPktConfig1ForRx);
		// Start RX with Packet handler settings
		vRadio_StartRX(pRadioConfiguration->Radio_ChannelNumber,
			pRadioConfiguration->Radio_PacketLength);

		break;

	default:
		break;
	} /* switch */

#else

	if( bMain_IT_Status & SI446X_CMD_GET_INT_STATUS_REP_PH_PEND_PACKET_RX_PEND_BIT )
	{
		HAL_GPIO_TogglePin ( LED_ST_GPIO_Port, LED_ST_Pin );

		// PHR was put in the RX FIFO in an LSB order, Payload in an MSB order. Store both in MSB in customRadioPacket[]
		g_pRadioRxPkt[0u] = bBitOrderReverse(g_pRadioRxPkt[0u]);
		g_pRadioRxPkt[1u] = bBitOrderReverse(g_pRadioRxPkt[1u]);

		nRxPkt++;
		nRxStamp = HAL_GetTick();

		Dump("Rx", g_pRadioRxPkt, 0x40);
		if ( nRxPkt % 250 == 0 )
		{
			printf ( "R" );
		}

//		CallbackRecvPacket( customRadioPacket, 0x40 );
		CallbackRecvPacket( &g_pRadioRxPkt[2], (0x40 - 2) );

//		// Configure PKT_CONFIG1 for RX
//		si446x_set_property(SI446X_PROP_GRP_ID_PKT, 1, SI446X_PROP_GRP_INDEX_PKT_CONFIG1, bPktConfig1ForRx);
//		// Start RX with Packet handler settings
//		vRadio_StartRX(pRadioConfiguration->Radio_ChannelNumber,
//			pRadioConfiguration->Radio_PacketLength);
	}

	if( bMain_IT_Status & SI446X_CMD_GET_INT_STATUS_REP_PH_PEND_PACKET_SENT_PEND_BIT )
	{
//		// Configure PKT_CONFIG1 for RX
//		si446x_set_property(SI446X_PROP_GRP_ID_PKT, 1, SI446X_PROP_GRP_INDEX_PKT_CONFIG1, bPktConfig1ForRx);
//		// Start RX with Packet handler settings
//		vRadio_StartRX(pRadioConfiguration->Radio_ChannelNumber,
//			pRadioConfiguration->Radio_PacketLength);

		HAL_GPIO_TogglePin ( LED_ST_GPIO_Port, LED_ST_Pin );

		nTxPkt++;
		nTxStamp = HAL_GetTick();

		// Custom message sent successfully
		if ( nTxPkt % 250 == 0 )
		{
			printf ( "T" );
		}
	}

//	if	(	bMain_IT_Status &
//			( SI446X_CMD_GET_INT_STATUS_REP_PH_PEND_PACKET_RX_PEND_BIT | SI446X_CMD_GET_INT_STATUS_REP_PH_PEND_PACKET_SENT_PEND_BIT )
//			)
//	{
//		// Configure PKT_CONFIG1 for RX
//		si446x_set_property(SI446X_PROP_GRP_ID_PKT, 1, SI446X_PROP_GRP_INDEX_PKT_CONFIG1, bPktConfig1ForRx);
//		// Start RX with Packet handler settings
//		vRadio_StartRX(pRadioConfiguration->Radio_ChannelNumber,
//			pRadioConfiguration->Radio_PacketLength);
//	}

#endif


#else


#if OLD

	switch ( bMain_IT_Status )
	{
	//========================================================================
	//  Transmit
	case SI446X_CMD_GET_INT_STATUS_REP_PH_PEND_PACKET_SENT_PEND_BIT:

		HAL_GPIO_TogglePin ( LED_ST_GPIO_Port, LED_ST_Pin );
//		HAL_GPIO_TogglePin ( LED_ON_B_GPIO_Port, LED_ON_B_Pin );

		nTxPkt++;
		nTxStamp = HAL_GetTick();
		if ( nTxPkt % 250 == 0 )
		{
			printf ( "T" );
		}
		/* Clear Packet Sending flag */

		break;

	//========================================================================
	//  Receive
	case SI446X_CMD_GET_INT_STATUS_REP_PH_PEND_PACKET_RX_PEND_BIT:

		HAL_GPIO_TogglePin ( LED_ST_GPIO_Port, LED_ST_Pin );

		nRxPkt++;
		nRxStamp = HAL_GetTick();

		Dump("Rx", customRadioPacket, 0x40);
		if ( nRxPkt % 250 == 0 )
		{
			printf ( "R" );
		}

		CallbackRecvPacket( customRadioPacket, 0x40 );

		//	Half-Duplex
		if ( GetRFMMode() != RFMModeTx )
		{
			//  송신모드가 아닌경우 수신 Start
			// Start RX with radio packet length
			vRadio_StartRX (
				pRadioConfiguration->Radio_ChannelNumber,
				pRadioConfiguration->Radio_PacketLength );
		}

		break;

	default:
		break;
	}

#else

	if( bMain_IT_Status & SI446X_CMD_GET_INT_STATUS_REP_PH_PEND_PACKET_RX_PEND_BIT )
	{
		HAL_GPIO_TogglePin ( LED_ST_GPIO_Port, LED_ST_Pin );

		nRxPkt++;
		nRxStamp = HAL_GetTick();

		Dump("Rx", g_pRadioRxPkt, 0x40);
		if ( nRxPkt % 250 == 0 )
		{
			printf ( "R" );
		}

		CallbackRecvPacket( g_pRadioRxPkt, 0x40 );
	}

	if( bMain_IT_Status & SI446X_CMD_GET_INT_STATUS_REP_PH_PEND_PACKET_SENT_PEND_BIT )
	{
		HAL_GPIO_TogglePin ( LED_ST_GPIO_Port, LED_ST_Pin );

		nTxPkt++;
		nTxStamp = HAL_GetTick();

		// Custom message sent successfully
		if ( nTxPkt % 250 == 0 )
		{
			printf ( "T" );
		}
	}

#endif


#endif

#if defined(USE_SHOW_PKT)

	static int s_oldTick = 0;

	if ( s_bShowPkt != 0 &&
		( nTick - s_oldTick ) >= 1000 )
	{
		//	1 sec

		printf("PKT : Tx(%d) / Rx(%d) / Hop(%d) / Drop(%d) / RspID( 0x%04X ) / RxErr(%d) / Crc(%d) / RSSI(%d)\n", //__func__,
				nTxPkt, nRxPkt, nHopPkt, nDropPkt, g_flagRspID, nRxErr, nCrcErr, g_nRSSI );

		s_oldTick = nTick;
	}
#endif
}

/*!
 * This function returns the actual PHY payload length extracted from the PHR (i.e. length w/o FCS)
 *
 * @return  PHY payload lenght excluding FCS
 */
//========================================================================
U16 wPayloadLenghtFromPhr(U8* pbPhrMsb)
//========================================================================
{
	SEGMENT_VARIABLE(wLength, U16, SEG_XDATA);
	SEGMENT_VARIABLE(bPhrLsbUpperByte, U8, SEG_XDATA);
	SEGMENT_VARIABLE(bPhrLsbLowerByte, U8, SEG_XDATA);

	// Get the lenght from the PHR in MSB
	bPhrLsbUpperByte = bBitOrderReverse(*pbPhrMsb);
	bPhrLsbLowerByte = bBitOrderReverse(*(pbPhrMsb+1));
	wLength = ((bPhrLsbUpperByte & 0x0003)<<8) | (bPhrLsbLowerByte & 0x00FF);

	// Lenght in PHR includes FCS length (2 or 4 bytes). Adjust returned value accordingly
	if ((bPhrLsbUpperByte & 0x10) == 0x10)
	{
		// 2-byte FCS
		return (wLength-2);
	}
	else
	{
		// 4-byte FCS
		return (wLength-4);
	}
}

/*!
 * This function is used to reverse the bit order of the input byte
 *
 * @return  Reversed byte.
 */
//========================================================================
U8 bBitOrderReverse(U8 bByteToReverse)
//========================================================================
{
	bByteToReverse = (bByteToReverse & 0xF0) >> 4 | (bByteToReverse & 0x0F) << 4;
	bByteToReverse = (bByteToReverse & 0xCC) >> 2 | (bByteToReverse & 0x33) << 2;
	bByteToReverse = (bByteToReverse & 0xAA) >> 1 | (bByteToReverse & 0x55) << 1;

	return bByteToReverse;
}


//========================================================================
int SendPacket( const char *sBuf, int nSize )
//========================================================================
{
	//	printf("%s(%d)\n", __func__, __LINE__);

	Dump("Tx", sBuf, 0x40);

	//	CH1 : 1, 3, 5
	//	CH2 :  2, 4, 6
	int nCh = ChTS1_1 + g_idxTrainSet * 2 + ((g_nCarNo + 1) % 2); // 현재 호차 채널

	vRadio_StartTx_Variable_Packet (
		nCh,	//g_idxTrainSet,	//		pRadioConfiguration->Radio_ChannelNumber,
		&sBuf[0],
		pRadioConfiguration->Radio_PacketLength );

#if defined(USE_RFT_TX_MULTI_SEND)
	if( GetDevID() == DevRF900T )
	{
		//	2회차 전송 : ch + 1
		//	음성 전송 Timing : 14.49 msec
//		HAL_Delay( 5 );		//	5 msec 후 전송.
		HAL_Delay( 2 );		//	2 msec 후 전송.

		nCh = ChTS1_1 + g_idxTrainSet * 2 + ( (g_nCarNo) % 2);	//	타채널
		vRadio_StartTx_Variable_Packet (
			nCh,	//g_idxTrainSet + 1,	//		pRadioConfiguration->Radio_ChannelNumber,
			&sBuf[0],
			pRadioConfiguration->Radio_PacketLength );
	}
#endif	// defined(USE_RFT_TX_MULTI_SEND)

	return TRUE;

}


//========================================================================
int SendPktCh	( int nCh, const char *sBuf, int nSize )
//========================================================================
{
	Dump("Tx", sBuf, 0x40);

	vRadio_StartTx_Variable_Packet (
		nCh,	//		pRadioConfiguration->Radio_ChannelNumber,
		&sBuf[0],
		pRadioConfiguration->Radio_PacketLength );

	return TRUE;
}


//========================================================================
int cmd_pktmon      ( int argc, char * argv[] )
//========================================================================
{
	//	bEnable ( 1 / 0 )
	int bEnable = 1;	//	Default : Enable

	switch ( argc )
	{
	case 2:		sscanf( argv[1], "%d", &bEnable );	//	cmd [Enable]
//	case 2:		sText = argv[1];					//	sscanf( argv[1], "%s", sText );		//	cmd [Text]
		break;
	}

	printf( "%s(%d) - En(%d)\n", __func__, __LINE__, bEnable );

	s_bShowPkt = bEnable;
}
