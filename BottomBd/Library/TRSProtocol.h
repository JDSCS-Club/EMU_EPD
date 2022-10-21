/*------------------------------------------------------------------------------------------
	Project			: It Library
	Description		: SMRT Protocol Header

	Writer			: $Author: it $
	Revision		: $Rev: 1446 $
	Date			: 2018. 04.
	Copyright		: Piranti Corp.
	 
	Revision History 
	1. 2018. 04.	: Created
-------------------------------------------------------------------------------------------*/

#ifndef TRS_PROTOCOL_H
#define TRS_PROTOCOL_H

//========================================================================
// Header

#include "typedef.h"

#define SIL_RFM	1						//	무정전 SIL 프로토콜		-	TCMS( CH3 ) / TRS( CH5 )
//#define		TRS_SD_BAT		1		//	배터리 정보 추가. 201006_대곡소사40R_TRS-무정전장치_인터페이스.xlsx
//#define Line_4 	1

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

	eSDRTcms	=	0x21,
	eSDTcms		=	0x31,

	eSDRTrs		=	0x22,
	eSDTrs		=	0x32,

	eACKCODE	=	0x0A,
};

//=============================================================================
//	SDR	: TCMS -> EPD( RFM )

typedef struct _SDR_t
{
	uint8_t			cSDR;   			//	0	:	0x21
	uint8_t			c0x11;   			// 	1	:	0x11
	uint8_t			bYY_02;
	uint8_t			bMM_03;
	uint8_t			bDD_04;
	uint8_t			bhh_05;
	uint8_t			bmm_06;
	uint8_t			bss_07;
	uint8_t			bTrainNoVld:1;		//	8
	uint8_t			bSpare8_1:7;		//
	uint8_t			nTrainNo;			//	편성번호
	uint8_t			nSpare10[3];		//	10 ~ 12

} SDR_t, *PSDR_t;

typedef struct _FRAME_SDR
{
	uint8_t		nSTX;				//	STX	0x02

	SDR_t		sdr;				//	SDR Data

	uint8_t		nETX;				//	ETX	0x03
	uint8_t		nBCC1;				//	BCC
	uint8_t		nBCC2;				//	BCC
}	FRAME_SDR, *PFRAME_SDR;

//=============================================================================
//	SD	: EPD( RFM ) -> TCMS

typedef struct _SD_t
{
	uint8_t			cSD;   				//	0	:	0x30
	uint8_t			c0x22;   			// 	1	:	0x22
	uint8_t			bRFMFault:1;		//	2	: 	무선모듈불량
	uint8_t			bSpare2_1:7;   		//
	uint8_t			bPAOn:1;			//	3	: 	방송출력
	uint8_t			bLEDOn:1;			//		: 	조명On
	uint8_t			bSpare3_2:6;   		//
	uint8_t			nBatVal;			//	4	: 	배터리 잔량 ( 0 ~ 100 % )
	uint8_t			nSpare5[5];
	uint8_t			nVer0X_00:4;		//		: 	Ver 0X.00
	uint8_t			nVerX0_00:4;		//	10	: 	Ver X0.00
	uint8_t			nVer00_0X:4;		//		: 	Ver 00.0X
	uint8_t			nVer00_X0:4;		//	11	: 	Ver 00.X0
} SD_t, *PSD_t;


typedef struct _FRAME_SD
{
	uint8_t		nSTX;				//	STX	0x02

	SD_t		sd;					//	SD Data

	uint8_t		nETX;				//	ETX	0x03
	uint8_t		nBCC1;				//	BCC
	uint8_t		nBCC2;				//	BCC
}	FRAME_SD, *PFRAME_SD;


//=============================================================================
//	SDR	: TRS -> EPD( RFM )

typedef struct _TRSDR_t
{
	uint8_t			cSDR;   			//	0	:	0x21
	uint8_t			c0x11;   			// 	1	:	0x11
	uint8_t			bTRSFault:1;		//	2	: 	열차무선장치 불량
	uint8_t			bSpare2_1:7;   		//
	uint8_t			nSpare3[7];			//	3 ~ 9
} TRSDR_t, *PTRSDR_t;

typedef struct _FRAME_TRSDR
{
	uint8_t		nSTX;				//	STX	0x02

	TRSDR_t		sdr;				//	SDR Data

	uint8_t		nETX;				//	ETX	0x03
	uint8_t		nBCC1;				//	BCC
	uint8_t		nBCC2;				//	BCC
}	FRAME_TRSDR, *PFRAME_TRSDR;


//=============================================================================
//	SD	: EPD( RFM ) -> TRS

typedef struct _TRSD_t
{
	uint8_t			cSD;   				//	0	:	0x30
	uint8_t			c0x22;   			// 	1	:	0x22
	uint8_t			bRFMFault:1;		//	2	: 	무선모듈불량
	uint8_t			bSpare2_1:7;   		//
	uint8_t			nSpare3[7];			//	3 ~ 9
} TRSD_t, *PTRSD_t;

typedef struct _FRAME_TRSD
{
	uint8_t		nSTX;				//	STX	0x02

	TRSD_t		sd;					//	SD Data

	uint8_t		nETX;				//	ETX	0x03
	uint8_t		nBCC1;				//	BCC
	uint8_t		nBCC2;				//	BCC
} FRAME_TRSD, *PFRAME_TRSD;


#pragma pack()

#endif	//	TRS_PROTOCOL_H

