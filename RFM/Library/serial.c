
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ctype.h>
#include <string.h>
#include <stdarg.h>
#include <stdint.h>

//#include "main.h"

/* USER CODE BEGIN 1 */

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
#endif

//=============================================================================
#endif	//	stm32f207
//=============================================================================

#include "queueBuf.h"		//	Queue_t

#define DEBUG_RX_QUEUE_SIZE	64

//xQueueHandle xDebugQueueRx;
//#define MAX_UART0_COUNT		15


Queue_t		g_qDebug, g_qRS485;

uint8_t		dataDbgTx[8];	//	console

uint8_t		dataTx1[8];		//	UART1 - RS485

uint8_t		bufDbgTx[DEBUG_RX_QUEUE_SIZE];	// console
static int	cntDbgTx;						// console


UART_HandleTypeDef *phuart1 = NULL;
UART_HandleTypeDef *phuart2 = NULL;			//	Debug

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
	if ( phuart2 )
	{
		HAL_UART_Transmit( phuart2, (uint8_t *)&ch, 1, 0xFFFF );
		if ( ch == '\n' )
		{
			HAL_UART_Transmit( phuart2, (uint8_t *)"\r", 1, 0xFFFF );
		}
	}

	//========================================================================
	//	RS232
	//*
	if ( phuart1 )
	{
		HAL_UART_Transmit( phuart1, (uint8_t *)&ch, 1, 0xFFFF );

		if ( ch == '\n' )
		{
			HAL_UART_Transmit( phuart1, (uint8_t *)"\r", 1, 0xFFFF );
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

	while( !qcount( &g_qDebug ) )
	{
//		osDelay(0);
#if defined( USE_FREERTOS )
		taskYIELD();
#else
		return -1;		//	Non-Blocking ( OS less )
//		HAL_Delay( 0 );
#endif
	}
	c = qget( &g_qDebug );

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
	//	Debug Console
	if ( huart->Instance == USART2 )
	{
		//	문자 Queue에 쌓기.
		qput( &g_qDebug, dataDbgTx[0] );

		//	입력내용 콘솔로 출력.
//DEL		HAL_UART_Transmit( huart, (uint8_t *)&dataDbgTx[0], 1, 0xFFFF );

		HAL_UART_Receive_IT( huart, dataDbgTx, 1 );
	}

	//===========================================================================
	//	RS232
	if ( huart->Instance == USART1 )
	{
		qput( &g_qDebug, dataTx1[0] );
//		qput( &g_qRS485, dataTx1[0] );	//	입력내용 콘솔로 출력.

//DEL		HAL_UART_Transmit( huart, (uint8_t *)&dataTx1[0], 1, 0xFFFF );

		HAL_UART_Receive_IT( huart, dataTx1, 1 );
	}

	//===========================================================================
}


//=============================================================================
#endif	//	stm32f207
//=============================================================================



// ============================================================================
void SerialInitQueue( void )
//===========================================================================
{
//	xDebugQueueRx = xQueueCreate( DEBUG_RX_QUEUE_SIZE, sizeof( char ) );

	//	Debug Queue
	init_queue( &g_qDebug );

	//	RS485 Queue
	init_queue( &g_qRS485 );
}

//===========================================================================
void SerialInit( UART_HandleTypeDef *_phuart1, UART_HandleTypeDef *_phuart2 )
//===========================================================================
{
	SerialInitQueue();

	//===========================================================================
	//	Set Serial Handle
	phuart1 = _phuart1;
	phuart2 = _phuart2;

	//===========================================================================

#if defined(_WIN32)
#else

	//	UART Console
	if ( phuart2 )
	{
		HAL_UART_Receive_IT( phuart2, dataDbgTx, 1 );
	}

	//	UART RS485
	if ( phuart1 )
	{
		HAL_UART_Receive_IT( phuart1, dataTx1, 1 );
	}

#endif
}


/*
 * \brief
                입력이 있는지 확인
		Rx Queue의 Empty를 체크
 * \author
                *
 * \param
                *
 * \return
                return 1 when input exist, else 0
 * \date
                *
 * \version
                *
 * \see
                *
 */
//===========================================================================
int input_check( void )
//===========================================================================
{
	return qcount( &g_qDebug );
//	return (((rear+QMAX)-front) % QMAX);
	/*
	portBASE_TYPE xStatus;
	uint8_t	c;

	xStatus = xQueuePeek(xDebugQueueRx, &c, 0);
	return xStatus;
	*/
}


/* USER CODE END 1 */