/*------------------------------------------------------------------------------------------
	Project			: Font
	Description		:

	Writer			:
	Revision		:
	Date			: 2020. 1.
	Copyright		: Piranti Corp.

	Revision History
	1. 2020. 1.	: Created
-------------------------------------------------------------------------------------------*/

//#define  PHAN_LIB_LOCAL
//#define  PHANFONT_LOCAL
//#define  PHANFONTENG_LOCAL

#include "PHan_Lib.h"
#include "PHanFont.h"
#include "PHanFontEng.h"

#include <stdio.h>

#include <string.h>		//	memset()

//========================================================================
#if defined(PHANFONT_LOCAL)
//========================================================================

//========================================================================
uint16_t PHan_CnvCodeWan2Johab(uint16_t WanCode)
//========================================================================
{
	uint16_t	index;
	uint16_t	hcode, lcode;

	hcode = (WanCode >> 8) & 0xFF;
	lcode = WanCode & 0x0ff;

	index = (hcode - 0x0B0) * 94 + (lcode - 0x0A1);

	return wWanToJohabTable[index];
}

//static declaration 은 속도를 높이기 위한것임.
static uint8_t _uChosung, _uJoongsung, _uJongsung;
static uint8_t uChosung, uJoongsung, uJongsung;
static uint8_t uChoType, uJooType, uJonType;

//========================================================================
void PHan_HanFontLoad( const uint8_t *HanCode, PHAN_FONT_OBJ *FontPtr )   /* 한글 일반 폰트 생성 */
//========================================================================
{
	uint8_t i;
	uint16_t wHanCode;

	FontPtr->Code_Type = PHAN_HANGUL_CODE;
	FontPtr->Size_Char = 2;

	wHanCode = (uint16_t)HanCode[0] << 8 | (uint16_t)HanCode[1] & 0x00FF;

//	printf("%c%c : 0x%02X%02X\n", HanCode[0], HanCode[1], HanCode[0], HanCode[1] );

	wHanCode = PHan_CnvCodeWan2Johab( wHanCode );
	//printf("a %x \n", wHanCode );

	//seperate phoneme code
	_uChosung	=	( wHanCode >> 10 ) & 0x001F;	//	Chosung code
	_uJoongsung	=	( wHanCode >> 5 ) & 0x001F;		//	Joongsung code
	_uJongsung	=	wHanCode & 0x001F;				//	Jongsung code

	//make font index
	uChosung	=	ChoIdxTbl[_uChosung];			//	Chosung index
	uJoongsung	=	JooIdxTbl[_uJoongsung];			//	Joongsung index
	uJongsung	=	JonIdxTbl[_uJongsung];			//	Jongsung index

	//decide a character type (몇번째 벌을 사용할지 결정)
	uChoType = uJongsung ? ChoTypeCaseJongYes[uJoongsung] : ChoTypeCaseJongNo[uJoongsung];
	//'ㄱ'(1) 이나 'ㅋ'(16) 인경우는
	uJooType	=	( ( uChosung == 0 || uChosung == 1 || uChosung == 16 ) ? 0 : 1 ) + ( uJongsung ? 2 : 0 );
	uJonType	=	JonType[uJoongsung];

	//printf( "%c%c : 0x%02X%02X ( 0x%04X : 0x%02X / 0x%02X / 0x%02X | 0x%02X / 0x%02X / 0x%02X )\n", 
	//		HanCode[0], HanCode[1], HanCode[0], HanCode[1],
	//		wHanCode, 
	//		_uChosung, _uJoongsung, _uJongsung,
	//		uChosung, uJoongsung, uJongsung );

	for ( i = 0; i < 32; i++ )
	{
		FontPtr->FontBuffer[i] = K_font[uChoType * 20 + uChosung][i];
		FontPtr->FontBuffer[i] |= K_font[160 + uJooType * 22 + uJoongsung][i];
	}

	//combine Jongsung
	if ( uJongsung )
	{
		for ( i = 0; i < 32; i++ )	FontPtr->FontBuffer[i] |= K_font[248 + uJonType * 28 + uJongsung][i];
	}
}

//========================================================================
#else	//	defined(PHANFONT_LOCAL)
//========================================================================

//========================================================================
void PHan_HanFontLoad( const uint8_t *HanCode, PHAN_FONT_OBJ *FontPtr )   /* 한글 일반 폰트 생성 */
//========================================================================
{
	int i;

	FontPtr->Code_Type = PHAN_HANGUL_CODE;
	FontPtr->Size_Char = 2;

	for ( i = 0; i < 32; i++ )
	{
		FontPtr->FontBuffer[i] = 0xFF;
	}
}

//========================================================================
#endif	//	defined(PHANFONT_LOCAL)
//========================================================================


//========================================================================
#if defined(PHANFONTENG_LOCAL)
//========================================================================

//========================================================================
void PHan_EngFontLoad( const uint8_t *HanCode, PHAN_FONT_OBJ *FontPtr ) 
//========================================================================
{
	uint16_t i;
	char EngCode;

	FontPtr->Code_Type = PHAN_ENG_CODE;
	FontPtr->Size_Char = 1;

	EngCode = *HanCode;

	EngCode -= 0x20;  // FONT는 스페이스 부터 시작한다.

	for ( i = 0 ; i < 16 ; i++ )
	{
		 FontPtr->FontBuffer[ i ] = wEngFon[EngCode][i];		
	}
}

//========================================================================
#endif	//	defined(PHANFONTENG_LOCAL)
//========================================================================


/*----------------------------------------------------------------------*/
/*	한글 일반 폰트(24x24)를 bTemp1Font[72]에 LOAD한다.					*/
/*----------------------------------------------------------------------*/
//========================================================================
uint16_t PHan_FontLoad( uint8_t *HanCode, PHAN_FONT_OBJ *FontPtr )   // 한글 일반 폰트 생성
//========================================================================
{

	//static declaration 은 속도를 높이기 위한것임.
	//uint16_t uChosung, uJoongsung, uJongsung, uChoType, uJooType,uJonType;

	// 버퍼 초기화
	memset(FontPtr->FontBuffer, 0x00, 32);


	FontPtr->Code_Type = PHAN_NULL_CODE;
	// 한글코드인지 감별
	//
	if( !HanCode[0] || HanCode[0] == 0x0A )   // 문자열 마지막
	{
		FontPtr->Code_Type = PHAN_END_CODE;
		FontPtr->Size_Char = 1;
		return PHAN_END_CODE;
	}
	else if( HanCode[0] & 0x80 )              // 한글 코드인경우
	{
		FontPtr->Code_Type = PHAN_HANGUL_CODE;
		FontPtr->Size_Char = 2;
		PHan_HanFontLoad( HanCode, FontPtr );
		return PHAN_HANGUL_CODE;
	}
	else                                      // 영문 코드
	{
		FontPtr->Code_Type = PHAN_ENG_CODE;
		FontPtr->Size_Char = 1;
		PHan_EngFontLoad( HanCode, FontPtr );
		return PHAN_ENG_CODE;
	}

	return FontPtr->Code_Type;
}

