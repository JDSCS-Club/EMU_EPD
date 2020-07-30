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

uint8_t Disp_Font;										// 1 = ���� 2 = ���� : k_font, �� �� : k_font5

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
/*	�ѱ� �Ϲ� ��Ʈ(24x24)�� bTemp1Font[72]�� LOAD�Ѵ�.					*/
/*----------------------------------------------------------------------*/
uint16_t PHan_FontLoad( uint8_t *HanCode, PHAN_FONT_OBJ *FontPtr )   // �ѱ� �Ϲ� ��Ʈ ���� 
{

	//static declaration �� �ӵ��� ���̱� ���Ѱ���.
	//uint16_t uChosung, uJoongsung, uJongsung, uChoType, uJooType,uJonType;

	// ���� �ʱ�ȭ
	memset(FontPtr->FontBuffer, 0x00, 32);


	FontPtr->Code_Type = PHAN_NULL_CODE;	
	// �ѱ��ڵ����� ���� 
	// 
	if( !HanCode[0] || HanCode[0] == 0x0A )   // ���ڿ� ������
	{
		FontPtr->Code_Type = PHAN_END_CODE;
		FontPtr->Size_Char = 1;
		return PHAN_END_CODE;
	}
	else if( HanCode[0] & 0x80 )              // �ѱ� �ڵ��ΰ�� 
	{
		FontPtr->Code_Type = PHAN_HANGUL_CODE;
		FontPtr->Size_Char = 2;
		PHan_HanFontLoad( HanCode, FontPtr );
		return PHAN_HANGUL_CODE;
	}
	else                                      // ���� �ڵ� 
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
//	const uint8_t  *Str2[17] = { "  �� ȯ  ", "  �� ȯ  ", "  �� ȯ  ", "  ȸ ��  ", " �ÿ��� ", "�������Ա�", "  �� ��  ", "�Ǵ��Ա�", "���տ��", "  �� ��  ", "  �� ��  ", "������Ա�", " �ŵ��� ", "ȫ���Ա�", " �ż��� ", " ��ġ�� " , "  �� ��  " };
//
//	int Station_Index = 0;
//	int idx;
//
//	for ( idx = 0; idx < 17; idx++ )
//	{
//		if ( Num2[idx] == StationCode ) { Station_Index = idx; break; }	// 	Last_Station = ������
////		if(Num2[lp] == Line_Number) { Station_Index = lp; break; }		 	// Line_Number = �뼱��ȣ
//	}
//
//	if ( Station_Index > 16 ) Station_Index = 16;
//
//	int Side_Font_Index = 0;
//
//	if ( Station_Index == 5 ) 			Side_Font_Index = 0;	// 	�������Ա�
//	else if ( Station_Index == 8 ) 		Side_Font_Index = 4;	// 	���տ��
//	else if ( Station_Index == 11 )		Side_Font_Index = 8;	// 	������Ա�
//	else if ( ( Station_Index == 0 ) || ( Station_Index == 1 ) || ( Station_Index == 2 ) )Side_Font_Index = 12;// ��ȯ
//	else if ( ( Station_Index == 9 ) )	Side_Font_Index = 16;	// 	�Ｚ
//	else if ( ( Station_Index == 10 ) )	Side_Font_Index = 20;	// 	����
//	else if ( ( Station_Index == 6 ) )	Side_Font_Index = 24;	// 	����
//	else if ( ( Station_Index == 16 ) )	Side_Font_Index = 28;	// 	�Ҽ�
//	else if ( ( Station_Index == 3 ) )	Side_Font_Index = 32;	// 	ȸ��
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

//static declaration �� �ӵ��� ���̱� ���Ѱ���.
static uint8_t _uChosung, _uJoongsung, _uJongsung;
static uint8_t uChosung, uJoongsung, uJongsung;
static uint8_t uChoType, uJooType, uJonType;
void PHan_HanFontLoad( const uint8_t *HanCode, PHAN_FONT_OBJ *FontPtr )   /* �ѱ� �Ϲ� ��Ʈ ���� */
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

	//decide a character type (���° ���� ������� ����)
	uChoType = uJongsung ? ChoTypeCaseJongYes[uJoongsung] : ChoTypeCaseJongNo[uJoongsung];
	//'��'(1) �̳� '��'(16) �ΰ���
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

	EngCode -= 0x20;  // FONT�� �����̽� ���� �����Ѵ�.

	for ( i = 0 ; i < 16 ; i++ )
	{
		 FontPtr->FontBuffer[ i ] = wEngFon[EngCode][i];		
	}
}

/*----------------------------------------------------------------------*/
/*	bTemp1Font[72]�� ������ �ѱ� �Ϲ� ��Ʈ��							*/
/*	Line Image Buffer�� �����Ѵ�.										*/
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
