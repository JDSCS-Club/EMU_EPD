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

#include <stdio.h>			//	printf()

#include <stdint.h>			//	uint32_t

#include <string.h>			//	memset()

#include "typedef.h"		//	ip_addr

#include "main.h"

#include "audio.h"

//========================================================================
// Define

uint16_t sine_table[256] = {

   0x0000, 0x0324, 0x0647, 0x096a, 0x0c8b, 0x0fab, 0x12c8, 0x15e2,
   0x18f8, 0x1c0b, 0x1f19, 0x2223, 0x2528, 0x2826, 0x2b1f, 0x2e11,
   0x30fb, 0x33de, 0x36ba, 0x398c, 0x3c56, 0x3f17, 0x41ce, 0x447a,
   0x471c, 0x49b4, 0x4c3f, 0x4ebf, 0x5133, 0x539b, 0x55f5, 0x5842,
   0x5a82, 0x5cb4, 0x5ed7, 0x60ec, 0x62f2, 0x64e8, 0x66cf, 0x68a6,
   0x6a6d, 0x6c24, 0x6dca, 0x6f5f, 0x70e2, 0x7255, 0x73b5, 0x7504,
   0x7641, 0x776c, 0x7884, 0x798a, 0x7a7d, 0x7b5d, 0x7c29, 0x7ce3,
   0x7d8a, 0x7e1d, 0x7e9d, 0x7f09, 0x7f62, 0x7fa7, 0x7fd8, 0x7ff6,
   0x7fff, 0x7ff6, 0x7fd8, 0x7fa7, 0x7f62, 0x7f09, 0x7e9d, 0x7e1d,
   0x7d8a, 0x7ce3, 0x7c29, 0x7b5d, 0x7a7d, 0x798a, 0x7884, 0x776c,
   0x7641, 0x7504, 0x73b5, 0x7255, 0x70e2, 0x6f5f, 0x6dca, 0x6c24,
   0x6a6d, 0x68a6, 0x66cf, 0x64e8, 0x62f2, 0x60ec, 0x5ed7, 0x5cb4,
   0x5a82, 0x5842, 0x55f5, 0x539b, 0x5133, 0x4ebf, 0x4c3f, 0x49b4,
   0x471c, 0x447a, 0x41ce, 0x3f17, 0x3c56, 0x398c, 0x36ba, 0x33de,
   0x30fb, 0x2e11, 0x2b1f, 0x2826, 0x2528, 0x2223, 0x1f19, 0x1c0b,
   0x18f8, 0x15e2, 0x12c8, 0x0fab, 0x0c8b, 0x096a, 0x0647, 0x0324,
   0x0000, 0xfcdc, 0xf9b9, 0xf696, 0xf375, 0xf055, 0xed38, 0xea1e,
   0xe708, 0xe3f5, 0xe0e7, 0xdddd, 0xdad8, 0xd7da, 0xd4e1, 0xd1ef,
   0xcf05, 0xcc22, 0xc946, 0xc674, 0xc3aa, 0xc0e9, 0xbe32, 0xbb86,
   0xb8e4, 0xb64c, 0xb3c1, 0xb141, 0xaecd, 0xac65, 0xaa0b, 0xa7be,
   0xa57e, 0xa34c, 0xa129, 0x9f14, 0x9d0e, 0x9b18, 0x9931, 0x975a,
   0x9593, 0x93dc, 0x9236, 0x90a1, 0x8f1e, 0x8dab, 0x8c4b, 0x8afc,
   0x89bf, 0x8894, 0x877c, 0x8676, 0x8583, 0x84a3, 0x83d7, 0x831d,
   0x8276, 0x81e3, 0x8163, 0x80f7, 0x809e, 0x8059, 0x8028, 0x800a,
   0x8000, 0x800a, 0x8028, 0x8059, 0x809e, 0x80f7, 0x8163, 0x81e3,
   0x8276, 0x831d, 0x83d7, 0x84a3, 0x8583, 0x8676, 0x877c, 0x8894,
   0x89bf, 0x8afc, 0x8c4b, 0x8dab, 0x8f1e, 0x90a1, 0x9236, 0x93dc,
   0x9593, 0x975a, 0x9931, 0x9b18, 0x9d0e, 0x9f14, 0xa129, 0xa34c,
   0xa57e, 0xa7be, 0xaa0b, 0xac65, 0xaecd, 0xb141, 0xb3c1, 0xb64c,
   0xb8e4, 0xbb86, 0xbe32, 0xc0e9, 0xc3aa, 0xc674, 0xc946, 0xcc22,
   0xcf05, 0xd1ef, 0xd4e1, 0xd7da, 0xdad8, 0xdddd, 0xe0e7, 0xe3f5,
   0xe708, 0xea1e, 0xed38, 0xf055, 0xf375, 0xf696, 0xf9b9, 0xfcdc,
};

uint16_t null_table[256] = { 0, };

uint16_t nAudioTable = 0;

uint16_t bufAudio[I2S_DMA_LOOP_SIZE * I2S_DMA_LOOP_QCNT] = { 0, };	//	512
	//	[ Frame1 | Frame2 ]

uint16_t bufAudioTemp[I2S_DMA_LOOP_SIZE];		//	Temp Audio Buffer

uint16_t bufAudioRF[I2S_DMA_LOOP_SIZE * 2];	//	RF Recv Send Buffer

uint16_t pwm_table[ 256 ] = { 0, };

static int s_bDebug = 0;

uint32_t	g_eAudioMode = 0;		//	Audio Mode

uint32_t	g_eAudioSample = eASampleDefault;		//	Audio Sampling ( Default : 8 KHz )


QBuf_t		g_qBufAudioRFRx;		//	Audio Queue Buffer	( RF Rx Buffer )
uint16_t	g_bufAudioRFRx[I2S_DMA_LOOP_SIZE * I2S_DMA_LOOP_QCNT] = { 0, };	//	512

QBuf_t		g_qBufAudioRFTx;		//	Audio Queue Buffer	( RF Tx Buffer )
uint16_t	g_bufAudioRFTx[I2S_DMA_LOOP_SIZE * I2S_DMA_LOOP_QCNT] = { 0, };	//	512


uint16_t	t_audio_buff[I2S_DMA_LOOP_SIZE * 2];   // RADIO_MAX_PACKET_LENGTH * 2];
uint16_t	r_audio_buff[I2S_DMA_LOOP_SIZE * 2];   //[RADIO_MAX_PACKET_LENGTH * 2];


//========================================================================
// Function


//========================================================================
void	AudioDebugEnable	( void )
//========================================================================
{
	if ( s_bDebug )
	{
		printf( "%s(%d) - Disable\n", __func__, __LINE__ );
		s_bDebug = 0;
	}
	else
	{
		printf( "%s(%d) - Enable\n", __func__, __LINE__ );
		s_bDebug = 1;
	}
}

//========================================================================
void AudioInit( void )
//========================================================================
{
//DEL	printf( "%s(%d)\n", __func__, __LINE__ );

	//	pAudioTable = sine_table;
//	nAudioTable = 0;
	nAudioTable = 2;	//	Loopback

	//	Init RF Audio Rx Buffer
	qBufInit( &g_qBufAudioRFRx, (uint8_t *)g_bufAudioRFRx, ( I2S_DMA_LOOP_SIZE * 2 ) * I2S_DMA_LOOP_QCNT );
	qBufInit( &g_qBufAudioRFTx, (uint8_t *)g_bufAudioRFTx, ( I2S_DMA_LOOP_SIZE * 2 ) * I2S_DMA_LOOP_QCNT );

	memset( bufAudioRF, 0x00, sizeof( bufAudioRF ) );

	HAL_I2SEx_TransmitReceive_DMA( &hi2s3, (uint16_t*)sine_table, (uint16_t*)bufAudio, 256 );
}

//========================================================================
void AudioTxSine( void )
//========================================================================
{
	nAudioTable = 0;

	HAL_I2SEx_TransmitReceive_DMA( &hi2s3, (uint16_t*)sine_table, (uint16_t*)bufAudio, 256 );
}


//========================================================================
void AudioTxNull( void )
//========================================================================
{
	nAudioTable = 1;
	//	pAudioTable = null_table;
	HAL_I2SEx_TransmitReceive_DMA( &hi2s3, (uint16_t*)null_table, (uint16_t*)bufAudio, 256 );
}

//========================================================================
void AudioRxTxLoop( void )
//========================================================================
{
//	g_bModeAudioRF = 0;
	g_eAudioMode |= ( eAModLoopbackDMA );
	g_eAudioMode &= ~( eAModRFTx | eAModRFRx );

	nAudioTable = 2;
	//	pAudioTable = sine_table;
	HAL_I2SEx_TransmitReceive_DMA( &hi2s3, (uint16_t*)bufAudio, (uint16_t*)&bufAudio[256], 256 );
}

//========================================================================
void AudioTxStop( void )
//========================================================================
{
	nAudioTable = 3;
	//	pAudioTable = null_table;
}


void ( *pCallback_I2SEx_TxRxCpltCallback )( I2S_HandleTypeDef *hi2s ) = NULL;


//========================================================================
void	SetCallbackI2STxRxCplt	( void ( *pCallbackTxRxCplt )( I2S_HandleTypeDef *hi2s ) )
//========================================================================
{
	pCallback_I2SEx_TxRxCpltCallback = pCallbackTxRxCplt;
}

/**
  * @brief Rx Transfer completed callbacks
  * @param  hi2s: pointer to a I2S_HandleTypeDef structure that contains
  *         the configuration information for I2S module
  * @retval None
  */
//========================================================================
void HAL_I2SEx_TxRxCpltCallback( I2S_HandleTypeDef *hi2s )
//========================================================================
{
	/* Prevent unused argument(s) compilation warning */
	/* NOTE : This function Should not be modified, when the callback is needed,
			  the HAL_I2S_RxCpltCallback could be implemented in the user file
	 */

//	printf( "%s(%d)\n", __func__, __LINE__ );

	if ( pCallback_I2SEx_TxRxCpltCallback )
	{
		pCallback_I2SEx_TxRxCpltCallback( hi2s );
	}
	else
	{
		//	Default Loopback
		//	pAudioTable = sine_table;
		memcpy( &bufAudio[0], &bufAudio[I2S_DMA_LOOP_SIZE], I2S_DMA_LOOP_SIZE * 2 );
		HAL_I2SEx_TransmitReceive_DMA( &hi2s3, (uint16_t*)bufAudio, (uint16_t*)&bufAudio[I2S_DMA_LOOP_SIZE], I2S_DMA_LOOP_SIZE );
	}
}

//========================================================================
//
//		Audio Chip ( XE3005 / MAX9860 )
//
//========================================================================

int		g_nAudioIC = AudioNone;

//========================================================================
void SetAudioIC( int nAudioIC )
//========================================================================
{
	g_nAudioIC = nAudioIC;
}

//========================================================================
int GetAudioIC( void )
//========================================================================
{
	return g_nAudioIC;
}

//========================================================================
//
//		Codec ( MAX9860 )
//
//========================================================================

//========================================================================
int WriteI2CCodec( uint8_t addr, uint8_t data )
//========================================================================
{
	if ( GetAudioIC() != AudioMAX9860 )	return 0;

	int cntRetry;

	cntRetry = 0;

	char buf[10];
	memset( buf, 0, sizeof( buf ) );
	buf[0] = data;

	while ( HAL_I2C_Mem_Write( &hi2c1, (uint16_t)( 0x10 << 1 ), (uint16_t)addr, I2C_MEMADD_SIZE_8BIT, buf, (uint16_t)1, 1000 ) != HAL_OK && cntRetry < 10 ) cntRetry++;

	if ( cntRetry == 10 )   return 0;
	else					return 1;
}

//========================================================================
void InitCodecMAX9860   ( void )
//========================================================================
{
	printf( "%s(%d)\n", __func__, __LINE__ );

	SetAudioIC( AudioMAX9860 );

#define MCLOCK_FREQUENCY	12000000	//	Codec Master Clock ( 12 MHz )

#define AUDIO_FREQ			8000 		//	Audio Freq : 8 KHz

#define MAX9860 0						//	unsure of address

#define N (( 65536 * 96 * AUDIO_FREQ ) / MCLOCK_FREQUENCY )		//	N = (65536*96*F_lrclk)/F_pclk
#define NHI (N>>8)						//	high 7 bits of N

	//#define NLO (N&0b000000011111111)		//	NLO is lower 8 bits of N
#define NLO (N&0xFF)		//	NLO is lower 8 bits of N

	//========================================================================
	//  Audio Master Clock 13 MHz 설정.

	//========================================================================
	//  Codec 초기화. ( MAX9860ETG+ )

	/*	Initialize the MAX9860 by setting registers	*/
	//	registers at 0x03, 0x04, and 0x05 are clock control registers
	WriteI2CCodec( 0X03, 0X12 );	//	PSCLK = 01, FREQ = 10(12MHz), 8KHz = 0 (LRCLK = 8KHz)

	WriteI2CCodec( 0X04, NHI );	//	PLL =0,
	WriteI2CCodec( 0X05, NLO );

	//	WriteI2CCodec( 0X06, 0x2C );	//	MAS = 0, WCI = 0 (really not sure), DBCI = 1( Falling Edge SDIN ), DDLY = 0, HIZ = 1, TDM = 1, 
	//	WriteI2CCodec( 0X06, 0x24 );	//	MAS = 0, WCI = 0 (really not sure), DBCI = 1( Falling Edge SDIN ), DDLY = 0, HIZ = 0, TDM = 1, 
		//	Rising Edge
	WriteI2CCodec( 0X06, 0x04 );	//	MAS = 0, WCI = 0 (really not sure), DBCI = 0( Rising Edge SDIN ), DDLY = 0, HIZ = 0, TDM = 1, 

//sendTwoBytes(MAX9860, 0X07, 0x00);	//	ABCI = 0, ADLY = 0, ST = 0, BSEL = 000

//	WriteI2CCodec( 0X07, 0x20 );	//	ABCI = 1 ( Falling Edge SDOUT ), ADLY = 0, ST = 0, BSEL = 000

	//	Rising Edge
	WriteI2CCodec( 0X07, 0x00 );	//	ABCI = 0 ( Rising Edge SDOUT ), ADLY = 0, ST = 0, BSEL = 000

	//	Digital filters
	WriteI2CCodec( 0x08, 0x00 );	//AVFLT = 0, DVFLT = 0 (not sure which filter would be best for this, choosing no filter)

	//	Digital level control registers
	WriteI2CCodec( 0x09, 0x06 );	//0 DAC adjustment, this would require testing and/or a better understanding of the overall system
	WriteI2CCodec( 0x0A, 0x33 );	//both ADC's set to 0 adjustment, same reason as above AND mic not supported yet
	WriteI2CCodec( 0x0B, 0x00 );	//no gain on DAC, unsure of DVST bits safer to disable
//	WriteI2CCodec( 0x0B, 0x61 );	// DAC - DVG 11(+18) / DVST 0x01(0)
//	WriteI2CCodec( 0x0B, 0x01 );	// DAC - DVG 00(+0) / DVST 0x01(0)

	//	microphone input register
//	WriteI2CCodec( 0x0c, 0x00 );	//I haven't set this up to allow microphones (make MAX9860 master, ARM slave)
//	WriteI2CCodec( 0x0c, 0x4A );	// PAM = 10(+20) / PGAM = 0x0A(+10)

	WriteI2CCodec( 0x0c, 0x2A );	// PAM = 01(+0) / PGAM = 0x0A(+10)

//	WriteI2CCodec( 0x0c, 0x34 );	// PAM = 01(+0) / PGAM = 0x14(+0)

	//	AGC and Noise gate registers
//	WriteI2CCodec( 0x0E, 0x80 );	//sum of left & right noise gates for AGC & noise gate, AGCRLS shortest time, but AGC disabled since I am unsure of its need
	WriteI2CCodec( 0x0E, 0x00 );	//  Disable AGC
	WriteI2CCodec( 0x0F, 0x00 );	//Noise gate threshhold disabled, AGC signal threshhold -3bBFS

	//	power management register
//	WriteI2CCodec( 0x10, 0x88 );	//powered on, DAC on, both ADC's off (since this is not set up for microphone input)
	WriteI2CCodec( 0x10, 0x8A );	//powered on, DAC on, both ADC(Left) Enable
}

//========================================================================
int	AudioLoopbackDMA( void )
//========================================================================
{
	printf( "%s(%d)\n", __func__, __LINE__ );

	AudioRxTxLoop();

	return 0;
}

//========================================================================
void	QPutAudioStream( char * sBuf, int nSize )
//========================================================================
{
	//	RF Data로 부터 수신된 Audio Stream

	//	RF Data Recv -> Audio Out
	if ( g_eAudioMode & eAModRFRx )
	{
		//	Audio Buffer Put
		qBufPut( &g_qBufAudioRFRx, (uint8_t *)sBuf, nSize );
	}
}


//========================================================================
int		AudioLoopbackDMASpeex( void )
//========================================================================
{
	printf( "%s(%d)\n", __func__, __LINE__ );

	return 0;
}

//========================================================================
void	SetAudioLoopSampling( int _eAudioSample )
//========================================================================
{
	printf( "%s(%d) - 0x%08X\n", __func__, __LINE__, _eAudioSample );

	g_eAudioSample = _eAudioSample;
}

//========================================================================
int		AudioRFRxTx			( void )		//	RF Data Recv
//========================================================================
{
	//	Rx / Tx Data
	//	Rx <- RF Recv
	//	Tx -> RF Send

	printf( "%s(%d)\n", __func__, __LINE__ );

	g_eAudioMode |= ( eAModRFTx | eAModRFRx );

	return 0;
}


//========================================================================
int		AudioRFTx			( void )		//	RF Data Recv
//========================================================================
{
	//	Rx / Tx Data
	//	Tx -> RF Send

	printf( "%s(%d)\n", __func__, __LINE__ );

	g_eAudioMode |= eAModRFTx;
	g_eAudioMode &= ~eAModRFRx;

	return 0;
}


//========================================================================
int		AudioRFRx			( void )		//	RF Data Recv
//========================================================================
{
	//	Rx / Tx Data
	//	Rx <- RF Recv

	printf( "%s(%d)\n", __func__, __LINE__ );

	g_eAudioMode |= eAModRFRx;
	g_eAudioMode &= ~eAModRFTx;

	return 0;
}
