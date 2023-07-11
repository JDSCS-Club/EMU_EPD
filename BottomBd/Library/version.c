/*------------------------------------------------------------------------------------------
	Project			: LED
	Description		: 

	Writer			: $Author: zlkit $
	Revision		: $Rev: 1891 $
	Date			: 2019. 12.
	Copyright		: Piranti Corp. ( zlkit@piranti.co.kr )
	 
	Revision History 
	1. 2019. 12.	: Created
-------------------------------------------------------------------------------------------*/

//========================================================================
// Header

#include "version.h"
#include "VerInfo.h"		//	VER_INFO structure

//========================================================================
// Define

const char *g_sBoard = "RFM";

VER_INFO			g_Version = {
	FILEVER,	//	APP_MAIN_VER, APP_SUB_VER, APP_DETAIL_VER, APP_BUILD_VER,
	0,
};			// Version Information

/*
Version History

v1.0.0
	19/12/04
	- 최초작성.
	
*/

void Version_History()
{
	printf( "-----------------------------\r\n" );
	printf( "      RFM History			\r\n" );
	printf( " v1.0.0 : 19/12/04			\r\n" );
	printf( "        - 최초작성.		\r\n" );
}

