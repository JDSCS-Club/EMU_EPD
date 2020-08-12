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

//#define		I2S_DMA_LOOP_SIZE		( 60 / 2 )	//	[4Byte Commnad] [ 60Byte Stream Data ]
//#define		I2S_DMA_LOOP_QCNT		8		//	Queue Count
//#define		I2S_DMA_LOOP_SIZE		160		//

#define		I2S_DMA_LOOP_SIZE		32	//	[4Byte Commnad] [ 60Byte Stream Data ]
#define		I2S_DMA_LOOP_QCNT		4		//	Queue Count

//#include "rf_pa.h"				//	I2S_DMA_LOOP_SIZE / I2S_DMA_LOOP_QCNT

enum eAudioMode
{
	eAModLoopbackDMA	=	0x1,
	eAModRFTx			=	0x2,
	eAModRFRx			=	0x4,
};

extern int16_t sine_table[];
extern uint16_t null_table[];
extern uint16_t nAudioTable;

extern QBuf_t	g_qBufAudioRx;		//	Audio Queue Buffer	( RF Rx Buffer )
extern QBuf_t	g_qBufAudioTx;		//	Audio Queue Buffer	( RF Tx Buffer )

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
void	AudioStop				( void );
void	AudioTxStop				( void );

void	AudioRxTxLoop			( void );

void	SetCallbackI2STxRxCplt	( void ( *pCallbackTxRxCplt )( I2S_HandleTypeDef *hi2s ) );

//========================================================================
enum eAudioIC
{
	AudioNone		=	0x00,
	AudioMAX9860	=	0x01,
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

int		AudioLoopbackDMA		( void );
int 	AudioLoopbackDMASpeex	( void );

int		AudioLoopbackDMACompress( void );

void	LoopProcAudio			( void );

int		GetSpkVol	    ( void );
void	SetSpkVol	    ( int nSpkVol );



//void	QPutAudioStream			( char *sBuf, int nSize );

#endif