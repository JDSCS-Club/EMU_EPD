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

#include "QBuf.h"

#include "RFMProtocol.h"		//	RFPktDataLen

//#define		I2S_DMA_LOOP_SIZE		(RFPktDataLen/2)	//	[4Byte Commnad] [ 60Byte Stream Data ]

//#define		I2S_DMA_LOOP_SIZE		( 60 / 2 )	//	[4Byte Commnad] [ 60Byte Stream Data ]
//#define		I2S_DMA_LOOP_QCNT		8		//	Queue Count
//#define		I2S_DMA_LOOP_SIZE		160		//

enum eAudioConfI2SDMA
{
//	I2S_DMA_LOOP_SIZE	=	32,		//	[4Byte Commnad] [ 60Byte Stream Data ]
	I2S_DMA_LOOP_SIZE	=	(RFPktDataLen/2),	//	[4Byte Commnad] [ 60Byte Stream Data ]
//	I2S_DMA_LOOP_QCNT	=	4,					//	Queue Count
//	I2S_DMA_LOOP_QCNT	=	5,					//	Queue Count
	I2S_DMA_LOOP_QCNT	=	8,					//	Queue Count
};

enum eAudioMode
{
	eAModLoopbackDMA	=	0x1,
	eAModRFTx			=	0x2,
	eAModRFRx			=	0x4,
};

extern int16_t sine_table[];
extern uint16_t null_table[];

extern QBuf_t	g_qBufAudioRx;		//	Audio Queue Buffer	( RF Rx Buffer )
extern QBuf_t	g_qBufAudioTx;		//	Audio Queue Buffer	( RF Tx Buffer )

extern uint16_t	g_bufAudioRx[];		//	512
extern uint16_t	g_bufAudioTx[];		//	512

extern int16_t	t_audio_buff[];
extern int16_t	r_audio_buff[];


//========================================================================
//
//		Audio I2S
//
//========================================================================

//========================================================================
//	AudioXXXX - I2S 제어

void	AudioInit				( void );
void	AudioSine				( void );
void	AudioStop				( void );

void	SetCallbackI2STxRxCplt	( void ( *pCallbackTxRxCplt )( I2S_HandleTypeDef *hi2s ) );

void 	AudioSine_I2SEx_TxRxCpltCallback( I2S_HandleTypeDef *hi2s );
void 	AudioLoopback_I2SEx_TxRxCpltCallback( I2S_HandleTypeDef *hi2s );

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

void	AudioSpkVol				( int nSpkVol );

//========================================================================
//	Command Function

int 	cmd_audio	( int argc, char *argv[] );
int 	cmd_codec	( int argc, char *argv[] );

//========================================================================

#endif
