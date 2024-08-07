/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stdio.h"
#include "string.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define en_0 GPIOC->BRR  = (1<<7)   // PC7 OFF
#define en_1 GPIOC->BSRR = (1<<7)   // PC7 ON

#define rs_0 GPIOA->BRR  = (1<<9)   // PA9 OFF
#define rs_1 GPIOA->BSRR = (1<<9)   // PA9 ON

#define d4_0 GPIOB->BRR  = (1<<5)   // PB5 OFF
#define d4_1 GPIOB->BSRR = (1<<5)   // PB5 ON

#define d5_0 GPIOB->BRR  = (1<<4)   // PB4 OFF
#define d5_1 GPIOB->BSRR = (1<<4)   // PB4 ON

#define d6_0 GPIOB->BRR  = (1<<10)  // PB10 OFF
#define d6_1 GPIOB->BSRR = (1<<10)  // PB10 ON

#define d7_0 GPIOA->BRR  = (1<<8)   // PA8 OFF
#define d7_1 GPIOA->BSRR = (1<<8)   // PA8 ON

#define bl_off GPIOB->BRR  = (1<<6)  // PB6 ON
#define bl_on GPIOB->BSRR = (1<<6) // PB6 OFF

#define cursor_off 0x0c
#define cursor_on 0x0e
#define cursor_blink 0x0f

#define no_lcd 1
#define na_serial 2

char aonde = no_lcd;
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc;

UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_ADC_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void udelay(){
	int tempo = 7;
	while(tempo--);
}

void delayus(int tempo){
	while(tempo--) udelay();
}

void lcd_wrcom4(uint8_t com4){
	if((com4 & (1<<0))== 0) d4_0; else d4_1;
	if((com4 & (1<<1))== 0) d5_0; else d5_1;
	if((com4 & (1<<2))== 0) d6_0; else d6_1;
	if((com4 & (1<<3))== 0) d7_0; else d7_1;

	rs_0;
	en_1;
	delayus(5);
	en_0;
	HAL_Delay(5);
}

void lcd_wrcom(uint8_t com){
	if((com & (1<<4))== 0) d4_0; else d4_1;
	if((com & (1<<5))== 0) d5_0; else d5_1;
	if((com & (1<<6))== 0) d6_0; else d6_1;
	if((com & (1<<7))== 0) d7_0; else d7_1;

	rs_0;
	en_1;
	delayus(5);
	en_0;
	HAL_Delay(5);

	if((com & (1<<0))== 0) d4_0; else d4_1;
	if((com & (1<<1))== 0) d5_0; else d5_1;
	if((com & (1<<2))== 0) d6_0; else d6_1;
	if((com & (1<<3))== 0) d7_0; else d7_1;

	rs_0;
	en_1;
	delayus(5);
	en_0;
	HAL_Delay(5);
}

void lcd_init(uint8_t cursor){
	lcd_wrcom4(3);
	lcd_wrcom4(3);
	lcd_wrcom4(3);
	lcd_wrcom4(2);

	lcd_wrcom(0x28);
	lcd_wrcom(cursor);
	lcd_wrcom(0x06);
	lcd_wrcom(0x01);
}

void lcd_wrchar(uint8_t com){
	if((com & (1<<4))== 0) d4_0; else d4_1;
	if((com & (1<<5))== 0) d5_0; else d5_1;
	if((com & (1<<6))== 0) d6_0; else d6_1;
	if((com & (1<<7))== 0) d7_0; else d7_1;

	rs_1;
	en_1;
	delayus(5);
	en_0;
	HAL_Delay(5);

	if((com & (1<<0))== 0) d4_0; else d4_1;
	if((com & (1<<1))== 0) d5_0; else d5_1;
	if((com & (1<<2))== 0) d6_0; else d6_1;
	if((com & (1<<3))== 0) d7_0; else d7_1;

	rs_1;
	en_1;
	delayus(5);
	en_0;
	HAL_Delay(5);
}

void lcd_goto(uint8_t x, uint8_t y){
	uint8_t com = 0x80;
	if(y == 0) com = 0x80 + x;
	if(y == 1) com = 0xC0 + x;
	if(y == 2) com = 0x90 + x;
	if(y == 3) com = 0xD0 + x;
	lcd_wrcom(com);
}

void lcd_wrstr(char *str){
	while(*str){
		lcd_wrchar(*str);
		str++;
	}
}

int __io_putchar(int ch){
	if(aonde == no_lcd){
		if(ch != '\n') lcd_wrchar(ch);
	}
	if(aonde == na_serial){
		HAL_UART_Transmit(&huart2, (uint8_t *)&ch, 1, 10);
	}
	return ch;
}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
	int leitura, qtd,retorno, retorno2;
	float peso1, peso2;
	char resposta, peso_base,ch, ch2;
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
  MX_USART2_UART_Init();
  MX_ADC_Init();
  /* USER CODE BEGIN 2 */
  bl_on;
  lcd_init(cursor_off);
  lcd_goto(0,0);
  lcd_wrstr("Peso(kg): \0");
  lcd_goto(0,1);
  lcd_wrstr("Qtd: \0");
  HAL_ADC_Init(&hadc);

  aonde = na_serial;
  printf("Deseja iniciar a leitura? S/N \n");

  do{
	  retorno = HAL_UART_Receive(&huart2, (uint8_t *)&ch, 1, 2);
  } while(retorno != HAL_UART_ERROR_NONE);

  HAL_ADC_Init(&hadc);
  HAL_ADC_Start(&hadc);
  HAL_ADC_PollForConversion(&hadc, 1);
  leitura = HAL_ADC_GetValue(&hadc);
  peso1 = 0.0122100122 * leitura;
  aonde = no_lcd;
  lcd_goto(10,0);
  printf("%.3f\n", peso1);

  aonde = na_serial;
  printf("Peso do objeto base registrado: %.3f kg\n", peso1);
  printf("Por favor, adicione mais itens à balança e pressione 'S' para medir novamente.\n");

  do{
    retorno2 = HAL_UART_Receive(&huart2, (uint8_t *)&ch2, 1, 10000);
  } while(retorno2 != HAL_UART_ERROR_NONE);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

	  HAL_ADC_Start(&hadc);
	  HAL_ADC_PollForConversion(&hadc, 1);
	  leitura = HAL_ADC_GetValue(&hadc);
	  HAL_ADC_Stop(&hadc);
	  peso2 = 0.0122100122 * leitura;

	  aonde = no_lcd;
	  lcd_goto(10,0);
	  printf("%.3f\n", peso2);
	  aonde = na_serial;
	  printf("O PESO É:%.3f\n", peso2);
	  qtd = peso2/peso1;
	  printf("A QUANTIDADE É:%d\n",qtd);
	  aonde = no_lcd;
	  lcd_goto(5,1);
	  printf("%3d\n", qtd);
	  HAL_Delay(500);

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
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI14|RCC_OSCILLATORTYPE_HSI48;
  RCC_OscInitStruct.HSI48State = RCC_HSI48_ON;
  RCC_OscInitStruct.HSI14State = RCC_HSI14_ON;
  RCC_OscInitStruct.HSI14CalibrationValue = 16;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI48;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART2;
  PeriphClkInit.Usart2ClockSelection = RCC_USART2CLKSOURCE_PCLK1;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief ADC Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC_Init(void)
{

  /* USER CODE BEGIN ADC_Init 0 */

  /* USER CODE END ADC_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC_Init 1 */

  /* USER CODE END ADC_Init 1 */

  /** Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion)
  */
  hadc.Instance = ADC1;
  hadc.Init.ClockPrescaler = ADC_CLOCK_ASYNC_DIV1;
  hadc.Init.Resolution = ADC_RESOLUTION_12B;
  hadc.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc.Init.ScanConvMode = ADC_SCAN_DIRECTION_FORWARD;
  hadc.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  hadc.Init.LowPowerAutoWait = DISABLE;
  hadc.Init.LowPowerAutoPowerOff = DISABLE;
  hadc.Init.ContinuousConvMode = DISABLE;
  hadc.Init.DiscontinuousConvMode = DISABLE;
  hadc.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc.Init.DMAContinuousRequests = DISABLE;
  hadc.Init.Overrun = ADC_OVR_DATA_PRESERVED;
  if (HAL_ADC_Init(&hadc) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure for the selected ADC regular channel to be converted.
  */
  sConfig.Channel = ADC_CHANNEL_1;
  sConfig.Rank = ADC_RANK_CHANNEL_NUMBER;
  sConfig.SamplingTime = ADC_SAMPLETIME_1CYCLE_5;
  if (HAL_ADC_ConfigChannel(&hadc, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC_Init 2 */

  /* USER CODE END ADC_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  huart2.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart2.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5|GPIO_PIN_8|GPIO_PIN_9, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_10|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_RESET);

  /*Configure GPIO pin : PC13 */
  GPIO_InitStruct.Pin = GPIO_PIN_13;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : PA5 PA8 PA9 */
  GPIO_InitStruct.Pin = GPIO_PIN_5|GPIO_PIN_8|GPIO_PIN_9;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : PB10 PB4 PB5 PB6 */
  GPIO_InitStruct.Pin = GPIO_PIN_10|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : PC7 */
  GPIO_InitStruct.Pin = GPIO_PIN_7;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

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
  __disable_irq();
  while (1)
  {
  }
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
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
