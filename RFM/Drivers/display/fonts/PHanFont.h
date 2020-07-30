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


//#ifdef   PHANFONT_LOCAL
//#define  EXT_PHANFONT_DEF 
//#else
//#define  EXT_PHANFONT_DEF     extern
//#endif

//#include "Define.h"
#include <stdint.h>


extern const uint8_t ChoIdxTbl[];

//JOONGSUNG index table
extern const uint8_t JooIdxTbl[];

//JONGSUNG index table
extern const uint8_t JonIdxTbl[];

//---------------- ���� �ε��� TABLE --------------------
//�߼��� ���� �ʼ����� (��ħ�� ���°��)
extern const uint8_t ChoTypeCaseJongNo[];

//�߼��� ���� �ʼ����� (��ħ�� �ִ°��)
extern const uint8_t ChoTypeCaseJongYes[];

//�߼��� ���� ���� ����
extern const uint8_t JonType[];

extern const uint16_t wWanToJohabTable[];
//extern uint8_t Side_font[][32];
extern const uint8_t K_font[][32];

#endif
