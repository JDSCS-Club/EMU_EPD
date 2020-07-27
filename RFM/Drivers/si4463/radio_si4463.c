/*------------------------------------------------------------------------------------------
	Project			: RFM - Radio
	Description		: 

	Writer			: $Author: zlkit $
	Revision		: $Rev: 1891 $
	Date			: 2019. 10.
	Copyright		: Piranti Corp.	( sw@piranti.co.kr )
	 
	Revision History 
	1. 2019. 10.	: Created
-------------------------------------------------------------------------------------------*/

//========================================================================
// Header

//=============================================================================
#if defined(_WIN32)
//=============================================================================

#include "x86_hal_driver.h"

//=============================================================================
#else	//	stm32f207
//=============================================================================

#if defined(STM32F407xx)
#include "stm32f4xx_hal.h"
#elif defined(STM32F207xx)
#include "stm32f2xx_hal.h"
#endif

//=============================================================================
#endif
//=============================================================================

#if defined(USE_FREERTOS)
#include "cmsis_os.h"	//	osDelay
#endif

#include <stdio.h>		//	printf

#include "main.h"		//	hspi1

#include "radio_si4463.h"

#include "si4463.h"

//#include "serial.h"		//	input_check()
//#include "audio.h"		//	qBufPut()

#include <stdlib.h>

//========================================================================
// Define

//#define		SHOW_RF_DATA	1


si4463_t si4463;
uint8_t incomingBuffer[APP_PACKET_LEN];
uint8_t outgoingBuffer[APP_PACKET_LEN];

static uint8_t	s_nDbgLevel = 0;

#define			DBG( arg ... )
//#define			DBG		printf

//#define		DEBUG	1

//========================================================================
// Function

static int s_bInit = false;

//========================================================================
static int s_cntRFTx = 0;
static int s_cntRFRx = 0;
//========================================================================

//========================================================================
int			RF_IsInit				( void )
//========================================================================
{
	return s_bInit;
}

//========================================================================
int			nonDelay				( int nMSec )
//========================================================================
{
	return 0;
}


//========================================================================
void	RF_Init	( void )
//========================================================================
{
	printf( "%s(%d)\n", __func__, __LINE__ );

	/* Assign functions */
	si4463.IsClearToSend	=	SI4463_IsCTS;
	si4463.WriteRead		=	SI4463_WriteRead;
	si4463.Select			=	SI4463_Select;
	si4463.Deselect			=	SI4463_Deselect;
	si4463.SetShutdown		=	SI4463_SetShutdown;
	si4463.ClearShutdown	=	SI4463_ClearShutdown;
//DEL	si4463.DelayMs = osDelay;
//DEL	si4463.DelayMs = vTaskDelay;	//	tick
	si4463.DelayMs			=	HAL_Delay;		//	Ok - ���������� DMA�� ����.
//DEL	si4463.DelayMs = nonDelay;		//	Non-Delay - ���۾���. / DMA�� ������ ��ġ�� ����.

	/* Disable interrupt pin for init Si4463 */
	HAL_NVIC_DisableIRQ( EXTI0_IRQn );

	/* Init Si4463 with structure */

	int8_t ret;

	ret = SI4463_Init( &si4463 );

	printf( "%s(%d) - ret(%d)\n", __func__, __LINE__, ret );

	/* Clear RX FIFO before starting RX packets */
	SI4463_ClearRxFifo( &si4463 );
	/* Start RX mode.
	 * SI4463_StartRx() put a chip in non-armed mode in cases:
	 * - successfully receive a packet;
	 * - invoked RX_TIMEOUT;
	 * - invalid receive.
	 * For receiveing next packet you have to invoke SI4463_StartRx() again!*/
//	SI4463_StartRx( &si4463, 32, false, false, false );
	SI4463_StartRx( &si4463, 64, false, false, false );

#ifdef DEMOFEST
	/* Debug message on UART */
	//HAL_UART_Transmit(&huart1, "INIT\n", 5, 10);
#endif

  /* Enable interrupt pin and */
	HAL_NVIC_EnableIRQ( EXTI0_IRQn );
	/* Clear interrupts after enabling interrupt pin.
	 * Without it may be situation when interrupt is asserted but pin not cleared.*/
	SI4463_ClearInterrupts( &si4463 );

	//========================================================================
	s_bInit = true;
	//========================================================================
}


//========================================================================
void RF_PartInfo( void )
//========================================================================
{
	char nBuf[10];
	int i;
	int ret;

	memset( nBuf, 0, sizeof( nBuf ) );

	ret = SI4463_GetPartInfo( &si4463, (uint8_t *)nBuf );

	printf( "%s(%d) - ret(%d)\n", __func__, __LINE__, ret );

	printf( " - CMD_COMP    : %02X\n", nBuf[1] );
	printf( " - CHIPREV     : %02X\n", nBuf[2] );
	printf( " - PART[15:8]  : %02X\n", nBuf[3] );
	printf( " - PART[7:0]   : %02X\n", nBuf[4] );
	printf( " - PBUILD      : %02X\n", nBuf[5] );
	printf( " - ID[15:8]    : %02X\n", nBuf[6] );
	printf( " - ID[7:0]     : %02X\n", nBuf[7] );
	printf( " - CUSTOMER    : %02X\n", nBuf[8] );
	printf( " - ROMID       : %02X\n", nBuf[9] );

}


//========================================================================
void RF_FuncInfo( void )
//========================================================================
{
	char nBuf[10];
	int i;
	int ret;

	memset( nBuf, 0, sizeof( nBuf ) );

	ret = SI4463_GetFuncInfo( &si4463, (uint8_t *)nBuf );

	printf( "%s(%d) - ret(%d)\n", __func__, __LINE__, ret );

	printf( " - CMD_COMP    : %02X\n", nBuf[1] );
	printf( " - REVEXT      : %02X\n", nBuf[2] );
	printf( " - REVBRANCH   : %02X\n", nBuf[3] );
	printf( " - REVINT      : %02X\n", nBuf[4] );
	printf( " - PATCH[15:8] : %02X\n", nBuf[5] );
	printf( " - PATCH[7:0]  : %02X\n", nBuf[6] );
	printf( " - FUNC        : %02X\n", nBuf[7] );
}

//========================================================================
void RF_Info( void )
//========================================================================
{
	RF_PartInfo();

	RF_FuncInfo();

	//while ( 1 )
	//{
	//	HAL_Delay( 200 );

	//	if ( input_check() != 0 )
	//	{
	//		//	Ű�Է� �޴� ��� break;
	//		break;
	//	}
	//}
}

static int s_bIsTxMode = 0;

//========================================================================
void RF_Tx( void )
//========================================================================
{
	RF_TX_GPIO_Port->ODR |= RF_TX_Pin;	//	Tx High
	RF_RX_GPIO_Port->ODR &= ~RF_RX_Pin;	//	Rx Low

	RX_EN_GPIO_Port->ODR &= ~RX_EN_Pin;	//	Rx Disable

	s_bIsTxMode = 1;
}

//========================================================================
void RF_Rx( void )
//========================================================================
{
	RF_RX_GPIO_Port->ODR |= RF_RX_Pin;	//	Rx High
	RF_TX_GPIO_Port->ODR &= ~RF_TX_Pin;	//	Tx Low

	RX_EN_GPIO_Port->ODR |= RX_EN_Pin;	//	Rx Enable

	s_bIsTxMode = 0;
}

//========================================================================
void RF_Loopback( void )
//========================================================================
{
	RF_TX_GPIO_Port->ODR |= RF_TX_Pin;	//	Tx High
	RF_RX_GPIO_Port->ODR |= RF_RX_Pin;	//	Rx Low

	RX_EN_GPIO_Port->ODR |= RX_EN_Pin;	//	Rx Disable

	s_bIsTxMode = 1;
}

//========================================================================
void		RF_SendData				( char *sBuf, int nSize )
//========================================================================
{
	if ( s_bInit == false ) return;

	if ( nSize > APP_PACKET_LEN ) nSize = APP_PACKET_LEN;

	SI4463_Transmit( &si4463, (uint8_t *)sBuf, nSize );
}


//========================================================================
void		RF_RecvData				( char *sBuf, int nSize )
//========================================================================
{
	//========================================================================
	//	Callback Func
	if ( nSize > APP_PACKET_LEN ) nSize = APP_PACKET_LEN;

//	SI4463_Transmit( &si4463, sBuf, nSize );

	//	Audio Buffer Copy
	QPutAudioStream( sBuf, nSize );
}

//========================================================================
void	SI4463_Debug	( int nDbgLevel )
//========================================================================
{
	//	0 : Disable / 1 : Print Data / 2 : Detail
	s_nDbgLevel = nDbgLevel;
}

//========================================================================
void	SI4463_Test		( void )
//========================================================================
{
	int idx;
	int nCnt = 0;
	int i;

	char arrBuf[2][7] = {
		{0x44, 0x47, 0x57, 0x5E, 0x22, 0xCD, 0x65},
		{0xE0, 0xB3, 0x53, 0xAA, 0x7E, 0xF9, 0x21},
	};
	int	idxArr = 0;

	int currTime, oldTime, dtTime;

	int dataBps;

	oldTime = HAL_GetTick();	// xTaskGetTickCount();

	while ( 1 )
	{
		/* Send test packet */
		currTime = HAL_GetTick();	// xTaskGetTickCount();

		printf( "\n" );
		printf( "[%d][%03d] ", nCnt++, currTime );

//		printf( "." );

		if ( s_bIsTxMode )
		{
			dtTime = currTime - oldTime;

			oldTime = currTime;

			if ( dtTime != 0 )
			{
				printf( "Speed : %d bps\n", 6400000 / dtTime );
			}

			/*
			outgoingBuffer[0] = rand() & 0xFF;
			outgoingBuffer[1] = rand() & 0xFF;
			outgoingBuffer[2] = rand() & 0xFF;
			outgoingBuffer[3] = rand() & 0xFF;
			outgoingBuffer[4] = rand() & 0xFF;
			outgoingBuffer[5] = rand() & 0xFF;
			outgoingBuffer[6] = rand() & 0xFF;
			/*/

//			idxArr = ( idxArr + 1 ) % 2;
			idxArr++;
			/*
			memset( outgoingBuffer, 0x00, APP_PACKET_LEN );
			/*/
			for ( idx = 0; idx < APP_PACKET_LEN; idx++ )
			{
				outgoingBuffer[idx] = idxArr + idx;
			}
			//	*/

//			outgoingBuffer[0] = 0xE0;
//			outgoingBuffer[1] = 0xB3;
//			outgoingBuffer[2] = 0x53;
//			outgoingBuffer[3] = 0xAA;
//			outgoingBuffer[4] = 0x7E;
//			outgoingBuffer[5] = 0xF9;
//			outgoingBuffer[6] = 0x21;
			//	*/

//				[046] !
//				Tx:44 47 57 5E 22 CD 65 !
//
//				[047] !
//				Tx : E0 B3 53 AA 7E F9 21 !

			//	E0 B3 53 AA 7E F9 21

			for ( i = 0; i < 100; i++ )
			{
//				HAL_Delay( 10 );
				SI4463_Transmit( &si4463, outgoingBuffer, APP_PACKET_LEN );

				for ( idx = 0; idx < APP_PACKET_LEN; idx++ )
				{
					outgoingBuffer[idx]++;
				}
			}

			printf( "Tx Cnt: %d\n", s_cntRFTx );

			/*
			printf( "Tx:" );
			for ( idx = 0; idx < APP_PACKET_LEN; idx++ )
			{
				printf( "%02X ", outgoingBuffer[idx] );
			}
			//	*/

//			for ( idx = 0; idx < 100; idx++ )
//			{
//				HAL_Delay( 10 );
//			}
//			
//			HAL_Delay( 500 );
			//			uint32_t newDelay = 500 + ( ( rand() & 0xF ) * 100 );
//			HAL_Delay( newDelay );

			/*
			SI4463_GetChipStatus( &si4463 );
			SI4463_GetInterrupts( &si4463 );

//			HAL_Delay( 200 );

			if ( si4463.interrupts.packetSent )
			{
				printf( "[Send Comp.] " );
			}
			*/
		}
		else
		{
//			SI4463_ClearRxFifo( &si4463 );
//			SI4463_StartRx( &si4463, 32, false, false, false );

			HAL_Delay( 1000 );

//			SI4463_GetChipStatus( &si4463 );
//			SI4463_GetInterrupts( &si4463 );
//
//			if ( si4463.interrupts.packetRx )
//			{
//				printf( "Rx: %d\n", s_cntRFRx );
//			}

			printf( "Rx Cnt: %d\n", s_cntRFRx );
		}


		if ( input_check() != 0 )
		{
			//	Ű�Է� �޴� ��� break;
			printf( "TxBuf: " );
			for ( idx = 0; idx < APP_PACKET_LEN; idx++ )
			{
				printf( "%02X ", outgoingBuffer[idx] );
			}
			printf( "\n" );
			printf( "RxBuf: " );
			for ( idx = 0; idx < APP_PACKET_LEN; idx++ )
			{
				printf( "%02X ", incomingBuffer[idx] );
			}
			printf( "\n" );

			break;
		}

		/* End of send of test packet */
	}
}


//========================================================================
uint8_t	SI4463_IsCTS	( void )
//========================================================================
{
//DEL	return 1;	//	CTS Signal Not connected

	if ( HAL_GPIO_ReadPin( CTS_GPIO_Port, CTS_Pin ) == GPIO_PIN_SET )
	{
//		printf( "%s(%d) - SET\n", __func__, __LINE__ );
		return 1;
	}
	else
	{
//		printf( "%s(%d) - Clear\n", __func__, __LINE__ );
		return 0;
	}
}

//========================================================================
void	SI4463_WriteRead( const uint8_t * pTxData, uint8_t * pRxData, const uint16_t sizeTxData )
//========================================================================
{
	//	int i;
	//
	//	if ( s_nDbgLevel >= 2 )
	//	{
	//		printf( "Tx : " );
	//		for ( i = 0; i < sizeTxData; i++ )
	//		{
	//			printf( "%02X ", pTxData[i] );
	//		}
	//		printf( "\n" );
	//	}

	//	SPI1_NSS_GPIO_Port->ODR &= ~SPI1_NSS_Pin;	//	NSS Low

#if defined(USE_SI4464_DMA_SPI)

	s_bSI4464_DMA_Cplt = 1;

	//	DMA���.
	HAL_SPI_TransmitReceive_DMA( &hspi1, pTxData, pRxData, sizeTxData );

	while ( s_bSI4464_DMA_Cplt )
	{
		//	DMA�Ϸ�ñ��� Blocking
#if defined(USE_FREERTOS)
		osDelay( 0 );
#else
		HAL_Delay( 0 );
#endif
	}

#else	//	defined(USE_SI4464_DMA_SPI)

	//	
	HAL_SPI_TransmitReceive( &hspi1, (uint8_t *)pTxData, (uint8_t *)pRxData, sizeTxData, 100 );

#endif


//	SPI1_NSS_GPIO_Port->ODR |= SPI1_NSS_Pin;	//	NSS High


//	if ( s_nDbgLevel >= 2 )
//	{
//
//		printf( "Rx : " );
//		for ( i = 0; i < sizeTxData; i++ )
//		{
//			printf( "%02X ", pRxData[i] );
//		}
//		printf( "\n" );
//	}

}

//========================================================================
void	SI4463_SetShutdown( void )
//========================================================================
{
	printf( "[%d] Pin_SDN(1)\n", HAL_GetTick() );	// xTaskGetTickCount() );

	HAL_GPIO_WritePin( SHUTDOWN_GPIO_Port, SHUTDOWN_Pin, GPIO_PIN_SET );

//	printf( "%s(%d)\n", __func__, __LINE__ );
}

//========================================================================
void	SI4463_ClearShutdown( void )
//========================================================================
{
	printf( "[%d] Pin_SDN(0)\n", HAL_GetTick() );	// xTaskGetTickCount() );

	HAL_GPIO_WritePin( SHUTDOWN_GPIO_Port, SHUTDOWN_Pin, GPIO_PIN_RESET );

//	printf( "%s(%d)\n", __func__, __LINE__ );
}

//========================================================================
void	SI4463_Select( void )
//========================================================================
{
	HAL_GPIO_WritePin( nSEL_GPIO_Port, nSEL_Pin, GPIO_PIN_RESET );

//	printf( "%s(%d)\n", __func__, __LINE__ );
}

//========================================================================
void	SI4463_Deselect( void )
//========================================================================
{

#if defined(USE_SI4464_DMA_SPI)

	//	DMA��忡���� Complate Interrupt���� ó��.

#else

	HAL_GPIO_WritePin( nSEL_GPIO_Port, nSEL_Pin, GPIO_PIN_SET );

#endif

//	printf( "%s(%d)\n", __func__, __LINE__ );
}

#if defined(USE_SI4464_INT_FLAG)

static int	s_bIntFlag = 0;				//	Interrupt Flag �߻�����.

//========================================================================
int		SI4463_GetIntFlag()
//========================================================================
{
	return s_bIntFlag;
}

//========================================================================
void	SI4463_SetIntFlag( int bIntFlag )
//========================================================================
{
	s_bIntFlag = bIntFlag;
}

#endif	//	defined(USE_SI4464_INT_FLAG)



#if 0		//	main.c �� �̵�.
//========================================================================
void HAL_GPIO_EXTI_Callback( uint16_t GPIO_Pin )
//========================================================================
#else
void SI4463_INT_Callback( uint16_t GPIO_Pin )
#endif
{
	/* Prevent unused argument(s) compilation warning */
	UNUSED( GPIO_Pin );

#if defined(USE_SI4464_INT_FLAG)

	SI4463_SetIntFlag( 1 );

#else

	//========================================================================
	//	Interrupt Delay���.
	//	HAL_Delay()
	//		Interrupt���� HAL_Delay()���� DMA�� ������ �޾� DMA�� Block��.
	//	osDelay()
	//		Interrupt���� osDelay()���� ���α׷� ����.
	//========================================================================

	//	Interrupt
	SI4463_Interrupt();

#endif // defined(USE_SI4464_INT_FLAG)
}


//========================================================================
void	SI4463_Interrupt( void )
//========================================================================
{
	int idx;

	/* Get interrupts and work with it */
	SI4463_GetInterrupts( &si4463 );

	/* Handling PH interrupts */
	if ( si4463.interrupts.filterMatch )
	{
		/* Handling this interrupt here */
		/* Following instruction only for add breakpoints. May be deleted */
		si4463.interrupts.filterMatch = false;

		DBG( "<%d>\n", __LINE__ );
	}
	if ( si4463.interrupts.filterMiss )
	{
		/* Handling this interrupt here */
		/* Following instruction only for add breakpoints. May be deleted */
		si4463.interrupts.filterMiss = false;

		DBG( "<%d>\n", __LINE__ );
	}
	if ( si4463.interrupts.packetSent )
	{
		//========================================================================
		/* Handling this interrupt here */
		/* Clear TX FIFO */
		SI4463_ClearTxFifo( &si4463 );

		if ( s_nDbgLevel >= 1 )
		{
			printf( "[Sent]Tx: " );
			for ( idx = 0; idx < APP_PACKET_LEN; idx++ )
			{
				printf( "%02X ", outgoingBuffer[idx] );
			}
			printf( "\n" );
		}

		/* Re-arm StartRX */
		SI4463_StartRx( &si4463, APP_PACKET_LEN, false, false, false );

		/*Toggle led for indication*/
//		HAL_GPIO_TogglePin( LED_ONBOARD_GPIO_Port, LED_ONBOARD_Pin );
		/* Following instruction only for add breakpoints. May be deleted */
		si4463.interrupts.packetSent = false;

		s_cntRFTx++;			//	RF Tx Count

		DBG( "[Sent]\n" );
		//========================================================================
	}
	if ( si4463.interrupts.packetRx )
	{
		//========================================================================
		/* Handling this interrupt here */

		/* Clear incoming buffer */
		memset( incomingBuffer, 0x00, APP_PACKET_LEN );

		/* Get FIFO data */
		SI4463_ReadRxFifo( &si4463, incomingBuffer, APP_PACKET_LEN );

		RF_RecvData( (char *)incomingBuffer, APP_PACKET_LEN );

		if ( s_nDbgLevel >= 1 )
		{
			printf( "[%d]Rx: ", HAL_GetTick() );	//	xTaskGetTickCount() );
			for ( idx = 0; idx < APP_PACKET_LEN; idx++ )
			{
				printf( "%02X ", incomingBuffer[idx] );
			}
			printf( "\n" );
		}

		/* Clear RX FIFO */
		SI4463_ClearRxFifo( &si4463 );

		/* Start RX again.
		 * It need because after successful receive a packet the chip change
		 * state to READY.
		 * There is re-armed mode for StartRx but it not correctly working */
		SI4463_StartRx( &si4463, APP_PACKET_LEN, false, false, false );

		/*Toggle led for indication*/
//		HAL_GPIO_TogglePin( LED_ONBOARD_GPIO_Port, LED_ONBOARD_Pin );

		/* Following instruction only for add breakpoints. May be deleted */
		si4463.interrupts.packetRx = false;

		s_cntRFRx++;			//	RF Rx Count

		//========================================================================
	}
	if ( si4463.interrupts.crcError )
	{
		/* Handling this interrupt here */

		/* Following instruction only for add breakpoints. May be deleted */
		si4463.interrupts.crcError = false;
		DBG( "<%d>\n", __LINE__ );
	}
	if ( si4463.interrupts.txFifoAlmostEmpty )
	{
		/* Handling this interrupt here */

		/* Following instruction only for add breakpoints. May be deleted */
		si4463.interrupts.txFifoAlmostEmpty = false;
		DBG( "<%d>\n", __LINE__ );
	}
	if ( si4463.interrupts.rxFifoAlmostFull )
	{
		/* Handling this interrupt here */

		/* Following instruction only for add breakpoints. May be deleted */
		si4463.interrupts.rxFifoAlmostFull = false;
		DBG( "<%d>\n", __LINE__ );
	}

	/* Handling Modem interrupts */
	if ( si4463.interrupts.postambleDetect )
	{
		/* Handling this interrupt here */

		/* Following instruction only for add breakpoints. May be deleted */
		si4463.interrupts.postambleDetect = false;
		DBG( "<%d>\n", __LINE__ );
	}
	if ( si4463.interrupts.invalidSync )
	{
		/* Handling this interrupt here */
		/* Following instruction only for add breakpoints. May be deleted */
		si4463.interrupts.invalidSync = false;
		DBG( "<%d>\n", __LINE__ );
	}
	if ( si4463.interrupts.rssiJump )
	{
		/* Handling this interrupt here */
		/* Following instruction only for add breakpoints. May be deleted */
		si4463.interrupts.rssiJump = false;
		DBG( "<%d>\n", __LINE__ );
	}
	if ( si4463.interrupts.rssi )
	{
		/* Handling this interrupt here */
		/* Following instruction only for add breakpoints. May be deleted */
		si4463.interrupts.rssi = false;
		DBG( "<%d>\n", __LINE__ );
	}
	if ( si4463.interrupts.invalidPreamble )
	{
		/* Handling this interrupt here */
		/* Following instruction only for add breakpoints. May be deleted */
		si4463.interrupts.invalidPreamble = false;
		DBG( "<%d>\n", __LINE__ );
	}
	if ( si4463.interrupts.preambleDetect )
	{
		/* Handling this interrupt here */
		/* Following instruction only for add breakpoints. May be deleted */
		si4463.interrupts.preambleDetect = false;
		DBG( "<%d>\n", __LINE__ );
	}
	if ( si4463.interrupts.syncDetect )
	{
		/* Handling this interrupt here */
		/* Following instruction only for add breakpoints. May be deleted */
		si4463.interrupts.syncDetect = false;
		DBG( "<%d>\n", __LINE__ );
	}

	/* Handling Chip interrupts */
	if ( si4463.interrupts.cal )
	{
		/* Handling this interrupt here */
		SI4463_GetChipStatus( &si4463 );
		/* Following instruction only for add breakpoints. May be deleted */
		si4463.interrupts.cal = false;
		DBG( "<%d>\n", __LINE__ );
	}
	if ( si4463.interrupts.fifoUnderflowOverflowError )
	{
		/* Handling this interrupt here */
		/* Clear RX FIFO */
		SI4463_ClearRxFifo( &si4463 );
		/* Claer Chip Status errors if exists */
		SI4463_GetChipStatus( &si4463 );
		/* Following instruction only for add breakpoints. May be deleted */
		si4463.interrupts.fifoUnderflowOverflowError = false;
		DBG( "<%d>\n", __LINE__ );
	}
	if ( si4463.interrupts.stateChange )
	{
		/* Handling this interrupt here */
		SI4463_GetChipStatus( &si4463 );
		/* Following instruction only for add breakpoints. May be deleted */
		si4463.interrupts.stateChange = false;
		DBG( "<%d>\n", __LINE__ );
	}
	if ( si4463.interrupts.cmdError )
	{
		/* Handling this interrupt here */
		SI4463_GetChipStatus( &si4463 );
		/* Following instruction only for add breakpoints. May be deleted */
		si4463.interrupts.cmdError = false;
		DBG( "<%d>\n", __LINE__ );
	}
	if ( si4463.interrupts.chipReady )
	{
		/* Handling this interrupt here */
		SI4463_GetChipStatus( &si4463 );
		/* Following instruction only for add breakpoints. May be deleted */
		si4463.interrupts.chipReady = false;
		DBG( "<%d>\n", __LINE__ );
	}
	if ( si4463.interrupts.lowBatt )
	{
		/* Handling this interrupt here */
		SI4463_GetChipStatus( &si4463 );
		/* Following instruction only for add breakpoints. May be deleted */
		si4463.interrupts.lowBatt = false;
		DBG( "<%d>\n", __LINE__ );
	}
	if ( si4463.interrupts.wut )
	{
		/* Handling this interrupt here */
		SI4463_GetChipStatus( &si4463 );
		/* Following instruction only for add breakpoints. May be deleted */
		si4463.interrupts.wut = false;
		DBG( "<%d>\n", __LINE__ );
	}

	/* Clear All interrupts before exit */
	SI4463_ClearAllInterrupts( &si4463 );

	DBG( "!\n" );
}


//========================================================================
