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

/*------------------------------------------------------------------------*/
/*                          Global variables                              */
/*------------------------------------------------------------------------*/
SEGMENT_VARIABLE(bMain_IT_Status, U8, SEG_XDATA);
SEGMENT_VARIABLE(lPer_MsCnt, U16, SEG_DATA);

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
SEGMENT_VARIABLE(rollingCounter, U16, SEG_XDATA);

SEGMENT_VARIABLE(bPropValue1, U8, SEG_XDATA);
SEGMENT_VARIABLE(bPropValue2, U8, SEG_XDATA);
SEGMENT_VARIABLE(bModulationType, U8, SEG_XDATA);
SEGMENT_VARIABLE(bPktConfig1ForRx, U8, SEG_XDATA);
SEGMENT_VARIABLE(abPacketReference[RADIO_CONFIGURATION_DATA_ACK_MAC_PAYLOAD_LENGTH+2], U8, SEG_XDATA) = RADIO_CONFIGURATION_DATA_ACK_PAYLOAD;

/*------------------------------------------------------------------------*/
/*                      Local function prototypes                         */
/*------------------------------------------------------------------------*/

// Compare the expected custom payload with the incoming payload
BIT  gSampleCode_StringCompare(U8* pbiPacketContent, U8* pbiString, U8 bLength, BIT gIgnorePhrFcsDw);

// Waiting for a short period of time
void vSample_Code_Wait(U16 wWaitTime);

// Send "ACK" message  
void vSampleCode_SendAcknowledge(void);

// Send the custom message
BIT  gSampleCode_SendVariablePacket(void);

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

		if( (currTick - s_nTick) >= 1 )
		{
			//	1 msec Tick
			//	Proc( s_nTick - currTick )
			s_nTick = currTick;

			if (lPer_MsCnt < 0xFFFF)
			{
				lPer_MsCnt++;
			}
		}

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

void CallbackRecvPacket( const char *pData, int nSize )
{

#if 0
	//  Queue Buffer Put
//		printf ( "P" );

	RFMPkt	*pRFPkt = (RFMPkt *)&customRadioPacket[0];

	if( GetDevID() == DevRF900T && pRFPkt->hdr.nPktID == PktCall )
	{
		//  송신기
		uint16_t	 *pAudioBuf = (uint16_t*)pRFPkt->dat.data;

		//*
//#if defined(USE_CODEC_MAX9860)
//			//  MAX9860 : Codec I2C로 볼륨조절.
//#else
		if ( GetAudioIC() == AudioXE3005 )
		{
			//  XE3005 : PCM 값으로 볼륨조절.
			if ( g_nSpkLevel == 2 )
			{
				//  Spk Volume 조절 ( X 4 )
				for ( i = 0; i < I2S_DMA_LOOP_SIZE; i++ )
				{
					pAudioBuf[i] = (int16_t)pAudioBuf[i] << 2;
				}
			}
			else if ( g_nSpkLevel == 3 )
			{
				//  Spk Volume 조절 ( X 16 )
				for ( i = 0; i < I2S_DMA_LOOP_SIZE; i++ )
				{
					pAudioBuf[i] = (int16_t)pAudioBuf[i] << 4;
				}
			}
		}
//#endif
		//  */

		stampRx = nTick;
		SetRFMMode( RFMModeRx );

#if defined(USE_RFT_ONLY_RX_SPK_ON)
		//  송신기 : 수신중인 경우 SPK ON
		HAL_GPIO_WritePin( SPK_ON_GPIO_Port, SPK_ON_Pin, GPIO_PIN_SET );
#endif

		//  Red LED On
		HAL_GPIO_WritePin ( LED_ON_B_GPIO_Port, LED_ON_B_Pin, GPIO_PIN_SET ); //  RED LED

		//  통화 : 송신기 -> 송신기
		qBufPut( &g_qBufAudioRFRx, (uint8_t*)pAudioBuf, ( I2S_DMA_LOOP_SIZE * 2 ) );
	}
	else if (
				pRFPkt->hdr.addrSrc == DevRF900T
				&& pRFPkt->hdr.addrDest == DevRF900M
				&& pRFPkt->hdr.nPktID == PktPA
			  )
	{
		if ( GetDevID() == DevRF900M )
		{
			//  수신기
			uint16_t	 *pAudioBuf = (uint16_t*)pRFPkt->dat.data;

			//  방송 : 송신기 -> 수신기
			qBufPut( &g_qBufAudioRFRx, (uint8_t*)pAudioBuf, ( I2S_DMA_LOOP_SIZE * 2 ) );

			// 조명 On
			HAL_GPIO_WritePin ( LIGHT_ON_GPIO_Port, LIGHT_ON_Pin, GPIO_PIN_SET );

			stampRx = nTick;
			SetRFMMode( RFMModeRx );

			//  수신기 Spk Relay On
			HAL_GPIO_WritePin( AUDIO_ON_GPIO_Port, AUDIO_ON_Pin, GPIO_PIN_SET );
		}
		else
		{
			//========================================================================
			uint16_t	 *pAudioBuf = (uint16_t*)pRFPkt->dat.data;

//#if defined(USE_CODEC_MAX9860)
//				//  MAX9860 : Codec I2C로 볼륨조절.
//#else
			if ( GetAudioIC() == AudioXE3005 )
			{
				//  XE3005 : PCM 값으로 볼륨조절.
				if ( g_nSpkLevel == 2 )
				{
					//  Spk Volume 조절 ( X 4 )
					for ( i = 0; i < I2S_DMA_LOOP_SIZE; i++ )
					{
						pAudioBuf[i] = (int16_t)pAudioBuf[i] << 2;
					}
				}
				else if ( g_nSpkLevel == 3 )
				{
					//  Spk Volume 조절 ( X 16 )
					for ( i = 0; i < I2S_DMA_LOOP_SIZE; i++ )
					{
						pAudioBuf[i] = (int16_t)pAudioBuf[i] << 4;
					}
				}
			}
//#endif

#if defined(USE_RFT_ONLY_RX_SPK_ON)
			//  송신기 : 수신중인 경우 SPK ON
			HAL_GPIO_WritePin( SPK_ON_GPIO_Port, SPK_ON_Pin, GPIO_PIN_SET );
#endif

			//  방송 : 송신기 -> 수신기
			qBufPut( &g_qBufAudioRFRx, (uint8_t*)pAudioBuf, ( I2S_DMA_LOOP_SIZE * 2 ) );
			//========================================================================

			//  송신기
			stampRx = nTick;
			SetRFMMode( RFMModeRx );

			//  Red LED On
			HAL_GPIO_WritePin ( LED_ON_B_GPIO_Port, LED_ON_B_Pin, GPIO_PIN_SET ); //  RED LED
		}
	}

	if ( GetDbgLevel() > 0 )
	{
		for ( i = 0; i < 64; i++ )	printf( "%02X ", customRadioPacket[i] );
		printf( "\n" );
	}

	//========================================================================
	//  Status Data
	if ( pRFPkt->hdr.nPktID == PktStat )
	{
		//	상태정보 수신.
		printf ( "[Stat] Car:%d\n", pRFPkt->dat.stat.nCarNo );
	}

	if ( GetDevID() == DevRF900M )
	{
		//  수신기 조명제어.

		if ( pRFPkt->hdr.nPktID == PktLightOff )
		{
			// 조명 Off 명령 수신시.
			HAL_GPIO_WritePin ( LIGHT_ON_GPIO_Port, LIGHT_ON_Pin, GPIO_PIN_RESET );
		}
		else if ( pRFPkt->hdr.nPktID == PktLightOn )
		{
			// 조명 Off 명령 수신시.
			HAL_GPIO_WritePin ( LIGHT_ON_GPIO_Port, LIGHT_ON_Pin, GPIO_PIN_SET );
		}
	}

	idx++;

	if ( idx % 250 == 0 )
	{
		//  1초간격
		printf ( "R" );
		//for ( i = 0; i < 64; i++ )	printf( "%02X ", customRadioPacket[i] );
		//printf( "\n" );
	}

	if ( GetRFMMode() != RFMModeTx )
	{
		//  송신모드가 아닌경우 수신 Start
		// Start RX with radio packet length
		vRadio_StartRX (
			pRadioConfiguration->Radio_ChannelNumber,
			pRadioConfiguration->Radio_PacketLength );
	}
#endif

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
	static int s_oldTick = 0;

	static SEGMENT_VARIABLE(lPktSending, U8, SEG_XDATA) = 0u;

	bMain_IT_Status = bRadio_Check_Tx_RX();

#if defined( USE_I333802_15_4G )

	switch (bMain_IT_Status)
	{
	case SI446X_CMD_GET_INT_STATUS_REP_PH_PEND_PACKET_SENT_PEND_BIT:

		if(gSampleCode_StringCompare(&customRadioPacket[0],&abPacketReference[0],(U8)(RADIO_CONFIGURATION_DATA_ACK_MAC_PAYLOAD_LENGTH+2),COMPARE_PHR_AND_PAYLOAD_FULL) == TRUE)
		{
			// Message "ACK" sent successfully
			//        vHmi_ChangeLedState(eHmi_Led4_c, eHmi_LedBlinkOnce_c);

			// Restore packet field settings for regular packet sending
			// Packet length, CRC and DW settings may have been different for ACK
			// Configure field length
			si446x_set_property(SI446X_PROP_GRP_ID_PKT, 2, SI446X_PROP_PKT_FIELD_2_LENGTH_FIELD_2_LENGTH_12_8_INDEX, 0x00, ((pRadioConfiguration->Radio_PacketLength)-2));
			//Enable/disable Data Whitening based on how regular packets were configured in radio_config.h
			bRadio_FindProperty(pRadioConfiguration->Radio_ConfigurationArray, SI446X_PROP_GRP_ID_PKT, SI446X_PROP_GRP_INDEX_PKT_FIELD_2_CONFIG, &bPropValue1);
			//Turn on/off ALT CRC engine, turn on/off primary CRC engine based on how regular packets were configured in radio_config.h
			bRadio_FindProperty(pRadioConfiguration->Radio_ConfigurationArray, SI446X_PROP_GRP_ID_PKT, SI446X_PROP_GRP_INDEX_PKT_FIELD_2_CRC_CONFIG, &bPropValue2);
			si446x_set_property(SI446X_PROP_GRP_ID_PKT, 2, SI446X_PROP_GRP_INDEX_PKT_FIELD_2_CONFIG, bPropValue1, bPropValue2);

			// Configure PKT_CONFIG1 for RX
			si446x_set_property(SI446X_PROP_GRP_ID_PKT, 1, SI446X_PROP_GRP_INDEX_PKT_CONFIG1, bPktConfig1ForRx);
			// Start RX with Packet handler settings
			vRadio_StartRX(pRadioConfiguration->Radio_ChannelNumber,0u);
		}
		else
		{
			// Custom message sent successfully
			//        vHmi_ChangeLedState(eHmi_Led1_c, eHmi_LedBlinkOnce_c);

			// Configure expected ACK PHR to match to the packet that was just sent
			// Set FCS and DW fileds for the expected ACK
			abPacketReference[0u] = (customRadioPacket[0u]) & 0x18;
			if ((bBitOrderReverse(customRadioPacket[0u]) & 0x10) == 0x10)
			{
				// Expected ACK length should be ACK lenght + 2 FCS bytes
				abPacketReference[1u] = bBitOrderReverse((U8)(RADIO_CONFIGURATION_DATA_ACK_MAC_PAYLOAD_LENGTH + 2));
			}
			else
			{
				// Expected ACK length should be ACK lenght + 4 FCS bytes
				abPacketReference[1u] = bBitOrderReverse((U8)(RADIO_CONFIGURATION_DATA_ACK_MAC_PAYLOAD_LENGTH + 4));
			}

			// Configure PKT_CONFIG1 for RX
			si446x_set_property(SI446X_PROP_GRP_ID_PKT, 1, SI446X_PROP_GRP_INDEX_PKT_CONFIG1, bPktConfig1ForRx);
			// Start RX with Packet handler settings
			vRadio_StartRX(pRadioConfiguration->Radio_ChannelNumber,0u);
		}

		/* Clear Packet Sending flag */
		lPktSending = 0u;

		break;

	case SI446X_CMD_GET_INT_STATUS_REP_PH_PEND_PACKET_RX_PEND_BIT:

		// PHR was put in the RX FIFO in an LSB order, Payload in an MSB order. Store both in MSB in customRadioPacket[]
		customRadioPacket[0u] = bBitOrderReverse(customRadioPacket[0u]);
		customRadioPacket[1u] = bBitOrderReverse(customRadioPacket[1u]);

		Dump("Rx", customRadioPacket, 0x40);

		if (gSampleCode_StringCompare(&customRadioPacket[0], &abPacketReference[0], (U8)(RADIO_CONFIGURATION_DATA_ACK_MAC_PAYLOAD_LENGTH+2),COMPARE_PHR_AND_PAYLOAD_FULL) == TRUE)
		{
			// Configure PKT_CONFIG1 for RX
			si446x_set_property(SI446X_PROP_GRP_ID_PKT, 1, SI446X_PROP_GRP_INDEX_PKT_CONFIG1, bPktConfig1ForRx);
			// Start RX with Packet handler settings
			vRadio_StartRX(pRadioConfiguration->Radio_ChannelNumber,0u);

			printf("%s(%d) - Ack\n", __func__, __LINE__);
			//        // Buzz once
			//        vHmi_ChangeBuzzState(eHmi_BuzzOnce_c);
			//
			//        // "ACK" arrived successfully
			//        vHmi_ChangeLedState(eHmi_Led1_c, eHmi_LedBlinkOnce_c);
			//        vHmi_ChangeLedState(eHmi_Led2_c, eHmi_LedBlinkOnce_c);
			//        vHmi_ChangeLedState(eHmi_Led3_c, eHmi_LedBlinkOnce_c);
			//        vHmi_ChangeLedState(eHmi_Led4_c, eHmi_LedBlinkOnce_c);

			break;
		}
		else
		{
			if (gSampleCode_StringCompare((U8 *) &customRadioPacket[0u], pRadioConfiguration->Radio_CustomPayload,
					pRadioConfiguration->Radio_PacketLength,COMPARE_PHR_LENGTH_AND_PAYLOAD) == TRUE )
			{
				// Custom packet arrived
				// Blink once LED1 as payload match
				//          vHmi_ChangeLedState(eHmi_Led1_c, eHmi_LedBlinkOnce_c);

				// Send ACK back
				vSampleCode_SendAcknowledge();
				printf("%s(%d) - Send Ack\n", __func__, __LINE__);
			}
			else
			{
				// Incorrect packet content arrived
				// Configure PKT_CONFIG1 for RX
				si446x_set_property(SI446X_PROP_GRP_ID_PKT, 1, SI446X_PROP_GRP_INDEX_PKT_CONFIG1, bPktConfig1ForRx);
				// Start RX with Packet handler settings
				vRadio_StartRX(pRadioConfiguration->Radio_ChannelNumber,0u);
			}
		}
		break;

	default:
		break;
	} /* switch */

#else

	static int idx = 0;

	switch ( bMain_IT_Status )
	{
	//========================================================================
	//  Transmit
	case SI446X_CMD_GET_INT_STATUS_REP_PH_PEND_PACKET_SENT_PEND_BIT:

		HAL_GPIO_TogglePin ( LED_ST_GPIO_Port, LED_ST_Pin );
//		HAL_GPIO_TogglePin ( LED_ON_B_GPIO_Port, LED_ON_B_Pin );

		idx++;

		printf ( "T" );

		if ( idx % 250 == 0 )
		{
			printf ( "T" );

			//for ( i = 0; i < 64; i++ )	printf( "%02X ", bufRFTx[i] );
			//printf( "\n" );
		}
		/* Clear Packet Sending flag */
		lPktSending = 0u;
		break;

	//========================================================================
	//  Receive
	case SI446X_CMD_GET_INT_STATUS_REP_PH_PEND_PACKET_RX_PEND_BIT:

		HAL_GPIO_TogglePin ( LED_ST_GPIO_Port, LED_ST_Pin );

		Dump("Rx", customRadioPacket, 0x40);

		CallbackRecvPacket( customRadioPacket, 0x40 );

		vRadio_StartRX (
			pRadioConfiguration->Radio_ChannelNumber,
			pRadioConfiguration->Radio_PacketLength );

		break;

	default:
		break;
	}


#endif

	if( ( nTick - s_oldTick ) >= 1000 )
	{
		//	1 sec

		printf("%s : Tx(%d) / Rx(%d)\n",__func__, nTxPkt, nRxPkt);

		s_oldTick = nTick;
	}

	if ((lPer_MsCnt >= PACKET_SEND_INTERVAL) && (0u == lPktSending))
	{
		if (TRUE == gSampleCode_SendVariablePacket())
		{
			/* Reset packet send counter */
			lPer_MsCnt = 0u;

			/* Set Packet Sending flag */
			lPktSending = 1u;
		}
	}
}

/*!
 * This function is used to compare the content of the received packet to a string
 *
 * @return  None.
 */
BIT gSampleCode_StringCompare(U8* pbiPacketContent, U8* pbiString, U8 bLength, BIT gIgnorePhrFcsDw)
{
	if ( gIgnorePhrFcsDw && (wPayloadLenghtFromPhr(pbiPacketContent) == wPayloadLenghtFromPhr(pbiString)) )
	{
		// Compare input arrays excluding PHR
		pbiPacketContent += 2;
		pbiString += 2;
		while ((*pbiPacketContent++ == *pbiString++) && (bLength > 2u))
		{
			if( (--bLength) == 2u )
			{
				return TRUE;
			}
		}
	}
	else
	{
		// Compare input arrays including PHR
		while ((*pbiPacketContent++ == *pbiString++) && (bLength > 0u))
		{
			if( (--bLength) == 0u )
			{
				return TRUE;
			}
		}
	}

	return FALSE;
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
 * This function is used to wait for a little time.
 *
 * @return  None.
 */
void vSample_Code_Wait(U16 wWaitTime)
{
	SEGMENT_VARIABLE(wDelay, U16 , SEG_DATA) = wWaitTime;

	for (; wDelay--; )
	{
		NOP();
	}
}

/*!
 * This function is used to send "ACK"back to the sender.
 *
 * @return  None.
 */
void vSampleCode_SendAcknowledge(void)
{
	SEGMENT_VARIABLE(bAckFrameLengthUpperByte, U8, SEG_XDATA);
	SEGMENT_VARIABLE(bAckFrameLengthLowerByte, U8, SEG_XDATA);
	SEGMENT_VARIABLE(bPhrLsbUpperByte, U8, SEG_XDATA);
	SEGMENT_VARIABLE(bCnt, U8, SEG_XDATA);


	// Configure field length
	si446x_set_property(SI446X_PROP_GRP_ID_PKT, 2, SI446X_PROP_PKT_FIELD_2_LENGTH_FIELD_2_LENGTH_12_8_INDEX, 0x00, RADIO_CONFIGURATION_DATA_ACK_MAC_PAYLOAD_LENGTH);	

	// Get FCS and DW fields from the received PHR to match CRC and DW settings of the ACK
	bPhrLsbUpperByte = (bBitOrderReverse(customRadioPacket[0u]) & 0x18);


	switch (bPhrLsbUpperByte)
	{
	case PHR_CRC16_DW_DIS:

		if (bModulationType == MOD_TYPE_2GFSK)
		{
			//Override PKT_CONFIG1
			//No CRC invert, CRC endian MSBYTE first, Bit order LSBIT first, disable 4GFSK
			si446x_set_property(SI446X_PROP_GRP_ID_PKT, 1, SI446X_PROP_GRP_INDEX_PKT_CONFIG1, 0x83);
			//Disable Data Whitening (PKT_FIELD_2_CONFIG), disable 4GFSK
			//Turn on ALT CRC engine, turn off primary CRC engine (PKT_FIELD_2_CRC_CONFIG)
			si446x_set_property(SI446X_PROP_GRP_ID_PKT, 2, SI446X_PROP_GRP_INDEX_PKT_FIELD_2_CONFIG, 0x00, 0x11);
		}
		else if (bModulationType == MOD_TYPE_4GFSK)
		{
			//Override PKT_CONFIG1
			//No CRC invert, CRC endian MSBYTE first, Bit order LSBIT first, enable 4GFSK
			si446x_set_property(SI446X_PROP_GRP_ID_PKT, 1, SI446X_PROP_GRP_INDEX_PKT_CONFIG1, 0xA3);
			//Disable Data Whitening (PKT_FIELD_2_CONFIG), enable 4GFSK
			//Turn on ALT CRC engine, turn off primary CRC engine (PKT_FIELD_2_CRC_CONFIG)
			si446x_set_property(SI446X_PROP_GRP_ID_PKT, 2, SI446X_PROP_GRP_INDEX_PKT_FIELD_2_CONFIG, 0x10, 0x11);
		}

		// Generate ACK Frame Length (i.e. length of MAC payload (Field 2) + length of CRC)
		bAckFrameLengthUpperByte = (RADIO_CONFIGURATION_DATA_ACK_MAC_PAYLOAD_LENGTH & 0x0700)>>8;
		bAckFrameLengthLowerByte = (RADIO_CONFIGURATION_DATA_ACK_MAC_PAYLOAD_LENGTH & 0x00FF) + 2;

		break;

	case PHR_CRC16_DW_EN:

		if (bModulationType == MOD_TYPE_2GFSK)
		{
			//Override PKT_CONFIG1
			//No CRC invert, CRC endian MSBYTE first, Bit order LSBIT first, disable 4GFSK
			si446x_set_property(SI446X_PROP_GRP_ID_PKT, 1, SI446X_PROP_GRP_INDEX_PKT_CONFIG1, 0x83);
			//Enable Data Whitening (PKT_FIELD_2_CONFIG), disable 4GFSK
			//Turn on ALT CRC engine, turn off primary CRC engine (PKT_FIELD_2_CRC_CONFIG)
			si446x_set_property(SI446X_PROP_GRP_ID_PKT, 2, SI446X_PROP_GRP_INDEX_PKT_FIELD_2_CONFIG, 0x02, 0x11);
		}
		else if (bModulationType == MOD_TYPE_4GFSK)
		{
			//Override PKT_CONFIG1
			//No CRC invert, CRC endian MSBYTE first, Bit order LSBIT first, enable 4GFSK
			si446x_set_property(SI446X_PROP_GRP_ID_PKT, 1, SI446X_PROP_GRP_INDEX_PKT_CONFIG1, 0xA3);
			//Enable Data Whitening (PKT_FIELD_2_CONFIG), enable 4GFSK
			//Turn on ALT CRC engine, turn off primary CRC engine (PKT_FIELD_2_CRC_CONFIG)
			si446x_set_property(SI446X_PROP_GRP_ID_PKT, 2, SI446X_PROP_GRP_INDEX_PKT_FIELD_2_CONFIG, 0x12, 0x11);
		}

		// Generate ACK Frame Length (i.e. length of MAC payload (Field 2) + length of CRC)
		bAckFrameLengthUpperByte = (RADIO_CONFIGURATION_DATA_ACK_MAC_PAYLOAD_LENGTH & 0x0700)>>8;
		bAckFrameLengthLowerByte = (RADIO_CONFIGURATION_DATA_ACK_MAC_PAYLOAD_LENGTH & 0x00FF) + 2;

		break;

	case PHR_CRC32_DW_DIS:

		if (bModulationType == MOD_TYPE_2GFSK)
		{
			//Override PKT_CONFIG1
			//CRC invert, CRC endian MSBYTE first, Bit order LSBIT first, disable 4GFSK
			si446x_set_property(SI446X_PROP_GRP_ID_PKT, 1, SI446X_PROP_GRP_INDEX_PKT_CONFIG1, 0x87);
			// Disable Data Whitening (PKT_FIELD_2_CONFIG), disable 4GFSK
			//Turn off ALT CRC engine, turn on primary CRC engine (PKT_FIELD_2_CRC_CONFIG)
			si446x_set_property(SI446X_PROP_GRP_ID_PKT, 2, SI446X_PROP_GRP_INDEX_PKT_FIELD_2_CONFIG, 0x00, 0x22);
		}
		else if (bModulationType == MOD_TYPE_4GFSK)
		{
			//Override PKT_CONFIG1
			//CRC invert, CRC endian MSBYTE first, Bit order LSBIT first, enable 4GFSK
			si446x_set_property(SI446X_PROP_GRP_ID_PKT, 1, SI446X_PROP_GRP_INDEX_PKT_CONFIG1, 0xA7);
			// Disable Data Whitening (PKT_FIELD_2_CONFIG), enable 4GFSK
			//Turn off ALT CRC engine, turn on primary CRC engine (PKT_FIELD_2_CRC_CONFIG)
			si446x_set_property(SI446X_PROP_GRP_ID_PKT, 2, SI446X_PROP_GRP_INDEX_PKT_FIELD_2_CONFIG, 0x10, 0x22);
		}

		// Generate ACK Frame Length (i.e. length of MAC payload (Field 2) + length of CRC)
		bAckFrameLengthUpperByte = (RADIO_CONFIGURATION_DATA_ACK_MAC_PAYLOAD_LENGTH & 0x0700)>>8;
		bAckFrameLengthLowerByte = (RADIO_CONFIGURATION_DATA_ACK_MAC_PAYLOAD_LENGTH & 0x00FF) + 4;

		break;

	case PHR_CRC32_DW_EN:

		if (bModulationType == MOD_TYPE_2GFSK)
		{
			//Override PKT_CONFIG1
			//CRC invert, CRC endian MSBYTE first, Bit order LSBIT first, disable 4GFSK
			si446x_set_property(SI446X_PROP_GRP_ID_PKT, 1, SI446X_PROP_GRP_INDEX_PKT_CONFIG1, 0x87);
			// Enable Data Whitening (PKT_FIELD_2_CONFIG), disable 4GFSK
			//Turn off ALT CRC engine, turn on primary CRC engine (PKT_FIELD_2_CRC_CONFIG)
			si446x_set_property(SI446X_PROP_GRP_ID_PKT, 2, SI446X_PROP_GRP_INDEX_PKT_FIELD_2_CONFIG, 0x02, 0x22);
		}
		else if (bModulationType == MOD_TYPE_4GFSK)
		{
			//Override PKT_CONFIG1
			//CRC invert, CRC endian MSBYTE first, Bit order LSBIT first, enable 4GFSK
			si446x_set_property(SI446X_PROP_GRP_ID_PKT, 1, SI446X_PROP_GRP_INDEX_PKT_CONFIG1, 0xA7);
			// Enable Data Whitening (PKT_FIELD_2_CONFIG), enable 4GFSK
			//Turn off ALT CRC engine, turn on primary CRC engine (PKT_FIELD_2_CRC_CONFIG)
			si446x_set_property(SI446X_PROP_GRP_ID_PKT, 2, SI446X_PROP_GRP_INDEX_PKT_FIELD_2_CONFIG, 0x12, 0x22);
		}

		// Generate ACK Frame Length (i.e. length of MAC payload (Field 2) + length of CRC)
		bAckFrameLengthUpperByte = (RADIO_CONFIGURATION_DATA_ACK_MAC_PAYLOAD_LENGTH & 0x0700)>>8;
		bAckFrameLengthLowerByte = (RADIO_CONFIGURATION_DATA_ACK_MAC_PAYLOAD_LENGTH & 0x00FF) + 4;

		break;
	}

	// Generate payload
	// Set up payload arrays in MSB
	abPacketReference[0u] = (customRadioPacket[0u] & 0x18) | bBitOrderReverse(bAckFrameLengthUpperByte);
	abPacketReference[1u] = bBitOrderReverse(bAckFrameLengthLowerByte);

	for (bCnt=0; bCnt<(RADIO_CONFIGURATION_DATA_ACK_MAC_PAYLOAD_LENGTH+2); bCnt++)
	{
		customRadioPacket[bCnt] = abPacketReference[bCnt];
	}

	// Sending ACK packet
	vRadio_StartTx_Variable_Packet_MultiField(pRadioConfiguration->Radio_ChannelNumber, &customRadioPacket[0], RADIO_CONFIGURATION_DATA_ACK_MAC_PAYLOAD_LENGTH+2);

}

/*!
 * This function is used to reverse the bit order of the input byte
 *
 * @return  Reversed byte.
 */

U8 bBitOrderReverse(U8 bByteToReverse)
{
#if 0
	bByteToReverse = (bByteToReverse & 0xF0) >> 4 | (bByteToReverse & 0x0F) << 4;
	bByteToReverse = (bByteToReverse & 0xCC) >> 2 | (bByteToReverse & 0x33) << 2;
	bByteToReverse = (bByteToReverse & 0xAA) >> 1 | (bByteToReverse & 0x55) << 1;
#endif
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

	//*

	memcpy( &buf[2], sBuf, 0x40 - 2);

	Dump("Tx", buf, 0x40);

	/*/

//	buf[2] = 0x02;
//	buf[3] = 0x00;
//	buf[4] = 0x6A;

	memcpy( &buf[5], sBuf, 0x40 - 5);

	//	*/

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

/*!
 * This function is used to send the custom packet.
 *
 * @return  None.
 */
BIT gSampleCode_SendVariablePacket(void)
{
	//	printf("%s(%d)\n", __func__, __LINE__);

	SEGMENT_VARIABLE(boPbPushTrack,  U8, SEG_DATA);
	SEGMENT_VARIABLE(lTemp,         U16, SEG_DATA);
	SEGMENT_VARIABLE(pos,            U8, SEG_DATA);

	//  gHmi_PbIsPushed(&boPbPushTrack, &lTemp);

	//  if( boPbPushTrack & eHmi_Pb1_c)
	if( 1 )	//boPbPushTrack & eHmi_Pb1_c)
	{
		for(pos = 0u; pos < pRadioConfiguration->Radio_PacketLength; pos++)
		{
			customRadioPacket[pos] = pRadioConfiguration->Radio_CustomPayload[pos];
		}

		// Override PKT_CONFIG1
		bRadio_FindProperty(pRadioConfiguration->Radio_ConfigurationArray, SI446X_PROP_GRP_ID_PKT, SI446X_PROP_GRP_INDEX_PKT_CONFIG1, &bPropValue1);
		// Configure PH field split, CRC endian, bit order for RX
		si446x_set_property(SI446X_PROP_GRP_ID_PKT, 1, SI446X_PROP_GRP_INDEX_PKT_CONFIG1, bPropValue1);

		// Send custom packet
		vRadio_StartTx_Variable_Packet_MultiField(pRadioConfiguration->Radio_ChannelNumber, &customRadioPacket[0], pRadioConfiguration->Radio_PacketLength);

		/* Packet sending initialized */
		return TRUE;
	}

	return FALSE;
}

