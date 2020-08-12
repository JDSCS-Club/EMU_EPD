/*------------------------------------------------------------------------------------------
	Project			: RFM
	Description		: 

	Writer			: $Author: zlkit $
	Revision		: $Rev: 1891 $
	Date			: 2019. 10.
	Copyright		: Piranti Corp.	( sw@piranti.co.kr )
	 
	Revision History 
	1. 2020. 02.	: Created
-------------------------------------------------------------------------------------------*/

//========================================================================
#ifndef KEY_PAD_H
#define KEY_PAD_H
//========================================================================

void	GetKeyStat	( int *btnStat );

void	LoopProcKey ( uint32_t tickCurr );

//========================================================================
//		KeyPad ( Dome Button )
//========================================================================

void	KeyMenu			( int bValue );
void	KeyUp			( int bValue );
void	KeyOK			( int bValue );

void	KeyLight		( int bValue );
void	KeyDown			( int bValue );
void	KeyVol			( int bValue );

//========================================================================
//		Side Button
//========================================================================

void	KeyPtt			( int bValue );
void	KeySos			( int bValue );

//========================================================================
//		Side Button
//========================================================================

void	KeyPwrOnOff		( int bValue );


//========================================================================
#endif
//========================================================================

