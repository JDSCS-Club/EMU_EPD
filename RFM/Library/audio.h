/*------------------------------------------------------------------------------------------
	Project			: RFM - Audio
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

#ifndef AUDIO_H
#define AUDIO_H

#include "stm32f4xx_hal.h"		//	I2S_HandleTypeDef

#include "queueBuf.h"

//#define		I2S_DMA_LOOP_SIZE		(RFPktDataLen/2)	//	[4Byte Commnad] [ 60Byte Stream Data ]

#define		I2S_DMA_LOOP_SIZE		( 60 / 2 )	//	[4Byte Commnad] [ 60Byte Stream Data ]
#define		I2S_DMA_LOOP_QCNT		8		//	Queue Count

//#include "rf_pa.h"				//	I2S_DMA_LOOP_SIZE / I2S_DMA_LOOP_QCNT

enum eAudioMode
{
	eAModLoopbackDMA	=	0x1,
	eAModRFTx			=	0x2,
	eAModRFRx			=	0x4,
};

enum eAudioSample
{
	eASample8KHz		=	0x0,
	eASample4KHz		=	0x1,
	eASample2KHz		=	0x2,
	eASample1KHz		=	0x3,
	eASampleDefault		=	eASample8KHz,
};

extern uint32_t	g_eAudioSample;

extern uint16_t sine_table[];
extern uint16_t null_table[];
extern uint16_t nAudioTable;

extern QBuf_t	g_qBufAudioRFRx;		//	Audio Queue Buffer	( RF Rx Buffer )
extern QBuf_t	g_qBufAudioRFTx;		//	Audio Queue Buffer	( RF Tx Buffer )

extern uint16_t	g_bufAudioRFRx[];		//	512
extern uint16_t	g_bufAudioRFTx[];		//	512

//========================================================================
//
//		Audio I2S
//
//========================================================================


//========================================================================
//	AudioXXXX - I2S 제어

void	AudioInit				( void );
void	AudioTxSine				( void );
void	AudioTxNull				( void );
void	AudioTxStop				( void );

void	AudioRxTxLoop			( void );


//========================================================================
enum eAudioIC
{
	AudioNone		=	0x00,
	AudioXE3005		=	0x01,
	AudioMAX9860	=	0x02,
};

extern int	g_nAudioIC;

void	SetAudioIC( int nAudioIC );
int		GetAudioIC( void );

//========================================================================


//========================================================================
//
//		Codec ( MAX9860 )
//
//========================================================================

int		WriteI2CCodec		( uint8_t addr, uint8_t data );
void	InitCodecMAX9860	( void );

//========================================================================

int		AudioLoopback			( void );
int		AudioLoopbackDMA		( void );

int		AudioCodecLoopback		( int bOnOff );

void	SetCallbackI2STxRxCplt	( void ( *pCallbackTxRxCplt )( I2S_HandleTypeDef *hi2s ) );

void	QPutAudioStream			( char *sBuf, int nSize );

int		AudioLoopbackDMASpeex	( void );

void	SetAudioLoopSampling	( int _eAudioSample );

int		AudioRFRxTx				( void );		//	RF Data Tx/Rx

int		AudioRFTx				( void );		//	RF Data Tx
int		AudioRFRx				( void );		//	RF Data Rx

void	AudioDebugEnable		( void );

#endif
