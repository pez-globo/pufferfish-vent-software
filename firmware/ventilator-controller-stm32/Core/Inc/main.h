/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32h7xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define SET_MANUAL_BREATH_Pin GPIO_PIN_2
#define SET_MANUAL_BREATH_GPIO_Port GPIOE
#define VSYS_3V3_PGOOD_Pin GPIO_PIN_3
#define VSYS_3V3_PGOOD_GPIO_Port GPIOE
#define VSYS_5V0_PGOOD_Pin GPIO_PIN_4
#define VSYS_5V0_PGOOD_GPIO_Port GPIOE
#define BAT_MEAS_EN_Pin GPIO_PIN_5
#define BAT_MEAS_EN_GPIO_Port GPIOE
#define LED1_EN_Pin GPIO_PIN_6
#define LED1_EN_GPIO_Port GPIOE
#define B1_Pin GPIO_PIN_13
#define B1_GPIO_Port GPIOC
#define LTC4421_PWR_nFAULT2_Pin GPIO_PIN_0
#define LTC4421_PWR_nFAULT2_GPIO_Port GPIOF
#define LTC4421_PWR_nFAULT1_Pin GPIO_PIN_1
#define LTC4421_PWR_nFAULT1_GPIO_Port GPIOF
#define BAT_CHARGE_EN_Pin GPIO_PIN_2
#define BAT_CHARGE_EN_GPIO_Port GPIOF
#define PRESS2_EN_Pin GPIO_PIN_3
#define PRESS2_EN_GPIO_Port GPIOF
#define MOTOR3_UART_Pin GPIO_PIN_7
#define MOTOR3_UART_GPIO_Port GPIOF
#define BAT_MEAS_ANALOG_Pin GPIO_PIN_8
#define BAT_MEAS_ANALOG_GPIO_Port GPIOF
#define SENSE_O2_OUT_Pin GPIO_PIN_9
#define SENSE_O2_OUT_GPIO_Port GPIOF
#define DRIVE2_CH3_EN_Pin GPIO_PIN_0
#define DRIVE2_CH3_EN_GPIO_Port GPIOA
#define ALARM1_MED_Pin GPIO_PIN_1
#define ALARM1_MED_GPIO_Port GPIOA
#define DRIVE1_CH1_EN_Pin GPIO_PIN_3
#define DRIVE1_CH1_EN_GPIO_Port GPIOA
#define GPIO2_Pin GPIO_PIN_4
#define GPIO2_GPIO_Port GPIOC
#define GPIO1_Pin GPIO_PIN_5
#define GPIO1_GPIO_Port GPIOC
#define LD1_Pin GPIO_PIN_0
#define LD1_GPIO_Port GPIOB
#define DRIVE1_CH3_EN_Pin GPIO_PIN_1
#define DRIVE1_CH3_EN_GPIO_Port GPIOB
#define SET_PWR_ON_OFF_Pin GPIO_PIN_2
#define SET_PWR_ON_OFF_GPIO_Port GPIOB
#define PRESS4_EN_Pin GPIO_PIN_11
#define PRESS4_EN_GPIO_Port GPIOF
#define MOTOR4_EN_Pin GPIO_PIN_12
#define MOTOR4_EN_GPIO_Port GPIOF
#define MOTOR3_STEP_Pin GPIO_PIN_13
#define MOTOR3_STEP_GPIO_Port GPIOF
#define PRESS3_EN_Pin GPIO_PIN_15
#define PRESS3_EN_GPIO_Port GPIOF
#define LTC4421_PWR_nCH1_Pin GPIO_PIN_0
#define LTC4421_PWR_nCH1_GPIO_Port GPIOG
#define LTC4421_PWR_nCH2_Pin GPIO_PIN_1
#define LTC4421_PWR_nCH2_GPIO_Port GPIOG
#define SET_LOCK_Pin GPIO_PIN_7
#define SET_LOCK_GPIO_Port GPIOE
#define GPIO3_Pin GPIO_PIN_8
#define GPIO3_GPIO_Port GPIOE
#define SENSE_O2_EN_Pin GPIO_PIN_9
#define SENSE_O2_EN_GPIO_Port GPIOE
#define MOTOR2_EN_Pin GPIO_PIN_10
#define MOTOR2_EN_GPIO_Port GPIOE
#define PRESS1_EN_Pin GPIO_PIN_11
#define PRESS1_EN_GPIO_Port GPIOE
#define MOTOR2_DIR_Pin GPIO_PIN_12
#define MOTOR2_DIR_GPIO_Port GPIOE
#define MOTOR3_DIR_Pin GPIO_PIN_13
#define MOTOR3_DIR_GPIO_Port GPIOE
#define MOTOR2_STEP_Pin GPIO_PIN_14
#define MOTOR2_STEP_GPIO_Port GPIOE
#define MOTOR3_EN_Pin GPIO_PIN_15
#define MOTOR3_EN_GPIO_Port GPIOE
#define ALARM1_LOW_Pin GPIO_PIN_13
#define ALARM1_LOW_GPIO_Port GPIOB
#define SER_IN_Pin GPIO_PIN_14
#define SER_IN_GPIO_Port GPIOB
#define DRIVE2_CH7_EN_Pin GPIO_PIN_15
#define DRIVE2_CH7_EN_GPIO_Port GPIOB
#define STLINK_RX_Pin GPIO_PIN_8
#define STLINK_RX_GPIO_Port GPIOD
#define STLINK_TX_Pin GPIO_PIN_9
#define STLINK_TX_GPIO_Port GPIOD
#define MOTOR4_STEP_Pin GPIO_PIN_10
#define MOTOR4_STEP_GPIO_Port GPIOD
#define MOTOR1_STEP_Pin GPIO_PIN_11
#define MOTOR1_STEP_GPIO_Port GPIOD
#define MOTOR1_DIR_Pin GPIO_PIN_12
#define MOTOR1_DIR_GPIO_Port GPIOD
#define DRIVE2_CH1_EN_Pin GPIO_PIN_14
#define DRIVE2_CH1_EN_GPIO_Port GPIOD
#define DRIVE2_CH2_EN_Pin GPIO_PIN_15
#define DRIVE2_CH2_EN_GPIO_Port GPIOD
#define SET_ALARM_EN_Pin GPIO_PIN_2
#define SET_ALARM_EN_GPIO_Port GPIOG
#define SET_100_O2_Pin GPIO_PIN_3
#define SET_100_O2_GPIO_Port GPIOG
#define PRESS_VDD_EN_Pin GPIO_PIN_4
#define PRESS_VDD_EN_GPIO_Port GPIOG
#define I2C1_RESET_Pin GPIO_PIN_5
#define I2C1_RESET_GPIO_Port GPIOG
#define I2C2_RESET_Pin GPIO_PIN_6
#define I2C2_RESET_GPIO_Port GPIOG
#define MOTOR1_EN_Pin GPIO_PIN_7
#define MOTOR1_EN_GPIO_Port GPIOG
#define PRESS6_EN_Pin GPIO_PIN_8
#define PRESS6_EN_GPIO_Port GPIOG
#define DRIVE2_CH4_EN_Pin GPIO_PIN_6
#define DRIVE2_CH4_EN_GPIO_Port GPIOC
#define DRIVE2_CH5_EN_Pin GPIO_PIN_7
#define DRIVE2_CH5_EN_GPIO_Port GPIOC
#define DRIVE1_CH6_EN_Pin GPIO_PIN_8
#define DRIVE1_CH6_EN_GPIO_Port GPIOC
#define DRIVE2_CH6_EN_Pin GPIO_PIN_9
#define DRIVE2_CH6_EN_GPIO_Port GPIOC
#define CPU_UART_CLK_Pin GPIO_PIN_8
#define CPU_UART_CLK_GPIO_Port GPIOA
#define CPU_UART_RX_Pin GPIO_PIN_10
#define CPU_UART_RX_GPIO_Port GPIOA
#define CPU_UART_CTS_Pin GPIO_PIN_11
#define CPU_UART_CTS_GPIO_Port GPIOA
#define CPU_UART_RTS_Pin GPIO_PIN_12
#define CPU_UART_RTS_GPIO_Port GPIOA
#define BUZZ1_EN_Pin GPIO_PIN_15
#define BUZZ1_EN_GPIO_Port GPIOA
#define SER_CLK_Pin GPIO_PIN_10
#define SER_CLK_GPIO_Port GPIOC
#define SER_CLR_N_Pin GPIO_PIN_11
#define SER_CLR_N_GPIO_Port GPIOC
#define SER_RCLK_Pin GPIO_PIN_12
#define SER_RCLK_GPIO_Port GPIOC
#define OEMIII_RXPin_Pin GPIO_PIN_0
#define OEMIII_RXPin_GPIO_Port GPIOD
#define MOTOR1_UART_Pin GPIO_PIN_1
#define MOTOR1_UART_GPIO_Port GPIOD
#define LTC4421_PWR_nVALID1_Pin GPIO_PIN_4
#define LTC4421_PWR_nVALID1_GPIO_Port GPIOD
#define LTC4421_PWR_nVALID2_Pin GPIO_PIN_5
#define LTC4421_PWR_nVALID2_GPIO_Port GPIOD
#define LTC4421_PWR_nDISABLE1_Pin GPIO_PIN_6
#define LTC4421_PWR_nDISABLE1_GPIO_Port GPIOD
#define LTC4421_PWR_nDISABLE2_Pin GPIO_PIN_7
#define LTC4421_PWR_nDISABLE2_GPIO_Port GPIOD
#define LED3_EN_Pin GPIO_PIN_10
#define LED3_EN_GPIO_Port GPIOG
#define ALARM1_HIGH_Pin GPIO_PIN_12
#define ALARM1_HIGH_GPIO_Port GPIOG
#define PRESSX_EN_Pin GPIO_PIN_13
#define PRESSX_EN_GPIO_Port GPIOG
#define MOTOR4_DIR_Pin GPIO_PIN_14
#define MOTOR4_DIR_GPIO_Port GPIOG
#define LED2_EN_Pin GPIO_PIN_15
#define LED2_EN_GPIO_Port GPIOG
#define DRIVE1_CH2_EN_Pin GPIO_PIN_3
#define DRIVE1_CH2_EN_GPIO_Port GPIOB
#define DRIVE1_CH4_EN_Pin GPIO_PIN_4
#define DRIVE1_CH4_EN_GPIO_Port GPIOB
#define DRIVE1_CH5_EN_Pin GPIO_PIN_5
#define DRIVE1_CH5_EN_GPIO_Port GPIOB
#define CPU_UART_TX_Pin GPIO_PIN_6
#define CPU_UART_TX_GPIO_Port GPIOB
#define DRIVE1_CH7_EN_Pin GPIO_PIN_7
#define DRIVE1_CH7_EN_GPIO_Port GPIOB
#define PRESS5_EN_Pin GPIO_PIN_0
#define PRESS5_EN_GPIO_Port GPIOE
#define MOTOR4_UART_Pin GPIO_PIN_1
#define MOTOR4_UART_GPIO_Port GPIOE
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
