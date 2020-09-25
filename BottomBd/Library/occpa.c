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

//========================================================================
// Header

#include <stdint.h>		//	uint8_t

#include "occpa.h"

#include "main.h"		//	huart2
#include "serial.h"

//========================================================================
// Define

const char *sCmdOccPaStart	=	"occ 1\r";
const char *sCmdOccPaStop	=	"occ 0\r";

//========================================================================
// Function

//========================================================================
void RFMOccPaStart( void )
//========================================================================
{
	//	RFMBase -> RFM 명령 전송 : UART2
	printf("%s(%d)\n", __func__, __LINE__ );

	HAL_UART_Transmit( &huart2, (uint8_t *)sCmdOccPaStart, strlen(sCmdOccPaStart), 0xFFFF );
}

//========================================================================
void RFMOccPaStop( void )
//========================================================================
{
	//	RFMBase -> RFM 명령 전송 : UART2
	printf("%s(%d)\n", __func__, __LINE__ );

	HAL_UART_Transmit( &huart2, (uint8_t *)sCmdOccPaStop, strlen(sCmdOccPaStop), 0xFFFF );
}


//========================================================================
int cmd_occ(int argc, char *argv[])
//========================================================================
{
	//	RFMBase -> RFM 명령 전송 : UART2
	printf("%s(%d)\n", __func__, __LINE__ );

    //	occ [0/1]

	int		bOnOff = 0;

    switch ( argc )
    {
    case 2:		sscanf( argv[1], "%d", &bOnOff );		//	cmd [1/0]
        break;
    }

    if ( bOnOff == 1 )		RFMOccPaStart();
    else					RFMOccPaStop();

	return 0;
}



//========================================================================
