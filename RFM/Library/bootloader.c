/*------------------------------------------------------------------------------------------
	Project			: RFM
	Description		: Bootloader

	Writer			: $Author: zlkit $
	Revision		: $Rev: 1891 $
	Date			: 2019. 01.
	Copyright		: Piranti Corp. ( zlkit@piranti.co.kr )
	 
	Revision History 
	1. 2019. 01.	: Created
-------------------------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ctype.h>
#include <string.h>
#include <stdarg.h>

#include "stm32f4xx_hal.h"
#include "stm32f4xx.h"

#include "bootloader.h"

#include "iap_common.h"		//	IAP Common
#include "iap_menu.h"		//	IAP ( In-Application Programming )

#include "ymodem.h"			//	Y-Modem

#include "main.h"			//	huart2 / MX_IWDG_Disable()

//	STM32F407 Embedded Bootloader ( AN2606 - P.29 )
//#define BOOT_ROM_ADDRESS		(uint32_t)0x1FFF77DE

#define APPLICATION_ADDRESS1   (uint32_t)0x08020000 

//	Sector 0 0x0800 0000 - 0x0800 3FFF 16 Kbyte
//	Sector 1 0x0800 4000 - 0x0800 7FFF 16 Kbyte
//	Sector 2 0x0800 8000 - 0x0800 BFFF 16 Kbyte
//	Sector 3 0x0800 C000 - 0x0800 FFFF 16 Kbyte
//	Sector 4 0x0801 0000 - 0x0801 FFFF 64 Kbyte
//	Sector 5 0x0802 0000 - 0x0803 FFFF 128 Kbyte
//	Sector 6 0x0804 0000 - 0x0805 FFFF 128 Kbyte
//			.
//			.
//			.
//	Sector 11 0x080E 0000 - 0x080F FFFF 128 Kbyte

typedef  void (*pFunction)(void);

pFunction	Jump_To_Application;
uint32_t	JumpAddress;

unsigned char id_buf[3];

//========================================================================
int g_nBootMode = BModeApp;			//	Default Application Boot
//========================================================================

//========================================================================
void SetBootMode( int nBootMode )
//========================================================================
{
	g_nBootMode = nBootMode;
}

//========================================================================
int GetBootMode( void )
//========================================================================
{
	return g_nBootMode;
}


//========================================================================
void JumpToSTBootloader(void)
//========================================================================
{
  typedef  void (*pFunction)(void);
  /* OK for L4 and F446 */
#define SYS_MEM_ADDRESS     ((uint32_t)0x1FFF0000)

  pFunction JumpToApplication;
  uint32_t JumpAddress;

  //========================================================================
  //	Disable Watchdog
  MX_IWDG_Disable();
  //========================================================================

  /* Disable all interrupts, clocks and PLLs */
  HAL_RCC_DeInit();

#ifdef STM32L476xx
  /* Enable the SYSCFG APB Clock */
  RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;
  /*  System Flash memory mapped at 0x00000000  */
  __HAL_SYSCFG_REMAPMEMORY_SYSTEMFLASH();

#endif

  /* Jump to system memory */
  JumpAddress = *(__IO uint32_t*) (SYS_MEM_ADDRESS + 4);
  JumpToApplication = (pFunction) JumpAddress;
  /* Initialize user application's Stack Pointer */
  __set_MSP(*(__IO uint32_t*) SYS_MEM_ADDRESS);
  JumpToApplication();
}


//========================================================================
void BootLoaderTask(void)
//========================================================================
{
	/* The parameters are not used. */
	int		i, j, k;
	int		fsize = 0;

	int		readnum = 0, rremain = 0;
	unsigned char	adata[512];
	uint32_t		flashAddr = 0x020000;

	FLASH_EraseInitTypeDef flash1;

	HAL_Delay( 500 );

	printf( "BootLoader\n" );

	//========================================================================
	//	Y-Modem Master 설정.
	SetYModemMaster( 1 );
	//========================================================================

	//========================================================================
	//	IAP ( In-Application Programming )

	//	Dev ID가 0x00인 경우 ">" 전송.
	uint8_t ch;

	if ( IsYModemMaster() )
	{
		ch = '>';
		HAL_UART_Transmit( &huart2, &ch, 1, TX_TIMEOUT );
	}

	HAL_StatusTypeDef ret;

	CHECK_ENTER:
	
	ret = HAL_UART_Receive( &huart2, &ch, 1, 3000 );		//	3 초 Timeout

	if ( ret == HAL_OK )
	{
		//	'\r' 엔터 키 수신시. - Y-Modem Upgrade 수행.

		printf( "%s(%d) - RS485 Input : 0x%02X\n", __func__, __LINE__, ch );

		if ( ch == '>' )
		{
			goto CHECK_ENTER;
		}

		if ( ch == '\r' )
		{
			//IAP_Menu();
			SerialDownload();	//	Y-Modem Serial Download
		}
	}

//	if ( input_check() )
//	{
//		//	Console Key 입력이 있을 경우 IAP Menu로 진입.
//		IAP_Menu();
//	}

//	vTaskDelay( 1000 * BOOTLOADER_START_DELAY );

	//========================================================================

#if defined( USE_YMODEM_EXT_FLASH )

	char buf[256];
	FlashReadData( 0, buf, 256 );

	printf( "Check Upgrade App : 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X\n", 
		buf[0], buf[1], buf[2], buf[3], buf[4] );

#endif

	if	(	//	DFU Mode ( Menu + OK + SOS 버튼을 누른상태에서 전원 On )
			HAL_GPIO_ReadPin( DOME1_GPIO_Port, DOME1_Pin ) == 0			//	Menu
			|| HAL_GPIO_ReadPin( DOME3_GPIO_Port, DOME3_Pin ) == 0		//	OK
			|| HAL_GPIO_ReadPin( SOS_KEY_GPIO_Port, SOS_KEY_Pin ) == 0	//	SOS
		)
	{
		//========================================================================
		//	Jump to Embedded Bootloader
		//	DFU Mode
		printf("STBootLoader Mode ( DFU Mode )\n");
		JumpToSTBootloader();
		//========================================================================
	}
	else if	(	GetBootMode() == BModeBoot				//	Boot Mode로 진입 시.
			//|| HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_11) == 0	//	Test버튼 누른상태에서 Booting 시.
		)
	{
		//	reset 할 때 test 버튼 누를 시
		printf("BootLoader Mode\n");
		HAL_Delay(1);
	}
#if defined( USE_YMODEM_EXT_FLASH )
	else if ( ( buf[0] == 0xaa ) && ( buf[1] == 0x55 ) )
#else
	else if ( (*(char*)(0x0801F020) == 0xaa) && (*(char*)(0x0801F021) == 0x55) )
#endif
	{
		//	Upgrade 할 이미지가 존재하는 경우.
		printf("Find a New Version.\n");

		//========================================================================
		//	Erase Internal Flash
		//	App 영역 삭제

		//*
		FLASH_If_Erase( APPLICATION_ADDRESS );
		/*/
		for ( int idxSector = FLASH_SECTOR_5; idxSector <= FLASH_SECTOR_11; idxSector++ )
		{
			HAL_FLASH_Unlock();
			flash1.TypeErase	=	FLASH_TYPEERASE_SECTORS;
			flash1.Banks		=	FLASH_BANK_1;
			flash1.Sector		=	idxSector;		//	FLASH_SECTOR_5 ~ FLASH_SECTOR_11;
			flash1.NbSectors	=	1;
			flash1.VoltageRange =	FLASH_VOLTAGE_RANGE_3;
			printf( "Internal flash erase old app #%d\n", flash1.Sector );
			HAL_FLASHEx_Erase( &flash1, NULL );
			HAL_FLASH_Lock();
		}
		//	*/
		printf( "Internal flash erase done.\n" );
		//========================================================================

#if defined( USE_YMODEM_EXT_FLASH )
		fsize = ( buf[2] << 16 | buf[3] << 8 | buf[4] );		//	S/W Size
#else
		fsize = (*(char*)0x08010022 << 16 | *(char*)0x08010023 << 8 | *(char*)0x08010024);  // s/w size
#endif
		printf("New S/W size = %d\n", fsize);

		readnum = fsize / 512 + 1;
		rremain = fsize % 512;
		//printf("file size =%x\n",fsize);

		//========================================================================
		//	Write External Flash to Internal App

#if defined( USE_YMODEM_EXT_FLASH )
		SelectFlash( 0 );		//	MX25L128 ( PA4 - SPI_CS0 )
#endif


		for ( j = 0; j < readnum; j++ )
		{
			memset(adata,0x00,sizeof(adata));

#if defined( USE_YMODEM_EXT_FLASH )
			FlashRead512( flashAddr, adata );
			FlashWait();
#endif


//			 dump(adata,16,1);

			if ( j == ( readnum - 1 ) )
			{
				//*
				FLASH_If_Write( flashAddr, (uint32_t*)adata, 512 / 4 );
				/*/
				HAL_FLASH_Unlock();
				for ( k = 0; k < rremain; k++ )
				{
					HAL_FLASH_Program( FLASH_TYPEPROGRAM_BYTE, APPLICATION_ADDRESS1 + (j * 512) + k, adata[0+k]);
				}
				HAL_FLASH_Lock();
				//	*/
			}
			else
			{
				//*
				FLASH_If_Write( flashAddr, (uint32_t*)adata, 512 / 4 );
				/*/
				HAL_FLASH_Unlock();
				for ( k = 0; k < 512; k++ )
				{
					HAL_FLASH_Program( FLASH_TYPEPROGRAM_BYTE, APPLICATION_ADDRESS1 + (j * 512) + k, adata[0+k]);
				}
				HAL_FLASH_Lock();
				//	*/
			}

//			dump(APPLICATION_ADDRESS1 + (j * 512),16,1);

			//	*/

			flashAddr = flashAddr + 512;	

			printf ("#");
		}


		printf("\nNew SW Flash Done\n");
		//========================================================================

#if defined( USE_YMODEM_EXT_FLASH )

		//	Magic Number ( 0xaa55 )삭제.
		Flash32KBErase( 0x0000 );

#else
		HAL_FLASH_Unlock();
		flash1.TypeErase = FLASH_TYPEERASE_SECTORS;
		flash1.Banks = FLASH_BANK_1;
		flash1.Sector = FLASH_SECTOR_4;		//  0x08010000 ~ 0x0801ffff
		flash1.NbSectors = 1;
		flash1.VoltageRange = FLASH_VOLTAGE_RANGE_3;
		printf("Internal flash command erase old app #%d\n", flash1.Sector);
		HAL_FLASHEx_Erase(&flash1, NULL);	
		HAL_FLASH_Lock();
		printf("Internal flash erase command\n");
  
#endif
		printf("\n");
		printf("New S/W Boot\n");
		HAL_Delay(1000);
		if	( (((*(__IO uint32_t*)APPLICATION_ADDRESS1) & 0x2FFE0000 ) == 0x20000000)
			 ||	( ( ( *( __IO uint32_t* )APPLICATION_ADDRESS1 ) & 0x2FF00000 ) == 0x20000000 )		//	ST4STM32(Ac6 SystemWorkbench) Binary Image
			)
		{
			printf(" Jump to Main Application......\n");
			JumpAddress = *(__IO uint32_t*) (APPLICATION_ADDRESS1 + 4);
			Jump_To_Application = (pFunction) JumpAddress;
//			vPortEnterCritical();
			HAL_DeInit();
			/* Initialize user application's Stack Pointer */
			__set_MSP(*(__IO uint32_t*) APPLICATION_ADDRESS1);
			Jump_To_Application();
		}
	}
	else
	{
		printf("\n");
		printf("%s(%d) : Automatic Boot\n", __func__, __LINE__);
		HAL_Delay(1000);

		printf("0x%08X : 0x%08X\n", APPLICATION_ADDRESS1, *(__IO uint32_t*)APPLICATION_ADDRESS1 );

		if	( (((*(__IO uint32_t*)APPLICATION_ADDRESS1) & 0x2FFE0000 ) == 0x20000000)			//	Keil Binary Image
			|| (((*(__IO uint32_t*)APPLICATION_ADDRESS1) & 0x2FF00000 ) == 0x20000000)		//	ST4STM32(Ac6 SystemWorkbench) Binary Image
			)
		{
			printf(" Jump to Main Application......\n");
		}

		JumpAddress = *(__IO uint32_t*) (APPLICATION_ADDRESS1 + 4);
		Jump_To_Application = (pFunction) JumpAddress;

		//========================================================================
		//	De-Initialize
//		HAL_TIM_Base_Stop( &htim2 );

//		vPortEnterCritical();
		HAL_DeInit();
		//========================================================================

		/* Initialize user application's Stack Pointer */
		__set_MSP(*(__IO uint32_t*) APPLICATION_ADDRESS1);

		Jump_To_Application();
	}

	while ( 1 )
	{
		//	while(1) - Task 종료시 Blocking 됨.
		HAL_Delay( 1000 );
	}
}


//========================================================================
int cmd_stboot(int argc, char *argv[])
//========================================================================
{
	printf( "Jump To STM32 Bootloader\n" );

	JumpToSTBootloader();

	return 0;
}

