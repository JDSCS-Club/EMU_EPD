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
#ifndef TYPEDEF_H
#define TYPEDEF_H
//========================================================================


#include <stdint.h>


#ifdef _WIN32	/* FatFs development platform */

#include <windows.h>
#include <tchar.h>

#else			/* Embedded platform */

/* This type MUST be 8 bit */
typedef unsigned char	BYTE;

/* These types MUST be 16 bit */
typedef short			SHORT;
typedef unsigned short	WORD;
typedef unsigned short	WCHAR;

/* These types MUST be 16 bit or 32 bit */
typedef int				INT;
typedef unsigned int	UINT;

/* These types MUST be 32 bit */
typedef long			LONG;
typedef unsigned long	DWORD;

#endif

typedef char char_t;

//typedef unsigned char		BYTE;

//typedef short             SHORT;
//typedef unsigned short	WORD;
							
//typedef int               INT;
//typedef unsigned int      UINT;
							
//typedef long              LONG;
//typedef unsigned int		DWORD;	//	defined ..\Middlewares\Third_Party\FatFs\src\integer.h

typedef char                CHAR;
typedef signed char         INT8;
typedef unsigned char       UCHAR;
typedef unsigned char       UINT8;
typedef unsigned char       BYTE;
typedef signed short        INT16;
typedef unsigned short      USHORT;
typedef unsigned short      UINT16;
typedef unsigned short      WORD;
typedef signed int          INT32;
typedef unsigned int        UINT32;
typedef unsigned long       ULONG;


typedef unsigned char 		BIT;
typedef unsigned char 		U8;
typedef unsigned int 		U16;
typedef unsigned long 		U32;

typedef signed char 		S8;
typedef signed int 			S16;
typedef signed long 		S32;


#ifndef TRUE
#define		TRUE			(1)
#endif

#ifndef FALSE
#define		FALSE			(0)
#endif

#if 0
typedef __int64             LONGLONG;
typedef __int64             LONG64;
typedef signed __int64      INT64;
typedef unsigned __int64    ULONGLONG;
typedef unsigned __int64    DWORDLONG;
typedef unsigned __int64    ULONG64;
typedef unsigned __int64    DWORD64;
typedef unsigned __int64    UINT64;
#endif


//=============================================================================
//	Network Address

//	ItLib ip Address
typedef struct ip_addr {
	union {
		struct { UCHAR s_b1, s_b2, s_b3, s_b4; } S_un_b;
		struct { USHORT s_w1, s_w2; } S_un_w;
		ULONG S_addr;
	} S_un;

#if 0	
#define s_addr  S_un.S_addr			//  can be used for most tcp & ip code
#define s_host  S_un.S_un_b.s_b2    // host on imp
#define s_net   S_un.S_un_b.s_b1    // network
#define s_imp   S_un.S_un_w.s_w2    // imp
#define s_impno S_un.S_un_b.s_b4    // imp #
#define s_lh    S_un.S_un_b.s_b3    // logical host
#endif	// s_addr
} IP_ADDR, *PIP_ADDR;
//=============================================================================


//=============================================================================
#endif
//=============================================================================
