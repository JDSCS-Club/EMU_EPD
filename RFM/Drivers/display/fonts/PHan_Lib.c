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

#ifndef  LIB_INCLUDE
#define  PHAN_LIB_LOCAL  
#define  PHANFONT_LOCAL
#define  PHANFONTENG_LOCAL

#include "PHan_Lib.h"
#include "PHanFont.h"
#include "PHanFontEng.h"

#include <stdio.h>

#include <string.h>		//	memset()

#endif

uint8_t Disp_Font;										// 1 = 정면 2 = 측면 : k_font, 그 외 : k_font5

//----------------------------------------------------------------------------
//
//          TITLE : PHan_CnvCodeWan2Johab
//
//          WORK  : 
//
//----------------------------------------------------------------------------
uint16_t PHan_CnvCodeWan2Johab(uint16_t WanCode)
{
	uint16_t	index;
	uint16_t	hcode, lcode;

	hcode = (WanCode >> 8) & 0xFF;
	lcode = WanCode & 0x0ff;

	index = (hcode - 0x0B0) * 94 + (lcode - 0x0A1);

	return wWanToJohabTable[index];
}

//----------------------------------------------------------------------------
//
//          TITLE : PHan_FontLoad
//
//          WORK  : 
//
//----------------------------------------------------------------------------
/*----------------------------------------------------------------------*/
/*	한글 일반 폰트(24x24)를 bTemp1Font[72]에 LOAD한다.					*/
/*----------------------------------------------------------------------*/
uint16_t PHan_FontLoad( uint8_t *HanCode, PHAN_FONT_OBJ *FontPtr )   // 한글 일반 폰트 생성 
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

//
////========================================================================
//int GetStation2FontIdx( int StationCode )
////========================================================================
//{
//	const uint8_t  Num2[17] = { 0, 			99, 		98, 		97, 			96,  		2, 				11, 		12, 	    18, 		 19, 			23, 		28, 			34, 		39, 	  131, 			195,		10 };
//	const uint8_t  *Str2[17] = { "  순 환  ", "  순 환  ", "  순 환  ", "  회 송  ", " 시운전 ", "을지로입구", "  성 수  ", "건대입구", "종합운동장", "  삼 성  ", "  교 대  ", "서울대입구", " 신도림 ", "홍대입구", " 신설동 ", " 까치산 " , "  뚝 섬  " };
//
//	int Station_Index = 0;
//	int idx;
//
//	for ( idx = 0; idx < 17; idx++ )
//	{
//		if ( Num2[idx] == StationCode ) { Station_Index = idx; break; }	// 	Last_Station = 종착역
////		if(Num2[lp] == Line_Number) { Station_Index = lp; break; }		 	// Line_Number = 노선번호
//	}
//
//	if ( Station_Index > 16 ) Station_Index = 16;
//
//	int Side_Font_Index = 0;
//
//	if ( Station_Index == 5 ) 			Side_Font_Index = 0;	// 	을지로입구
//	else if ( Station_Index == 8 ) 		Side_Font_Index = 4;	// 	종합운동장
//	else if ( Station_Index == 11 )		Side_Font_Index = 8;	// 	서울대입구
//	else if ( ( Station_Index == 0 ) || ( Station_Index == 1 ) || ( Station_Index == 2 ) )Side_Font_Index = 12;// 순환
//	else if ( ( Station_Index == 9 ) )	Side_Font_Index = 16;	// 	삼성
//	else if ( ( Station_Index == 10 ) )	Side_Font_Index = 20;	// 	교대
//	else if ( ( Station_Index == 6 ) )	Side_Font_Index = 24;	// 	성수
//	else if ( ( Station_Index == 16 ) )	Side_Font_Index = 28;	// 	뚝섬
//	else if ( ( Station_Index == 3 ) )	Side_Font_Index = 32;	// 	회송
//
//	return Side_Font_Index;
//}
//


//----------------------------------------------------------------------------
// Side Font Load - add by dr@cula
//----------------------------------------------------------------------------
//void Side_Font_Load(PHAN_FONT_OBJ *FontPtr, uint8_t Font_Index)
//{
//	uint8_t i;
//	
//	for(i=0;i<32;i++) FontPtr->FontBuffer[i] = Side_font[Font_Index][i];
//}

//----------------------------------------------------------------------------
//
//          TITLE : PHan_FontLoad
//
//          WORK  : 
//
//----------------------------------------------------------------------------
//static uint16_t uChosung, uJoongsung, uJongsung, uChoType, uJooType,uJonType;

//static declaration 은 속도를 높이기 위한것임.
static uint8_t _uChosung, _uJoongsung, _uJongsung;
static uint8_t uChosung, uJoongsung, uJongsung;
static uint8_t uChoType, uJooType, uJonType;
void PHan_HanFontLoad( const uint8_t *HanCode, PHAN_FONT_OBJ *FontPtr )   /* 한글 일반 폰트 생성 */
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

//----------------------------------------------------------------------------
//
//          TITLE : PHan_EngFontLoad
//
//          WORK  : 
//
//----------------------------------------------------------------------------
void PHan_EngFontLoad( const uint8_t *HanCode, PHAN_FONT_OBJ *FontPtr ) 
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

/*----------------------------------------------------------------------*/
/*	bTemp1Font[72]에 조합한 한글 일반 폰트를							*/
/*	Line Image Buffer로 전송한다.										*/
/*----------------------------------------------------------------------*/

#if 0

void PHan_MoveHanFont( uint8_t *ImageBuffer, PHAN_FONT_OBJ *FontPtr, int8_t Xpos)
{
	int	i, j;
	i = 0;

	for ( j = 0 ; j < 2 ; j++ )		// 16 x 16 (2 Bytes)
	{
		ImageBuffer[ Xpos*2 + j ] = FontPtr->FontBuffer[i*2 +j];
	}
}

void PHan_DisHanFont(PHAN_FONT_OBJ *FontPtr)
{
	uint16_t	i, j, Loop;
	uint16_t	FontSize = FontPtr->Size_Char;


	for ( i = 0 ; i < 16 ; i++ )		// 16 Lines per Font/Char
	{
		for ( j = 0 ; j < FontSize ; j++ )		// 16 x 16 (2 Bytes)
		{
			for( Loop=0; Loop<8; Loop++ )
			{
				if( FontPtr->FontBuffer[i*FontSize +j] & (0x80>>Loop)) printf("*");
				else                                         printf(" ");
				
			}
		}
		printf("   - \n");
		//printf("%x\n",FontPtr->FontBuffer[i]);
	}
}

#endif
