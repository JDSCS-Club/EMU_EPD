/*------------------------------------------------------------------------------------------
	Project			: It Library
	Description		: SMRT Protocol Header

	Writer			: $Author: it $
	Revision		: $Rev: 1446 $
	Date			: 2018. 04.
	Copyright		: Piranti Corp.
	 
	Revision History 
	1. 2018. 04.	: Created
	
	TNI - Board에서는  Union 지원이 안됨 필요한것들 만들어서 사용
-------------------------------------------------------------------------------------------*/

#ifndef TRS_PROTOCOL_H
#define TRS_PROTOCOL_H


//========================================================================
// Header

#include "typedef.h"

//#define		TRS_SD_BAT		1		//	배터리 정보 추가. 201006_대곡소사40R_TRS-무정전장치_인터페이스.xlsx
#define Line_4 	1

//========================================================================
// Define

enum eSMRT2LedRs485Def
{
	RS485_BAUD		=	38400,
};

enum eRS485Def
{
	eSTX		=	0x02,
	eETX		=	0x03,

	eSDR		=	0x21,
	eSD			=	0x31,

	eACKCODE	=	0x0A,
};

//=============================================================================
//	SDR	: TRS -> EPD( RFM )

typedef struct _SDR_t
{
#if defined(Line_4)
	uint8_t			cSDR;   			//	0	:	0x21
	uint8_t			c0x11;   			// 	1	:	0x11
	uint8_t			bYY_02;
	uint8_t			bMM_03;
	uint8_t			bDD_04;
	uint8_t			bhh_05;
	uint8_t			bmm_06;
	uint8_t			bss_07;
	uint8_t			bEmergency_08;
	uint8_t			bReq_SelfTest_09;
	uint8_t			bReq_TraceDate_10;
	uint8_t			bSpare_11;
	uint8_t			bSpare_12;
	uint8_t			nWatchDog;   		//	7	:	Watchdog count ( 00 ~ FFh )

#else


	uint8_t			cSDR;   			//	0	:	0x21
	uint8_t			c0x11;   			// 	1	:	0x11
	uint8_t			bSpare2_0:1;   		//	2
	uint8_t			bOccPaActive:1;   	//
	uint8_t			bSpare2_2:6;   		//
	uint8_t			bSpare3[2];   		//	3 ~ 4
	uint8_t			bOccPaStart:1;   	//	5
	uint8_t			bOccPaStop:1;   	//
	uint8_t			bSpare5_2:6;   		//
	uint8_t			bSpare6;   			//	6
	uint8_t			nWatchDog;   		//	7	:	Watchdog count ( 00 ~ FFh )
#endif

} SDR_t, *PSDR_t;

// 4호선
typedef struct _SDR_t_4line
{
	uint8_t			cSDR;   			//	0	:	0x21
	uint8_t			c0x11;   			// 	1	:	0x11
	uint8_t			bYY_02;
	uint8_t			bMM_03;
	uint8_t			bDD_04;
	uint8_t			bhh_05;
	uint8_t			bmm_06;
	uint8_t			bss_07;
	uint8_t			bEmergency_08;
	uint8_t			bReq_SelfTest_09;
	uint8_t			bReq_TraceDate_10;
	uint8_t			bSpare_11;
	uint8_t			bSpare_12;
	uint8_t			nWatchDog;   		//	7	:	Watchdog count ( 00 ~ FFh )
} SDR_t_4line, *PSDR_t_4line;

typedef struct _FRAME_SDR
{
	uint8_t		nSTX;				//	STX	0x02
	SDR_t		sdr;				//	SDR Data
	uint8_t		nETX;				//	ETX	0x03
	uint8_t		nBCC1;				//	BCC
	uint8_t		nBCC2;				//	BCC
}	FRAME_SDR, *PFRAME_SDR;

//=============================================================================
//	SD	: EPD( RFM ) -> TRS

typedef struct _SD_t
{

#if defined(Line_4)
	uint8_t			cSD;   				//	0	:	0x31
	uint8_t			c0x22;   			// 	1	:	0x22
	uint8_t			bSpare1[4];   		//	2 ~ 4
	uint8_t 		bDate[6];
	uint8_t			bTraceCode;
	uint8_t			bTraceFlag;
	uint8_t			bTraceData[7];
	uint8_t			bSpare2[3];
	uint8_t			nWatchDog;   		//	7	:	Watchdog count ( 00 ~ FFh

#else


	uint8_t			cSD;   				//	0	:	0x30
	uint8_t			c0x22;   			// 	1	:	0x22
	uint8_t			bSpare2[3];   		//	2 ~ 4
	uint8_t			bOccPaStart:1;   	//	5
	uint8_t			bOccPaStop:1;   	//
	uint8_t			bSpare5_2:6;   		//
	uint8_t			bSpare6;   			//	6
	#if defined(TRS_SD_BAT)
		uint8_t			nSBATR1;   			//	7
		uint8_t			nSBATR2;   			//	8
		uint8_t			nSBATR3;   			//	9
		uint8_t			nSBATR4;   			//	10
		uint8_t			nSBATR5;   			//	11
		uint8_t			nSBATR6;   			//	12
		uint8_t			bSpare13;   		//	13
		uint8_t			bSpare14;   		//	14
	#endif	//	defined(TRS_IF_BAT)
	uint8_t			nWatchDog;   		//	7	:	Watchdog count ( 00 ~ FFh )

#endif

} SD_t, *PSD_t;

 // 4호선 장치
typedef struct _SD_t_4line
{
	uint8_t			cSD;   				//	0	:	0x31
	uint8_t			c0x22;   			// 	1	:	0x22
	uint8_t			bSpare1[4];   		//	2 ~ 4
	uint8_t 		bDate[6];
	uint8_t			bTraceCode;
	uint8_t			bTraceFlag;
	uint8_t			bTraceData[7];
	uint8_t			bSpare2[3];
	uint8_t			nWatchDog;   		//	7	:	Watchdog count ( 00 ~ FFh )
} SD_t_4line, *PSD_t_4line;


typedef struct _FRAME_SD
{
	uint8_t		nSTX;				//	STX	0x02

	SD_t		sd;					//	SD Data

	uint8_t		nETX;				//	ETX	0x03
	uint8_t		nBCC1;				//	BCC
	uint8_t		nBCC2;				//	BCC
}	FRAME_SD, *PFRAME_SD;


#pragma pack()

#endif//SMRT_PROTOCOL_H

