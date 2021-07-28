/*------------------------------------------------------------------------------------------
	Project			: RFM config
	Description		: 

	Writer			: $Author: zlkit $
	Revision		: $Rev: 1891 $
	Date			: 2020. 08.
	Copyright		: Piranti Corp.	( sw@piranti.co.kr )
	 
	Revision History 
	1. 2020. 08.	: Created
-------------------------------------------------------------------------------------------*/

//========================================================================
#ifndef RFM_CONFIG_H
#define RFM_CONFIG_H
//========================================================================

//========================================================================
//	Configuration

//========================================================================
//	그룹주파수 모드 : [ 1, 2 ] [ 3, 4 ] [ 5, 6 ], [ 7, 8 ], [ 9, 10 ]
//		전송 :  2 -> 4 -> 6 -> 8 -> 10
//			1 <- 3 <- 5 <- 7 <- 9
#define		USE_COMM_MODE_CH_GRP			1		//	그룹주파수 모드. - [ 1, 2 ] [ 3, 4 ] ...

#define		USE_RF_COMM_MODE				1		//	RF 통신 Mode

enum eRFMode
{
	RFMode1			= 	1,				//
	RFMode2			= 	2,				//	그룹주파수 모드.
	RFModeDefault	=	RFMode1,
	RFModeMax		=	2,
};

//========================================================================
//	Audio 압축관련.
//#define		USE_AUDIO_INTERPOL_COMPRESS		1		//	보간압축사용.

#if defined( USE_AUDIO_INTERPOL_COMPRESS )	//	보간압축사용.

//#define	AUDIO_COMPR_RATE	8	//	Audio 압축율.
//#define	AUDIO_COMPR_RATE	6	//	Audio 압축율.
#define	AUDIO_COMPR_RATE	4	//	Audio 압축율.
//#define	AUDIO_COMPR_RATE	2	//	Audio 압축율.
//#define	AUDIO_COMPR_RATE	1	//	Audio 압축율.

#endif	//	defined( USE_AUDIO_INTERPOL_COMPRESS )	//	보간압축사용.

#define		USE_AUDIO_ADPCM						1		//	ADPCM 압축 사용. 1/4 압축.


//========================================================================
//	RF 설정.

//	장치별 Channel 구분. Data 충돌 방지.
#define		USE_CH_ISO_DEV					1		//	장치별 수신 채널 분리. ( Channel Isolation Each Device )

//========================================================================
//	패킷 설정.
#define		USE_HOPPING						1		//	Hopping

#define		USE_HOP_MANUAL					1		//	수동 중계 설정.

#if defined(USE_HOP_MANUAL)

//g_nManHopping	=	0;			//	On(1) / Off(2) / Unused(0 : Other)
#define		DEFAULT_HOP_MAN_VAL				2		//	Default - Off(2)
//#define		DEFAULT_HOP_MAN_VAL				0		//	Default - Off(2)

#define		USE_HOP_CH						1		//	Hopping 시 타 채널 사용. 1->2->1->2

#define		USE_HOP_FORCE					1		//	강제 중계 설정.

#endif	//	defined(USE_HOP_MANUAL)


//========================================================================
//	방송/통화
#define		TIMEOUT_RXSTAT					1000		//	1 sec ( 1000 msec )

//========================================================================
//	상태정보 송/수신
#define		USE_STAT_REQ					1		//	상태정보 요청/응답 Enable

#if defined(USE_STAT_REQ)

#define		TIME_STAT_REQ					300		//	상태정보 요청 주기. 200 msec ( 모든 호차 상태정보 요청주기 200 X 12 msec )

#else
//	상태정보 요청/응답 방식인 경우 -> 주기적으로 상태정보 전송 Disable
#define		USE_SEND_STATUS					1		//	상태정보 전송 사용.
#endif


#define		TIMEOUT_RECV_STATUS				4		//	상태정보 수신 Timeout. ( seconds )

//========================================================================
//	Route 상태정보		-	무정전장치 Routing 경로 설정. ( 중간에 고장난 수신기 Pass )
#define		USE_ROUTE_REQ					1		//	Route 상태 정보

#define		USE_ROUTE_REQ_RFM				1		//	수신기에서 Route 상태 정보 요청 ( Req : 1 -> 2 / Rsp : 1 <- 2 )

#if defined(USE_ROUTE_REQ)
#define		TIME_ROUTE_REQ					1//10		//	Route 상태정보 요청 주기 10초.

#define		TIMEOUT_RECV_ROUTE				10		//	상태정보 수신 Timeout. ( seconds )
#endif

//	Route 수신기 -> 송신기
#define		USE_ROUTE_NEAREST_RFM			1		//	수신기 -> 송신기 중계 연결. ( 가장가까운 수신기에서 송신기로 중계 )

//========================================================================
//	모니터링
#define		USE_SHOW_PKT					1		//	패킷 상태 모니터링.
#define		DEFAULT_EN_SHOW_PKT				0		//	패킷 상태 모니터링 - 초기값 Off(0)

//DEL #define		USE_STAT_LIGHT					1		//	상태정보 정보 Light 추가. ( 송신기 Light 상태에 따른 수신기 조명제어 )

//========================================================================
//	송신기
#define		USE_RFT_ONLY_RX_SPK_ON			1		//	송신기 수신중일때 만 Spk On.

//========================================================================
//	송신기 메뉴
#define		USE_RFT_MENU_DIAG				1		//	송신기 진단 메뉴 사용.

#define		USE_RFT_MENU_TRAINSET			1		//	송신기 편성설정.

#define		USE_RFT_MENU_RFTID				1		//	송신기 ID 설정.

#define		USE_RFT_MENU_SELFTEST			1		//	송신기 자가진단 메뉴.

#define		USE_RFT_MENU_STAT				1		//	상태정보 현시 메뉴 사용.

#define		USE_RFT_MENU_DEV_VER			1		//	장치 버전정보.

#define		USE_RFT_MENU_LOOP				1		//	송신기 메뉴 Up/Down Loop.

#define		USE_RFT_MENU_CMD				1		//	송신기 Command 명령.

#define		USE_RFT_MENU_SETTING			1		//	송신기 설정메뉴.

//========================================================================
//	송신기 -> 수신기에 등록.
#define		USE_RFT_REG_TO_RFM				1		//	송신기 가까운 수신기에 등록. (중계동작)


//========================================================================
//	Upgrade 관련
#define		TIMEOUT_UPGRADE					300		//	300 sec ( 5 min ) - Upgrade 모드 진입후 5분동안 Rx Packet이 없는경우 Normal모드로 변경.

//========================================================================
//	시험용 설정
//#define		USE_TEST_PTT_AUDIO_LOOP			1		//	Test PTT Key Audio Loopback


//#define		USE_TEST_RF_TX_LOOP				1		//	Test RF TX Loop

//========================================================================
//	대승객방송
#define		USE_RFM_OCC_PA					1		//	대승객방송 사용. ( 대곡-소사 )


//========================================================================
#endif	//	RFM_CONFIG_H
//========================================================================


