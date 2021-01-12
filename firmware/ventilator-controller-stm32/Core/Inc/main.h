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
#define MOTOR1_DIR_Pin GPIO_PIN_3
#define MOTOR1_DIR_GPIO_Port GPIOE
#define VSYS_5V0_PGOOD_Pin GPIO_PIN_4
#define VSYS_5V0_PGOOD_GPIO_Port GPIOE
#define BLOWER_CTRL_EN_Pin GPIO_PIN_5
#define BLOWER_CTRL_EN_GPIO_Port GPIOE
#define LED1_EN_Pin GPIO_PIN_6
#define LED1_EN_GPIO_Port GPIOE
#define BLOWER_STATUS_Pin GPIO_PIN_13
#define BLOWER_STATUS_GPIO_Port GPIOC
#define BLOWER_SPI_CTRL_nCSC_Pin GPIO_PIN_0
#define BLOWER_SPI_CTRL_nCSC_GPIO_Port GPIOF
#define BLOWER_SPI_DRV_nCS_Pin GPIO_PIN_1
#define BLOWER_SPI_DRV_nCS_GPIO_Port GPIOF
#define STM32_SHDN_INT_Pin GPIO_PIN_2
#define STM32_SHDN_INT_GPIO_Port GPIOF
#define ADC_O2_Leak_Detect_Pin GPIO_PIN_3
#define ADC_O2_Leak_Detect_GPIO_Port GPIOF
#define TEMP4_Pin GPIO_PIN_4
#define TEMP4_GPIO_Port GPIOF
#define TEMP2_Pin GPIO_PIN_5
#define TEMP2_GPIO_Port GPIOF
#define BUZZ1_EN_Pin GPIO_PIN_6
#define BUZZ1_EN_GPIO_Port GPIOF
#define CO2_UART_TX_Pin GPIO_PIN_7
#define CO2_UART_TX_GPIO_Port GPIOF
#define ALARM1_SENSE_Pin GPIO_PIN_8
#define ALARM1_SENSE_GPIO_Port GPIOF
#define TEMP1_Pin GPIO_PIN_9
#define TEMP1_GPIO_Port GPIOF
#define TEMP3_Pin GPIO_PIN_10
#define TEMP3_GPIO_Port GPIOF
#define DRIVE_SENSE_4_Pin GPIO_PIN_0
#define DRIVE_SENSE_4_GPIO_Port GPIOC
#define ADC_FiO2_Pin GPIO_PIN_3
#define ADC_FiO2_GPIO_Port GPIOC
#define MOTOR1_CS_Pin GPIO_PIN_0
#define MOTOR1_CS_GPIO_Port GPIOA
#define ALARM1_MED_Pin GPIO_PIN_1
#define ALARM1_MED_GPIO_Port GPIOA
#define MOTOR2_CS_Pin GPIO_PIN_2
#define MOTOR2_CS_GPIO_Port GPIOA
#define LD1_Pin GPIO_PIN_0
#define LD1_GPIO_Port GPIOB
#define DRIVE1_CH4_EN_Pin GPIO_PIN_1
#define DRIVE1_CH4_EN_GPIO_Port GPIOB
#define SET_PWR_ON_OFF_Pin GPIO_PIN_2
#define SET_PWR_ON_OFF_GPIO_Port GPIOB
#define DRIVE_SENSE_1_Pin GPIO_PIN_11
#define DRIVE_SENSE_1_GPIO_Port GPIOF
#define DRIVE_SENSE_2_Pin GPIO_PIN_12
#define DRIVE_SENSE_2_GPIO_Port GPIOF
#define EXT_MEM_PROTECT_Pin GPIO_PIN_13
#define EXT_MEM_PROTECT_GPIO_Port GPIOF
#define STM32_nKILL_Pin GPIO_PIN_0
#define STM32_nKILL_GPIO_Port GPIOG
#define SOLENOID_DRIVE_8_Pin GPIO_PIN_1
#define SOLENOID_DRIVE_8_GPIO_Port GPIOG
#define SET_LOCK_Pin GPIO_PIN_7
#define SET_LOCK_GPIO_Port GPIOE
#define I2C4_MUX_S1_Pin GPIO_PIN_9
#define I2C4_MUX_S1_GPIO_Port GPIOE
#define MOTOR2_EN_Pin GPIO_PIN_10
#define MOTOR2_EN_GPIO_Port GPIOE
#define I2C4_MUX_S2_Pin GPIO_PIN_11
#define I2C4_MUX_S2_GPIO_Port GPIOE
#define MOTOR2_DIR_Pin GPIO_PIN_12
#define MOTOR2_DIR_GPIO_Port GPIOE
#define I2C4_MUX_EN_N_Pin GPIO_PIN_13
#define I2C4_MUX_EN_N_GPIO_Port GPIOE
#define MOTOR2_STEP_Pin GPIO_PIN_14
#define MOTOR2_STEP_GPIO_Port GPIOE
#define I2C1_MUX_EN_N_Pin GPIO_PIN_15
#define I2C1_MUX_EN_N_GPIO_Port GPIOE
#define I2C2_SMBALSERT_Pin GPIO_PIN_12
#define I2C2_SMBALSERT_GPIO_Port GPIOB
#define BLOWER_SPI_CLK_Pin GPIO_PIN_13
#define BLOWER_SPI_CLK_GPIO_Port GPIOB
#define BLOWER_SPI_MISO_Pin GPIO_PIN_14
#define BLOWER_SPI_MISO_GPIO_Port GPIOB
#define BLOWER_SPI_MOSI_Pin GPIO_PIN_15
#define BLOWER_SPI_MOSI_GPIO_Port GPIOB
#define STLINK_RX_Pin GPIO_PIN_8
#define STLINK_RX_GPIO_Port GPIOD
#define STLINK_TX_Pin GPIO_PIN_9
#define STLINK_TX_GPIO_Port GPIOD
#define MOTOR1_STEP_Pin GPIO_PIN_10
#define MOTOR1_STEP_GPIO_Port GPIOD
#define I2C4_SMBALSERT_Pin GPIO_PIN_11
#define I2C4_SMBALSERT_GPIO_Port GPIOD
#define DRIVE1_CH5_EN_Pin GPIO_PIN_12
#define DRIVE1_CH5_EN_GPIO_Port GPIOD
#define DRIVE1_CH6_EN_Pin GPIO_PIN_13
#define DRIVE1_CH6_EN_GPIO_Port GPIOD
#define DRIVE1_CH7_EN_Pin GPIO_PIN_14
#define DRIVE1_CH7_EN_GPIO_Port GPIOD
#define I2C2_MUX_S2_Pin GPIO_PIN_15
#define I2C2_MUX_S2_GPIO_Port GPIOD
#define SET_ALARM_EN_Pin GPIO_PIN_2
#define SET_ALARM_EN_GPIO_Port GPIOG
#define SET_100_O2_Pin GPIO_PIN_3
#define SET_100_O2_GPIO_Port GPIOG
#define I2C2_MUX_S1_Pin GPIO_PIN_4
#define I2C2_MUX_S1_GPIO_Port GPIOG
#define BMP_CS_Pin GPIO_PIN_5
#define BMP_CS_GPIO_Port GPIOG
#define EXT_MEM_HOLD_Pin GPIO_PIN_6
#define EXT_MEM_HOLD_GPIO_Port GPIOG
#define MOTOR1_EN_Pin GPIO_PIN_7
#define MOTOR1_EN_GPIO_Port GPIOG
#define I2C1_MUX_S1_Pin GPIO_PIN_8
#define I2C1_MUX_S1_GPIO_Port GPIOG
#define DRIVE1_CH1_EN_Pin GPIO_PIN_6
#define DRIVE1_CH1_EN_GPIO_Port GPIOC
#define I2C1_MUX_S2_Pin GPIO_PIN_7
#define I2C1_MUX_S2_GPIO_Port GPIOC
#define DRIVE1_CH3_EN_Pin GPIO_PIN_8
#define DRIVE1_CH3_EN_GPIO_Port GPIOC
#define CO2_UART_RX_Pin GPIO_PIN_8
#define CO2_UART_RX_GPIO_Port GPIOA
#define UART_FDO2_TX_Pin GPIO_PIN_9
#define UART_FDO2_TX_GPIO_Port GPIOA
#define CPU_UART_RX_Pin GPIO_PIN_10
#define CPU_UART_RX_GPIO_Port GPIOA
#define OEM_UART_RX_Pin GPIO_PIN_11
#define OEM_UART_RX_GPIO_Port GPIOA
#define EXT_MEM_CS_Pin GPIO_PIN_12
#define EXT_MEM_CS_GPIO_Port GPIOA
#define JTAG_TMS_Pin GPIO_PIN_13
#define JTAG_TMS_GPIO_Port GPIOA
#define JTAG_TCLK_Pin GPIO_PIN_14
#define JTAG_TCLK_GPIO_Port GPIOA
#define JTAG_TDI_Pin GPIO_PIN_15
#define JTAG_TDI_GPIO_Port GPIOA
#define SER_CLK_Pin GPIO_PIN_10
#define SER_CLK_GPIO_Port GPIOC
#define SER_CLR_N_Pin GPIO_PIN_11
#define SER_CLR_N_GPIO_Port GPIOC
#define SER_RCLK_Pin GPIO_PIN_12
#define SER_RCLK_GPIO_Port GPIOC
#define SET_WINDOW_Pin GPIO_PIN_0
#define SET_WINDOW_GPIO_Port GPIOD
#define OEM_UART_TX_Pin GPIO_PIN_1
#define OEM_UART_TX_GPIO_Port GPIOD
#define SER_RCLKD2_Pin GPIO_PIN_2
#define SER_RCLKD2_GPIO_Port GPIOD
#define SPI1_CS5_Pin GPIO_PIN_3
#define SPI1_CS5_GPIO_Port GPIOD
#define SET_NEBULIZATION_Pin GPIO_PIN_4
#define SET_NEBULIZATION_GPIO_Port GPIOD
#define ALARM1_LOW_Pin GPIO_PIN_5
#define ALARM1_LOW_GPIO_Port GPIOD
#define SER_IN_Pin GPIO_PIN_6
#define SER_IN_GPIO_Port GPIOD
#define LTC4421_PWR_nDISABLE2_Pin GPIO_PIN_7
#define LTC4421_PWR_nDISABLE2_GPIO_Port GPIOD
#define OEM_UART_EN_Pin GPIO_PIN_9
#define OEM_UART_EN_GPIO_Port GPIOG
#define LED3_EN_Pin GPIO_PIN_10
#define LED3_EN_GPIO_Port GPIOG
#define WD_RST_Pin GPIO_PIN_11
#define WD_RST_GPIO_Port GPIOG
#define ALARM1_HIGH_Pin GPIO_PIN_12
#define ALARM1_HIGH_GPIO_Port GPIOG
#define PRESSX_EN_Pin GPIO_PIN_13
#define PRESSX_EN_GPIO_Port GPIOG
#define MOTOR4_DIR_Pin GPIO_PIN_14
#define MOTOR4_DIR_GPIO_Port GPIOG
#define LED2_EN_Pin GPIO_PIN_15
#define LED2_EN_GPIO_Port GPIOG
#define JTAG_TDO_Pin GPIO_PIN_3
#define JTAG_TDO_GPIO_Port GPIOB
#define JTAG_RST_Pin GPIO_PIN_4
#define JTAG_RST_GPIO_Port GPIOB
#define DRIVE1_CH2_EN_Pin GPIO_PIN_5
#define DRIVE1_CH2_EN_GPIO_Port GPIOB
#define CPU_UART_TX_Pin GPIO_PIN_6
#define CPU_UART_TX_GPIO_Port GPIOB
#define UART_FDO2_RX_Pin GPIO_PIN_7
#define UART_FDO2_RX_GPIO_Port GPIOB
#define EXTRA_UART_RX_Pin GPIO_PIN_0
#define EXTRA_UART_RX_GPIO_Port GPIOE
#define EXTRA_UART_TX_Pin GPIO_PIN_1
#define EXTRA_UART_TX_GPIO_Port GPIOE
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
