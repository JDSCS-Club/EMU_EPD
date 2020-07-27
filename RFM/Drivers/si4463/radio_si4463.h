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

/*------------------------------------------------------------------------------------------
	[ 구현목록 정의 ]
	* Func1 구현. 
		- Func1 기능 세부내용.

	* Func2 구현. 
-------------------------------------------------------------------------------------------*/


//========================================================================
#ifndef RADIO_SI4463_H
#define RADIO_SI4463_H
//========================================================================

//========================================================================
// Header

#include "typedef.h"		//	uint8_t

#include "si4463.h"			//	si4463_t

//========================================================================
// Define

#define B1_Pin					GPIO_PIN_13
#define B1_GPIO_Port			GPIOC
#define B1_EXTI_IRQn			EXTI15_10_IRQn
#define CTS_Pin					GPIO_PIN_6		//	GPIO_PIN_2
#define CTS_GPIO_Port			GPIOE			//	GPIOA
#define SHUTDOWN_Pin			GPIO_PIN_5		//	GPIO_PIN_3
#define SHUTDOWN_GPIO_Port		GPIOC			//	GPIOA

//	STANDBY ( PE7 )
#define LED_ONBOARD_Pin			GPIO_PIN_7		//	GPIO_PIN_5
#define LED_ONBOARD_GPIO_Port	GPIOE			//	GPIOA
#define nSEL_Pin				GPIO_PIN_4		//	GPIO_PIN_10
#define nSEL_GPIO_Port			GPIOA			//	GPIOB
//#define SI4463_IRQ_Pin			GPIO_PIN_4		//	GPIO_PIN_10
//#define SI4463_IRQ_GPIO_Port	GPIOC			//	GPIOA
//#define TMS_Pin					GPIO_PIN_13
//#define TMS_GPIO_Port			GPIOA
//#define TCK_Pin					GPIO_PIN_14
//#define TCK_GPIO_Port			GPIOA

#define DEMOFEST 				(0x01)

//#define APP_PACKET_LEN			((uint16_t) 10)
#define APP_PACKET_LEN			((uint16_t) 64)

extern si4463_t si4463;
extern uint8_t incomingBuffer[APP_PACKET_LEN];
extern uint8_t outgoingBuffer[APP_PACKET_LEN];


//========================================================================

#define		USE_SI4464_INT_FLAG		1		//	Interrupt Flag방식 사용.



//========================================================================
// Function

int			RF_IsInit				( void );

void		RF_Init					( void );

void		RF_Info					( void );

void		RF_Tx					( void );
void		RF_Rx					( void );
void		RF_Loopback				( void );

void		RF_SendData				( char *sBuf, int nSize );

void		RF_RecvData				( char *sBuf, int nSize );		//	Callback Func


void		SI4463_Debug			( int nDbgLevel );				//	0 : Disable / 1 : Print Data / 2 : Detail

void		SI4463_Test				( void );

uint8_t		SI4463_IsCTS			( void );
void		SI4463_WriteRead		( const uint8_t * pTxData, uint8_t * pRxData, const uint16_t sizeTxData );
void		SI4463_SetShutdown		( void );
void		SI4463_ClearShutdown	( void );
void		SI4463_Select			( void );
void		SI4463_Deselect			( void );

void		SI4463_Interrupt		( void );		//	Interrupt Handler

#if defined(USE_SI4464_INT_FLAG)

int			SI4463_GetIntFlag		( void );
void		SI4463_SetIntFlag		( int bIntFlag );

#endif	//	defined(USE_SI4464_INT_FLAG)

//========================================================================
#endif
//========================================================================


