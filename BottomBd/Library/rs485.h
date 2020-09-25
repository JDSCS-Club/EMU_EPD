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

void	LoopProcRS485		( void );


void	vRs485Task			( void *pvParameters );

void	ProcessFrameSD		( const uint8_t *pbuf, int length );
void	ProcessFrameSDR		( const uint8_t *pbuf, int length );

void	ProcessFrame		( const uint8_t *pbuf, int length );

uint8_t	GetRspDevIdMin		( void );		//	응답 Device중 가장 낮은 ID return.

void 	SendSD				( const FRAME_SDR *pSdr );		//  Send SD Data


int 	cmd_sd				( int argc, char *argv[] );		//	Send SD

//===========================================================================
#if __cplusplus
};	//	extern "C"
#endif
//===========================================================================

#endif	/* _RS485_H_ */

