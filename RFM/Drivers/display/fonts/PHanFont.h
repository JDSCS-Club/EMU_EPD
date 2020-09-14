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

#ifndef _PHANFONT_H_
#define _PHANFONT_H_

#include <stdint.h>


extern const uint8_t ChoIdxTbl[];

//JOONGSUNG index table
extern const uint8_t JooIdxTbl[];

//JONGSUNG index table
extern const uint8_t JonIdxTbl[];

//---------------- 벌수 인덱스 TABLE --------------------
//중성에 따른 초성종류 (받침이 없는경우)
extern const uint8_t ChoTypeCaseJongNo[];

//중성에 따른 초성종류 (받침이 있는경우)
extern const uint8_t ChoTypeCaseJongYes[];

//중성에 따른 종성 종류
extern const uint8_t JonType[];

extern const uint16_t wWanToJohabTable[];
//extern uint8_t Side_font[][32];
extern const uint8_t K_font[][32];

#endif
