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

	eSDR		=	0x40,
	eSD			=	0x41,
	eAddrLED	=	0x21,
	eAddrCtrl	=	0x20,

	eACKCODE	=	0x0A,
};

//=============================================================================
//	SDR	: TRS -> EPD( RFM )

typedef struct _SDR_t
{
	uint8_t			cSDR;   			//	0	:	0x20
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
//	SD	: EPD( RFM ) -> TRS

typedef struct _SD_t
{
	uint8_t			cSD;   				//	0	:	0x30
	uint8_t			c0x22;   			// 	1	:	0x22
	uint8_t			bSpare2[3];   		//	2 ~ 4
	uint8_t			bOccPaStart:1;   	//	5
	uint8_t			bOccPaStop:1;   	//
	uint8_t			bSpare5_2:6;   		//
	uint8_t			bSpare6;   			//	6
	uint8_t			nWatchDog;   		//	7	:	Watchdog count ( 00 ~ FFh )
} SD_t, *PSD_t;


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

