/*------------------------------------------------------------------------------------------
	Project			: RFM
	Description		: Bootloader

	Writer			: $Author: zlkit $
	Revision		: $Rev: 1891 $
	Date			: 2019. 01.
	Copyright		: Piranti Corp. ( zlkit@piranti.co.kr )
	 
	Revision History 
	1. 2019. 01.	: Created
-------------------------------------------------------------------------------------------*/

#ifndef	BOOTLOADER_H
#define BOOTLOADER_H

void BootLoaderTask (void );

#define			BOOTLOADER_START_DELAY		3		//	3 Sec

void JumpToSTBootloader(void);		//	STM32 Internal Bootloader

enum eBootMode
{
	BModeNone	=	0,
	BModeBoot	=	1,
	BModeApp	=	2,
};

void	SetBootMode	( int nMode );
int		GetBootMode	( void );

//========================================================================
int 	cmd_stboot ( int argc, char *argv[] );
//========================================================================

#endif	/* BOOTLOADER_H */

