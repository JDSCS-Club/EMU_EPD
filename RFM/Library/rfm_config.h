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

#define		USE_RFT_MENU_TRAINSET			1		//	송신기 편성설정.

#define		USE_RFT_ONLY_RX_SPK_ON			1		//	송신기 수신중일때 만 Spk On.

//#define		USE_AUDIO_INTERPOL_COMPRESS		1		//	보간압축사용.

//#define		USE_SEND_STATUS					1		//	상태정보 전송 사용.

#define		USE_SHOW_PKT					1		//	패킷 상태 모니터링.

#define		USE_TEST_PTT_AUDIO_LOOP			1		//	Test PTT Key Audio Loopback

#define		USE_HOPPING						1		//	Hopping

//#define		USE_IEEE802_15_4G	1		//	radio_config_802_15_4g.h 사용.


//========================================================================
#endif	//	RFM_CONFIG_H
//========================================================================


