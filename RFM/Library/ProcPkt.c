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

int nTxStamp = 0;
int nRxStamp = 0;

uint16_t	g_flagRspID 	=	0x00;				//  범위 안의 Device ID Flag ( 0 ~ 15 bit )
uint8_t	 	g_nPktSeq 		=	0x00;				//  Packet Sequence


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
	if ( GetDbgLevel() == 0 )	return;

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
	vRadio_StartRX(
		pRadioConfiguration->Radio_ChannelNumber,
		pRadioConfiguration->Radio_PacketLength );

#endif

	return TRUE;
}

//========================================================================
void CallbackRecvPacket( const char *pData, int nSize )
//========================================================================
{
	const RFMPkt	*pRFPkt = (const RFMPkt *)pData;

#if defined(USE_HOPPING)

	//========================================================================
	//	Packet Filtering
	//		- Pkt 처리 여부 확인.
	if ( pRFPkt->hdr.nSeq != 0 && pRFPkt->hdr.nSeq == g_nPktSeq )
	{
		//	이미 처리된 Packet Skip.
		nDropPkt++;
		return ;
	}

	//========================================================================
	//	Hopping
	if ( pRFPkt->hdr.nSeq != 0 && pRFPkt->hdr.nIDFlag != 0 &&
		((~pRFPkt->hdr.nIDFlag) & g_flagRspID != 0 ) )
	{
		//	전송 범위 밖의 Device가 수신된 경우.
		//	Rsp Flag 설정 후에 전송.
		nHopPkt++;
		char buf[64];
		memcpy( buf, pData, 64 );
		RFMPkt	*pSendPkt = (RFMPkt *)buf;
		pSendPkt->hdr.nIDFlag |= g_flagRspID;

		SendPacket( buf, nSize );
		//
	}

	g_nPktSeq = pRFPkt->hdr.nSeq;	//	Packet Seq 갱신.

	//	Device ID Flag 확인.

#endif	//	defined(USE_HOPPING)


	//  Queue Buffer Put
//		printf ( "P" );


	if( GetDevID() == DevRF900T && pRFPkt->hdr.nPktCmd == PktCall )
	{
		//  송신기
		uint16_t	 *pAudioBuf = (uint16_t*)pRFPkt->dat.data;

//		stampRx = nTick;
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
	else if (
			pRFPkt->hdr.nPktCmd == PktPA
#if defined(USE_HOPPING)

#else
				&& pRFPkt->hdr.addrSrc == DevRF900T
				&& pRFPkt->hdr.addrDest == DevRF900M
#endif
			  )
	{
		if ( GetDevID() == DevRF900M )
		{
			//  수신기
			uint16_t	 *pAudioBuf = (uint16_t*)pRFPkt->dat.data;

			//  방송 : 송신기 -> 수신기
			qBufPut( &g_qBufAudioRx, (uint8_t*)pAudioBuf, ( I2S_DMA_LOOP_SIZE * 2 ) );

			// 조명 On
			HAL_GPIO_WritePin ( LIGHT_ON_GPIO_Port, LIGHT_ON_Pin, GPIO_PIN_SET );

//			stampRx = nTick;
			SetRFMMode( RFMModeRx );

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
//			stampRx = nTick;
			SetRFMMode( RFMModeRx );

			//  Red LED On
			HAL_GPIO_WritePin ( LED_ON_B_GPIO_Port, LED_ON_B_Pin, GPIO_PIN_SET ); //  RED LED
		}
	}

	//========================================================================
	//  Status Data
	if ( pRFPkt->hdr.nPktCmd == PktStat )
	{
		int nRspID = pRFPkt->dat.stat.nCarNo;
		//	상태정보 수신.
//		printf ( "[Stat] Car:%d\n", pRFPkt->dat.stat.nCarNo );

		if( nRspID < 16 )
		{
			//	장치 응답 Flag 설정.
			g_flagRspID	|= ( 0x1 << nRspID );
		}
	}

	if ( GetDevID() == DevRF900M )
	{
		//  수신기 조명제어.

		if ( pRFPkt->hdr.nPktCmd == PktLightOff )
		{
			// 조명 Off 명령 수신시.
			HAL_GPIO_WritePin ( LIGHT_ON_GPIO_Port, LIGHT_ON_Pin, GPIO_PIN_RESET );
		}
		else if ( pRFPkt->hdr.nPktCmd == PktLightOn )
		{
			// 조명 Off 명령 수신시.
			HAL_GPIO_WritePin ( LIGHT_ON_GPIO_Port, LIGHT_ON_Pin, GPIO_PIN_SET );
		}
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


#endif

#if 1
	static int s_oldTick = 0;

	if( ( nTick - s_oldTick ) >= 1000 )
	{
		//	1 sec

		printf("%s : Tx(%d) / Rx(%d) / Hop(%d) / Drop(%d) / RspID( 0x%04X )\n",__func__,
				nTxPkt, nRxPkt, nHopPkt, nDropPkt, g_flagRspID );

		s_oldTick = nTick;
	}
#endif
}

/*!
 * This function returns the actual PHY payload length extracted from the PHR (i.e. length w/o FCS)
 *
 * @return  PHY payload lenght excluding FCS
 */
U16 wPayloadLenghtFromPhr(U8* pbPhrMsb)
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

U8 bBitOrderReverse(U8 bByteToReverse)
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

#if defined(USE_IEEE802_15_4G)

	char buf[0x40];

	buf[0] = 0x18;
	buf[1] = 0x02;

	memcpy( &buf[2], sBuf, 0x40 - 2);

	Dump("Tx", buf, 0x40);

	// Override PKT_CONFIG1
	bRadio_FindProperty(pRadioConfiguration->Radio_ConfigurationArray, SI446X_PROP_GRP_ID_PKT, SI446X_PROP_GRP_INDEX_PKT_CONFIG1, &bPropValue1);
	// Configure PH field split, CRC endian, bit order for RX
	si446x_set_property(SI446X_PROP_GRP_ID_PKT, 1, SI446X_PROP_GRP_INDEX_PKT_CONFIG1, bPropValue1);

	// Send custom packet
	vRadio_StartTx_Variable_Packet_MultiField(pRadioConfiguration->Radio_ChannelNumber, &buf[0], pRadioConfiguration->Radio_PacketLength);

	/* Packet sending initialized */

	return TRUE;

#else

	Dump("Tx", sBuf, 0x40);

	vRadio_StartTx_Variable_Packet (
		pRadioConfiguration->Radio_ChannelNumber,
		&sBuf[0],
		pRadioConfiguration->Radio_PacketLength );

	return TRUE;

#endif
}

