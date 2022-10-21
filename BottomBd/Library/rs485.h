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

#ifndef	_RS485_H_
#define	_RS485_H_

//===========================================================================
#if __cplusplus
extern "C" {
#endif
//===========================================================================

#include "TRSProtocol.h"	//	FRAME_SDR

void	InitRS485			( void );

void	LoopProcRS485_3ch   ( void );
void    LoopProcRS485_5ch   ( void );
void    LoopProcRFM_2ch   	( void );


void	vRs485Task			( void *pvParameters );

void	ProcessFrameSD		( const uint8_t *pbuf, int length );
void	ProcessFrameSDR		( const uint8_t *pbuf, int length,int nCh );

void	ProcessFrame		( const uint8_t *pbuf, int length,int nCh );

uint8_t	GetRspDevIdMin		( void );		//	응답 Device중 가장 낮은 ID return.

void 	SendSD_3			( const FRAME_SDR *pSdr,int nCh );		//  Send SD Data
void 	SendSD_5			( const FRAME_SDR *pSdr,int nCh );		//  Send SD Data

int 	cmd_sd				( int argc, char *argv[] );		//	Send SD
int 	cmd_sd2				( int argc, char *argv[] );		//	Send SD2

void Dump( char *sTitle, char *sBuf, int nSize );

int	SendRS485 ( char *bufTx, int nSize,int nCh );

//===========================================================================
#if __cplusplus
};	//	extern "C"
#endif
//===========================================================================

#endif	/* _RS485_H_ */

