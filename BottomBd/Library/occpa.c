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
#include "naranja_boron.h"
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

	HAL_UART_Transmit( &huart2, (uint8_t *)sCmdOccPaStart, strlen(sCmdOccPaStart), 100 );
}

//========================================================================
void RFMOccPaStop( void )
//========================================================================
{
	//	RFMBase -> RFM 명령 전송 : UART2
	printf("%s(%d)\n", __func__, __LINE__ );

	HAL_UART_Transmit( &huart2, (uint8_t *)sCmdOccPaStop, strlen(sCmdOccPaStop), 100 );
}


//========================================================================
int cmd_occ(int argc)
//========================================================================
{
	//	RFMBase -> RFM 명령 전송 : UART2
	printf("%s(%d)\n", __func__, __LINE__ );

    //	occ [0/1]

    static int		bOnOff = 0;
        
    static int sCnt = 0;    
     
    static int sOccFlag = 0;
        
    static int sProCnt = 0;
        
            
    if(!uDI_getMasterIn) //접점이 ON 되면 동작.
    {
        
        bOnOff = 1;
            
         sOccFlag = 1;
            
        sCnt++;
            
        if(!(sCnt%10))
        {
            printf("---Master Input \n\r" ); 
        }
    }
    else
    {
        bOnOff = 0;
    }

//    switch ( argc )
//    {
//    case 2:		sscanf( argv[1], "%d", &bOnOff );		//	cmd [1/0]
//        break;
//    }

    if ( (bOnOff == 1) && (sOccFlag == 1) )
    {
		RFMOccPaStart();
    }
    else if ( (bOnOff == 0) && (sOccFlag == 1) )	
    {
        sProCnt++;
        
        if(sProCnt > 3)
        {
            sOccFlag = 0;
        }
        RFMOccPaStop();
    }

	return 0;
}



//========================================================================
