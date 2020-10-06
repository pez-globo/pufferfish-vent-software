/* USER CODE BEGIN Header */
/**
 * Original work Copyright 2020, STMicroelectronics
 * Modified work Copyright 2020, the Pez Globo team and the Pufferfish project
 *contributors
 *
 ******************************************************************************
 * @file           : main.cpp
 * @brief          : Main program body
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

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <array>

#include "Pufferfish/AlarmsManager.h"
#include "Pufferfish/Driver/Button/Button.h"
#include "Pufferfish/Driver/I2C/ExtendedI2CDevice.h"
#include "Pufferfish/Driver/I2C/HoneywellABP.h"
#include "Pufferfish/Driver/I2C/SDP.h"
#include "Pufferfish/Driver/I2C/SFM3000.h"
#include "Pufferfish/Driver/I2C/TCA9548A.h"
#include "Pufferfish/Driver/Indicators/AuditoryAlarm.h"
#include "Pufferfish/Driver/Indicators/LEDAlarm.h"
#include "Pufferfish/Driver/Serial/Nonin/NoninOEM3.h"
#include "Pufferfish/Driver/ShiftedOutput.h"
#include "Pufferfish/HAL/HAL.h"
#include "Pufferfish/HAL/STM32/BufferedUART.h"
#include "Pufferfish/HAL/STM32/HAL.h"
#include "Pufferfish/HAL/STM32/HALI2CDevice.h"
#include "Pufferfish/Statuses.h"
#include "Pufferfish/Driver/SPI/BMP388.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc3;

CRC_HandleTypeDef hcrc;

I2C_HandleTypeDef hi2c1;
I2C_HandleTypeDef hi2c2;
I2C_HandleTypeDef hi2c4;

SPI_HandleTypeDef hspi1;

TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim3;
TIM_HandleTypeDef htim4;
TIM_HandleTypeDef htim5;
TIM_HandleTypeDef htim8;
TIM_HandleTypeDef htim12;

UART_HandleTypeDef huart4;
UART_HandleTypeDef huart7;
UART_HandleTypeDef huart8;
UART_HandleTypeDef huart1;
UART_HandleTypeDef huart3;

/* USER CODE BEGIN PV */

/* Timeout for the Adc poll conversion */
static const uint32_t adc_poll_timeout = 10;

namespace PF = Pufferfish;

/* NoninOEM TODO: Creating an object for UART for Nonin OEM interface */
volatile PF::Driver::Serial::Nonin::NoninOEMUART oem_uart(huart4);
/* NoninOEM TODO: Creating an object for NoninOEM */
PF::Driver::Serial::Nonin::NoninOEM oemobj(oem_uart);
/* NoninOEM TODO: Packet measurements */
PF::Driver::Serial::Nonin::PacketMeasurements test_sensor_measurements;
/* NoninOEM TODO: status byte error */
PF::Driver::Serial::Nonin::StatusByteError frame_error_status;

/* Create an object for ADC3 of AnalogInput Class */
PF::HAL::HALAnalogInput adc3_input(hadc3, adc_poll_timeout);

// The following lines suppress Eclipse CDT's warning about C-style casts;
// those come from STM32CubeMX-generated #define constants, which we have no
// control over

// Interface Board
PF::HAL::HALDigitalOutput ser_clock(
    *SER_CLK_GPIO_Port,  // @suppress("C-Style cast instead of C++ cast") // NOLINT(cppcoreguidelines-pro-type-cstyle-cast)
    SER_CLK_Pin,
    true);
PF::HAL::HALDigitalOutput ser_clear(
    *SER_CLR_N_GPIO_Port,  // @suppress("C-Style cast instead of C++ cast") // NOLINT(cppcoreguidelines-pro-type-cstyle-cast)
    SER_CLR_N_Pin,
    false);
PF::HAL::HALDigitalOutput ser_r_clock(
    *SER_RCLK_GPIO_Port,  // @suppress("C-Style cast instead of C++ cast") // NOLINT(cppcoreguidelines-pro-type-cstyle-cast)
    SER_RCLK_Pin,
    true);
PF::HAL::HALDigitalOutput ser_input(
    *SER_IN_GPIO_Port,  // @suppress("C-Style cast instead of C++ cast") // NOLINT(cppcoreguidelines-pro-type-cstyle-cast)
    SER_IN_Pin,
    true);

PF::HAL::HALDigitalOutput board_led1(
    *LD1_GPIO_Port,  // @suppress("C-Style cast instead of C++ cast") // NOLINT(cppcoreguidelines-pro-type-cstyle-cast)
    LD1_Pin);  // @suppress("C-Style cast instead of C++ cast") // NOLINT(cppcoreguidelines-pro-type-cstyle-cast)
PF::Driver::ShiftRegister leds_reg(ser_input, ser_clock, ser_r_clock, ser_clear);

PF::Driver::ShiftedOutput alarm_led_r(leds_reg, 0);
PF::Driver::ShiftedOutput alarm_led_g(leds_reg, 1);
PF::Driver::ShiftedOutput alarm_led_b(leds_reg, 2);
PF::Driver::ShiftedOutput led_alarm_en(leds_reg, 3);
PF::Driver::ShiftedOutput led_full_o2(leds_reg, 4);
// NOLINTNEXTLINE(readability-magic-numbers)
PF::Driver::ShiftedOutput led_manual_breath(leds_reg, 5);
// NOLINTNEXTLINE(readability-magic-numbers)
PF::Driver::ShiftedOutput led_lock(leds_reg, 6);

PF::HAL::HALDigitalOutput alarm_reg_high(
    *ALARM1_HIGH_GPIO_Port,  // @suppress("C-Style cast instead of C++ cast") // NOLINT(cppcoreguidelines-pro-type-cstyle-cast)
    ALARM1_HIGH_Pin);  // @suppress("C-Style cast instead of C++ cast") // NOLINT(cppcoreguidelines-pro-type-cstyle-cast)
PF::HAL::HALDigitalOutput alarm_reg_med(
    *ALARM1_MED_GPIO_Port,  // @suppress("C-Style cast instead of C++ cast") // NOLINT(cppcoreguidelines-pro-type-cstyle-cast)
    ALARM1_MED_Pin);  // @suppress("C-Style cast instead of C++ cast") // NOLINT(cppcoreguidelines-pro-type-cstyle-cast)
PF::HAL::HALDigitalOutput alarm_reg_low(
    *ALARM1_LOW_GPIO_Port,  // @suppress("C-Style cast instead of C++ cast") // NOLINT(cppcoreguidelines-pro-type-cstyle-cast)
    ALARM1_LOW_Pin);  // @suppress("C-Style cast instead of C++ cast") // NOLINT(cppcoreguidelines-pro-type-cstyle-cast)
PF::HAL::HALDigitalOutput alarm_buzzer(
    *BUZZ1_EN_GPIO_Port,  // @suppress("C-Style cast instead of C++ cast") // NOLINT(cppcoreguidelines-pro-type-cstyle-cast)
    BUZZ1_EN_Pin);  // @suppress("C-Style cast instead of C++ cast") // NOLINT(cppcoreguidelines-pro-type-cstyle-cast)
/*BMP388 */
PF::HAL::HALDigitalOutput bmp388ChipSelect(*BMP388_CS_GPIO_Port, BMP388_CS_Pin);
PF::HAL::HALSPIDevice spi1Bmp388(hspi1, bmp388ChipSelect);
PF::Driver::SPI::BMP388 barometric(spi1Bmp388);

PF::Driver::Indicators::LEDAlarm alarm_dev_led(alarm_led_r, alarm_led_g, alarm_led_b);
PF::Driver::Indicators::AuditoryAlarm alarm_dev_sound(
    alarm_reg_high, alarm_reg_med, alarm_reg_low, alarm_buzzer);
PF::AlarmsManager h_alarms(alarm_dev_led, alarm_dev_sound);

PF::HAL::HALDigitalInput button_alarm_en(
    *SET_ALARM_EN_GPIO_Port,  // @suppress("C-Style cast instead of C++ cast") // NOLINT(cppcoreguidelines-pro-type-cstyle-cast)
    SET_ALARM_EN_Pin,
    true);
PF::HAL::HALDigitalInput button_full_o2(
    *SET_100_O2_GPIO_Port,  // @suppress("C-Style cast instead of C++ cast") // NOLINT(cppcoreguidelines-pro-type-cstyle-cast)
    SET_100_O2_Pin,
    true);
PF::HAL::HALDigitalInput button_manual_breath(
    *SET_MANUAL_BREATH_GPIO_Port,  // @suppress("C-Style cast instead of C++ cast") // NOLINT(cppcoreguidelines-pro-type-cstyle-cast)
    SET_MANUAL_BREATH_Pin,
    true);
PF::HAL::HALDigitalInput button_lock(
    *SET_LOCK_GPIO_Port,  // @suppress("C-Style cast instead of C++ cast") // NOLINT(cppcoreguidelines-pro-type-cstyle-cast)
    SET_LOCK_Pin,
    true);
PF::HAL::HALDigitalInput button_power(
    *SET_PWR_ON_OFF_GPIO_Port,  // @suppress("C-Style cast instead of C++ cast") // NOLINT(cppcoreguidelines-pro-type-cstyle-cast)
    SET_PWR_ON_OFF_Pin,
    true);

PF::Driver::Button::Debouncer switch_debounce;
PF::Driver::Button::EdgeDetector switch_transition;
PF::Driver::Button::Button button_membrane(button_alarm_en, switch_debounce);

// Solenoid Valves
PF::HAL::HALPWM drive1_ch1(htim2, TIM_CHANNEL_4);
PF::HAL::HALPWM drive1_ch2(htim2, TIM_CHANNEL_2);
PF::HAL::HALPWM drive1_ch3(htim3, TIM_CHANNEL_4);
PF::HAL::HALPWM drive1_ch4(htim3, TIM_CHANNEL_1);
PF::HAL::HALPWM drive1_ch5(htim3, TIM_CHANNEL_2);
PF::HAL::HALPWM drive1_ch6(htim3, TIM_CHANNEL_3);
PF::HAL::HALPWM drive1_ch7(htim4, TIM_CHANNEL_2);
PF::HAL::HALPWM drive2_ch1(htim4, TIM_CHANNEL_3);
PF::HAL::HALPWM drive2_ch2(htim4, TIM_CHANNEL_4);
PF::HAL::HALPWM drive2_ch3(htim5, TIM_CHANNEL_1);
PF::HAL::HALPWM drive2_ch4(htim8, TIM_CHANNEL_1);
PF::HAL::HALPWM drive2_ch5(htim8, TIM_CHANNEL_2);
PF::HAL::HALPWM drive2_ch6(htim8, TIM_CHANNEL_4);
PF::HAL::HALPWM drive2_ch7(htim12, TIM_CHANNEL_2);

// Base I2C Devices
// Note: I2C1 is marked I2C2 in the control board v1.0 schematic, and vice versa
PF::HAL::HALI2CDevice i2c_hal_mux1(hi2c2, PF::Driver::I2C::TCA9548A::default_i2c_addr);
PF::HAL::HALI2CDevice i2c_hal_mux2(hi2c1, PF::Driver::I2C::TCA9548A::default_i2c_addr);

PF::HAL::HALI2CDevice i2c_hal_press1(hi2c1, PF::Driver::I2C::abpxxxx001pg2a3.i2c_addr);
PF::HAL::HALI2CDevice i2c_hal_press2(hi2c1, PF::Driver::I2C::abpxxxx001pg2a3.i2c_addr);
PF::HAL::HALI2CDevice i2c_hal_press3(hi2c1, PF::Driver::I2C::abpxxxx001pg2a3.i2c_addr);
PF::HAL::HALI2CDevice i2c_hal_press7(hi2c1, PF::Driver::I2C::abpxxxx030pg2a3.i2c_addr);
PF::HAL::HALI2CDevice i2c_hal_press8(hi2c1, PF::Driver::I2C::abpxxxx030pg2a3.i2c_addr);
PF::HAL::HALI2CDevice i2c_hal_press9(hi2c1, PF::Driver::I2C::abpxxxx001pg2a3.i2c_addr);
PF::HAL::HALI2CDevice i2c_hal_press13(hi2c2, PF::Driver::I2C::SDPSensor::sdp8xx_i2c_addr);
PF::HAL::HALI2CDevice i2c_hal_press14(hi2c2, PF::Driver::I2C::SDPSensor::sdp3x_i2c_addr);
PF::HAL::HALI2CDevice i2c_hal_press15(hi2c2, PF::Driver::I2C::SDPSensor::sdp3x_i2c_addr);
PF::HAL::HALI2CDevice i2c_hal_press16(hi2c2, PF::Driver::I2C::SFM3000::default_i2c_addr);
PF::HAL::HALI2CDevice i2c_hal_press17(hi2c2, PF::Driver::I2C::SDPSensor::sdp3x_i2c_addr);
PF::HAL::HALI2CDevice i2c_hal_press18(hi2c2, PF::Driver::I2C::SDPSensor::sdp3x_i2c_addr);

// I2C Mux
PF::Driver::I2C::TCA9548A i2c_mux1(i2c_hal_mux1);
PF::Driver::I2C::TCA9548A i2c_mux2(i2c_hal_mux2);

// Extended I2C Device
PF::Driver::I2C::ExtendedI2CDevice i2c_ext_press1(i2c_hal_press1, i2c_mux2, 0);
PF::Driver::I2C::ExtendedI2CDevice i2c_ext_press2(i2c_hal_press2, i2c_mux2, 2);
PF::Driver::I2C::ExtendedI2CDevice i2c_ext_press3(i2c_hal_press3, i2c_mux2, 4);
PF::Driver::I2C::ExtendedI2CDevice i2c_ext_press7(i2c_hal_press7, i2c_mux2, 1);
PF::Driver::I2C::ExtendedI2CDevice i2c_ext_press8(i2c_hal_press8, i2c_mux2, 3);
// NOLINTNEXTLINE(readability-magic-numbers)
PF::Driver::I2C::ExtendedI2CDevice i2c_ext_press9(i2c_hal_press9, i2c_mux2, 5);

PF::Driver::I2C::ExtendedI2CDevice i2c_ext_press13(i2c_hal_press13, i2c_mux1, 0);
PF::Driver::I2C::ExtendedI2CDevice i2c_ext_press14(i2c_hal_press14, i2c_mux1, 2);
PF::Driver::I2C::ExtendedI2CDevice i2c_ext_press15(i2c_hal_press15, i2c_mux1, 4);
PF::Driver::I2C::ExtendedI2CDevice i2c_ext_press16(i2c_hal_press16, i2c_mux1, 1);
PF::Driver::I2C::ExtendedI2CDevice i2c_ext_press17(i2c_hal_press17, i2c_mux1, 3);
PF::Driver::I2C::ExtendedI2CDevice i2c_ext_press18(
    i2c_hal_press18,
    i2c_mux1,
    // NOLINTNEXTLINE(readability-magic-numbers)
    5);

// Actual usable sensor
PF::Driver::I2C::HoneywellABP i2c_press1(i2c_ext_press1, PF::Driver::I2C::abpxxxx001pg2a3);
PF::Driver::I2C::HoneywellABP i2c_press2(i2c_ext_press2, PF::Driver::I2C::abpxxxx001pg2a3);
PF::Driver::I2C::HoneywellABP i2c_press3(i2c_ext_press3, PF::Driver::I2C::abpxxxx001pg2a3);
PF::Driver::I2C::HoneywellABP i2c_press7(i2c_ext_press7, PF::Driver::I2C::abpxxxx030pg2a3);
PF::Driver::I2C::HoneywellABP i2c_press8(i2c_ext_press8, PF::Driver::I2C::abpxxxx030pg2a3);
PF::Driver::I2C::HoneywellABP i2c_press9(i2c_ext_press9, PF::Driver::I2C::abpxxxx001pg2a3);
PF::Driver::I2C::SDPSensor i2c_press13(i2c_ext_press13);
PF::Driver::I2C::SDPSensor i2c_press14(i2c_ext_press14);
PF::Driver::I2C::SDPSensor i2c_press15(i2c_ext_press15);
PF::Driver::I2C::SFM3000 i2c_press16(i2c_ext_press16);
PF::Driver::I2C::SDPSensor i2c_press17(i2c_ext_press17);
PF::Driver::I2C::SDPSensor i2c_press18(i2c_ext_press18);

// Buffered UARTs
volatile Pufferfish::HAL::LargeBufferedUART buffered_uart3(huart3);

// Test list
// NOLINTNEXTLINE(readability-magic-numbers)
std::array<PF::Driver::Testable *, 14> i2c_test_list{
    {&i2c_mux1,
     &i2c_mux2,
     &i2c_press1,
     &i2c_press2,
     &i2c_press3,
     &i2c_press7,
     &i2c_press8,
     &i2c_press9,
     &i2c_press13,
     &i2c_press14,
     &i2c_press15,
     &i2c_press16,
     &i2c_press17,
     &i2c_press18}};

int interface_test_state = 0;
int interface_test_millis = 0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_SPI1_Init(void);
static void MX_I2C1_Init(void);
static void MX_UART4_Init(void);
static void MX_ADC3_Init(void);
static void MX_CRC_Init(void);
static void MX_I2C2_Init(void);
static void MX_TIM2_Init(void);
static void MX_UART7_Init(void);
static void MX_UART8_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_USART3_UART_Init(void);
static void MX_I2C4_Init(void);
static void MX_TIM3_Init(void);
static void MX_TIM4_Init(void);
static void MX_TIM5_Init(void);
static void MX_TIM8_Init(void);
static void MX_TIM12_Init(void);
static void Bmp388_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void interface_test_loop() {
  // get state of buttons
  bool l_alarm_en = button_alarm_en.read();
  bool l_o2 = button_full_o2.read();
  bool l_manual = button_manual_breath.read();
  bool l_lock = button_lock.read();
  // bool l_power = button_power.read();

  // simply write back
  led_alarm_en.write(l_alarm_en);
  led_full_o2.write(l_o2);
  led_manual_breath.write(l_manual);
  led_lock.write(l_lock);

  // cycle though alarms
  //  if (!l_power) {
  //    hAlarms.clearAll();
  //  } else if (PF::HAL::millis() - interface_test_millis > 100) {
  //    hAlarms.add(PF::AlarmStatus::highPriority);
  //    interface_test_millis = PF::HAL::millis();
  //    if (interface_test_state) {
  //      interface_test_state--;
  //      hAlarms.add(static_cast<PF::AlarmStatus>(interface_test_state));
  //    } else {
  //      interface_test_state = static_cast<int>(PF::AlarmStatus::noAlarm);
  //      hAlarms.clearAll();
  //    }
  //  }
}
/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void) {
  /* USER CODE BEGIN 1 */

  /*
   * FIXME: Added for testing
   * Local variable to read ADC3 input
   */
  uint32_t adc3_data = 0;

  /* Nonin TODO: Local variable to count packets of data received */
  uint32_t packet_count = 0;
  /* Nonin TODO */
  bool data_ready = false;
  double temp;
  double pres;
  PF::Driver::SPI::TrimValues values;
  uint32_t current_time = 0;
  /* Nonin TODO */
  std::array<uint32_t, 4> testcase_results = {0U};

  PF::Driver::Button::EdgeState state;
  bool mem_buttonstate = false;
  /* TODO: Added for testing Nonin OEM III */
  PF::Driver::Serial::Nonin::NoninOEM::NoninPacketStatus return_status;

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_SPI1_Init();
  MX_I2C1_Init();
  MX_UART4_Init();
  MX_ADC3_Init();
  MX_CRC_Init();
  MX_I2C2_Init();
  MX_TIM2_Init();
  MX_UART7_Init();
  MX_UART8_Init();
  MX_USART1_UART_Init();
  MX_USART3_UART_Init();
  MX_I2C4_Init();
  MX_TIM3_Init();
  MX_TIM4_Init();
  MX_TIM5_Init();
  MX_TIM8_Init();
  MX_TIM12_Init();
  Bmp388_Init();
  /* USER CODE BEGIN 2 */
  PF::HAL::micros_delay_init();
  interface_test_millis = PF::HAL::millis();
  /* Nonin TODO: setupIRQ of BufferredUART for setting the UART reception */
  oem_uart.setup_irq();

  /* Start the ADC3 by invoking AnalogInput::Start() */
  adc3_input.start();

  buffered_uart3.setup_irq();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  static const uint32_t blink_low_delay = 5;
  static const uint32_t loop_delay = 50;
  while (true) {
    /* Nonin TODO: Invoking the NoninOEM output method */
    return_status = oemobj.output(test_sensor_measurements);
    if (return_status == PF::Driver::Serial::Nonin::NoninOEM::NoninPacketStatus::available) {
      packet_count = packet_count + 1;

      /// Nonin TODO: Test Scenario 1 On sensor disconnected from Nonin OEM III
      /// module
      if (packet_count == 1) {
        testcase_results[0] = static_cast<uint32_t>(test_sensor_measurements.sensor_disconnect[0]);
      }

      /// Nonin TODO: Test Scenario 2 On sensor connected to Nonin OEM III
      /// module and no contact with  finger clip sensor
      if (packet_count == 1) {
        testcase_results[1] = static_cast<uint32_t>(test_sensor_measurements.sensor_alarm[0]);
      }
      /// Nonin TODO: Test Scenario 3 Time validation for 15 frames is 5 seconds
      if (packet_count == 1) {
        current_time = PF::HAL::millis();
      }
      /// Nonin TODO: define magic numbers in meaningful variable names
      // NOLINTNEXTLINE(readability-magic-numbers)
      if (packet_count == 16) {
        current_time = PF::HAL::millis() - current_time;
        /* Validate time for 5000 milli-seconds */
        testcase_results[2] =
            /// Nonin TODO: define magic numbers in meaningful variable names
            // NOLINTNEXTLINE(readability-magic-numbers)
            static_cast<uint32_t>(current_time >= 5000 && current_time < 5100);
      }
    }
    /* Nonin TODO : Added to resolve warnings */
    testcase_results[3] = static_cast<uint32_t>(static_cast<bool>(testcase_results[2]));

    PF::AlarmManagerStatus stat = h_alarms.update(PF::HAL::millis());
    if (stat != PF::AlarmManagerStatus::ok) {
      Error_Handler();
    }
    board_led1.write(false);
    PF::HAL::delay(blink_low_delay);
    board_led1.write(true);
    interface_test_loop();
    leds_reg.update();

    for (PF::Driver::Testable *t : i2c_test_list) {
      if (t->test() != PF::I2CDeviceStatus::ok) {
        board_led1.write(false);
      }
    }
    PF::HAL::delay(loop_delay);

    uint8_t receive = 0;
    while (buffered_uart3.read(receive) == PF::BufferStatus::ok) {
      board_led1.write(true);
      buffered_uart3.write(receive);
      PF::HAL::AtomicSize written_size = 0;
      std::array<uint8_t, 2> repeat_string = {receive, receive};
      buffered_uart3.write(repeat_string.data(), repeat_string.size(), written_size);
    }
    /* USER CODE END WHILE */

    /**
     * FIXME: Added for testing 
     * Read the Analog data of ADC3 and validate the return value
     */
    if (adc3_input.read(adc3_data) != PF::ADCStatus::ok) {
      /* Error Handle */
    } else {
      /* Else statements*/
    }
    button_membrane.read_state(mem_buttonstate, state);
    if (state != PF::Driver::Button::EdgeState::rising_edge) {
      board_led1.write(true);
      PF::HAL::delay(5);
		
  }
    /* USER CODE BEGIN 3 */
    barometric.get_data_ready_interrupt(data_ready);
    if (data_ready) {
      barometric.read_calibration_data(values);
      barometric.read_compensate_temperature(temp);
    }
    if ((temp < -45) && (temp > 85)) {
      board_led1.write(true);
    }
    barometric.read_compensate_pressure(pres);
    if (pres < 300 && pres > 1100) {
      board_led1.write(true);
    }
  }
  /* USER CODE END 3 */
}

/**
 * @brief BMP388 device configuration
 * @retval None
 */
static void Bmp388_Init(void) {
  uint8_t flash_chip_id;
  /* SPI Flash memory ID */
  barometric.get_chip_id(flash_chip_id);
  if (flash_chip_id != 0x50) {
    board_led1.write(true);
  }
  barometric.enable_spi_communication(PF::Driver::SPI::SPIInterfaceType::spi4);
  barometric.enable_pressure(PF::Driver::SPI::RegisterSet::enable);
  barometric.enable_temperature(PF::Driver::SPI::RegisterSet::enable);
  barometric.select_power_mode(PF::Driver::SPI::PowerModes::normal);
  barometric.interrupt_pin_output_type(PF::Driver::SPI::RegisterSet::disable);
  barometric.pressure_over_sampling(PF::Driver::SPI::Oversampling::standard_resolution);
  barometric.temperature_over_sampling(PF::Driver::SPI::Oversampling::standard_resolution);
  barometric.output_data_rate(PF::Driver::SPI::TimeStandby::time80ms);
  barometric.set_iiR_filter(PF::Driver::SPI::FilterCoefficient::coef0);
}
/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void) {
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

  /** Supply configuration update enable
   * */
  HAL_PWREx_ConfigSupply(PWR_LDO_SUPPLY);
  /** Configure the main internal regulator output voltage 
   * */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);

  while (!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {}
  /** Macro to configure the PLL clock source 
   * */
  __HAL_RCC_PLL_PLLSOURCE_CONFIG(RCC_PLLSOURCE_HSE);
  /** Initializes the CPU, AHB and APB busses clocks 
  * */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI | RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_BYPASS;
  RCC_OscInitStruct.HSIState = RCC_HSI_DIV1;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 1;
  RCC_OscInitStruct.PLL.PLLN = 24;
  RCC_OscInitStruct.PLL.PLLP = 2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  RCC_OscInitStruct.PLL.PLLR = 2;
  RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1VCIRANGE_3;
  RCC_OscInitStruct.PLL.PLLVCOSEL = RCC_PLL1VCOWIDE;
  RCC_OscInitStruct.PLL.PLLFRACN = 0;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB busses clocks 
  * */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2
                              |RCC_CLOCKTYPE_D3PCLK1|RCC_CLOCKTYPE_D1PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB3CLKDivider = RCC_APB3_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV1;
  RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK) {
    Error_Handler();
  }
  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_USART3 | RCC_PERIPHCLK_UART4
      | RCC_PERIPHCLK_UART7 | RCC_PERIPHCLK_USART1
      | RCC_PERIPHCLK_UART8 | RCC_PERIPHCLK_SPI1
      | RCC_PERIPHCLK_I2C2 | RCC_PERIPHCLK_ADC
      | RCC_PERIPHCLK_I2C1 | RCC_PERIPHCLK_I2C4;
  PeriphClkInitStruct.PLL2.PLL2M = 1;
  PeriphClkInitStruct.PLL2.PLL2N = 19;
  PeriphClkInitStruct.PLL2.PLL2P = 3;
  PeriphClkInitStruct.PLL2.PLL2Q = 2;
  PeriphClkInitStruct.PLL2.PLL2R = 2;
  PeriphClkInitStruct.PLL2.PLL2RGE = RCC_PLL2VCIRANGE_3;
  PeriphClkInitStruct.PLL2.PLL2VCOSEL = RCC_PLL2VCOMEDIUM;
  PeriphClkInitStruct.PLL2.PLL2FRACN = 0;
  PeriphClkInitStruct.Spi123ClockSelection = RCC_SPI123CLKSOURCE_PLL;
  PeriphClkInitStruct.Usart234578ClockSelection = RCC_USART234578CLKSOURCE_D2PCLK1;
  PeriphClkInitStruct.Usart16ClockSelection = RCC_USART16CLKSOURCE_D2PCLK2;
  PeriphClkInitStruct.I2c123ClockSelection = RCC_I2C123CLKSOURCE_D2PCLK1;
  PeriphClkInitStruct.I2c4ClockSelection = RCC_I2C4CLKSOURCE_D3PCLK1;
  PeriphClkInitStruct.AdcClockSelection = RCC_ADCCLKSOURCE_PLL2;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
 * @brief ADC3 Initialization Function
 * @param None
 * @retval None
 */
static void MX_ADC3_Init(void) {
  /* USER CODE BEGIN ADC3_Init 0 */

  /* USER CODE END ADC3_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC3_Init 1 */

  /* USER CODE END ADC3_Init 1 */
  /** Common config 
  */
  hadc3.Instance = ADC3;
  hadc3.Init.ClockPrescaler = ADC_CLOCK_ASYNC_DIV1;
  hadc3.Init.Resolution = ADC_RESOLUTION_16B;
  hadc3.Init.ScanConvMode = ADC_SCAN_DISABLE;
  hadc3.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  hadc3.Init.LowPowerAutoWait = DISABLE;
  hadc3.Init.ContinuousConvMode = ENABLE;
  hadc3.Init.NbrOfConversion = 1;
  hadc3.Init.DiscontinuousConvMode = DISABLE;
  hadc3.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc3.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc3.Init.ConversionDataManagement = ADC_CONVERSIONDATA_DR;
  hadc3.Init.Overrun = ADC_OVR_DATA_OVERWRITTEN;
  hadc3.Init.LeftBitShift = ADC_LEFTBITSHIFT_NONE;
  hadc3.Init.OversamplingMode = DISABLE;
  if (HAL_ADC_Init(&hadc3) != HAL_OK) {
    Error_Handler();
  }
  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_7;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SamplingTime = ADC_SAMPLETIME_1CYCLE_5;
  sConfig.SingleDiff = ADC_SINGLE_ENDED;
  sConfig.OffsetNumber = ADC_OFFSET_NONE;
  sConfig.Offset = 0;
  if (HAL_ADC_ConfigChannel(&hadc3, &sConfig) != HAL_OK) {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC3_Init 2 */

  /* USER CODE END ADC3_Init 2 */
}

/**
 * @brief CRC Initialization Function
 * @param None
 * @retval None
 */
static void MX_CRC_Init(void) {
  /* USER CODE BEGIN CRC_Init 0 */

  /* USER CODE END CRC_Init 0 */

  /* USER CODE BEGIN CRC_Init 1 */

  /* USER CODE END CRC_Init 1 */
  hcrc.Instance = CRC;
  hcrc.Init.DefaultPolynomialUse = DEFAULT_POLYNOMIAL_DISABLE;
  hcrc.Init.DefaultInitValueUse = DEFAULT_INIT_VALUE_ENABLE;
  hcrc.Init.GeneratingPolynomial = 517762881;
  hcrc.Init.CRCLength = CRC_POLYLENGTH_32B;
  hcrc.Init.InputDataInversionMode = CRC_INPUTDATA_INVERSION_BYTE;
  hcrc.Init.OutputDataInversionMode = CRC_OUTPUTDATA_INVERSION_ENABLE;
  hcrc.InputDataFormat = CRC_INPUTDATA_FORMAT_BYTES;
  if (HAL_CRC_Init(&hcrc) != HAL_OK) {
    Error_Handler();
  }
  /* USER CODE BEGIN CRC_Init 2 */

  /* USER CODE END CRC_Init 2 */
}

/**
 * @brief I2C1 Initialization Function
 * @param None
 * @retval None
 */
static void MX_I2C1_Init(void) {
  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.Timing = 0x00602173;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK) {
    Error_Handler();
  }
  /** Configure Analogue filter 
  */
  if (HAL_I2CEx_ConfigAnalogFilter(&hi2c1, I2C_ANALOGFILTER_ENABLE) != HAL_OK) {
    Error_Handler();
  }
  /** Configure Digital filter 
  */
  if (HAL_I2CEx_ConfigDigitalFilter(&hi2c1, 0) != HAL_OK) {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */
}

/**
 * @brief I2C2 Initialization Function
 * @param None
 * @retval None
 */
static void MX_I2C2_Init(void) {
  /* USER CODE BEGIN I2C2_Init 0 */

  /* USER CODE END I2C2_Init 0 */

  /* USER CODE BEGIN I2C2_Init 1 */

  /* USER CODE END I2C2_Init 1 */
  hi2c2.Instance = I2C2;
  hi2c2.Init.Timing = 0x10707DBC;
  hi2c2.Init.OwnAddress1 = 0;
  hi2c2.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c2.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c2.Init.OwnAddress2 = 0;
  hi2c2.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
  hi2c2.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c2.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c2) != HAL_OK) {
    Error_Handler();
  }
  /** Configure Analogue filter 
  */
  if (HAL_I2CEx_ConfigAnalogFilter(&hi2c2, I2C_ANALOGFILTER_ENABLE) != HAL_OK) {
    Error_Handler();
  }
  /** Configure Digital filter 
  */
  if (HAL_I2CEx_ConfigDigitalFilter(&hi2c2, 0) != HAL_OK) {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C2_Init 2 */

  /* USER CODE END I2C2_Init 2 */
}

/**
 * @brief I2C4 Initialization Function
 * @param None
 * @retval None
 */
static void MX_I2C4_Init(void) {
  /* USER CODE BEGIN I2C4_Init 0 */

  /* USER CODE END I2C4_Init 0 */

  /* USER CODE BEGIN I2C4_Init 1 */

  /* USER CODE END I2C4_Init 1 */
  hi2c4.Instance = I2C4;
  hi2c4.Init.Timing = 0x10707DBC;
  hi2c4.Init.OwnAddress1 = 0;
  hi2c4.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c4.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c4.Init.OwnAddress2 = 0;
  hi2c4.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
  hi2c4.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c4.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c4) != HAL_OK) {
    Error_Handler();
  }
  /** Configure Analogue filter 
  */
  if (HAL_I2CEx_ConfigAnalogFilter(&hi2c4, I2C_ANALOGFILTER_ENABLE) != HAL_OK) {
    Error_Handler();
  }
  /** Configure Digital filter 
  */
  if (HAL_I2CEx_ConfigDigitalFilter(&hi2c4, 0) != HAL_OK) {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C4_Init 2 */

  /* USER CODE END I2C4_Init 2 */
}

/**
 * @brief SPI1 Initialization Function
 * @param None
 * @retval None
 */
static void MX_SPI1_Init(void) {
  /* USER CODE BEGIN SPI1_Init 0 */

  /* USER CODE END SPI1_Init 0 */

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_16;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 0x0;
  hspi1.Init.NSSPMode = SPI_NSS_PULSE_ENABLE;
  hspi1.Init.NSSPolarity = SPI_NSS_POLARITY_LOW;
  hspi1.Init.FifoThreshold = SPI_FIFO_THRESHOLD_01DATA;
  hspi1.Init.TxCRCInitializationPattern = SPI_CRC_INITIALIZATION_ALL_ZERO_PATTERN;
  hspi1.Init.RxCRCInitializationPattern = SPI_CRC_INITIALIZATION_ALL_ZERO_PATTERN;
  hspi1.Init.MasterSSIdleness = SPI_MASTER_SS_IDLENESS_00CYCLE;
  hspi1.Init.MasterInterDataIdleness = SPI_MASTER_INTERDATA_IDLENESS_00CYCLE;
  hspi1.Init.MasterReceiverAutoSusp = SPI_MASTER_RX_AUTOSUSP_DISABLE;
  hspi1.Init.MasterKeepIOState = SPI_MASTER_KEEP_IO_STATE_DISABLE;
  hspi1.Init.IOSwap = SPI_IO_SWAP_DISABLE;
  if (HAL_SPI_Init(&hspi1) != HAL_OK) {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */
}

/**
 * @brief TIM2 Initialization Function
 * @param None
 * @retval None
 */
static void MX_TIM2_Init(void) {
  /* USER CODE BEGIN TIM2_Init 0 */

  /* USER CODE END TIM2_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 0;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 0;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK) {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK) {
    Error_Handler();
  }
  if (HAL_TIM_PWM_Init(&htim2) != HAL_OK) {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK) {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_2) != HAL_OK) {
    Error_Handler();
  }
  if (HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_4) != HAL_OK) {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */
  HAL_TIM_MspPostInit(&htim2);
}

/**
 * @brief TIM3 Initialization Function
 * @param None
 * @retval None
 */
static void MX_TIM3_Init(void) {
  /* USER CODE BEGIN TIM3_Init 0 */

  /* USER CODE END TIM3_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM3_Init 1 */

  /* USER CODE END TIM3_Init 1 */
  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 0;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 0;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_PWM_Init(&htim3) != HAL_OK) {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK) {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_1) != HAL_OK) {
    Error_Handler();
  }
  if (HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_2) != HAL_OK) {
    Error_Handler();
  }
  if (HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_3) != HAL_OK) {
    Error_Handler();
  }
  if (HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_4) != HAL_OK) {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM3_Init 2 */

  /* USER CODE END TIM3_Init 2 */
  HAL_TIM_MspPostInit(&htim3);
}

/**
 * @brief TIM4 Initialization Function
 * @param None
 * @retval None
 */
static void MX_TIM4_Init(void) {
  /* USER CODE BEGIN TIM4_Init 0 */

  /* USER CODE END TIM4_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM4_Init 1 */

  /* USER CODE END TIM4_Init 1 */
  htim4.Instance = TIM4;
  htim4.Init.Prescaler = 0;
  htim4.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim4.Init.Period = 0;
  htim4.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim4.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_PWM_Init(&htim4) != HAL_OK) {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim4, &sMasterConfig) != HAL_OK) {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim4, &sConfigOC, TIM_CHANNEL_2) != HAL_OK) {
    Error_Handler();
  }
  if (HAL_TIM_PWM_ConfigChannel(&htim4, &sConfigOC, TIM_CHANNEL_3) != HAL_OK) {
    Error_Handler();
  }
  if (HAL_TIM_PWM_ConfigChannel(&htim4, &sConfigOC, TIM_CHANNEL_4) != HAL_OK) {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM4_Init 2 */

  /* USER CODE END TIM4_Init 2 */
  HAL_TIM_MspPostInit(&htim4);
}

/**
 * @brief TIM5 Initialization Function
 * @param None
 * @retval None
 */
static void MX_TIM5_Init(void) {
  /* USER CODE BEGIN TIM5_Init 0 */

  /* USER CODE END TIM5_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM5_Init 1 */

  /* USER CODE END TIM5_Init 1 */
  htim5.Instance = TIM5;
  htim5.Init.Prescaler = 0;
  htim5.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim5.Init.Period = 0;
  htim5.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim5.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_PWM_Init(&htim5) != HAL_OK) {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim5, &sMasterConfig) != HAL_OK) {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim5, &sConfigOC, TIM_CHANNEL_1) != HAL_OK) {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM5_Init 2 */

  /* USER CODE END TIM5_Init 2 */
  HAL_TIM_MspPostInit(&htim5);
}

/**
 * @brief TIM8 Initialization Function
 * @param None
 * @retval None
 */
static void MX_TIM8_Init(void) {
  /* USER CODE BEGIN TIM8_Init 0 */

  /* USER CODE END TIM8_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};
  TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig = {0};

  /* USER CODE BEGIN TIM8_Init 1 */

  /* USER CODE END TIM8_Init 1 */
  htim8.Instance = TIM8;
  htim8.Init.Prescaler = 0;
  htim8.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim8.Init.Period = 0;
  htim8.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim8.Init.RepetitionCounter = 0;
  htim8.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_PWM_Init(&htim8) != HAL_OK) {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterOutputTrigger2 = TIM_TRGO2_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim8, &sMasterConfig) != HAL_OK) {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
  sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
  if (HAL_TIM_PWM_ConfigChannel(&htim8, &sConfigOC, TIM_CHANNEL_1) != HAL_OK) {
    Error_Handler();
  }
  if (HAL_TIM_PWM_ConfigChannel(&htim8, &sConfigOC, TIM_CHANNEL_2) != HAL_OK) {
    Error_Handler();
  }
  if (HAL_TIM_PWM_ConfigChannel(&htim8, &sConfigOC, TIM_CHANNEL_4) != HAL_OK) {
    Error_Handler();
  }
  sBreakDeadTimeConfig.OffStateRunMode = TIM_OSSR_DISABLE;
  sBreakDeadTimeConfig.OffStateIDLEMode = TIM_OSSI_DISABLE;
  sBreakDeadTimeConfig.LockLevel = TIM_LOCKLEVEL_OFF;
  sBreakDeadTimeConfig.DeadTime = 0;
  sBreakDeadTimeConfig.BreakState = TIM_BREAK_DISABLE;
  sBreakDeadTimeConfig.BreakPolarity = TIM_BREAKPOLARITY_HIGH;
  sBreakDeadTimeConfig.BreakFilter = 0;
  sBreakDeadTimeConfig.Break2State = TIM_BREAK2_DISABLE;
  sBreakDeadTimeConfig.Break2Polarity = TIM_BREAK2POLARITY_HIGH;
  sBreakDeadTimeConfig.Break2Filter = 0;
  sBreakDeadTimeConfig.AutomaticOutput = TIM_AUTOMATICOUTPUT_DISABLE;
  if (HAL_TIMEx_ConfigBreakDeadTime(&htim8, &sBreakDeadTimeConfig) != HAL_OK) {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM8_Init 2 */

  /* USER CODE END TIM8_Init 2 */
  HAL_TIM_MspPostInit(&htim8);

}

/**
 * @brief TIM12 Initialization Function
 * @param None
 * @retval None
 */
static void MX_TIM12_Init(void)
{
  /* USER CODE BEGIN TIM12_Init 0 */

  /* USER CODE END TIM12_Init 0 */

  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM12_Init 1 */

  /* USER CODE END TIM12_Init 1 */
  htim12.Instance = TIM12;
  htim12.Init.Prescaler = 0;
  htim12.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim12.Init.Period = 0;
  htim12.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim12.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_PWM_Init(&htim12) != HAL_OK) {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim12, &sConfigOC, TIM_CHANNEL_2) != HAL_OK) {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM12_Init 2 */

  /* USER CODE END TIM12_Init 2 */
  HAL_TIM_MspPostInit(&htim12);
}

/**
 * @brief UART4 Initialization Function
 * @param None
 * @retval None
 */
static void MX_UART4_Init(void) {

  /* USER CODE BEGIN UART4_Init 0 */

  /* USER CODE END UART4_Init 0 */

  /* USER CODE BEGIN UART4_Init 1 */

  /* USER CODE END UART4_Init 1 */
  huart4.Instance = UART4;
  huart4.Init.BaudRate = 9600;
  huart4.Init.WordLength = UART_WORDLENGTH_8B;
  huart4.Init.StopBits = UART_STOPBITS_1;
  huart4.Init.Parity = UART_PARITY_NONE;
  huart4.Init.Mode = UART_MODE_TX_RX;
  huart4.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart4.Init.OverSampling = UART_OVERSAMPLING_16;
  huart4.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart4.Init.ClockPrescaler = UART_PRESCALER_DIV1;
  huart4.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart4) != HAL_OK) {
    Error_Handler();
  }
  if (HAL_UARTEx_SetTxFifoThreshold(&huart4, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK) {
    Error_Handler();
  }
  if (HAL_UARTEx_SetRxFifoThreshold(&huart4, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK) {
    Error_Handler();
  }
  if (HAL_UARTEx_DisableFifoMode(&huart4) != HAL_OK) {
    Error_Handler();
  }
  /* USER CODE BEGIN UART4_Init 2 */

  /* USER CODE END UART4_Init 2 */

}

/**
 * @brief UART7 Initialization Function
 * @param None
 * @retval None
 */
static void MX_UART7_Init(void)
{
  /* USER CODE BEGIN UART7_Init 0 */

  /* USER CODE END UART7_Init 0 */

  /* USER CODE BEGIN UART7_Init 1 */

  /* USER CODE END UART7_Init 1 */
  huart7.Instance = UART7;
  huart7.Init.BaudRate = 115200;
  huart7.Init.WordLength = UART_WORDLENGTH_8B;
  huart7.Init.StopBits = UART_STOPBITS_1;
  huart7.Init.Parity = UART_PARITY_NONE;
  huart7.Init.Mode = UART_MODE_TX_RX;
  huart7.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart7.Init.OverSampling = UART_OVERSAMPLING_16;
  huart7.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart7.Init.ClockPrescaler = UART_PRESCALER_DIV1;
  huart7.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_HalfDuplex_Init(&huart7) != HAL_OK) {
    Error_Handler();
  }
  if (HAL_UARTEx_SetTxFifoThreshold(&huart7, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK) {
    Error_Handler();
  }
  if (HAL_UARTEx_SetRxFifoThreshold(&huart7, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK) {
    Error_Handler();
  }
  if (HAL_UARTEx_DisableFifoMode(&huart7) != HAL_OK) {
    Error_Handler();
  }
  /* USER CODE BEGIN UART7_Init 2 */

  /* USER CODE END UART7_Init 2 */
}

/**
 * @brief UART8 Initialization Function
 * @param None
 * @retval None
 */
static void MX_UART8_Init(void)
{
  /* USER CODE BEGIN UART8_Init 0 */

  /* USER CODE END UART8_Init 0 */

  /* USER CODE BEGIN UART8_Init 1 */

  /* USER CODE END UART8_Init 1 */
  huart8.Instance = UART8;
  huart8.Init.BaudRate = 115200;
  huart8.Init.WordLength = UART_WORDLENGTH_8B;
  huart8.Init.StopBits = UART_STOPBITS_1;
  huart8.Init.Parity = UART_PARITY_NONE;
  huart8.Init.Mode = UART_MODE_TX_RX;
  huart8.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart8.Init.OverSampling = UART_OVERSAMPLING_16;
  huart8.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart8.Init.ClockPrescaler = UART_PRESCALER_DIV1;
  huart8.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_HalfDuplex_Init(&huart8) != HAL_OK) {
    Error_Handler();
  }
  if (HAL_UARTEx_SetTxFifoThreshold(&huart8, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK) {
    Error_Handler();
  }
  if (HAL_UARTEx_SetRxFifoThreshold(&huart8, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK) {
    Error_Handler();
  }
  if (HAL_UARTEx_DisableFifoMode(&huart8) != HAL_OK) {
    Error_Handler();
  }
  /* USER CODE BEGIN UART8_Init 2 */

  /* USER CODE END UART8_Init 2 */
}

/**
 * @brief USART1 Initialization Function
 * @param None
 * @retval None
 */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_RTS_CTS;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  huart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart1.Init.ClockPrescaler = UART_PRESCALER_DIV1;
  huart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart1) != HAL_OK) {
    Error_Handler();
  }
  if (HAL_UARTEx_SetTxFifoThreshold(&huart1, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK) {
    Error_Handler();
  }
  if (HAL_UARTEx_SetRxFifoThreshold(&huart1, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK) {
    Error_Handler();
  }
  if (HAL_UARTEx_DisableFifoMode(&huart1) != HAL_OK) {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */
}

/**
 * @brief USART3 Initialization Function
 * @param None
 * @retval None
 */
static void MX_USART3_UART_Init(void) {

  /* USER CODE BEGIN USART3_Init 0 */

  /* USER CODE END USART3_Init 0 */

  /* USER CODE BEGIN USART3_Init 1 */

  /* USER CODE END USART3_Init 1 */
  huart3.Instance = USART3;
  huart3.Init.BaudRate = 115200;
  huart3.Init.WordLength = UART_WORDLENGTH_8B;
  huart3.Init.StopBits = UART_STOPBITS_1;
  huart3.Init.Parity = UART_PARITY_NONE;
  huart3.Init.Mode = UART_MODE_TX_RX;
  huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart3.Init.OverSampling = UART_OVERSAMPLING_16;
  huart3.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart3.Init.ClockPrescaler = UART_PRESCALER_DIV1;
  huart3.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart3) != HAL_OK) {
    Error_Handler();
  }
  if (HAL_UARTEx_SetTxFifoThreshold(&huart3, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK) {
    Error_Handler();
  }
  if (HAL_UARTEx_SetRxFifoThreshold(&huart3, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK) {
    Error_Handler();
  }
  if (HAL_UARTEx_DisableFifoMode(&huart3) != HAL_OK) {
    Error_Handler();
  }
  /* USER CODE BEGIN USART3_Init 2 */

  /* USER CODE END USART3_Init 2 */

}

/**
 * @brief GPIO Initialization Function
 * @param None
 * @retval None
 */
static void MX_GPIO_Init(void) {
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOG_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOE, BAT_MEAS_EN_Pin | LED1_EN_Pin | GPIO3_Pin | SENSE_O2_EN_Pin
      | MOTOR2_EN_Pin | PRESS1_EN_Pin | MOTOR2_DIR_Pin | MOTOR3_DIR_Pin
      | MOTOR2_STEP_Pin | MOTOR3_EN_Pin | PRESS5_EN_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOF, BAT_CHARGE_EN_Pin|PRESS2_EN_Pin|BMP388_CS_Pin|PRESS4_EN_Pin
                          |MOTOR4_EN_Pin|MOTOR3_STEP_Pin|PRESS3_EN_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, ALARM1_MED_Pin | BUZZ1_EN_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, GPIO2_Pin | GPIO1_Pin | SER_CLK_Pin | SER_CLR_N_Pin
      | SER_RCLK_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, LD1_Pin | ALARM1_LOW_Pin | SER_IN_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOD, MOTOR4_STEP_Pin | MOTOR1_STEP_Pin | MOTOR1_DIR_Pin | LTC4421_PWR_nDISABLE1_Pin
      | LTC4421_PWR_nDISABLE2_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOG, PRESS_VDD_EN_Pin | I2C1_RESET_Pin | I2C2_RESET_Pin | MOTOR1_EN_Pin
      | PRESS6_EN_Pin | LED3_EN_Pin | ALARM1_HIGH_Pin | PRESSX_EN_Pin
      | MOTOR4_DIR_Pin | LED2_EN_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : SET_MANUAL_BREATH_Pin VSYS_3V3_PGOOD_Pin VSYS_5V0_PGOOD_Pin SET_LOCK_Pin */
  GPIO_InitStruct.Pin = SET_MANUAL_BREATH_Pin | VSYS_3V3_PGOOD_Pin | VSYS_5V0_PGOOD_Pin | SET_LOCK_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /*Configure GPIO pins : BAT_MEAS_EN_Pin LED1_EN_Pin GPIO3_Pin SENSE_O2_EN_Pin 
                           MOTOR2_EN_Pin PRESS1_EN_Pin MOTOR2_DIR_Pin MOTOR3_DIR_Pin 
                           MOTOR2_STEP_Pin MOTOR3_EN_Pin PRESS5_EN_Pin */
  GPIO_InitStruct.Pin = BAT_MEAS_EN_Pin | LED1_EN_Pin | GPIO3_Pin | SENSE_O2_EN_Pin
      | MOTOR2_EN_Pin | PRESS1_EN_Pin | MOTOR2_DIR_Pin | MOTOR3_DIR_Pin
      | MOTOR2_STEP_Pin | MOTOR3_EN_Pin | PRESS5_EN_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /*Configure GPIO pin : B1_Pin */
  GPIO_InitStruct.Pin = B1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : LTC4421_PWR_nFAULT2_Pin LTC4421_PWR_nFAULT1_Pin */
  GPIO_InitStruct.Pin = LTC4421_PWR_nFAULT2_Pin|LTC4421_PWR_nFAULT1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);

  /*Configure GPIO pins : BAT_CHARGE_EN_Pin PRESS2_EN_Pin BMP388_CS_Pin PRESS4_EN_Pin
                           MOTOR4_EN_Pin MOTOR3_STEP_Pin PRESS3_EN_Pin */
  GPIO_InitStruct.Pin = BAT_CHARGE_EN_Pin|PRESS2_EN_Pin|BMP388_CS_Pin|PRESS4_EN_Pin
                          |MOTOR4_EN_Pin|MOTOR3_STEP_Pin|PRESS3_EN_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);

  /*Configure GPIO pins : ALARM1_MED_Pin BUZZ1_EN_Pin */
  GPIO_InitStruct.Pin = ALARM1_MED_Pin | BUZZ1_EN_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : GPIO2_Pin GPIO1_Pin SER_CLK_Pin SER_CLR_N_Pin 
                           SER_RCLK_Pin */
  GPIO_InitStruct.Pin = GPIO2_Pin | GPIO1_Pin | SER_CLK_Pin | SER_CLR_N_Pin
      | SER_RCLK_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : LD1_Pin ALARM1_LOW_Pin SER_IN_Pin */
  GPIO_InitStruct.Pin = LD1_Pin | ALARM1_LOW_Pin | SER_IN_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : SET_PWR_ON_OFF_Pin */
  GPIO_InitStruct.Pin = SET_PWR_ON_OFF_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(SET_PWR_ON_OFF_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : LTC4421_PWR_nCH1_Pin LTC4421_PWR_nCH2_Pin SET_ALARM_EN_Pin SET_100_O2_Pin */
  GPIO_InitStruct.Pin = LTC4421_PWR_nCH1_Pin | LTC4421_PWR_nCH2_Pin | SET_ALARM_EN_Pin | SET_100_O2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

  /*Configure GPIO pins : MOTOR4_STEP_Pin MOTOR1_STEP_Pin MOTOR1_DIR_Pin LTC4421_PWR_nDISABLE1_Pin 
                           LTC4421_PWR_nDISABLE2_Pin */
  GPIO_InitStruct.Pin = MOTOR4_STEP_Pin | MOTOR1_STEP_Pin | MOTOR1_DIR_Pin | LTC4421_PWR_nDISABLE1_Pin
      | LTC4421_PWR_nDISABLE2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  /*Configure GPIO pins : PRESS_VDD_EN_Pin I2C1_RESET_Pin I2C2_RESET_Pin MOTOR1_EN_Pin 
                           PRESS6_EN_Pin LED3_EN_Pin ALARM1_HIGH_Pin PRESSX_EN_Pin 
                           MOTOR4_DIR_Pin LED2_EN_Pin */
  GPIO_InitStruct.Pin = PRESS_VDD_EN_Pin | I2C1_RESET_Pin | I2C2_RESET_Pin | MOTOR1_EN_Pin
      | PRESS6_EN_Pin | LED3_EN_Pin | ALARM1_HIGH_Pin | PRESSX_EN_Pin
      | MOTOR4_DIR_Pin | LED2_EN_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

  /*Configure GPIO pins : LTC4421_PWR_nVALID1_Pin LTC4421_PWR_nVALID2_Pin */
  GPIO_InitStruct.Pin = LTC4421_PWR_nVALID1_Pin | LTC4421_PWR_nVALID2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void) {
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

  /* USER CODE END Error_Handler_Debug */
}

#ifdef USE_FULL_ASSERT
/**
 * @brief  Reports the name of the source file and the source line number
 *         where the assert_param error has occurred.
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t *file, uint32_t line) {
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line
     number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line)
   */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
