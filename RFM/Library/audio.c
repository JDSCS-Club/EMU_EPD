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


#include "codec.h"
//#ifdef HAVE_CONFIG_H
//#include "config.h"
//#endif
#include <speex/speex.h>

//========================================================================
// Define

#if 0
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

#else
//	Sine Wave
//	Numbers Per Row	: 16
int16_t sine_table[160] = {
	-1, 6252, 12803, 18323, 23170, 27165, 30381, 32165, 32767, 32165, 30162, 27165, 23170, 18323, 12274, 6252, 0, -6253, -12804, -18324, -23171, -27166, -30382, -32166, -32768, -32166, -30163, -27166, -23171, -18324, -12275, -6253,
	-1, 6252, 12803, 18323, 23170, 27165, 30381, 32165, 32767, 32165, 30162, 27165, 23170, 18323, 12274, 6252, 0, -6253, -12804, -18324, -23171, -27166, -30382, -32166, -32768, -32166, -30163, -27166, -23171, -18324, -12275, -6253,
	-1, 6252, 12803, 18323, 23170, 27165, 30381, 32165, 32767, 32165, 30162, 27165, 23170, 18323, 12274, 6252, 0, -6253, -12804, -18324, -23171, -27166, -30382, -32166, -32768, -32166, -30163, -27166, -23171, -18324, -12275, -6253,
	-1, 6252, 12803, 18323, 23170, 27165, 30381, 32165, 32767, 32165, 30162, 27165, 23170, 18323, 12274, 6252, 0, -6253, -12804, -18324, -23171, -27166, -30382, -32166, -32768, -32166, -30163, -27166, -23171, -18324, -12275, -6253,
	-1, 6252, 12803, 18323, 23170, 27165, 30381, 32165, 32767, 32165, 30162, 27165, 23170, 18323, 12274, 6252, 0, -6253, -12804, -18324, -23171, -27166, -30382, -32166, -32768, -32166, -30163, -27166, -23171, -18324, -12275, -6253,
};

#endif

uint16_t speex_sine_table[160]; 	//	256 * 5 = 160 * 8

uint16_t null_table[256] = { 0, };

uint16_t nAudioTable = 0;

//uint16_t bufAudio[I2S_DMA_LOOP_SIZE * I2S_DMA_LOOP_QCNT] = { 0, };	//	512
	//	[ Frame1 | Frame2 ]
volatile uint16_t bufAudio[FRAME_SIZE * 2] = { 0, };	//	512

volatile uint16_t bufAudioCodec[FRAME_SIZE * 2] = { 0, };	//	512

//========================================================================
//	Audio Buffer ( PCM )
QBuf_t		g_qBufAudioRx;		//	Audio Queue Buffer	( RF Rx Buffer )
uint16_t	g_bufAudioRx[I2S_DMA_LOOP_SIZE * I2S_DMA_LOOP_QCNT] = { 0, };	//	512

QBuf_t		g_qBufAudioTx;		//	Audio Queue Buffer	( RF Tx Buffer )
uint16_t	g_bufAudioTx[I2S_DMA_LOOP_SIZE * I2S_DMA_LOOP_QCNT] = { 0, };	//	512

int16_t		t_audio_buff[I2S_DMA_LOOP_SIZE * 2];
int16_t		r_audio_buff[I2S_DMA_LOOP_SIZE * 2];

//========================================================================

//========================================================================
// Function

//========================================================================
void AudioInit( void )
//========================================================================
{
//DEL	printf( "%s(%d)\n", __func__, __LINE__ );

	//	pAudioTable = sine_table;
//	nAudioTable = 0;
	nAudioTable = 2;	//	Loopback

	//========================================================================
	//	Init RF Audio Rx Buffer
	qBufInit( &g_qBufAudioRx, (uint8_t *)g_bufAudioRx, ( I2S_DMA_LOOP_SIZE * 2 ) * I2S_DMA_LOOP_QCNT );
	qBufInit( &g_qBufAudioTx, (uint8_t *)g_bufAudioTx, ( I2S_DMA_LOOP_SIZE * 2 ) * I2S_DMA_LOOP_QCNT );

	HAL_I2SEx_TransmitReceive_DMA( &hi2s3, (uint16_t*)sine_table, (uint16_t*)bufAudio, 256 );

	//========================================================================
	//	Speex Codec
	Speex_Init();

	//========================================================================
	//	Speex Encoding -> Decoding

	int i;
	int index;

	index = 0;

	//	sine table copy
	memcpy( speex_sine_table, sine_table, 160 * 2 );

	int tick_start, tick_end;

	tick_start = HAL_GetTick();

#if 1
	//========================================================================
	//	Encoding

    speex_bits_reset(&bits);

	/* Encode the frame */
	speex_encode_int(enc_state, &speex_sine_table[0], &bits);
	/* Copy the bits to an array of char that can be decoded */
	speex_bits_write(&bits, (char *)out_bytes, ENCODED_FRAME_SIZE);

	//========================================================================
	//	Decoding
	/* Copy the encoded data into the bit-stream struct */
	speex_bits_read_from(&bits, (char *)out_bytes, ENCODED_FRAME_SIZE);
	/* Decode the data */
	speex_decode_int(dec_state, &bits, &speex_sine_table[0] );
#endif

	tick_end = HAL_GetTick();

	printf("%s : [%d] %d / %d\n", __func__, tick_end - tick_start, tick_start, tick_end);

	//========================================================================
}

//========================================================================
void AudioTxSine( void )
//========================================================================
{
	nAudioTable = 0;

	HAL_I2SEx_TransmitReceive_DMA( &hi2s3, (uint16_t*)sine_table, (uint16_t*)bufAudio, 256 );
}


//========================================================================
void AudioStop( void )
//========================================================================
{
	nAudioTable = 1;
	//	pAudioTable = null_table;
	HAL_I2SEx_TransmitReceive_DMA( &hi2s3, (uint16_t*)null_table, (uint16_t*)bufAudio, 256 );

	SetCallbackI2STxRxCplt( NULL );
}

//========================================================================
void AudioRxTxLoop( void )
//========================================================================
{
//	g_bModeAudioRF = 0;
//	g_eAudioMode |= ( eAModLoopbackDMA );
//	g_eAudioMode &= ~( eAModRFTx | eAModRFRx );

	nAudioTable = 2;
	//	pAudioTable = sine_table;
	HAL_I2SEx_TransmitReceive_DMA( &hi2s3, (uint16_t*)bufAudio, (uint16_t*)&bufAudio[I2S_DMA_LOOP_SIZE], I2S_DMA_LOOP_SIZE );
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
	//		Spk. Vol
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

	//========================================================================
	//	Set Speaker Volume
//    WriteI2CCodec( 0x09, 0x0C );	//  0x0C ( -3 )
//    WriteI2CCodec( 0x09, 0x1E );	//  0x0C ( -12 )
//    WriteI2CCodec( 0x09, 0x2A );	//  0x0C ( -18 )
//    WriteI2CCodec( 0x09, 0x30 );	//  0x0C ( -21 )

//	//	sine 파 출력 검사. ( I2S Data 영역 검사 )
//	WriteI2CCodec( 0x09, 0x36 );	//  0x0C ( -24 )

//    WriteI2CCodec( 0x09, 0x3E );	//  0x0C ( -28 )
//            WriteI2CCodec( 0x0B, 0x20 );	//  01 ( +6 dB )
}

//========================================================================
void Default_I2SEx_TxRxCpltCallback( I2S_HandleTypeDef *hi2s )
//========================================================================
{
	//	printf( "%s(%d)\n", __func__, __LINE__ );

	//========================================================================
	//	Speex Loopback
	//	pAudioTable = sine_table;
	memcpy( &bufAudio[0], &bufAudio[I2S_DMA_LOOP_SIZE], I2S_DMA_LOOP_SIZE * 2 );
	HAL_I2SEx_TransmitReceive_DMA( &hi2s3, (uint16_t*)bufAudio, (uint16_t*)&bufAudio[I2S_DMA_LOOP_SIZE], I2S_DMA_LOOP_SIZE );
}

//========================================================================
int	AudioLoopbackDMA( void )
//========================================================================
{
	printf( "%s(%d)\n", __func__, __LINE__ );

	SetCallbackI2STxRxCplt( Default_I2SEx_TxRxCpltCallback );

	//	Speex ( 1 frame ) : 160 sample ( 320 bytes ) / 20 msec
	HAL_I2SEx_TransmitReceive_DMA( &hi2s3, (uint16_t*)bufAudio, (uint16_t*)&bufAudio[FRAME_SIZE], I2S_DMA_LOOP_SIZE );

	return 0;
}

//========================================================================
void AudioSpeex_I2SEx_TxRxCpltCallback( I2S_HandleTypeDef *hi2s )
//========================================================================
{
	//	printf( "%s(%d)\n", __func__, __LINE__ );

	//========================================================================
	//	Speex Loopback
	//	pAudioTable = sine_table;
//	memcpy( &bufAudio[0], &bufAudio[FRAME_SIZE], FRAME_SIZE );

	//	Audio Buffer Put
	qBufPut( &g_qBufAudioRx, (uint8_t *)&bufAudio[FRAME_SIZE], FRAME_SIZE );
	qBufGet( &g_qBufAudioTx, (uint8_t *)&bufAudio[0], FRAME_SIZE );

	HAL_I2SEx_TransmitReceive_DMA( &hi2s3, (uint16_t*)bufAudio, (uint16_t*)&bufAudio[FRAME_SIZE], FRAME_SIZE );
}

//	interpolation compress ( 보간압축 )
#define	AUDIO_COMPR_RATE	8	//	Audio 압축율.
//#define	AUDIO_COMPR_RATE	4	//	Audio 압축율.
//#define	AUDIO_COMPR_RATE	2	//	Audio 압축율.
//#define	AUDIO_COMPR_RATE	1	//	Audio 압축율.

#define	FRAME_ENC_SIZE		(I2S_DMA_LOOP_SIZE / AUDIO_COMPR_RATE)

static int16_t	bufAudioEnc[FRAME_ENC_SIZE];
static int16_t	bufAudioDec[FRAME_ENC_SIZE];

//========================================================================
void AudioLoopback_I2SEx_TxRxCpltCallback( I2S_HandleTypeDef *hi2s )
//========================================================================
{
	//	printf( "%s(%d)\n", __func__, __LINE__ );

	//	Loopback Encoding / Decoding

	//	Audio Buffer Put
//	qBufPut( &g_qBufAudioRx, (uint8_t *)&bufAudio[FRAME_SIZE], FRAME_SIZE );
//	qBufGet( &g_qBufAudioTx, (uint8_t *)&bufAudio[0], FRAME_SIZE );

	int16_t	*inBuf = &bufAudio[I2S_DMA_LOOP_SIZE];
	int16_t	*outBuf = &bufAudio[0];

	//	Encoding : 8 KHz -> 2 KHz
	int i;
	for( i = 0; i < FRAME_ENC_SIZE; i++ )
	{
		bufAudioEnc[i] = inBuf[ i * AUDIO_COMPR_RATE];
	}

	memcpy( bufAudioDec, bufAudioEnc, FRAME_ENC_SIZE * 2 );

	int dtVal;	//	sample 보간.

	//	Decoding : 2 KHz -> 8 KHz
	for( i = 0; i < I2S_DMA_LOOP_SIZE; i++ )
	{
		if ( i % AUDIO_COMPR_RATE == 0 )
		{
			outBuf[ i ] = bufAudioDec[i / AUDIO_COMPR_RATE];
			if( i == ( I2S_DMA_LOOP_SIZE - 1) ) dtVal = 0;
			else dtVal = (bufAudioDec[(i / AUDIO_COMPR_RATE) + 1] - outBuf[ i ]) / AUDIO_COMPR_RATE;
		}
		else
		{
			//	sample간 보간.
			outBuf[ i ] = outBuf[ i - 1 ] + dtVal;
		}
//		outBuf[ i ] = bufAudioDec[i / AUDIO_COMPR_RATE];
	}

	HAL_I2SEx_TransmitReceive_DMA( &hi2s3, (uint16_t*)outBuf, (uint16_t*)inBuf, FRAME_SIZE );
}

//========================================================================
void AudioSine_I2SEx_TxRxCpltCallback( I2S_HandleTypeDef *hi2s )
//========================================================================
{
	//	printf( "%s(%d)\n", __func__, __LINE__ );

	//========================================================================
	//	Speex Loopback
	//	pAudioTable = sine_table;
//	memcpy( &bufAudio[0], &bufAudio[FRAME_SIZE], FRAME_SIZE );

#if 1

	static int idx = 0;

	HAL_I2SEx_TransmitReceive_DMA( 	&hi2s3,
									(uint16_t*)&speex_sine_table[idx*160],			//	Audio Tx
									(uint16_t*)&bufAudio[FRAME_SIZE],
									FRAME_SIZE
									);

//	idx = (idx + 1) % 8;

#else

	HAL_I2SEx_TransmitReceive_DMA( 	&hi2s3,
									(uint16_t*)&sine_table[0],			//	Audio Tx
									(uint16_t*)&bufAudio[FRAME_SIZE],
									256
									);

#endif
}

//========================================================================
int		AudioLoopbackDMACompress( void )
//========================================================================
{
	printf( "%s(%d)\n", __func__, __LINE__ );

	SetCallbackI2STxRxCplt( AudioLoopback_I2SEx_TxRxCpltCallback );

	//	Speex ( 1 frame ) : 160 sample ( 320 bytes ) / 20 msec
	HAL_I2SEx_TransmitReceive_DMA( &hi2s3, (uint16_t*)bufAudio, (uint16_t*)&bufAudio[FRAME_SIZE], FRAME_SIZE );

	return 0;
}

//========================================================================
int		AudioLoopbackDMASpeex( void )
//========================================================================
{
	printf( "%s(%d)\n", __func__, __LINE__ );

	SetCallbackI2STxRxCplt( AudioSpeex_I2SEx_TxRxCpltCallback );

	//	Speex ( 1 frame ) : 160 sample ( 320 bytes ) / 20 msec
	HAL_I2SEx_TransmitReceive_DMA( &hi2s3, (uint16_t*)bufAudio, (uint16_t*)&bufAudio[FRAME_SIZE], FRAME_SIZE );

	return 0;
}

//========================================================================
int		AudioPlayDMASine( void )
//========================================================================
{
	printf( "%s(%d)\n", __func__, __LINE__ );

	SetCallbackI2STxRxCplt( AudioSine_I2SEx_TxRxCpltCallback );

	//	Speex ( 1 frame ) : 160 sample ( 320 bytes ) / 20 msec
	HAL_I2SEx_TransmitReceive_DMA( &hi2s3, (uint16_t*)bufAudio, (uint16_t*)&bufAudio[FRAME_SIZE], FRAME_SIZE );

	return 0;
}

static int tick_start, tick_mid, tick_mid2, tick_end;

//========================================================================
void	LoopProcAudio			( void )
//========================================================================
{

#if 0

	while( qBufCnt(&g_qBufAudioRx) >= FRAME_SIZE )
	{

#if 1
		tick_start = HAL_GetTick();

		qBufGet( &g_qBufAudioRx, &bufAudioCodec[0], FRAME_SIZE );
		/* Flush all the bits in the struct so we can encode a new frame */
		speex_bits_reset(&bits);

		//========================================================================
		//	Encoding
		/* Encode the frame */
		speex_encode_int(enc_state, (spx_int16_t*)&bufAudioCodec[0], &bits);
		/* Copy the bits to an array of char that can be decoded */
		speex_bits_write(&bits, (char *)out_bytes, ENCODED_FRAME_SIZE);

		tick_mid = HAL_GetTick();

		//========================================================================
		//	Decoding
		/* Copy the encoded data into the bit-stream struct */
		speex_bits_read_from(&bits, (char *)out_bytes, ENCODED_FRAME_SIZE);
		/* Decode the data */
		speex_decode_int(dec_state, &bits, (spx_int16_t*)&bufAudioCodec[FRAME_SIZE]);

		tick_mid2 = HAL_GetTick();

		qBufPut( &g_qBufAudioTx, &bufAudioCodec[FRAME_SIZE], FRAME_SIZE );

		tick_end = HAL_GetTick();

		printf("[%d] %d / %d / %d / %d\n", tick_end - tick_start, tick_start, tick_mid, tick_mid2, tick_end );

#else

		qBufGet( &g_qBufAudioRx, &bufAudioCodec[0], FRAME_SIZE );
		qBufPut( &g_qBufAudioTx, &bufAudioCodec[0], FRAME_SIZE );

#endif

	}

#else

#if 0

	int i;

	while( qBufCnt(&g_qBufAudioTx) <= FRAME_SIZE * ( I2S_DMA_LOOP_QCNT - 1) )
	{
		/* we prepare two buffers of decoded data: */
		/* the first one, */

		tick_start = HAL_GetTick();

		for(i=0;i<ENCODED_FRAME_SIZE; i++)
		{
			input_bytes[i] = male_voice[sample_index++];
		}

		speex_bits_read_from(&bits, input_bytes, ENCODED_FRAME_SIZE);
		speex_decode_int(dec_state, &bits, (spx_int16_t*)&bufAudioCodec[0]);

//		for( i = 0; i < FRAME_SIZE; i++ )
//		{
////			printf("%d ", (int16_t)bufAudioCodec[i] );
//			bufAudioCodec[i] <<= 2;
//		}

		qBufPut( &g_qBufAudioTx, &bufAudioCodec[0], FRAME_SIZE );

		tick_end = HAL_GetTick();

		printf("[%d] %d / %d\n", tick_end - tick_start, tick_start , tick_end );

//		for( i = 0; i < FRAME_SIZE; i++ )
//		{
//			printf("%d ", (int16_t)bufAudioCodec[i] );
//		}
//		printf("\n");
	}

	if( sample_index >= ALL_FRAMES ) sample_index = 0;

#else

//	{
//
//		qBufPut( &g_qBufAudioTx, &bufAudioCodec[0], FRAME_SIZE );
//
//	}

#endif

#endif

}

//========================================================================
void	AudioSpkVol	    ( int nSpkVol )
//========================================================================
{
    //========================================================================
    //	Codec MAX9860ETG+
    if ( HAL_OK == HAL_I2C_IsDeviceReady( &hi2c1, (uint16_t)( 0x10 << 1 ), 2, 2 ) )
    {
        // Power DAC / ADC Disable
//        WriteI2CCodec( 0x10, 0x00 );	//powered on, DAC on, both ADC(Left) Enable
        switch ( nSpkVol )
        {
        case 0:

            WriteI2CCodec( 0x09, 0xBA );	//  Mute ( 0xBC )
//            WriteI2CCodec( 0x0B, 0x00 );	//  00 ( 0 dB )
            break;
        case 1:
            WriteI2CCodec( 0x09, 0x0C );	//  0x0C ( -3 )
//            WriteI2CCodec( 0x0B, 0x20 );	//  01 ( +6 dB )
            break;
        case 2:
            WriteI2CCodec( 0x09, 0x06 );	//  0x02 ( 0 dB ) DAC adjustment, this would require testing and/or a better understanding of the overall system
//            WriteI2CCodec( 0x0B, 0x40 );	//  10 ( +12 dB )
            break;
        case 3:
            WriteI2CCodec( 0x09, 0x00 );	//  0x02 ( +3 )
//            WriteI2CCodec( 0x0B, 0x60 );	//  10 ( +18 dB )
            break;
        }

        // Power DAC / ADC Enable
//        WriteI2CCodec( 0x10, 0x8A );	//powered on, DAC on, both ADC(Left) Enable
    }

}


////========================================================================
//void	QPutAudioStream( char * sBuf, int nSize )
////========================================================================
//{
//	//	RF Data로 부터 수신된 Audio Stream
//
//	//	RF Data Recv -> Audio Out
//	if ( g_eAudioMode & eAModRFRx )
//	{
//		//	Audio Buffer Put
//		qBufPut( &g_qBufAudioRx, (uint8_t *)sBuf, nSize );
//	}
//}


//========================================================================
int cmd_audio( int argc, char *argv[] )
//========================================================================
{
	//	audio [ loop / null / sine / spk / mute ] [0/1 - spk relay]
	if ( argc < 2 )
	{
		printf( "%s(%d) - return\n", __func__, __LINE__ );
		return 0;
	}

	if ( strcmp( argv[1], "spk" ) == 0 )
	{
		if ( argc >= 3 )
		{
			if ( strcmp( argv[2], "1" ) == 0 )
			{
				//	Spk On
				printf( "%s(%d) - Spk Relay : On\n", __func__, __LINE__ );
				HAL_GPIO_WritePin( SPK_ON_GPIO_Port, SPK_ON_Pin, GPIO_PIN_SET );
			}
			else if ( strcmp( argv[2], "0" ) == 0 )
			{
				//	Spk On
				printf( "%s(%d) - Spk Relay : Off\n", __func__, __LINE__ );
				HAL_GPIO_WritePin( SPK_ON_GPIO_Port, SPK_ON_Pin, GPIO_PIN_RESET );
			}
		}
	}
	else if ( strcmp( argv[1], "loop" ) == 0 )
	{
		//	Audio Loop Test
		printf( "%s(%d) - loop\n", __func__, __LINE__ );

		AudioRxTxLoop();

		//	Spk On
		HAL_GPIO_WritePin( SPK_ON_GPIO_Port, SPK_ON_Pin, GPIO_PIN_SET );
	}
	else if ( strcmp( argv[1], "null" ) == 0 )
	{
		//	Audio Output Null
		printf( "%s(%d) - null\n", __func__, __LINE__ );

//		//	Spk Off
//		HAL_GPIO_WritePin( SPK_ON_GPIO_Port, SPK_ON_Pin, GPIO_PIN_RESET );

		AudioTxNull();
	}
	else if ( strcmp( argv[1], "sine" ) == 0 )
	{
		//	Audio Output Sine Wave
		printf( "%s(%d) - sine\n", __func__, __LINE__ );
		AudioTxSine();

//		//	Spk On
//		HAL_GPIO_WritePin( SPK_ON_GPIO_Port, SPK_ON_Pin, GPIO_PIN_SET );
	}
	else if ( strcmp( argv[1], "stop" ) == 0 )
	{
		//	Audio Loop Test
		printf( "%s(%d) - %s\n", __func__, __LINE__, argv[1] );

		AudioTxStop();
	}
	else
	{
		printf( "%s(%d) - Invalid Cmd : %s\n", __func__, __LINE__, argv[1] );
	}

	return 1;
}


//========================================================================
int cmd_codec( int argc, char *argv[] )
//========================================================================
{
	//	audio [ loop / null / sine / spk / mute ] [0/1 - spk relay]
	if ( argc < 2 )
	{
		printf( "%s(%d) - return\n", __func__, __LINE__ );
		return 0;
	}

	int bOnOff = 0;

	if ( argc >= 3 )
	{
		if ( strcmp( argv[2], "1" ) == 0 )
		{
			bOnOff = 1;
		}
		else if ( strcmp( argv[2], "0" ) == 0 )
		{
			bOnOff = 0;
		}
	}

	if ( strcmp( argv[1], "init" ) == 0 )
	{
		//	Audio Init
		printf( "%s(%d) - init\n", __func__, __LINE__ );

		AudioTxStop();
		HAL_Delay( 500 );

		InitCodecXE3005();
		HAL_Delay( 500 );

		AudioInit();
	}
	else if ( strcmp( argv[1], "mute" ) == 0 )
	{
		//	Audio Output Sine Wave
		printf( "%s(%d) - %s(%d)\n", __func__, __LINE__, argv[1], bOnOff );

		CodecMuteDAC( bOnOff );
	}
	else if ( strcmp( argv[1], "loop" ) == 0 )
	{
		//	Audio Codec Loopback
		printf( "%s(%d) - %s(%d)\n", __func__, __LINE__, argv[1], bOnOff );

		CodecLoopback( bOnOff );
	}
	else
	{
		printf( "%s(%d) - Invalid Cmd : %s\n", __func__, __LINE__, argv[1] );
	}

	return 1;
}


