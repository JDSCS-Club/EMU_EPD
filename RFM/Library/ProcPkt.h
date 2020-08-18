/*------------------------------------------------------------------------------------------
	Project			: RFM
	Description		: 

	Writer			: $Author: zlkit $
	Revision		: $Rev: 1891 $
	Date			: 2020. 07.
	Copyright		: Piranti Corp.	( zlkit@piranti.co.kr )
	 
	Revision History 
	1. 2020. 07.	: Created
-------------------------------------------------------------------------------------------*/

//========================================================================
#ifndef PROC_PKT_H
#define PROC_PKT_H
//========================================================================

//========================================================================
//	Dump Function

void	Dump		( const char *sTitle, const char *sBuf, int nSize );

//========================================================================
//	Packet Procedure
int		InitProcPkt ( void );

void	LoopProcPkt	( int nTick );

int		SendPacket	( const char *sBuf, int nSize );

void 	CallbackRecvPacket	( const char *pData, int nSize );

//========================================================================
//	Demo Example
void	TestProcPkt	( void );

//========================================================================
#endif
//========================================================================

