/**
  ******************************************************************************
  * @file    GPIO_Toggle\main.c
  * @author  MCD Application Team
  * @version V2.0.4
  * @date    26-April-2018
  * @brief   This file contains the main function for GPIO Toggle example.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2014 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */ 

/* Includes ------------------------------------------------------------------*/
#include "stm8s.h"

/**
  * @addtogroup GPIO_Toggle
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
enum {
  ALL_LED_OFF = 0,
  BLUE_LED_ON = 1,
  WHITE_LED_ON = 2,
  MAX_STATE = 3
};

/* Private define ------------------------------------------------------------*/
/* Evalboard I/Os configuration */

#define SWITCH_GPIO_PORT        (GPIOC)
#define SWITCH_GPIO_PIN         (GPIO_PIN_5)

#define LED_GPIO_PORT           (GPIOD)
#define WHITE_LED_GPIO_PIN      (GPIO_PIN_3)
#define BLUE_LED_GPIO_PIN       (GPIO_PIN_4)

/* Private macro -------------------------------------------------------------*/
#define SWITCH_ON       RESET
#define SWITCH_OFF      SET

#define READ_SWITCH()   (GPIO_ReadInputPin(SWITCH_GPIO_PORT, (GPIO_Pin_TypeDef)SWITCH_GPIO_PIN))

#define TIME_OF_SWITCH  500
#define DIMMING_CNT     (TIME_OF_SWITCH*50)
#define MAX_DUTY        1000
#define MIN_DUTY        0

#define SET_BLUE_LED(brightness)        (TIM2_SetCompare1((brightness)))
#define SET_WHITE_LED(brightness)       (TIM2_SetCompare2((brightness)))

#define LED_OFF MIN_DUTY
#define LED_ON  MAX_DUTY

#define GET_BRIGHT      0
#define GET_DARK        1

#define BRIGHTNESS_STEP         2
#define BRIGHTNESS_DELAY        0x180

/* Private variables ---------------------------------------------------------*/
static uint16_t white_duty,blue_duty,white_offset,blue_offset;

/* Private function prototypes -----------------------------------------------*/
void Delay (uint16_t nCount);
void run_dimming_mode(uint16_t* duty,uint16_t* offset,uint8_t state);
uint8_t update_state(uint8_t cur_state);

/* Private functions ---------------------------------------------------------*/
uint8_t update_state(uint8_t cur_state)
{
  uint8_t new_state= (cur_state+1)%MAX_STATE;
  
  switch (new_state)
  {
    case  ALL_LED_OFF:
      SET_BLUE_LED(LED_OFF);
      SET_WHITE_LED(LED_OFF);
      //GPIO_WriteLow(LED_GPIO_PORT, WHITE_LED_GPIO_PIN);
      //GPIO_WriteLow(LED_GPIO_PORT, BLUE_LED_GPIO_PIN);
      break;
    case  BLUE_LED_ON:
      SET_BLUE_LED(blue_duty);
      SET_WHITE_LED(LED_OFF);
      //GPIO_WriteHigh(LED_GPIO_PORT, BLUE_LED_GPIO_PIN);
      //GPIO_WriteLow(LED_GPIO_PORT, WHITE_LED_GPIO_PIN);
      //duty = LED_ON;
      //offset = GET_DARK;
      break;
    case  WHITE_LED_ON:
      SET_BLUE_LED(LED_OFF);
      SET_WHITE_LED(white_duty);
      //GPIO_WriteLow(LED_GPIO_PORT, BLUE_LED_GPIO_PIN);
      //GPIO_WriteHigh(LED_GPIO_PORT, WHITE_LED_GPIO_PIN);
      //duty = LED_ON;
      //offset = GET_DARK;
      break;
    default:
      // do nothing
      break;
  }
  
  return new_state;
}

void run_dimming_mode(uint16_t* duty,uint16_t* offset,uint8_t state)
{
  for(;READ_SWITCH()==SWITCH_ON;)
  {
    if ((*offset) == GET_DARK)
    {
      for(;READ_SWITCH()==SWITCH_ON&&(*duty)>MIN_DUTY;*duty-=BRIGHTNESS_STEP)
      {
        (state==BLUE_LED_ON)?SET_BLUE_LED(*duty):SET_WHITE_LED(*duty);
        Delay(BRIGHTNESS_DELAY);
      }
      
      if ((*duty) == MIN_DUTY)
      {
        *offset = GET_BRIGHT;
      }
    }
    else
    {
      for(;READ_SWITCH()==SWITCH_ON&&(*duty)<MAX_DUTY;*duty+=BRIGHTNESS_STEP)
      {
        (state==BLUE_LED_ON)?SET_BLUE_LED(*duty):SET_WHITE_LED(*duty);      
        Delay(BRIGHTNESS_DELAY);
      }
      
      if ((*duty) == MAX_DUTY)
      {
        *offset = GET_DARK;
      }
    }
  }
}
/* Public functions ----------------------------------------------------------*/

/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */

void main(void)
{
  uint32_t      cnt = 0;
  uint8_t       cur_state = ALL_LED_OFF;
  
  /* Initialize I/Os in Output Mode */
  // Active High
  GPIO_DeInit(LED_GPIO_PORT);
  GPIO_Init(LED_GPIO_PORT, (GPIO_Pin_TypeDef)WHITE_LED_GPIO_PIN, GPIO_MODE_OUT_PP_LOW_FAST);
  GPIO_Init(LED_GPIO_PORT, (GPIO_Pin_TypeDef)BLUE_LED_GPIO_PIN, GPIO_MODE_OUT_PP_LOW_FAST);
  // Active Low
  GPIO_DeInit(SWITCH_GPIO_PORT);
  GPIO_Init(SWITCH_GPIO_PORT, (GPIO_Pin_TypeDef)SWITCH_GPIO_PIN, GPIO_MODE_IN_PU_NO_IT);
  
/* Initialize Timer */
  TIM2_DeInit();
  TIM2_TimeBaseInit(TIM2_PRESCALER_32, MAX_DUTY);
  TIM2_OC1Init(TIM2_OCMODE_PWM1, TIM2_OUTPUTSTATE_ENABLE, MIN_DUTY, TIM2_OCPOLARITY_HIGH);
  TIM2_OC2Init(TIM2_OCMODE_PWM1, TIM2_OUTPUTSTATE_ENABLE, MIN_DUTY, TIM2_OCPOLARITY_HIGH);
    
  /* Start Timer as LED off state */
  TIM2_Cmd(ENABLE);
  SET_BLUE_LED(LED_OFF);
  SET_WHITE_LED(LED_OFF);
  
  white_duty = blue_duty = MAX_DUTY;
  white_offset = blue_offset = GET_DARK;
  
  while (1)
  {
    if (READ_SWITCH() == SWITCH_ON)
    {
      if(++cnt>=DIMMING_CNT && cur_state!=ALL_LED_OFF)
      {
        if (cur_state == BLUE_LED_ON) {
          run_dimming_mode(&blue_duty,&blue_offset,cur_state);
        } else {
          run_dimming_mode(&white_duty,&white_offset,cur_state);
        }
        
        cnt = 0;
      }
    }
    else
    {
      if (cnt >= TIME_OF_SWITCH)
      {
        cur_state = update_state(cur_state);
      }

      cnt = 0;
    }
  }
}

/**
  * @brief Delay
  * @param nCount
  * @retval None
  */
void Delay(uint16_t nCount)
{
  /* Decrement nCount value */
  while (nCount!=0 && READ_SWITCH()==SWITCH_ON)
  {
    nCount--;
  }
}

#ifdef USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *   where the assert_param error has occurred.
  * @param file: pointer to the source file name
  * @param line: assert_param error line source number
  * @retval None
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


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/