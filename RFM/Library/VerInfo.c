/*------------------------------------------------------------------------------------------
	Project			: LED
	Description		: 

	Writer			: $Author: zlkit $
	Revision		: $Rev: 1891 $
	Date			: 2019. 01.
	Copyright		: Piranti Corp. ( zlkit@piranti.co.kr )
	 
	Revision History 
	1. 2019. 01.	: Created
-------------------------------------------------------------------------------------------*/

//========================================================================
// Header

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//#include <tchar.h>			// TCHAR

#include "VerInfo.h"

//========================================================================
// Define

const char_t *g_sCompileDate = __DATE__;
const char_t *g_sCompileTime = __TIME__;

//========================================================================
// Function

VER_INFO *GetAppVer() { return &g_Version; }

//========================================================================
void	init_Version( void )			// Init Version	버전정포 Parsing
//========================================================================
{
	VER_INFO *pThis = &g_Version;

	static const char_t *const monthnames[] =
	{
		"Jan", "Feb", "Mar", "Apr", "May", "Jun",
		"Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
	};

	char_t	_year[10],	_month[10],		_day[10];
	char_t	_hour[10],	_min[10],		_sec[10];
	int32_t i;

	sscanf	(	g_sCompileDate, "%s %s %s",
					_month,
					_day,
					_year					);

	pThis->year	=	atoi(_year);

	for( i = 0; i < 12; i++ )
	{
		if( strcmp( _month, monthnames[ i ] ) == 0 )
		{
			pThis->month = i + 1U;
			break;
		}
	}

	pThis->day	=	atoi( _day );
	
	sscanf( g_sCompileTime,	"%s",	_hour);//,	_countof(_hour)	);
	sscanf( g_sCompileTime+3,	"%s",	_min);//,	_countof(_min)	);
	sscanf( g_sCompileTime+6,	"%s",	_sec);//,	_countof(_sec)	);

	pThis->hour	=	atoi( _hour	);
	pThis->min	=	atoi( _min	);
	pThis->sec	=	atoi( _sec	);
	
	pThis->ver_id	=	0;//VER_PC_INFOMEDIA;	// Version ID InfoMedia

	printf( "%s Compile Date - %s - %s\n",	__FUNCTION__, g_sCompileDate, g_sCompileTime );
	printf( "%s Version - %d.%d.%d.%d\n",	__FUNCTION__, pThis->ver_main, pThis->ver_sub, pThis->ver_det, pThis->ver_build );
}
