#ifndef __MAIN_H__
#define __MAIN_H__

#define EXTI_SWITCH_GPIO_PORT   (EXTI_PORT_GPIOC)

#define SWITCH_CHG_GPIO_PORT    (GPIOC)
#define SWITCH_GPIO_PIN         (GPIO_PIN_5)
#define CHG_GPIO_PIN            (GPIO_PIN_4)

#define LED_GPIO_PORT           (GPIOD)
#define WHITE_LED_GPIO_PIN      (GPIO_PIN_3)
#define BLUE_LED_GPIO_PIN       (GPIO_PIN_4)
#define RED_LED_GPIO_PIN        (GPIO_PIN_2)

#define SWITCH_ON       RESET
#define SWITCH_OFF      SET

#define CHARGING        RESET
#define FULL            SET

#define READ_SWITCH()   (GPIO_ReadInputPin(SWITCH_CHG_GPIO_PORT, (GPIO_Pin_TypeDef)SWITCH_GPIO_PIN))
#define READ_CHG_STAT() (GPIO_ReadInputPin(SWITCH_CHG_GPIO_PORT, (GPIO_Pin_TypeDef)CHG_GPIO_PIN))

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

#define PRESCALER_OF_CHG_MONITOR_TIMER  50
#define PEROID_OF_CHG_MONITOR_TIMER     65535

#endif