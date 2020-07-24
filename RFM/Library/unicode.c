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

//===========================================================================
// Header
#include <stdio.h>		//	NULL define 참조.

#include <stdlib.h>		//	abort()

#include <string.h>		//	memset()

#include "typedef.h"

#include "unicode.h"

//===========================================================================
// Function


//===========================================================================
static int _UTF8ToUnicode ( const char_t * UTF8, uint16_t  *uc )
//===========================================================================
{
	int tRequiredSize = 0;

	*uc = 0x0000;

	// ASCII byte 
	if ( 0 == ( UTF8[0] & 0x80 ) )
	{
		*uc = UTF8[0];
		tRequiredSize = 1;
	}
	// Start byte for 2byte
	else if ( 0xC0 == ( UTF8[0] & 0xE0 ) &&
		0x80 == ( UTF8[1] & 0xC0 ) )
	{
		*uc += ( UTF8[0] & 0x1F ) << 6;
		*uc += ( UTF8[1] & 0x3F ) << 0;
		tRequiredSize = 2;
	}
	// Start byte for 3byte
	else if ( 0xE0 == ( UTF8[0] & 0xE0 ) &&
		0x80 == ( UTF8[1] & 0xC0 ) &&
		0x80 == ( UTF8[2] & 0xC0 ) )
	{
		*uc += ( UTF8[0] & 0x1F ) << 12;
		*uc += ( UTF8[1] & 0x3F ) << 6;
		*uc += ( UTF8[2] & 0x3F ) << 0;
		tRequiredSize = 3;
	}
	else
	{
		// Invalid case
		//assert(false);
	}

	return tRequiredSize;
}

////===========================================================================
//static int UTF8ToUnicodeNum ( const char_t * UTF8, uint16_t *uc )
////===========================================================================
//{
//	int tRequiredSize = 0;
//
//	*uc = 0x0000;
//
//	// ASCII byte 
//	*uc += ( UTF8[0] & 0xff ) << 0;
//	tRequiredSize = 1;
//
//	return tRequiredSize;
//}

/*
//===========================================================================
static int UTF8NumToUnicodeNum( const char_t * szUTF8, int tUTF8Len, uint16_t* szUni )
//===========================================================================
{
	int tReadPos = 0;
	int i = 0;

	for ( i = 0; tReadPos < tUTF8Len; i++ )
	{
		if ( szUTF8[i] == NULL )
			return i;

		uint16_t tTempUnicodeNum = 0;
		int tSize = UTF8ToUnicodeNum( szUTF8 + tReadPos, &tTempUnicodeNum );
		if ( NULL != szUni )
			szUni[i] = tTempUnicodeNum;
		tReadPos += tSize;
	}
	return i;
}
*/

//===========================================================================
int UTF8ToUnicode( const char_t * szUTF8, uint16_t* szUni )
//===========================================================================
{
	int tReadPos = 0;
	int i = 0;
	int tUTF8Len;

	tUTF8Len = strlen( szUTF8 );

	for ( i = 0; tReadPos < tUTF8Len; i++ )
	{
		if ( szUTF8[i] == NULL )
			return i;

		uint16_t tTempUnicodeChar = 0;
		int tSize = _UTF8ToUnicode( szUTF8 + tReadPos, &tTempUnicodeChar );
		if ( NULL != szUni )
			szUni[i] = tTempUnicodeChar;
		szUni[i + 1] = '\0';
		tReadPos += tSize;
	}
	return i;
}


//===========================================================================
static int _UnicodeToUTF8 ( const uint16_t  *uc, char_t *UTF8 )
//===========================================================================
{
	int tRequiredSize = 0;

	uint16_t	ch = *uc;

	if ( ch < 0x80 )
	{
		UTF8[0] = ( ch & 0xFF );
		UTF8[1] = '\0';

		tRequiredSize = 1;
	}
	else if ( ch < 0x800 )
	{
		UTF8[0] = ( ch >> 6 ) | 0xC0;
		UTF8[1] = ( ch & 0x3F ) | 0x80;
		UTF8[2] = '\0';

		tRequiredSize = 2;
	}
	else
	{
		UTF8[0] = ( ch >> 12 ) | 0xE0;
		UTF8[1] = ( ( ch >> 6 ) & 0x3F ) | 0x80;
		UTF8[2] = ( ch & 0x3F ) | 0x80;
		UTF8[3] = '\0';

		tRequiredSize = 3;
	}

	return tRequiredSize;
}

//===========================================================================
int UnicodeToUTF8( const uint16_t * szUni, char_t * szUTF8 )
//===========================================================================
{
	int tReadPos = 0;
	int i = 0;

	for ( i = 0; szUni[i] != NULL; i++ )
	{
		uint16_t tTempUnicodeChar = 0;
		int tSize = _UnicodeToUTF8( &szUni[i], szUTF8 + tReadPos );

		tReadPos += tSize;
	}

	return i;
}


//===========================================================================
//
//		CP949 to Unicode
//
//===========================================================================


#if 1

//===========================================================================
// Define


/* Return code if invalid input after a shift sequence of n bytes was read.
   (xxx_mbtowc) */
#define RET_SHIFT_ILSEQ(n)  (-1-2*(n))
   /* Return code if invalid. (xxx_mbtowc) */
#define RET_ILSEQ           RET_SHIFT_ILSEQ(0)
/* Return code if only a shift sequence of n bytes was read. (xxx_mbtowc) */
#define RET_TOOFEW(n)       (-2-2*(n))
/* Retrieve the n from the encoded RET_... value. */
#define DECODE_SHIFT_ILSEQ(r)  ((unsigned int)(RET_SHIFT_ILSEQ(0) - (r)) / 2)
#define DECODE_TOOFEW(r)       ((unsigned int)(RET_TOOFEW(0) - (r)) / 2)
/* Maximum value of n that may be used as argument to RET_SHIFT_ILSEQ or RET_TOOFEW. */
#define RET_COUNT_MAX       ((INT_MAX / 2) - 1)

/* Return code if invalid. (xxx_wctomb) */
#define RET_ILUNI      -1
/* Return code if output buffer is too small. (xxx_wctomb, xxx_reset) */
#define RET_TOOSMALL   -2


#include "ksc5601.h"



//===========================================================================
static int cp949_mbtowc ( const unsigned char *s, uint16_t *pwc )
//===========================================================================
{
	int tRequiredSize = 0;

	unsigned char c = *s;
	/* Code set 0 (ASCII) */
	if ( c < 0x80 )
	{
		*pwc = c;
		tRequiredSize = 1;
		return 1;
	}
	/* UHC part 1 */
	if ( c >= 0x81 && c <= 0xa0 )
	{
//		return uhc_1_mbtowc( pwc, s );
		*pwc = '\0';
		return 2;	//	skip
	}
	if ( c >= 0xa1 && c < 0xff )
	{
		unsigned char c2 = s[1];
		if ( c2 < 0xa1 )
		{
			/* UHC part 2 */
			//return uhc_2_mbtowc( pwc, s );
			*pwc = '\0';
			return 2;	//	skip
		}
		else if ( c2 < 0xff && !( c == 0xa2 && c2 == 0xe8 ) )
		{
			/* Code set 1 (KS C 5601-1992, now KS X 1001:1998) */
			unsigned char buf[2];
			int ret;
			buf[0] = c - 0x80; buf[1] = c2 - 0x80;
			ret = ksc5601_mbtowc( pwc, buf );		//	한글변환.
			if ( ret != RET_ILSEQ )
				return ret;
			/* User-defined characters */
			if ( c == 0xc9 ) {
				*pwc = 0xe000 + ( c2 - 0xa1 );
				return 2;
			}
			if ( c == 0xfe ) {
				*pwc = 0xe05e + ( c2 - 0xa1 );
				return 2;
			}
		}
	}
}

//===========================================================================
int CP949ToUnicode( const char_t * CP949, uint16_t * szUni )
//===========================================================================
{
	int tReadPos = 0;
	int i = 0;
	int tUTF8Len;

	tUTF8Len = strlen( CP949 );

	for ( i = 0; tReadPos < tUTF8Len; i++ )
	{
		if ( CP949[i] == NULL )
			return i;

		uint16_t tTempUnicodeChar = 0;

		int tSize = cp949_mbtowc( (const unsigned char *)CP949 + tReadPos, &tTempUnicodeChar );
		if ( NULL != szUni )
			szUni[i] = tTempUnicodeChar;

		tReadPos += tSize;
	}
	return i;
}

//===========================================================================
static int cp949_wctomb ( const uint16_t wc, char_t *r )
//===========================================================================
{
	unsigned char buf[2];
	int ret;

	/* Code set 0 (ASCII) */

	if ( wc < 0x80 )
	{
		*r = ( wc & 0xFF );
		r[1] = '\0';

		return 1;
	}

//	ret = ascii_wctomb( conv, r, wc, n );
//	if ( ret != RET_ILUNI )
//		return ret;

	/* Code set 1 (KS C 5601-1992, now KS X 1001:1998) */
	if ( wc != 0x327e )
	{
		ret = ksc5601_wctomb( buf, wc );
		if ( ret != RET_ILUNI )
		{
			if ( ret != 2 ) abort();
//			if ( n < 2 )
//				return RET_TOOSMALL;
			r[0] = buf[0] + 0x80;
			r[1] = buf[1] + 0x80;
			return 2;
		}
	}

	/* UHC */
//	if ( wc >= 0xac00 && wc < 0xd7a4 )
//	{
//		if ( wc < 0xc8a5 )
//			return uhc_1_wctomb( conv, r, wc, n );
//		else
//			return uhc_2_wctomb( conv, r, wc, n );
//	}

	/* User-defined characters */
	if ( wc >= 0xe000 && wc < 0xe0bc )
	{
//		if ( n < 2 )
//			return RET_TOOSMALL;
		if ( wc < 0xe05e )
		{
			r[0] = 0xc9;
			r[1] = wc - 0xe000 + 0xa1;
		}
		else
		{
			r[0] = 0xfe;
			r[1] = wc - 0xe05e + 0xa1;
		}
		return 2;
	}

	return RET_ILUNI;
}

#endif


//===========================================================================
int UnicodeToCP949( const uint16_t * szUni, char_t * CP949 )
//===========================================================================
{
	int tReadPos = 0;
	int i = 0;

	for ( i = 0; szUni[i] != NULL; i++ )
	{
		uint16_t tTempUnicodeChar = 0;
		int tSize = cp949_wctomb( szUni[i], CP949 + tReadPos );

		tReadPos += tSize;
	}

	return 0;
}

//===========================================================================
int CP949ToUTF8 		( const char_t *szCP949, char_t *szUTF8 )
//===========================================================================
{
	uint16_t	szBuf[64];
	memset( szBuf, 0, sizeof( szBuf ) );

	CP949ToUnicode( szCP949, szBuf );

	return UnicodeToUTF8( szBuf, szUTF8 );
}

//===========================================================================
int UTF8ToCP949 		( const char_t *szUTF8, char_t *szCP949 )
//===========================================================================
{
	uint16_t	szBuf[64];
	memset( szBuf, 0, sizeof( szBuf ) );

	UTF8ToUnicode( szUTF8, szBuf );

	return UnicodeToCP949( szBuf, szCP949 );
}

//===========================================================================
int printUTF8( const char_t * szUTF8 )
//===========================================================================
{
	int idx;
	int cnt;

	printf( "%s(%d) : %s\n", __func__, __LINE__, szUTF8 );
	printf( "[ " );

	cnt = strlen( szUTF8 );

	for ( idx = 0; idx < cnt; idx++ )
	{
		printf( "%02X ", (uint8_t)szUTF8[idx] );
	}

	printf( "]\n" );

	return 0;
}

//===========================================================================
int printUTF16( const uint16_t * szUTF16 )
//===========================================================================
{
	int idx;

	printf( "%s(%d) : %s\n", __func__, __LINE__, szUTF16 );
	printf( "[ " );

	for ( idx = 0; szUTF16[idx] != '\0'; idx++ )
	{
		printf( "%02X ", (uint16_t)szUTF16[idx] );
	}

	printf( "]\n" );

	return 0;
}


//===========================================================================
