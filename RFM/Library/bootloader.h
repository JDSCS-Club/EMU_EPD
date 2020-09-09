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

#ifndef	_BOOTLOADER_H_

void BootLoaderTask (void );

#define			BOOTLOADER_START_DELAY		3		//	3 Sec

enum eBootMode
{
	BModeNone	=	0,
	BModeBoot	=	1,
	BModeApp	=	2,
};

void	SetBootMode	( int nMode );
int		GetBootMode	( void );

#endif	/* _BOOTLOADER_H_ */

