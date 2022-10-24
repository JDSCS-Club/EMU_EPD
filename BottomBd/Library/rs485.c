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

#include "version.h"

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

#include "occpa.h"				//	OCC PA

#include "naranja_boron.h"
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
	if ( onBCCCheck( payload+1, length-3 ) == bcc16 )
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

/******************************************************************************
	### BCC Check [TEXT~ETX] ###

	Input	: *pDat->수신 데이터 / nLen->수신 데이터 길이
	Output	: BCCOK->TRUE[1] / BCCNG->FALSE[0]
******************************************************************************/
int IsBCCOK(BYTE *pDat,int nLen)
{
	BYTE nBCC[2];
	int i;

	nBCC[0] = nBCC[1] = 0x00;

	for(i=0;i<nLen;i++)
	{
		nBCC[(i&0x01)] ^= pDat[i];
	}

	if(nBCC[0] != pDat[nLen] || nBCC[1] != pDat[nLen+1])
	{
		return FALSE;
	}
	else
	{
		return TRUE;
	}
}


//========================================================================
int	SendRS485 ( char *bufTx, int nSize,int nCh )
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
    if(nCh == 3)
    {
        //	Send RS485
        rts_high( &huart3 );

        ret = HAL_UART_Transmit( &huart3, (uint8_t *)bufTx, nSize, 0xFFFF );

        rts_low( &huart3 );
    }
    else if(nCh == 5)
    {
        //	Send RS485
        rts_high( &huart5 );

        ret = HAL_UART_Transmit( &huart5, (uint8_t *)bufTx, nSize, 0xFFFF );

        rts_low( &huart5 );
        
    }
	//========================================================================

//=============================================================================
#endif	//	stm32f207
//=============================================================================

	return nSize;
}

//========================================================================
void SendSD_3( const FRAME_SDR *pSdr,int nCh )		//	CH3 - TCMS
//========================================================================
{
    static int sRssCnt = 0;
    
	FRAME_SD sdfrm;
	memset( &sdfrm, 0, sizeof( sdfrm ) );

	sdfrm.nSTX				=	eSTX;	//	0x02;		//	STX

	SD_t	*pSd = &sdfrm.sd;

	pSd->cSD				=	eSDTcms;				//	SD
	pSd->c0x22				=	0x22;
	pSd->bRFMFault			=	(g_bRFMOk)?0:1;			//	RFM 모듈통신상태.
	pSd->bLEDOn				=	bCurLedCtr;				//	getLightOn();	//	조명점등상태.
	pSd->bPAOn				=	uAudioPlayFlag;			//	방송출력상태.
	pSd->nBatVal			=	getChargeRate();		//	베터리 잔량 0 ~ 100 %

	pSd->nVerX0_00			=	0;		//	v01.01
	pSd->nVer0X_00			=	APP_MAIN_VER;	//	2;
	pSd->nVer00_X0			=	0;;
	pSd->nVer00_0X			=	APP_SUB_VER;	//	0;

	sdfrm.nETX				=	eETX;	//	0x03;		//	ETX

	uint16_t u16BCC = onBCCCheck( (char *)&sdfrm, sizeof(FRAME_SD) - 2 );

	sdfrm.nBCC1				=	(u16BCC >> 8) & 0xFF;	//	BCC 1
	sdfrm.nBCC2				=	(u16BCC & 0xFF);		//	BCC 2

	uint32_t nTxStart, nTxEnd;

	nTxStart = HAL_GetTick();
	SendRS485((char *) &sdfrm, sizeof( FRAME_SD ),nCh );
	nTxEnd = HAL_GetTick();

	//Dump( "Rx : ", pSdr, sizeof( FRAME_SDR ) );

	Dump( "Tx : ",(char *) &sdfrm, sizeof( FRAME_SD ) );

//	printf( "%s : Rx Last(%08d) / Tx Start(%08d) / Tx End(%08d) / delta(%08d)\n", __func__,
//			g_nStampRx3, nTxStart, nTxEnd, (nTxStart - g_nStampRx3) );
}
//========================================================================
void SendSD_5( const FRAME_SDR *pSdr,int nCh )			//	CH5 - TRS
//========================================================================
{

	FRAME_TRSD sdfrm;
	memset( &sdfrm, 0, sizeof( sdfrm ) );

	sdfrm.nSTX				=	eSTX;	//	0x02;		//	STX

	TRSD_t	*pSd = &sdfrm.sd;

	pSd->cSD				=	eSDTrs;					//	SD

	pSd->c0x22				=	0x22;
	pSd->bRFMFault			=	(g_bRFMOk)?0:1;			//	RFM 모듈통신상태.

	sdfrm.nETX				=	eETX;	//	0x03;		//	ETX

	uint16_t u16BCC = onBCCCheck( (char *)&sdfrm, sizeof(FRAME_TRSD) - 2 );

	sdfrm.nBCC1				=	(u16BCC >> 8) & 0xFF;	//	BCC 1
	sdfrm.nBCC2				=	(u16BCC & 0xFF);		//	BCC 2

	uint32_t nTxStart, nTxEnd;

	nTxStart = HAL_GetTick();
	SendRS485((char *) &sdfrm, sizeof( FRAME_TRSD ),nCh );
	nTxEnd = HAL_GetTick();

	//Dump( "Rx : ", pSdr, sizeof( FRAME_SDR ) );

	//Dump( "Tx : ",(char *) &sdfrm, sizeof( FRAME_SD ) );

//	printf( "%s : Rx Last(%08d) / Tx Start(%08d) / Tx End(%08d) / delta(%08d)\n", __func__,
//			g_nStampRx3, nTxStart, nTxEnd, (nTxStart - g_nStampRx3) );
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
void ProcessFrameSDR( const uint8_t *pBuf, int nLen,int nCh )
//========================================================================
{
	//========================================================================
	//	SD 상태정보 응답.
    
    if(nCh == 3)
    {
    	//	CH3		-	TCMS

    	//	SD 전송.
        SendSD_3( (FRAME_SDR *)pBuf, nCh);

    	//========================================================================
    	//	SDR 처리
    	const FRAME_SDR	*pFrameSdr = (const FRAME_SDR *)pBuf;
    	const SDR_t		*pSdr = &pFrameSdr->sdr;

    	static int s_nTrainNo = -1;

    	//	편성번호값에 따른 편성번호 설정.
    	if ( pSdr->bTrainNoVld )
    	{
    		//	열차번호 설정
    		if ( s_nTrainNo != pSdr->nTrainNo )
			{
    	    	printf("Set Train No: %d\n", pSdr->nTrainNo );

    	    	char sBuf[256];
    	    	sprintf(sBuf, "ts %d\r", pSdr->nTrainNo );

    	    	printf(sBuf);

    	    	//	RFMBase -> RFM 명령 전송 : UART2
    	    	HAL_UART_Transmit( &huart2, (uint8_t *)sBuf, strlen(sBuf), 100 );
			}
    	}

    }
    else if(nCh == 5)
    {
    	//	CH5		-	TRS

        SendSD_5( (FRAME_SDR *)pBuf, nCh);

    	//========================================================================
    	//	SDR 처리
    	const FRAME_TRSDR	*pFrameSdr = (const FRAME_TRSDR *)pBuf;
    	const TRSDR_t		*pSdr = &pFrameSdr->sdr;

    	static int s_nTrsStat = -1;

    	if( s_nTrsStat != pSdr->bTRSFault )
    	{
    		if ( pSdr->bTRSFault )
    		{
    			printf("TRS SD : TRS Fault\n");
    		}
    		else
    		{
    			printf("TRS SD : TRS OK\n");
    		}

    		s_nTrsStat = pSdr->bTRSFault;
    	}
    }
    
/*
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

			//	OCC PA 시작명령. ( RFMBase -> RFM )
			RFMOccPaStart();
		}

		s_bOccPaStart = pSdr->bOccPaStart;
	}
	if ( s_bOccPaStop != pSdr->bOccPaStop )
	{
		if( pSdr->bOccPaStop )
		{
			printf("OCC PA Stop\n");

			//	OCC PA 종료명령. ( RFMBase -> RFM )
			RFMOccPaStop();
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
*/


	//========================================================================
}

//========================================================================
void ProcessFrame( const uint8_t *pBuf, int nLen,int nCh )
//========================================================================
{
	FRAME_SDR	*pSdr	=	(FRAME_SDR *)pBuf;
	FRAME_SD	*pSd	=	(FRAME_SD *)pBuf;

	if ( nLen == sizeof( FRAME_SDR ) && pSdr->sdr.cSDR == eSDRTcms )
	{
		//HAL_Delay(10);
		printf( "[%d] %s(%d) - TCMS SDR Recv ( %d )\n", HAL_GetTick(),  __func__, __LINE__, nLen );
		ProcessFrameSDR( pBuf, nLen, 3 );
	}
    else if ( nLen == sizeof( FRAME_TRSDR ) && pSdr->sdr.cSDR == eSDRTrs )
    {
		printf( "[%d] %s(%d) - TRS SDR Recv ( %d )\n", HAL_GetTick(),  __func__, __LINE__, nLen );
        ProcessFrameSDR( pBuf, nLen, 5 );
    }
//	else if ( nLen == sizeof( FRAME_SD ) && pSd->sd.cSD == eSD )
//	{
//        ProcessFrameSDR( pBuf, nLen, nCh );
////		printf( "[%d] %s(%d) - SD Recv ( %d )\n", HAL_GetTick(), __func__, __LINE__, length );
////		ProcessFrameSD( pbuf, length );
//	}
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

static uint32_t		length_2 = 0;
static uint8_t		rxbuffer_2[128];   //upgrade 수정

static uint32_t		length_5 = 0;
static uint8_t		rxbuffer_5[128];   //upgrade 수정

static int nFlagRxState = FlagNone;

//========================================================================
void InitRS485(void)
//========================================================================
{
	memset(rxbuffer, 0, sizeof(rxbuffer));
    memset(rxbuffer_2, 0, sizeof(rxbuffer_2));
    memset(rxbuffer_5, 0, sizeof(rxbuffer_5));

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
void LoopProcRS485_3ch(void)		//	TCMS
//========================================================================
{

	  int nTick;
	  static int s_nTick_F = 0;
	  static int s_nTick_R = 0;
	  static int s_RxnTick;

	  static int s_RxOkFlag =0;
	  static int s_RxOkLen =0;

	  int s_EtxLen = 0;

	int idx;

	//=============================================================================
     int nTick_2;
	 static int s_2_nTick_F = 0;
	 static int s_2_nTick_R = 0;
     char d;
     
        
    //=============================================================================
	//
	char	c;

	nTick = HAL_GetTick();

	while (qcount(&g_qUart3) > 0)
	{

		s_nTick_F = nTick;

		// 통신 중간에 타임 아웃시 카운터 클리어 하는 부분 추가.
		if ( (s_nTick_F - s_nTick_R) >= 5){  length = 0; printf("C"); }
//		if ( (s_nTick_F - s_nTick_R) >= 1000){  length = 0; printf("C"); }
		s_nTick_R = s_nTick_F;

		c = qget(&g_qUart3);
//				printf( "0x%02X ", c );

		rxbuffer[length++] = c;		//	Buffering

		s_EtxLen = (sizeof(FRAME_SDR) - 3);

		switch(length)
		{
		case 1: if(rxbuffer[0] != 0x02) length = 0; break;
		case 2: if(rxbuffer[1] != 0x21) length = 0; break;
		case 3: if(rxbuffer[2] != 0x11) length = 0; break;
		default:
			if(length >= sizeof(FRAME_SDR))
			{
				if(rxbuffer[s_EtxLen] == 0x03 )//&& IsBCCOK(&rxbuffer[1],(length-3)))
				{
//					printf("OK");
					s_RxOkFlag = 1;
					s_RxOkLen = length;

					s_RxnTick = nTick;

					Dump( "Rx : ", rxbuffer, length );

					length = 0;
				}
				else
				{
					printf( "[%d] %s(%d) - Invalid packet(%d) / framesize(%d)\n", HAL_GetTick(), __func__, __LINE__, length, sizeof(FRAME_SDR) );

					//===========================================================================
					init_queue(&g_qUart3);		//	Queue Clear
					//===========================================================================

					//	최대 패킷 사이즈보다 큰경우. -> Clear Buffer
					length = 0;
				}
			}
			break;
		}
	}

	// RX 수신 OK 하면.
//	if ( s_RxOkFlag == 1)
	if ( ((nTick - s_RxnTick) >= 10) && (s_RxOkFlag == 1))
	{
		s_RxOkFlag = 0;
		s_RxnTick = nTick;

		ProcessFrame(rxbuffer, s_RxOkLen,3);
	}

	//=============================================================================

}

//========================================================================
void LoopProcRS485_5ch(void)		//	TRS
//========================================================================
{
	  int nTick;
	  static int s_nTick_F = 0;
	  static int s_nTick_R = 0;
	  static int s_RxnTick;

	  static int s_RxOkFlag =0;
	  static int s_RxOkLen =0;

	  int s_EtxLen = 0;

	int idx;

	//=============================================================================
     int nTick_2;
	 static int s_2_nTick_F = 0;
	 static int s_2_nTick_R = 0;
     char d;
     
        
    //=============================================================================
	//
	char	c;

	nTick = HAL_GetTick();

	while (qcount(&g_qUart5) > 0)
	{

		s_nTick_F = nTick;

		// 통신 중간에 타임 아웃시 카운터 클리어 하는 부분 추가.
		if ( (s_nTick_F - s_nTick_R) >= 5){  length_5 = 0; printf("C"); }
//		if ( (s_nTick_F - s_nTick_R) >= 100){  length_5 = 0; printf("C"); }
		s_nTick_R = s_nTick_F;

		c = qget(&g_qUart5);
//				printf( "0x%02X ", c );

		rxbuffer_5[length_5++] = c;		//	Buffering

		s_EtxLen = (sizeof(FRAME_TRSDR) - 3);

		switch(length_5)
		{
		case 1: if(rxbuffer_5[0] != 0x02) length_5 = 0; break;
		case 2: if(rxbuffer_5[1] != 0x22) length_5 = 0; break;
		case 3: if(rxbuffer_5[2] != 0x11) length_5 = 0; break;
		default:
			if(length_5 >= sizeof(FRAME_TRSDR))
			{
				if(rxbuffer_5[s_EtxLen] == 0x03)// && IsBCCOK(&rxbuffer_5[1],(length_5-3)))
				{
					s_RxOkFlag = 1;
					s_RxOkLen = length_5;

					s_RxnTick = nTick;

					Dump( "Rx : ", rxbuffer_5, length_5 );

					length_5 = 0;
				}
				else
				{
					printf( "[%d] %s(%d) - Invalid packet(%d) FrameSize(%d)\n", HAL_GetTick(), __func__, __LINE__, length_5, sizeof(FRAME_TRSDR) );

					//===========================================================================
					init_queue(&g_qUart5);		//	Queue Clear
					//===========================================================================

					//	최대 패킷 사이즈보다 큰경우. -> Clear Buffer
					length_5 = 0;
				}
			}
		}
	}
	
	// RX 수신 OK 하면.
	if ( ((nTick - s_RxnTick) >= 10) && (s_RxOkFlag == 1))
	{
		s_RxOkFlag = 0;
		s_RxnTick = nTick;

		ProcessFrame(rxbuffer_5, s_RxOkLen,5);
	}
}

//========================================================================
void LoopProcRFM_2ch(void)
//========================================================================
{

	  int nTick;
	  static int s_nTick_F = 0;
	  static int s_nTick_R = 0;
	  static int s_RxnTick;

	  static int s_RxOkFlag =0;
	  static int s_RxOkLen =0;

	  int s_EtxLen = 0;

	int idx;

	//=============================================================================
     int nTick_2;
	 static int s_2_nTick_F = 0;
	 static int s_2_nTick_R = 0;
     char d;
     
        
    //=============================================================================
	//
	char	c;

	nTick = HAL_GetTick();

	while (qcount(&g_qUart2) > 0)
	{

		s_nTick_F = nTick;

		// 통신 중간에 타임 아웃시 카운터 클리어 하는 부분 추가.
		if ( (s_nTick_F - s_nTick_R) >= 5){  length_2 = 0; }
		s_nTick_R = s_nTick_F;

		c = qget(&g_qUart2);
		//		printf( "0x%02X ", c );

		rxbuffer_2[length_2++] = c;		//	Buffering

		s_EtxLen = (sizeof(FRAME_SDR) - 3);

		switch(length_2)
		{
		case 1:  break;
		default:
			if(length_2 >= 7 )
			{
				if (strncmp((char*)(rxbuffer_2), "RSSI_NG", 7) == 0)
				{
					if(uRssi_NgFlag < 6)
					{
						uRssi_NgFlag++;
					}

					 Dump( "Rx : ", rxbuffer_2, length_2 );

					 length_2 = 0;

				}
				else
				{
					init_queue(&g_qUart2);		//	Queue Clear
					
					length_2 = 0;
				}

			}
			break;
		}
	}
	
	// RX 수신 OK 하면.
	if ( ((nTick - s_RxnTick) >= 10) && (s_RxOkFlag == 1))
	{
		s_RxOkFlag = 0;
		s_RxnTick = nTick;

		//ProcessFrame(rxbuffer_2, s_RxOkLen);
	}
}

//========================================================================
int cmd_sd(int argc, char *argv[])
//========================================================================
{
	printf("%s(%d)\n", __func__, __LINE__ );

	FRAME_SDR	sdr;
	memset( &sdr, 0, sizeof(FRAME_SDR) );

	SendSD_3( &sdr, 3 );

	return 0;
}

//========================================================================
int cmd_sd2(int argc, char *argv[])
//========================================================================
{
	printf("%s(%d)\n", __func__, __LINE__ );

	FRAME_SDR	sdr;
	memset( &sdr, 0, sizeof(FRAME_SDR) );

	SendSD_5( &sdr, 5 );

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
		LoopProcRS485_3ch();

	}
}

