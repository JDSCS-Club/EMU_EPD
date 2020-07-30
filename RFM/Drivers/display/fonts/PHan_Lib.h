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

//------- 원본이외의 루틴에서 참조시 외부변수로 참조한다
#ifndef  _PHAN_LIB_H
#define  _PHAN_LIB_H

//#include "Define.h"
#include <stdint.h>


#ifdef   PHAN_LIB_LOCAL
#define  EXT_PHAN_LIB_DEF 
#else
#define  EXT_PHAN_LIB_DEF     extern
#endif

#define PHAN_NULL_CODE    0
#define PHAN_HANGUL_CODE  1
#define PHAN_ENG_CODE     2
#define PHAN_SPEC_CODE    3
#define PHAN_END_CODE     4

typedef struct 
{
	uint16_t HanCode;
	uint8_t Size_Char;  // 글자 1개의 바이트수(한글:2 영문:1)
	uint8_t Code_Type;  // 한/영/특수 문자인지 감별 

	uint8_t FontBuffer[32];
} PHAN_FONT_OBJ;


#ifdef __cplusplus
extern "C" {
#endif

//int GetStation2FontIdx( int StationCode );		//	Side_Font StationCode -> Index

//EXT_PHAN_LIB_DEF void Side_Font_Load(PHAN_FONT_OBJ *FontPtr, uint8_t Font_Index);
//EXT_PHAN_LIB_DEF uint16_t  PHan_FontLoad( uint8_t *HanCode, PHAN_FONT_OBJ *FontPtr );
EXT_PHAN_LIB_DEF void PHan_DisHanFont(PHAN_FONT_OBJ *FontPtr);
EXT_PHAN_LIB_DEF void PHan_MoveHanFont( uint8_t *ImageBuffer, PHAN_FONT_OBJ *FontPtr, int8_t Xpos);
EXT_PHAN_LIB_DEF void PHan_HanFontLoad( const uint8_t *HanCode, PHAN_FONT_OBJ *FontPtr );
EXT_PHAN_LIB_DEF void PHan_EngFontLoad( const uint8_t *HanCode, PHAN_FONT_OBJ *FontPtr ) ;


#ifdef __cplusplus
}
#endif



#endif