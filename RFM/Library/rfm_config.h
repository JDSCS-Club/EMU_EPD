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

//#define		USE_BOOTLOADER					1

//========================================================================
//	Audio 압축관련.
#define		USE_AUDIO_INTERPOL_COMPRESS		1		//	보간압축사용.

//#define		USE_AUDIO_SPEEX_CODEC			1		//	Speex Audio Codec 사용.

//========================================================================
//	RF 설정.
//#define		USE_IEEE802_15_4G				1		//	radio_config_802_15_4g.h 사용.

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

#define		USE_RFT_MENU_STAT				1		//	상태정보 현시 메뉴 사용.

#define		USE_RFT_MENU_DEV_VER			1		//	장치 버전정보.

#define		USE_RFT_MENU_LOOP				1		//	송신기 메뉴 Up/Down Loop.

#define		USE_RFT_MENU_CMD				1		//	송신기 Command 명령.

//========================================================================
//	송신기 -> 수신기에 등록.
#define		USE_RFT_REG_TO_RFM				1		//	송신기 가까운 수신기에 등록. (중계동작)


//========================================================================
//	Upgrade 관련
#define		TIMEOUT_UPGRADE					300		//	300 sec ( 5 min ) - Upgrade 모드 진입후 5분동안 Rx Packet이 없는경우 Normal모드로 변경.

//========================================================================
//	시험용 설정
//#define		USE_TEST_PTT_AUDIO_LOOP			1		//	Test PTT Key Audio Loopback

//========================================================================
#endif	//	RFM_CONFIG_H
//========================================================================


