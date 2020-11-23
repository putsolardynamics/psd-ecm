/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : ECM Module software
 * @author			: Szymon Kacperek
 * @date			: 14/08/2020
 ******************************************************************************
 * Code created by me is signed in longer * arrays. Naming is adapted to Google C/C++ Style Guide.
 *
 * NOTES
 * @ 9/09 CAN is set on different pins than on .sch. Set for test purposes.
 *
 ******************************************************************************
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "can.h"
#include "tim.h"
#include "usb_device.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/************************************************************************************************
 PRIVATE INCLUDES
 ************************************************************************************************/
#include "canopen_object_dict.h"
#include "usbd_cdc_if.h"

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

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/************************************************************************************************
 PRIVATE VARIABLES
 ************************************************************************************************/
uint8_t velocity = 0;
int counter = 0;
int increasing = 1;
int decreasing = 0;

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

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
  MX_CAN1_Init();
  MX_CAN2_Init();
  MX_TIM10_Init();
  MX_USB_DEVICE_Init();
  /* USER CODE BEGIN 2 */

	/************************************************************************************************
	 CAN PERIPHERIAL AND ITS COMPONENTS
	 ************************************************************************************************/
	CanopenObjectDictInit();
	CanConfigFilter(CAN_HIGH_SPEED, 0, 0x0000, 0x0000, 0x0000, 0x0000);
	CanInit(CAN_HIGH_SPEED);
//	CanInit(CAN_LOW_SPEED);
	HAL_TIM_Base_Start_IT(&htim10);

	/************************************************************************************************
	 TURNING ON THE MODULES
	 ************************************************************************************************/
	CanSendNmt(CAN_HIGH_SPEED, OPERATIONAL_STATE, bms.node_id,
			&can_frame_template);
	CanSendNmt(CAN_HIGH_SPEED, OPERATIONAL_STATE, inverter_1.node_id,
			&can_frame_template);
	CanSendNmt(CAN_HIGH_SPEED, OPERATIONAL_STATE, inverter_2.node_id,
			&can_frame_template);
	CanSendNmt(CAN_HIGH_SPEED, OPERATIONAL_STATE, mppt_1.node_id,
			&can_frame_template);
	CanSendNmt(CAN_HIGH_SPEED, OPERATIONAL_STATE, mppt_2.node_id,
			&can_frame_template);
	CanSendNmt(CAN_HIGH_SPEED, OPERATIONAL_STATE, mppt_3.node_id,
			&can_frame_template);
	CanSendNmt(CAN_HIGH_SPEED, OPERATIONAL_STATE, lights_controller.node_id,
			&can_frame_template);
	CanSendNmt(CAN_HIGH_SPEED, OPERATIONAL_STATE, dashboard.node_id,
			&can_frame_template);
	/************************************************************************************************
	 USB
	 ************************************************************************************************/

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	while (1) {
		counter++;
		if (counter % 20 == 0) {

			if (increasing == 1) {
				velocity++;
				if (velocity >= 0x8C) {
					increasing = 0;
					decreasing = 1;
				}

			}

			if (decreasing == 1) {
				velocity--;
				if (velocity <= 0x00) {
					decreasing = 0;
					increasing = 1;
				}
			}

			CanSendTpdo(CAN_HIGH_SPEED, inverter_1.node_id, velocity, &can_frame_template);
			HAL_Delay(117);

		}

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

	}
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 168;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 7;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
	/* User can add his own implementation to report the HAL error return state */

  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
