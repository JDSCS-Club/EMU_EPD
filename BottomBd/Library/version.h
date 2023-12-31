/*------------------------------------------------------------------------------------------
	Project			: RFM
	Description		: 

	Writer			: $Author: zlkit $
	Revision		: $Rev: 1891 $
	Date			: 2020. 08.
	Copyright		: Piranti Corp. ( zlkit@piranti.co.kr )
	 
	Revision History 
	1. 2020. 08.	: Created
-------------------------------------------------------------------------------------------*/

#ifndef APP_VER_H_
#define APP_VER_H_

//========================================================================
// Header

//========================================================================
// Define

#define	APP_MAIN_VER	2
#define	APP_SUB_VER		0
#define	APP_DETAIL_VER	0
#define	APP_BUILD_VER	49

// 전처리 과정에서 문자열로 변환.	ex) 1.0.0.0 -> "1.0.0.0"
#define str(x)		#x				
#define xstr(x)		str(x)

//========================================================================
// VS Version Info
#define FILEVER			APP_MAIN_VER, APP_SUB_VER, APP_DETAIL_VER, APP_BUILD_VER
#define PRODUCTVER		FILEVER
#define STRFILEVER		xstr(FILEVER)
#define STRPRODUCTVER	STRFILEVER
//========================================================================

//	#define APP_VER		STRFILEVER
#define APP_VER_DOT		APP_MAIN_VER.APP_SUB_VER.APP_DETAIL_VER.APP_BUILD_VER	
#define APP_VER			xstr(APP_VER_DOT)

#define APP_BUILD_DATE	( __DATE__ ", " __TIME__ )

/*
------------------------------------------------------------
[ Version History ]
------------------------------------------------------------

v1.1.0.X : 20/10/28
	* [송신기][MENU] : 유지보수 메뉴 진입 기능 추가.
		- [OK]버튼을 누른상태에서 [MENU]버튼을 3초간 누를 경우 Maintance모드로 진입함.

v1.1.0.X : 20/10/29
	* [RF] : RX Error Packet에 의한 오동작 디버깅.
		- Rx 패킷 CRC Error or Rx Error 발생시 Packet 처리 Skip. 

	* [송신기] : 수신중인 경우 방송/통화 버튼 Disable.


------------------------------------------------------------
[ 디버깅항목 ]
------------------------------------------------------------


------------------------------------------------------------
[ 구현필요사항 ]
------------------------------------------------------------


------------------------------------------------------------
[ 공통 라이브러리 관련 ]
------------------------------------------------------------



------------------------------------------------------------

*/

#define		HITLIB_VERSION_HISTORY			"\
\n	--------------------------				\
\n	Version History							\
\n	v1.0.0 : 19/01/29						\
\n	- 초기버전.								\
\n											\
\n"

/*
[ 구현일정 ]
	19/01/01

[ 구현사항 ]
	1. Window / Linux 호환가능하도록 구성.
	2. 

[ 장애내역 ]
	1. ...

[ 디버깅내역 ]
	1. ...

*/

#endif	//	APP_VER_H_
