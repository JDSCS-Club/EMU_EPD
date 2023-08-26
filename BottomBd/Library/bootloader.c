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

#if defined(STM32F407xx)
#include "stm32f4xx_hal.h"
#include "stm32f4xx.h"
#elif defined(STM32F207xx)
#include "stm32f2xx_hal.h"
#include "stm32f2xx.h"
#elif defined(STM32F103xE)	//	STM32F103xC
#include "stm32f1xx_hal.h"
#include "stm32f1xx.h"
#endif

#include "bootloader.h"

#include "flash_if.h"

//#include "iap_common.h"		//	IAP Common
//#include "iap_menu.h"		//	IAP ( In-Application Programming )

//#include "ymodem.h"			//	Y-Modem

#include "main.h"			//	huart2 / MX_IWDG_Disable()

#include "nvram.h"			//	AddrEEPUpgrMGN1

//========================================================================
//	printf disable
//#define		printf(arg, ...)
//========================================================================

//	STM32F407 Embedded Bootloader ( AN2606 - P.29 )
//#define BOOT_ROM_ADDRESS		(uint32_t)0x1FFF77DE

//#define ADDR_FLASH_APP   (uint32_t)0x08020000

//	Page 0 0x0800 0000 - 0x0800 07FF 2 Kbytes
//	Page 1 0x0800 0800 - 0x0800 0FFF 2 Kbytes
//	Page 2 0x0800 1000 - 0x0800 17FF 2 Kbytes
//	Page 3 0x0800 1800 - 0x0800 1FFF 2 Kbytes
//	.
//	.
//	.
//	Page 255 0x0807 F800 - 0x0807 FFFF 2 Kbytes

typedef  void (*pFunction)(void);

pFunction	Jump_To_Application;
uint32_t	mJumpAddress;

#if 0

//========================================================================
void JumpToSTBootloader(void)
//========================================================================
{
  typedef  void (*pFunction)(void);
  /* OK for L4 and F446 */
#define SYS_MEM_ADDRESS     ((uint32_t)0x1FFF0000)

  pFunction JumpToApplication;
  uint32_t sJumpAddress;

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
  sJumpAddress = *(__IO uint32_t*) (SYS_MEM_ADDRESS + 4);
  JumpToApplication = (pFunction) sJumpAddress;
  /* Initialize user application's Stack Pointer */
  __set_MSP(*(__IO uint32_t*) SYS_MEM_ADDRESS);
  JumpToApplication();
}

#endif


//========================================================================
void BootLoaderTask(void)
//========================================================================
{
	/* The parameters are not used. */
	int		i, j, k;
	int		fsize = 0;

	int		readnum = 0, rremain = 0;

//#if defined( USE_YMODEM_EXT_FLASH )
	uint8_t	adata[512];
//#else
//	uint8_t	*adata;
//#endif

	uint32_t		flashAddr = ADDR_FLASH_APP;	//	0x08020000;

	FLASH_EraseInitTypeDef flash1;

	//========================================================================
	//	MB85RC64T
#if 0
	I2C_BusScan( &hi2c2 );
	if ( HAL_OK == HAL_I2C_IsDeviceReady( &hi2c2, (uint16_t)( MB85RC64_ID ), 2, 2 ) )
	{
		//========================================================================
		//	MB85RC64T가 인식됨.
		printf("%s(%d) - Init MB85RC64T\n", __func__, __LINE__);
	}
#endif
	//========================================================================

	HAL_Delay( 500 );

	printf( "BootTask\n" );

	//========================================================================

#if defined( USE_YMODEM_EXT_FLASH )

	char buf[256];
	FlashReadData( 0, buf, 256 );

//	printf( "Check Upgrade App : 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X\n",
//		buf[0], buf[1], buf[2], buf[3], buf[4] );

#else

	char buf[10];
    MB85_HAL_ReadBytes( &hi2c2, 0xA0, AddrExtUpgrMGN1, (uint8_t *)buf, 5 );

#endif

//	if	(	GetBootMode() == BModeBoot				//	Boot Mode로 진입 시.
//			//|| HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_11) == 0	//	Test버튼 누른상태에서 Booting 시.
//		)
//	{
//		//	reset 할 때 test 버튼 누를 시
//		printf("BootLoader Mode\n");
//		HAL_Delay(1);
//	}
//#if defined( USE_YMODEM_EXT_FLASH )
//	else if ( ( buf[0] == 0xaa ) && ( buf[1] == 0x55 ) )
//#else
////	else if ( (*(char*)(0x0801F020) == 0xaa) && (*(char*)(0x0801F021) == 0x55) )
//	else
	if ( ( buf[0] == 0xaa ) && ( buf[1] == 0x55 )
			 &&	( ( ( *( __IO uint32_t* )ADDR_FLASH_IMGAPP ) & 0x2FF00000 ) == 0x20000000 )		//	Downlaod Image Correct
		)
	{
		//	Upgrade 할 이미지가 존재하는 경우.
		printf("Find a New Version.\n");

		//========================================================================
		//	Erase Internal Flash
		//	App 영역 삭제

		//*
//		FLASH_If_Erase( ADDR_FLASH_APP );

		//========================================================================
		//	ADDR_FLASH_PAGE_8 ( 0x08004000 ) ~ ADDR_FLASH_PAGE_64 ( 0x08020000 )
		FLASH_If_EraseSectors( ADDR_FLASH_APP, ADDR_FLASH_IMGBOOT );

		printf( "Internal flash erase done.\n" );
		//========================================================================

		//========================================================================
		printf( "Erase Delay\n" );
		HAL_Delay(3000);
		//========================================================================

#if defined( USE_YMODEM_EXT_FLASH )
		fsize = ( buf[2] << 16 | buf[3] << 8 | buf[4] );		//	S/W Size
#else

//		fsize = (*(char*)0x08010022 << 16 | *(char*)0x08010023 << 8 | *(char*)0x08010024);  // s/w size
		fsize = ( buf[2] << 16 | buf[3] << 8 | buf[4] );		//	S/W Size

#endif

//		printf("New S/W size = %d\n", fsize);

		//========================================================================
		/*
		printf("Erase Upgrade Config\n");
		memset(buf, 0, sizeof(buf));
	    M24_HAL_WriteBytes( &hi2c1, 0xA0, AddrEEPUpgrMGN1, (uint8_t *)buf, 5 );
	    //	*/
		//========================================================================

		readnum = fsize / 512 + 1;
		rremain = fsize % 512;
//		printf("readnum = %d\n", readnum);

//		printf("%s(%d) - Flash WritePorection( %d )\n", __func__, __LINE__,
//				FLASH_If_GetWriteProtectionStatus() );

		FLASH_If_WriteProtectionConfig( OB_WRPSTATE_DISABLE );

		//========================================================================
		//	Write External Flash to Internal App

#if defined( USE_YMODEM_EXT_FLASH )
		SelectFlash( 0 );		//	MX25L128 ( PA4 - SPI_CS0 )
#endif

		for ( j = 0; j < readnum; j++ )
		{
#if defined( USE_YMODEM_EXT_FLASH )
			memset( adata, 0x00, sizeof(adata) );
			FlashRead512( flashAddr, adata );
			FlashWait();
#else
//			memset( adata, 0x00, sizeof(adata) );
//			adata = (uint8_t *)( ADDR_FLASH_IMGAPP + ( 512 * j ) );
			memcpy( adata, (uint8_t *)( ADDR_FLASH_IMGAPP + ( 512 * j ) ), 512 );
#endif

			HAL_FLASH_Unlock();

			int nCntRetry;

			nCntRetry = 5;	//	5회 Re-Try

			while( FLASH_If_Write( flashAddr, (uint32_t *)adata, 512 / 4 ) != FLASHIF_OK && nCntRetry > 0 )
			{
				printf("Write Err\n");
				HAL_Delay( 1000 );
				nCntRetry--;
			}

			HAL_FLASH_Lock();

			//*
			printf ("#");
			/*/
			printf("0x%08X -> 0x%08X\n", adata, flashAddr);

			if( j <= 2 )
			{
				printf("0x%08X\n", adata);
				dump( (unsigned char *)adata, 512, 1 );
				printf("0x%08X\n", flashAddr);
				dump( (unsigned char *)flashAddr, 512, 1 );
			}
			//	*/

			flashAddr = flashAddr + 512;

//			HAL_Delay(5);		//
		}

		printf("\nNew SW Flash Done\n");
		//========================================================================

#if defined( USE_YMODEM_EXT_FLASH )

		//	Magic Number ( 0xaa55 )삭제.
		Flash32KBErase( 0x0000 );

#else

		//========================================================================
		printf("Erase Upgrade Config\n");
		memset(buf, 0, sizeof(buf));
	    MB85_HAL_WriteBytes( &hi2c2, 0xA0, AddrExtUpgrMGN1, (uint8_t *)buf, 5 );

		//========================================================================
		printf("Erase Upgrade Binary Image\n");
		FLASH_If_Erase( ADDR_FLASH_IMGAPP );		//	0x080A0000

		//========================================================================

#endif

		printf("\n");
		printf("New S/W Boot\n");
		HAL_Delay(1000);
		if	( (((*(__IO uint32_t*)ADDR_FLASH_APP) & 0x2FFE0000 ) == 0x20000000)
			 ||	( ( ( *( __IO uint32_t* )ADDR_FLASH_APP ) & 0x2FF00000 ) == 0x20000000 )		//	ST4STM32(Ac6 SystemWorkbench) Binary Image
			)
		{
			printf(" Jump App\n");
			mJumpAddress = *(__IO uint32_t*) (ADDR_FLASH_APP + 4);
			Jump_To_Application = (pFunction) mJumpAddress;
//			vPortEnterCritical();
			HAL_DeInit();
			/* Initialize user application's Stack Pointer */
			__set_MSP(*(__IO uint32_t*) ADDR_FLASH_APP);
			Jump_To_Application();
		}
	}
	else
	{
		printf("\n");
		printf("Automatic Boot\n");
		HAL_Delay(1000);

//		printf("0x%08X : 0x%08X\n", ADDR_FLASH_APP, *(__IO uint32_t*)ADDR_FLASH_APP );

		if	( (((*(__IO uint32_t*)ADDR_FLASH_APP) & 0x2FFE0000 ) == 0x20000000)			//	Keil Binary Image
			|| (((*(__IO uint32_t*)ADDR_FLASH_APP) & 0x2FF00000 ) == 0x20000000)		//	ST4STM32(Ac6 SystemWorkbench) Binary Image
			)
		{
			printf(" Jump App\n");
			mJumpAddress = *(__IO uint32_t*) (ADDR_FLASH_APP + 4);
			Jump_To_Application = (pFunction) mJumpAddress;
			//========================================================================
			//	De-Initialize
	//		HAL_TIM_Base_Stop( &htim2 );

	//		HAL_DeInit();
			//========================================================================

			/* Initialize user application's Stack Pointer */
			__set_MSP(*(__IO uint32_t*) ADDR_FLASH_APP);

			Jump_To_Application();
		}

//	    /* Test if user code is programmed starting from address "APPLICATION_ADDRESS" */
//	    if (((*(__IO uint32_t*)APPLICATION_ADDRESS) & 0x2FFE0000 ) == 0x20000000)
//	    {
//	      /* Jump to user application */
//	      JumpAddress = *(__IO uint32_t*) (APPLICATION_ADDRESS + 4);
//	      JumpToApplication = (pFunction) JumpAddress;
//	      /* Initialize user application's Stack Pointer */
//	      __set_MSP(*(__IO uint32_t*) APPLICATION_ADDRESS);
//	      JumpToApplication();
//	    }


	}

	while ( 1 )
	{
		//	while(1) - Task 종료시 Blocking 됨.
		HAL_Delay( 1000 );
	}
}

//
////========================================================================
//int cmd_stboot(int argc, char *argv[])
////========================================================================
//{
//	printf( "Jump To STM32 Bootloader\n" );
//
//	DispDFUMode();
//
//	JumpToSTBootloader();
//
//	return 0;
//}

