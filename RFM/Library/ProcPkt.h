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
extern int nTxPkt;		//	Tx Packet Count
extern int nRxPkt;		//	Rx Packet Count
extern int nHopPkt;		//	Hopping Packet Count
extern int nDropPkt;	//	Drop Packet Count

//========================================================================
extern int nRxErr;
extern int nCrcErr;

//========================================================================

extern int nTxStamp;	//	Tx Tick Stamp
extern int nRxStamp;	//	Rx Tick Stamp


extern int			g_bSetRspIDManual;	//  RspID Flag 수동설정. ( 디버깅용 )
extern uint16_t	 	g_flagRspID;		//  범위 안의 Device ID Flag ( 0 ~ 15 bit )
extern uint8_t	 	g_nPktSeq;			//  Packet Sequence


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

int 	cmd_pktmon	( int argc, char * argv[] );

//========================================================================
#endif
//========================================================================

