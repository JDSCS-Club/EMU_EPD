/*------------------------------------------------------------------------------------------
	Project			: LED
	Description		: RS485

	Writer			: $Author: zlkit $
	Revision		: $Rev: 1891 $
	Date			: 2019. 01.
	Copyright		: Piranti Corp. ( zlkit@piranti.co.kr )

	Revision History
	1. 2019. 01.	: Created
-------------------------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ctype.h>
#include <string.h>
#include <stdarg.h>


//=============================================================================
#if defined(_WIN32)
//=============================================================================

#include "x86_hal_driver.h"

//=============================================================================
#else	//	stm32f207
//=============================================================================

#if defined(STM32F207xx)
#include "stm32f2xx_hal.h"
#elif defined(STM32F103xE)	//	STM32F103xC
#include "stm32f1xx_hal.h"
#endif

#if defined(USE_FREERTOS)

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

#endif

//=============================================================================
#endif	//	stm32f207
//=============================================================================

#include "main.h"

#include "serial.h"

#include "rs485.h"

#include "TRSProtocol.h"		//	열차무선 Protocol

//========================================================================
uint16_t onBCCCheck(unsigned char *payload, int crc_length)
//========================================================================
{
	int crc_counter = 0;
	//unsigned short bcc_check;
	unsigned char bccCheck[2] ={0};

	if ((crc_length % 2) == 0) // odd case
	{
		for (crc_counter = 1; crc_counter< crc_length; crc_counter++)
		{
			if (crc_counter % 2) {
				bccCheck[0] ^= payload[crc_counter];
			} else {
				bccCheck[1] ^= payload[crc_counter];
			}
		}
	}
	else // even case
	{
		for (crc_counter = 1; crc_counter< crc_length - 1; crc_counter++)
		{
			if (crc_counter % 2) {
				bccCheck[0] ^= payload[crc_counter];
			} else {
				bccCheck[1] ^= payload[crc_counter];
			}
		}
		bccCheck[1] ^= payload[crc_counter];
	}
	return ((bccCheck[0] << 8) | bccCheck[1]);
}

//========================================================================
uint8_t check_Bcc( unsigned char *payload, int length )
//========================================================================
{
	uint16_t bcc16;
	bcc16 = ( *( payload + length - 1 ) << 8 ) | *( payload + length );
	if ( onBCCCheck( payload, length+1 ) == bcc16 )
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

//========================================================================
int	SendRS485 ( char *bufTx, int nSize )
//========================================================================
{
//	printf( "%s(%d) - %d\n", __func__, __LINE__, nSize );

//=============================================================================
#if defined(_WIN32)
//=============================================================================

//=============================================================================
#else	//	stm32f207
//=============================================================================

	int idx;
	int ret;

	//========================================================================
	//	Send RS485
	rts_high( &huart3 );

	ret = HAL_UART_Transmit( &huart3, (uint8_t *)bufTx, nSize, 0xFFFF );

	rts_low( &huart3 );
	//========================================================================

//=============================================================================
#endif	//	stm32f207
//=============================================================================

	return nSize;
}

//========================================================================
void SendSD( const FRAME_SDR *pSdr )
//========================================================================
{
	FRAME_SD sdfrm;
	memset( &sdfrm, 0, sizeof( sdfrm ) );

	sdfrm.nSTX				=	eSTX;	//	0x02;		//	STX
	sdfrm.sd.cSD			=	eSD;					//	SD

	sdfrm.sd.c0x22			=	0x22;

	sdfrm.sd.bOccPaStart	=	pSdr->sdr.bOccPaStart;
	sdfrm.sd.bOccPaStop		=	pSdr->sdr.bOccPaStop;

	sdfrm.sd.nWatchDog		=	pSdr->sdr.nWatchDog;

	sdfrm.nETX				=	eETX;	//	0x03;		//	ETX

	uint16_t u16BCC = onBCCCheck( (char *)&sdfrm, sizeof(FRAME_SD) - 2 );

	sdfrm.nBCC1				=	(u16BCC >> 8) & 0xFF;	//	BCC 1
	sdfrm.nBCC2				=	(u16BCC & 0xFF);		//	BCC 2

	uint32_t nTxStart, nTxEnd;

	nTxStart = HAL_GetTick();
	SendRS485( &sdfrm, sizeof( FRAME_SD ) );
	nTxEnd = HAL_GetTick();

	Dump( "Rx : ", pSdr, sizeof( FRAME_SDR ) );

	Dump( "Tx : ", &sdfrm, sizeof( FRAME_SD ) );

	printf( "%s : Rx Last(%08d) / Tx Start(%08d) / Tx End(%08d) / delta(%08d)\n", __func__,
			g_nStampRx3, nTxStart, nTxEnd, (nTxStart - g_nStampRx3) );
}

//========================================================================
void ProcessFrameSD( const uint8_t *pBuf, int nLen )
//========================================================================
{
	printf( "%s(%d)\n", __func__, __LINE__ );

	//	LED 상태정보 취합 후 Master 결정.
	//	nLEDID값이 가장 낮은 LED가 Master.

	const FRAME_SD	*pFrameSd = (const FRAME_SD *)pBuf;
	const SD_t		*pSd = &pFrameSd->sd;
}

//========================================================================
void ProcessFrameSDR( const uint8_t *pBuf, int nLen )
//========================================================================
{
	//========================================================================
	//	SD 상태정보 응답.
	SendSD( (FRAME_SDR *)pBuf );

	printf( "%s(%d)\n", __func__, __LINE__ );

	static int s_bOccPaStart = 0;
	static int s_bOccPaStop = 0;
	static int s_bOccPaActive = 0;

	//========================================================================
	//	SDR 처리
	const FRAME_SDR	*pFrameSdr = (const FRAME_SDR *)pBuf;
	const SDR_t		*pSdr = &pFrameSdr->sdr;

	printf("OCC PA : Start(%d) / Stop(%d) / Active(%d)\n",
			pSdr->bOccPaStart,
			pSdr->bOccPaStop,
			pSdr->bOccPaActive
			);

	if ( s_bOccPaStart != pSdr->bOccPaStart )
	{
		if( pSdr->bOccPaStart )
		{
			printf("OCC PA Start\n");
		}

		s_bOccPaStart = pSdr->bOccPaStart;
	}
	if ( s_bOccPaStop != pSdr->bOccPaStop )
	{
		if( pSdr->bOccPaStop )
		{
			printf("OCC PA Stop\n");
		}

		s_bOccPaStop = pSdr->bOccPaStop;
	}
	if ( s_bOccPaActive != pSdr->bOccPaActive )
	{
		if( pSdr->bOccPaActive )
		{
			printf("OCC PA Active\n");
		}

		s_bOccPaActive = pSdr->bOccPaActive;
	}


	//========================================================================
}

//========================================================================
void ProcessFrame( const uint8_t *pBuf, int nLen )
//========================================================================
{
	FRAME_SDR	*pSdr	=	(FRAME_SDR *)pBuf;
	FRAME_SD	*pSd	=	(FRAME_SD *)pBuf;

	if ( nLen == sizeof( FRAME_SDR ) && pSdr->sdr.cSDR == eSDR )
	{
//		printf( "[%d] %s(%d) - SDR Recv ( %d )\n", HAL_GetTick(),  __func__, __LINE__, length );
		ProcessFrameSDR( pBuf, nLen );
	}
	else if ( nLen == sizeof( FRAME_SD ) && pSd->sd.cSD == eSD )
	{
//		printf( "[%d] %s(%d) - SD Recv ( %d )\n", HAL_GetTick(), __func__, __LINE__, length );
//		ProcessFrameSD( pbuf, length );
	}
	else
	{
		printf( "%s(%d) - Invalid Data ( %d )\n", __func__, __LINE__, nLen );
	}
}

enum eRxState
{
	FlagNone = 0,
	FlagFindSTX = 1,
	FlagFindETX = 2,
	FlagFindBCC = 3,
};

static uint32_t		length = 0;
static uint8_t		rxbuffer[128];   //upgrade 수정
static int nFlagRxState = FlagNone;

//========================================================================
void InitRS485(void)
//========================================================================
{
	memset(rxbuffer, 0, sizeof(rxbuffer));

	rts_low(&huart3);		//	Recv Mode
	rts_low(&huart5);		//	Recv Mode

	HAL_UART_Receive_IT(&huart3, dataRx3, 1);
	HAL_UART_Receive_IT(&huart5, dataRx5, 1);
}

//========================================================================
void Dump( char *sTitle, char *sBuf, int nSize )
//========================================================================
{
	printf( "[%08d]%s ", HAL_GetTick(), sTitle );

	int idx;
	for ( idx = 0; idx < nSize; idx++ )
	{
		printf( "%02X ", sBuf[idx] );
	}
	printf("\n");
}

//========================================================================
void LoopProcRS485(void)
//========================================================================
{
	int idx;

	//=============================================================================
	//
	char	c;

	while (qcount(&g_qUart3) > 0)
	{
		c = qget(&g_qUart3);
		//		printf( "0x%02X ", c );

				//=============================================================================
				//	Check STX
		if (c == eSTX && nFlagRxState == FlagNone)
		{
			//			printf( "[STX] " );
			nFlagRxState = FlagFindSTX;
			length = 0;
		}
		else if (c == eETX && nFlagRxState == FlagFindSTX &&
			((length == (sizeof(FRAME_SDR) - 3)) ||	//	SDR ETX
			(length == (sizeof(FRAME_SD) - 3))		//	SD ETX
				)
			)
		{
			//			printf( "[ETX] " );
			nFlagRxState = FlagFindETX;
		}

		if (nFlagRxState == FlagFindSTX || nFlagRxState == FlagFindETX)
		{
			rxbuffer[length++] = c;		//	Buffering
	//			printf( "[%02X] ", c );
		}

		//	Check SDR / SD Frame
		if (nFlagRxState == FlagFindETX)
		{
			//			printf( "length(%d)\n", length );
			if (length == sizeof(FRAME_SDR) ||
				length == sizeof(FRAME_SD))
			{
				//				printf( "found frame(%d)\n", length );
				ProcessFrame(rxbuffer, length);

				length = 0;
				nFlagRxState = FlagNone;
			}
		}

		if (length >= sizeof(FRAME_SDR))
		{
			printf( "[%d] %s(%d) - Invalid packet(%d)\n", HAL_GetTick(), __func__, __LINE__, length );

			Dump( "Rx : ", rxbuffer, length );

			//===========================================================================
			init_queue(&g_qUart3);		//	Queue Clear
			//===========================================================================

			//	최대 패킷 사이즈보다 큰경우. -> Clear Buffer
			length = 0;
			nFlagRxState = FlagNone;

		}
	}	//while (qcount(&g_qRS485) > 0)

//#else
//
//	char c;
//	char buf[50];
//	memset(buf, 0, sizeof(buf));
//	//	STX
//	HAL_StatusTypeDef status;
//	status = HAL_UART_Receive(&huart3, &c, 1, 2000);	// timeout);
//
//	if (status != HAL_OK)
//	{
//		printf(">%02X<", status);
//		if (status == HAL_BUSY || status == HAL_ERROR)
//		{
//			resetSerial(&huart3);
//		}
////		continue;
//
//		return;
//	}
//
//	if (c != eSTX) return; //continue;
//	buf[0] = eSTX;
//	//	Addr
//
//	status = HAL_UART_Receive(&huart3, &c, 1, 2000);	// timeout);
//	if (status != HAL_OK)
//	{
//		printf(">%02X<", status);
//		if (status == HAL_BUSY || status == HAL_ERROR)
//		{
//			resetSerial(&huart3);
//		}
//		return; //continue;
//	}
//
//	if (c != 0x40) return; //continue;
//	buf[1] = 0x40;
//
//	//	Data
//	status = HAL_UART_Receive(&huart3, &buf[2], 26, 2000);	// timeout);
//
//	if (status != HAL_OK)
//	{
//		printf(">%02X<", status);
//		if (status == HAL_BUSY || status == HAL_ERROR)
//		{
//			resetSerial(&huart3);
//		}
//		return; //continue;
//	}
//	printf("[SDR] ");
//
//	printf("[ ");
//	for (idx = 0; idx < sizeof(FRAME_SDR); idx++)
//	{
//		printf("%02X ", buf[idx]);
//	}
//	printf("]\n");
//
//	//	check ETX
//	if (buf[25] == eETX)
//	{
//		ProcessFrame(buf, sizeof(FRAME_SDR));
//	}
//
//#endif
//
}


//========================================================================
int cmd_sd(int argc, char *argv[])
//========================================================================
{
	printf("%s(%d)\n", __func__, __LINE__ );

	FRAME_SDR	sdr;
	memset( &sdr, 0, sizeof(FRAME_SDR) );

	SendSD( &sdr );

	return 0;
}


//========================================================================
void vRs485Task(void *pvParameters)
//========================================================================
{
	/* The parameters are not used. */
	( void ) pvParameters;

	InitRS485();

//      	int8_t	c;
//	extern	xQueueHandle xUart1QueueRx;
 
//	printf( "%s - Current ID = %x\n", __func__, getLedType() );
	
//	HAL_HalfDuplex_Init(&huart1);
//	__HAL_RS485_DISABLE();


	while ( 1 )
	{

		LoopProcRS485();

	}
}

