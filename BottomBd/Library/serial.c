
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ctype.h>
#include <string.h>
#include <stdarg.h>
#include <stdint.h>

#include "serial.h"

//=============================================================================
#if defined(_WIN32)
//=============================================================================

#include "x86_hal_driver.h"

//=============================================================================
#else	//	stm32f207
//=============================================================================

#if defined(USE_FREERTOS)
#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"
#endif

#include "main.h"

#if defined(STM32F407xx)
#include "stm32f4xx_hal.h"
#elif defined(STM32F207xx)
#include "stm32f2xx_hal.h"
#elif defined(STM32F103xE)	//	STM32F103xC
#include "stm32f1xx_hal.h"
#endif

//=============================================================================
#endif	//	stm32f207
//=============================================================================

#include "QBuf.h"		//	Queue_t

Queue_t		g_qUart1;
Queue_t		g_qUart2;
Queue_t		g_qUart3;
Queue_t		g_qUart5;

uint8_t		dataRx1[8];		//	UART1 - Console	- 115200
uint8_t		dataRx2[8];		//	UART2 - RFM		- 115200
uint8_t		dataRx3[8];		//	UART3 - RS485	- 38400
uint8_t		dataRx5[8];		//	UART5 - RS485	- 38400

uint32_t	g_nStampRx1 = 0;		//	Last Data Recv TimeStamp
uint32_t	g_nStampRx2 = 0;
uint32_t	g_nStampRx3 = 0;
uint32_t	g_nStampRx5 = 0;

UART_HandleTypeDef *phuart1 = NULL;			//	Debug
UART_HandleTypeDef *phuart2 = NULL;			//	RFM
UART_HandleTypeDef *phuart3 = NULL;			//	RS485
UART_HandleTypeDef *phuart5 = NULL;			//	RS485


//=============================================================================
void	rts_high		( UART_HandleTypeDef *_phuart )
//=============================================================================
{
	HAL_HalfDuplex_EnableTransmitter( _phuart );	// disable receiver, enable transmitter

	if ( _phuart->Instance == USART3 )
	{
		HAL_GPIO_WritePin( RE_GPIO_Port, RE_Pin, GPIO_PIN_SET );		//	RTS High
	}

	if ( _phuart->Instance == UART5 )
	{
		HAL_GPIO_WritePin( RE1_GPIO_Port, RE1_Pin, GPIO_PIN_SET );		//	RTS High
	}
}

//=============================================================================
void	rts_low			( UART_HandleTypeDef *_phuart )
//=============================================================================
{
	if ( _phuart->Instance == USART3 )
	{
		HAL_GPIO_WritePin( RE_GPIO_Port, RE_Pin, GPIO_PIN_RESET );		//	RTS Low
	}

	if ( _phuart->Instance == UART5 )
	{
		HAL_GPIO_WritePin( RE1_GPIO_Port, RE1_Pin, GPIO_PIN_RESET );	//	RTS Low
	}

	HAL_HalfDuplex_EnableReceiver( _phuart );	// enable receiver, disable transmitter
}



//=============================================================================
#if defined(_WIN32)
//=============================================================================

//=============================================================================
#else	//	stm32f207
//=============================================================================


#ifdef __GNUC__
  /* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
     set to 'Yes') calls __io_putchar() */
  #define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
  #define GETCHAR_PROTOTYPE int __io_getchar(void)
#else
  #define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
  #define GETCHAR_PROTOTYPE int fgetc(FILE *f)
#endif /* __GNUC__ */


/**
  * @brief  Retargets the C library printf function to the USART.
  * @param  None
  * @retval None
  */
//===========================================================================
PUTCHAR_PROTOTYPE
//===========================================================================
{
	/* Place your implementation of fputc here */
	/* e.g. write a character to the EVAL_COM1 and Loop until the end of transmission */

	//========================================================================
	//	Console
	if ( phuart1 )
	{
		HAL_UART_Transmit( phuart1, (uint8_t *)&ch, 1, 0xFFFF );
		if ( ch == '\n' )
		{
			HAL_UART_Transmit( phuart1, (uint8_t *)"\r", 1, 0xFFFF );
		}
	}

	//========================================================================
	//	RS232
	/*
	if ( phuart2 )
	{
		HAL_UART_Transmit( phuart2, (uint8_t *)&ch, 1, 0xFFFF );
		if ( ch == '\n' )
		{
			HAL_UART_Transmit( phuart2, (uint8_t *)"\r", 1, 0xFFFF );
		}
	}
	//	*/
	//========================================================================

	return ch;
}

//===========================================================================
GETCHAR_PROTOTYPE
//===========================================================================
{
	char	c;

	while( !qcount( &g_qUart1 ) )
	{
//		osDelay(0);
#if defined( USE_FREERTOS )
		taskYIELD();
#else
		return -1;		//	Non-Blocking ( OS less )
//		HAL_Delay( 0 );
#endif
	}
	c = qget( &g_qUart1 );

	return c;
}


/**
  * @brief  Rx Transfer completed callbacks.
  * @param  huart pointer to a UART_HandleTypeDef structure that contains
  *                the configuration information for the specified UART module.
  * @retval None
  */
//===========================================================================
void HAL_UART_RxCpltCallback( UART_HandleTypeDef *huart )
//===========================================================================
{
	//	portBASE_TYPE	xHigherPriorityTaskWoken = pdFALSE;
	//	인터럽트.

	//===========================================================================
	if ( huart->Instance == USART1 && phuart1 )
	{
		//	Debug Console
		g_nStampRx1 = HAL_GetTick();

		//	입력내용 콘솔로 출력.
		//		HAL_UART_Transmit( huart, (uint8_t *)&dataRx1[0], 1, 0xFFFF );

				HAL_UART_Receive_IT( huart, dataRx1, 1 );

		//	문자 Queue에 쌓기.
		qput( &g_qUart1, dataRx1[0] );


	}
	//===========================================================================
	else if ( huart->Instance == USART2 && phuart2 )
	{
		//	RS232 - RFM
		g_nStampRx2 = HAL_GetTick();

		HAL_UART_Receive_IT( huart, dataRx2, 1 );

		qput( &g_qUart2, dataRx2[0] );


	}
	//===========================================================================
	else if ( huart->Instance == USART3 && phuart3 )
	{
		//	RS485
		g_nStampRx3 = HAL_GetTick();

		HAL_UART_Receive_IT( huart, dataRx3, 1 );

		qput( &g_qUart3, dataRx3[0] );

	}
	//===========================================================================
	else if ( huart->Instance == UART5 && phuart5 )
	{
		//	RS485
		g_nStampRx5 = HAL_GetTick();

		HAL_UART_Receive_IT( huart, dataRx5, 1 );

		qput( &g_qUart5, dataRx5[0] );


	}

	//===========================================================================
}


//=============================================================================
#endif	//	stm32f207
//=============================================================================


//=============================================================================
void resetSerial(UART_HandleTypeDef* uart)
//=============================================================================
{
	printf("%s(%d)\n", __FILE__, __LINE__);

	if (HAL_UART_Init(uart) != HAL_OK)
	{
		//_Error_Handler(__FILE__, __LINE__);
		printf("%s(%d)\n", __FILE__, __LINE__);
	}
}


/* check overrun and recovery on uart */
//	Add a comment to this line
//=============================================================================
void checkSerial(UART_HandleTypeDef* uart)
//=============================================================================
{

#if defined(_WIN32)
#else		//	STM32
	if (__HAL_UART_GET_FLAG(uart, UART_FLAG_ORE))
	{
		if (HAL_UART_Init(uart) != HAL_OK)
		{
			//_Error_Handler(__FILE__, __LINE__);
			printf("%s(%d)\n", __FILE__, __LINE__);
		}
		char c;
		HAL_UART_Receive_IT(uart, (uint8_t*)&c, 1);
		printf("[ERR] UART overrun detected\n");
		printf("UART Inst : 0x%08X\n", uart->Instance );
		printf("UART > err code : %x tStat: %x rStat: %x SR: %x DR %x \n", uart->ErrorCode, uart->gState, uart->RxState, uart->Instance->SR, uart->Instance->DR);
		printf("UART > BRR : %x CR1: %x CR2: %x CR3: %x GTPR %x \n", uart->Instance->BRR, uart->Instance->CR1, uart->Instance->CR2, uart->Instance->CR3, uart->Instance->GTPR);
	}

#endif
}



// ============================================================================
void SerialInitQueue( void )
//===========================================================================
{
	//	Debug Queue
	init_queue( &g_qUart1 );

	//	RFM
	init_queue( &g_qUart2 );

	//	RS485 Queue
	init_queue( &g_qUart3 );

	//	RS485 Queue
	init_queue( &g_qUart5 );
}

//===========================================================================
void SerialInit( UART_HandleTypeDef *_phuart1, UART_HandleTypeDef *_phuart2, UART_HandleTypeDef *_phuart3, UART_HandleTypeDef *_phuart5 )
//===========================================================================
{
	SerialInitQueue();

	//===========================================================================
	//	Set Serial Handle
	phuart1 = _phuart1;
	phuart2 = _phuart2;
	phuart3 = _phuart3;
	phuart5 = _phuart5;

	//===========================================================================

#if defined(_WIN32)
#else

	//	UART Console
	if ( phuart1 )		HAL_UART_Receive_IT( phuart1, dataRx1, 1 );
	//	RFM
	if ( phuart2 )		HAL_UART_Receive_IT( phuart2, dataRx2, 1 );
	//	UART RS485
	if ( phuart3 )		HAL_UART_Receive_IT( phuart3, dataRx3, 1 );
	//	UART RS485
	if ( phuart5 )		HAL_UART_Receive_IT( phuart5, dataRx5, 1 );

#endif
}

//===========================================================================
int input_check( void )
//===========================================================================
{
	return qcount( &g_qUart1 );
}

