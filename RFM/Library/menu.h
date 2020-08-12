/*------------------------------------------------------------------------------------------
	Project			: It Library
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

//========================================================================
#ifndef RF_PA_H
#define RF_PA_H
//========================================================================

#include "RFMProtocol.h"		//	

//========================================================================
//	Define

#define		USE_RFT_MENU_TRAINSET		1		//	송신기 편성설정.

#define		USE_RFT_ONLY_RX_SPK_ON		1		//	송신기 수신중일때 만 Spk On.

//========================================================================

#define		RF_CMD_SIZE				RFPktHdrLen			//	[4Byte Commnad]

//#define		I2S_DMA_LOOP_SIZE		(RFPktDataLen/2)	//	[4Byte Commnad] [ 60Byte Stream Data ]
//
//#define		I2S_DMA_LOOP_QCNT		8		//	Queue Count
////#define		I2S_DMA_LOOP_QCNT		6	//	Queue Count

//========================================================================

enum eRFMMode
{
	RFMModeNormal	=	0,
	RFMModeTx		=	1,
	RFMModeRx		=	2,
};

extern int		g_nSpkLevel;                //  Default (2) - 0(Mute) / 1 / 2(Normal) / 3

extern char		*g_sTrainSet[];
extern int		g_cntTrainSet;

//========================================================================
int		GetRFMMode		( void );
void	SetRFMMode		( int nRFMMode );
//========================================================================

void    UpdateLCDMain	( void );
void    UpdateLCDMenu	( void );

void    ProcBtnUp		( void );
void    ProcBtnDown		( void );

void    ProcBtnMenu		( void );
void    ProcBtnOK		( void );

#if defined(USE_ENV_TEST)

void	SetLoopRFTx		( int bEnable );	//	RF Data 전송 유지..

int		cmd_rftx		( int argc, char * argv[] );

#endif	//	defined(USE_ENV_TEST)

void	RF_PA_loop		( void );

int		RF_PA_Init		( void );

int		RF_PA_main		( void );

void	RF_PA_I2SEx_TxRxCpltCallback	( I2S_HandleTypeDef* hi2s );


//========================================================================
#endif
//========================================================================


