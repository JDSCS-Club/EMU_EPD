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

/*------------------------------------------------------------------------------------------
	[ 구현목록 정의 ]
	* Func1 구현. 
		- Func1 기능 세부내용.

	* Func2 구현. 
-------------------------------------------------------------------------------------------*/

//========================================================================
#ifndef UNICODE_H
#define UNICODE_H
//========================================================================

#include "typedef.h"

#ifdef __cplusplus
extern "C" {
#endif


//int UTF8ToUnicode 		( const char_t * UTF8, uint16_t  *uc );
int UTF8ToUnicode	( const char_t * szUTF8, uint16_t* szUni );
//int UTF8ToUnicodeNum 	( const char_t * UTF8, uint16_t  *uc );
//int UTF8NumToUnicodeNum	( const char_t * szUTF8, int tUTF8Len, uint16_t* szUni );
//int UnicodeToUTF8 		( const uint16_t  *uc, char_t *UTF8 );
int UnicodeToUTF8	( const uint16_t  *szUni, char_t *szUTF8 );

/*
 * CP949 is EUC-KR, extended with UHC (Unified Hangul Code).
 *
 * Some variants of CP949 (in JDK, Windows-2000, ICU) also add:
 *
 * 2. Private area mappings:
 *
 *        code           Unicode
 *    0xC9{A1..FE}   U+E000..U+E05D
 *    0xFE{A1..FE}   U+E05E..U+E0BB
 *
 * We add them too because, although there are backward compatibility problems
 * when a character from a private area is moved to an official Unicode code
 * point, they are useful for some people in practice.
 */

/*
	Encoding
			CP949	UTF-8	UTF-16
	가		0xB0A1	0xAC00	0xEAB080
	나		0xB3AA	0xB098	0xEB8298
	다		0xB4D9	0xB2E4	0xEB8BA4

*/

int CP949ToUnicode 		( const char_t *szCP949, uint16_t *szUni );

int UnicodeToCP949 		( const uint16_t *szUni, char_t *szCP949 );


int CP949ToUTF8 		( const char_t *szCP949, char_t *szUTF8 );

int UTF8ToCP949 		( const char_t *szUTF8, char_t *szCP949 );

int printUTF8			( const char_t *szUTF8 );

int printUTF16			( const uint16_t *szUTF16 );

#ifdef __cplusplus
}
#endif

//========================================================================
#endif
//========================================================================


