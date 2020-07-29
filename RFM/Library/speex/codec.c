/**
  ******************************************************************************
  * @file SpeexVocoder_STM32_SK/src/main.c 
  * @author  MCD Application Team
  * @version  V2.0.0
  * @date  04/27/2009
  * @brief  Main program body
  ******************************************************************************
  * @copy
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2009 STMicroelectronics</center></h2>
  */ 


/* Includes ------------------------------------------------------------------*/
#include "codec.h"
#include "voice.h"
//#include "drv_hd44780.h"  // Driver for LCD on IAR STM32-SK board

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include <speex/speex.h>
#include"arch.h"

/** @addtogroup SpeexVocoder_STM32_SK
  * @{
  */ 


/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define ALL_FRAMES      300   /* the encoded male voice length */

#define LCD_DELAY       0x7FFFFF
#define BUTTON_DELAY    0xFFFFF

#define KEY_NONE      0
#define BUTTON_WAKEUP 1
#define BUTTON_B1     2
#define BUTTON_B2     3
#define BUTTON_B3     4

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
__IO int16_t IN_Buffer[2][FRAME_SIZE];
__IO int16_t OUT_Buffer[2][FRAME_SIZE];
__IO int16_t *inBuffer = IN_Buffer[0];
__IO int16_t *outBuffer = OUT_Buffer[0];
uint32_t Flash_Address = RECORDING_START_ADDRESS;
__IO uint8_t Start_Encoding = 0;
__IO uint8_t Start_Decoding=0;
__IO uint8_t Start_Playing = 0;
__IO uint8_t Recording = 0;
__IO uint8_t Playing =0;
uint32_t Encoded_Frames=0;

SpeexBits bits;/* Holds bits so they can be read and written by the Speex routines */
void *enc_state, *dec_state;/* Holds the states of the encoder & the decoder */
int quality = 4, complexity=1, vbr=0, enh=1;/* SPEEX PARAMETERS, MUST REMAINED UNCHANGED */

char out_bytes[ENCODED_FRAME_SIZE];
char input_bytes[ENCODED_FRAME_SIZE];
uint16_t sample_index = 0;
__IO uint16_t NB_Frames=0;
uint8_t *pFlash = (uint8_t*)RECORDING_START_ADDRESS;

uint8_t Button;
__IO uint32_t dly;


/* Private function prototypes -----------------------------------------------*/
void Demo_Init(void);
void Speex_Init(void);
void InterruptConfig(void);
//u8   GetButton(void);
void InitButtons(void);

/* Private functions ---------------------------------------------------------*/



/**
  * @brief  Main program.
  * @param  None
  * @retval : None
  */
int codec_main(void)
{

  Demo_Init();
  Speex_Init();
    
  /* LCD init */
//  HD44780_PowerUpInit();
//  LCD_LIGHT_ON();
//
//  /* Display welcome message */
//  HD44780_StrShow(1, 1,  "    STM32F10x   ");
//  HD44780_StrShow(1, 2,  "    Speex Demo  ");

  /* Delay */
  for(dly=0; dly<LCD_DELAY; dly++);

  /* Init control buttons */
  InitButtons() ;
  
MENU_LABEL:

  
  
  /* Draw menu */
//  HD44780_StrShow(1, 1,  "1. MSG 2. RC-PLY");
//  HD44780_StrShow(1, 2,  "3. Rec 4. Play  ");

  while(1) 
  {
    Button = GetButton();
    
    /* Delay */
    for(dly=0; dly<BUTTON_DELAY; dly++);

    if(Button != KEY_NONE) break;
  }

  /* Playing welcome message ---------------------------------------------------*/
  if(Button == BUTTON_WAKEUP) 
  {
    int i;
    
//    HD44780_StrShow(1, 1,  "Play stored     ");
//    HD44780_StrShow(1, 2,  "encoded voice.  ");

    Start_Playing = MALE_VOICE;
    Vocoder_Init();
  
    /* we prepare two buffers of decoded data: */
    /* the first one, */
    for(i=0;i<ENCODED_FRAME_SIZE; i++)
    {
      input_bytes[i] = male_voice[sample_index++];
    } 
      
    speex_bits_read_from(&bits, input_bytes, ENCODED_FRAME_SIZE);
    speex_decode_int(dec_state, &bits, (spx_int16_t*)OUT_Buffer[0]);
  
    /* and the second one. */
    for(i=0;i<ENCODED_FRAME_SIZE; i++)
    {
      input_bytes[i] = male_voice[sample_index++];
    } 
 
    speex_bits_read_from(&bits, input_bytes, ENCODED_FRAME_SIZE);
    speex_decode_int(dec_state, &bits, (spx_int16_t*)OUT_Buffer[1]);

    NB_Frames++;

    Vocoder_Start();
  
    /* Now we wait until the playing of the buffers to re-decode ...*/
    while((NB_Frames < ALL_FRAMES)&&(GetButton() == KEY_NONE))
    {
      if(Start_Decoding == 1) /* we start decoding the first buffer */
      {
        for(i=0;i<ENCODED_FRAME_SIZE; i++)
        {
          input_bytes[i] = male_voice[sample_index++];
        }
        
        /* Copy the encoded data into the bit-stream struct */
        speex_bits_read_from(&bits, input_bytes, ENCODED_FRAME_SIZE);
        /* Decode the data */
        speex_decode_int(dec_state, &bits, (spx_int16_t*)OUT_Buffer[0]);
        
        Start_Decoding = 0;
        NB_Frames++;
      }
      if(Start_Decoding == 2) /* we start decoding the second buffer */
      {
        for(i=0;i<ENCODED_FRAME_SIZE; i++)
        {
          input_bytes[i] = male_voice[sample_index++];
        }
        
        /* Copy the encoded data into the bit-stream struct */
        speex_bits_read_from(&bits, input_bytes, ENCODED_FRAME_SIZE);
        /* Decode the data */
        speex_decode_int(dec_state, &bits, (spx_int16_t*)OUT_Buffer[1]);
        
        Start_Decoding = 0;
        NB_Frames++;
      }
      
      Button = GetButton();
    }
    
    sample_index = 0;
    NB_Frames = 0;
    inBuffer = IN_Buffer[0];
    outBuffer = OUT_Buffer[0];
    Vocoder_Stop();
    Button = KEY_NONE;
    goto MENU_LABEL;
  }

  /* Real time playing ---------------------------------------------------------*/
  if(Button == BUTTON_B1) 
  {  
    HD44780_StrShow(1, 1,  "Real time play  ");
    HD44780_StrShow(1, 2,  "                ");
      
    Vocoder_Init();
    Vocoder_Start();
    Start_Playing = REAL_VOICE;
  
    while(GetButton() == KEY_NONE)
    {    
      if(Start_Encoding == 1)
      {
        /* Flush all the bits in the struct so we can encode a new frame */
        speex_bits_reset(&bits);
        /* Encode the frame */
        speex_encode_int(enc_state, (spx_int16_t*)IN_Buffer[0], &bits);
        /* Copy the bits to an array of char that can be decoded */
        speex_bits_write(&bits, (char *)out_bytes, ENCODED_FRAME_SIZE);
        /* Copy the encoded data into the bit-stream struct */
        speex_bits_read_from(&bits, (char *)out_bytes, ENCODED_FRAME_SIZE);
        /* Decode the data */
        speex_decode_int(dec_state, &bits, (spx_int16_t*)OUT_Buffer[0]);

        Start_Encoding = 0;	
      }
      else if(Start_Encoding == 2)
      {
        /* Flush all the bits in the struct so we can encode a new frame */
        speex_bits_reset(&bits);
        /* Encode the frame */
        speex_encode_int(enc_state, (spx_int16_t*)IN_Buffer[1], &bits);
        /* Copy the bits to an array of char that can be decoded */
        speex_bits_write(&bits, (char *)out_bytes, ENCODED_FRAME_SIZE);
        /* Copy the encoded data into the bit-stream struct */
        speex_bits_read_from(&bits, (char *)out_bytes, ENCODED_FRAME_SIZE);
        /* Decode the data */
        speex_decode_int(dec_state, &bits, (spx_int16_t*)OUT_Buffer[1]);
        
        Start_Encoding = 0;
      }
    }
    
    Start_Encoding = 0;
    inBuffer = IN_Buffer[0];
    outBuffer = OUT_Buffer[0];
    Vocoder_Stop();
    Button = KEY_NONE;
    goto MENU_LABEL;
  }

  /* Record voice   ------------------------------------------------------------*/
  if(Button == BUTTON_B2) 
  {
    HD44780_StrShow(1, 1,  "Record Voice    ");
    HD44780_StrShow(1, 2,  "                ");
    
    Vocoder_Init();
    Voice_Recording_Init();

    Recording = 1;
    Encoded_Frames=0;

    HD44780_StrShow(1, 1,  "Recording..    ");
    HD44780_StrShow(1, 2,  "WakeUp to Stop ");

    Voice_Recording_Start();
    
    while(Recording && (GetButton() == KEY_NONE))
    {	 
      if(Flash_Address == RECORDING_END_ADDRESS)
      {
        Recording = 0;
        Start_Encoding = 0;	 
      }
      
      if(Start_Encoding == 1)
      {
        int i;
        
        /* Flush all the bits in the struct so we can encode a new frame */
        speex_bits_reset(&bits);
        /* Encode the frame */
        speex_encode_int(enc_state, (spx_int16_t*)IN_Buffer[0], &bits);
        /* Copy the bits to an array of char that can be decoded */
        speex_bits_write(&bits, (char *)out_bytes, ENCODED_FRAME_SIZE);
        
        for(i=0;i<ENCODED_FRAME_SIZE;i+=2)
        {
          FLASH_ProgramHalfWord(Flash_Address, (out_bytes[i]|out_bytes[i+1]<<8));
          Flash_Address+=2;
        }
        
        Start_Encoding = 0;
      }
      else if(Start_Encoding == 2)
      {
        int i;
        
        /* Flush all the bits in the struct so we can encode a new frame */
        speex_bits_reset(&bits);
        /* Encode the frame */
        speex_encode_int(enc_state, (spx_int16_t*)IN_Buffer[1], &bits);
        /* Copy the bits to an array of char that can be decoded */
        speex_bits_write(&bits, (char *)out_bytes, ENCODED_FRAME_SIZE);
        
        for(i=0;i<ENCODED_FRAME_SIZE;i+=2)
        {
          FLASH_ProgramHalfWord(Flash_Address, (out_bytes[i+1]<<8)|out_bytes[i]);
          Flash_Address+=2;
	}
        
        Start_Encoding = 0;
      } 
    }
   
    inBuffer = IN_Buffer[0];
    outBuffer = OUT_Buffer[0];
    Recording = 0;
    Start_Encoding = 0;
    Flash_Address = RECORDING_START_ADDRESS;  
    pFlash = (uint8_t*)RECORDING_START_ADDRESS;
    Voice_Recording_Stop();
    Button = KEY_NONE;
    goto MENU_LABEL;
  }
  
  /* Play recorded voice   -----------------------------------------------------*/
  if(Button == BUTTON_B3) 
  {
    Playing = 1;  
    Vocoder_Init();
    
//    HD44780_StrShow(1, 1,  "Playing...     ");
//    HD44780_StrShow(1, 2,  "WakeUp to Stop ");
  
    Voice_Playing_Start();
    Start_Decoding = 1;
  
    while ((NB_Frames!=Encoded_Frames)&& (GetButton() == KEY_NONE))
    {
      if(Start_Decoding == 1)
      {
        int i;
	
        /* Read 20 data from the flash memory */
        for(i=0;i<ENCODED_FRAME_SIZE; i++)
        {
          input_bytes[i] = *pFlash++;
        } 
  
        /* Copy the encoded data into the bit-stream struct */    
        speex_bits_read_from(&bits, (char*)input_bytes, ENCODED_FRAME_SIZE);  
        /* Decode the data */
        speex_decode_int(dec_state, &bits, (spx_int16_t *)OUT_Buffer[0]);
        /* Signal the end of the decoding */
        Start_Decoding = 0;
        
        NB_Frames++;
      }
      else if(Start_Decoding == 2)
      {
        int i;	
        
        /* Read 20 data from the flash memory */
        for(i=0;i<ENCODED_FRAME_SIZE; i++)
        {
          input_bytes[i] = *pFlash++;
        } 
  
        /* Copy the encoded data into the bit-stream struct */    
        speex_bits_read_from(&bits, (char*)input_bytes, ENCODED_FRAME_SIZE);  
        /* Decode the data */
        speex_decode_int(dec_state, &bits, (spx_int16_t *)OUT_Buffer[1]);
        /* Signal the end of the decoding */
        Start_Decoding = 0;
        
        NB_Frames++;
      }
    }
    
    Start_Encoding = 0;
    Start_Decoding = 0;
    Playing =0;
    NB_Frames = 0;
    inBuffer = IN_Buffer[0];
    outBuffer = OUT_Buffer[0];
    pFlash = (uint8_t*)RECORDING_START_ADDRESS;
    Flash_Address = RECORDING_START_ADDRESS;
    Button = KEY_NONE;
    goto MENU_LABEL;
  }
  
  while(1);
}



/**
  * @brief  Initializes the demonstration application.
  * @param  None
  * @retval : None
  */
void Demo_Init(void)
{
//  ErrorStatus HSEStartUpStatus;
//
//  /* RCC system reset(for debug purpose) */
//  RCC_DeInit();
//
//  /* Enable HSE */
//  RCC_HSEConfig(RCC_HSE_ON);
//
//  /* Wait till HSE is ready */
//  HSEStartUpStatus = RCC_WaitForHSEStartUp();
//
//  if (HSEStartUpStatus == SUCCESS)
//  {
//    /* Enable Prefetch Buffer */
//    FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);
//
//    /* Flash 2 wait state */
//    FLASH_SetLatency(FLASH_Latency_2);
//
//    /* HCLK = SYSCLK */
//    RCC_HCLKConfig(RCC_SYSCLK_Div1);
//
//    /* PCLK2 = HCLK */
//    RCC_PCLK2Config(RCC_HCLK_Div1);
//
//    /* PCLK1 = HCLK/2 */
//    RCC_PCLK1Config(RCC_HCLK_Div2);
//
//    /* PLLCLK = 8MHz * 9 = 72 MHz */
//    RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9);
//
//    /* Enable PLL */
//    RCC_PLLCmd(ENABLE);
//
//    /* Wait till PLL is ready */
//    while (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
//    {}
//
//    /* Select PLL as system clock source */
//    RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
//
//    /* Wait till PLL is used as system clock source */
//    while (RCC_GetSYSCLKSource() != 0x08)
//    {}
//  }
//
//  /* TIM2 and TIM3 clocks enable */
//  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2 | RCC_APB1Periph_TIM3 , ENABLE);
//
//   /* Enable GPIOA, GPIOC, ADC1 , AFIO and TIM1 clock */
//  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1 | RCC_APB2Periph_GPIOA| RCC_APB2Periph_GPIOC | RCC_APB2Periph_TIM1 | RCC_APB2Periph_AFIO, ENABLE);
//
//  /* Interrupt Configuration */
//  InterruptConfig();

}



/**
  * @brief  Configures the used IRQ Channels and sets their priority.
  * @param  None
  * @retval : None
  */
void InterruptConfig(void)
{ 

   /* 1 bit for pre-emption priority, 3 bits for subpriority */
  NVIC_SetPriorityGrouping(6); 

	 /* Enable the TIM2 Interrupt */
  NVIC_SetPriority(TIM2_IRQn, 0x00); /* 0x00 = 0x01 << 3 | (0x00 & 0x7*/
  NVIC_EnableIRQ(TIM2_IRQn);
  /* Enable the TIM3 Interrupt */
 	 /* Enable the TIM2 Interrupt */
  NVIC_SetPriority(TIM3_IRQn, 0x00); /* 0x00 = 0x01 << 3 | (0x00 & 0x7*/
  NVIC_EnableIRQ(TIM3_IRQn);
}



/**
  * @brief  Initializes the speex codec
  * @param  None
  * @retval : None
  */
void Speex_Init(void)
{
  /* Speex encoding initializations */ 
  speex_bits_init(&bits);
  enc_state = speex_encoder_init(&speex_nb_mode);
  speex_encoder_ctl(enc_state, SPEEX_SET_VBR, &vbr);
  speex_encoder_ctl(enc_state, SPEEX_SET_QUALITY,&quality);
  speex_encoder_ctl(enc_state, SPEEX_SET_COMPLEXITY, &complexity);
  /* speex decoding intilalization */
  dec_state = speex_decoder_init(&speex_nb_mode);
  speex_decoder_ctl(dec_state, SPEEX_SET_ENH, &enh);
}

/*************************************************************************
* Function Name  : InitButtons
* Description    : Init Buttons
* Input          : None
* Output         : None
* Return         : None
*************************************************************************/
void InitButtons(void) 
{
//  GPIO_InitTypeDef GPIO_InitStructure;
//
//  Button = 0;/* Set variables asociates */
//
//  /* WakeUp as input */
//  GPIO_InitStructure.GPIO_Pin   =  GPIO_Pin_0;
//  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN_FLOATING;
//  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//  GPIO_Init(GPIOA, &GPIO_InitStructure);
//
//  /* B2 as input */
//  GPIO_InitStructure.GPIO_Pin   =  GPIO_Pin_5;
//  //  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN_FLOATING;
//  //  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//  GPIO_Init(GPIOB, &GPIO_InitStructure);
//
//  /* B1 and B3 as input */
//  GPIO_InitStructure.GPIO_Pin   =  GPIO_Pin_13 | GPIO_Pin_4;
//  //  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN_FLOATING;
//  //  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//  GPIO_Init(GPIOC, &GPIO_InitStructure);
}

/*************************************************************************
* Function Name  : GetButton
* Description    : Get Push Button Value
* Input          : None
* Output         : None
* Return         : Push Button Value
*************************************************************************/
//u8 GetButton(void)
//{
//  if((GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_13)) == Bit_SET) return BUTTON_B1;     /* B1     */
//  if((GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_4))  == Bit_SET) return BUTTON_B3;     /* B3     */
//  if((GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_5))  == Bit_SET) return BUTTON_B2;     /* B2     */
//  if((GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0))  == Bit_SET) return BUTTON_WAKEUP; /* Wakeup */
//
//  return KEY_NONE;
//}



/**
  * @brief  Ovveride the _speex_putc function of the speex library
  * @param  None
  * @retval : None
  */
void _speex_putc(int ch, void *file)
{
  while(1)
  {
  };
}



/**
  * @brief  Ovveride the _speex_fatal function of the speex library
  * @param  None
  * @retval : None
  */
void _speex_fatal(const char *str, const char *file, int line)
{
  while(1)
  {
  };
}

#ifdef  USE_FULL_ASSERT


/**
  * @brief  Reports the name of the source file and the source line number
  *   where the assert_param error has occurred.
  * @param file: pointer to the source file name
  * @param line: assert_param error line source number
  * @retval : None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

/**
  * @}
  */ 

/******************* (C) COPYRIGHT 2009 STMicroelectronics *****END OF FILE****/
