/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2023 STMicroelectronics.
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
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c1;

RTC_HandleTypeDef hrtc;

UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_I2C1_Init(void);
static void MX_RTC_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
#define cursor_off 0x0c
#define cursor_on 0x0e
#define cursor_blink 0x0f

#define e_0 GPIOC->BRR=1<<7
#define e_1 GPIOC->BSRR=1<<7

#define rs_0 GPIOA->BRR=1<<9
#define rs_1 GPIOA->BSRR=1<<9

#define d4_0 GPIOB->BRR=1<<5
#define d4_1 GPIOB->BSRR=1<<5

#define d5_0 GPIOB->BRR=1<<4
#define d5_1 GPIOB->BSRR=1<<4

#define d6_0 GPIOB->BRR=1<<10
#define d6_1 GPIOB->BSRR=1<<10

#define d7_0 GPIOA->BRR=1<<8
#define d7_1 GPIOA->BSRR=1<<8

#define bl_on GPIOB->BSRR=(1<<6)
#define bl_off GPIOB->BRR=(1<<6)

#define celula_carga (GPIOC->IDR & (1<<13))

unsigned char tabChar[8]={0x00,0x06,0x09,0x09,0x06,0x00,0x00,0x00};

#define no_lcd 1
#define na_serial1 2
#define na_serial2 3

char onde = no_lcd;

void udelay(void){
int tempo=7;
while(tempo--);
}

void delayus(int tempo){
while(tempo--)udelay();
}

//ENVIA COMANDO DE 4 BITS
void lcd_wrcom4(uint8_t com4){
if((com4 & (1<<0))==0) d4_0; else d4_1;
if((com4 & (1<<1))==0) d5_0; else d5_1;
if((com4 & (1<<2))==0) d6_0; else d6_1;
if((com4 & (1<<3))==0) d7_0; else d7_1;
rs_0;
e_1;
delayus(5);
e_0;
HAL_Delay(5);
}

//ENVIA COMANDO DE 8 BITS
void lcd_wrcom(uint8_t com){
if((com & (1<<4))==0) d4_0; else d4_1;
if((com & (1<<5))==0) d5_0; else d5_1;
if((com & (1<<6))==0) d6_0; else d6_1;
if((com & (1<<7))==0) d7_0; else d7_1;
rs_0;
e_1;
delayus(5);
e_0;
delayus(5);
if((com & (1<<0))==0) d4_0; else d4_1;
if((com & (1<<1))==0) d5_0; else d5_1;
if((com & (1<<2))==0) d6_0; else d6_1;
if((com & (1<<3))==0) d7_0; else d7_1;
rs_0;
e_1;
delayus(5);
e_0;
HAL_Delay(5);
}

//ENVIA CARACTERE DE 8 BITS
void lcd_wrchar(uint8_t com){
if((com & (1<<4))==0) d4_0; else d4_1;
if((com & (1<<5))==0) d5_0; else d5_1;
if((com & (1<<6))==0) d6_0; else d6_1;
if((com & (1<<7))==0) d7_0; else d7_1;
rs_1;
e_1;
delayus(5);
e_0;
delayus(5);
if((com & (1<<0))==0) d4_0; else d4_1;
if((com & (1<<1))==0) d5_0; else d5_1;
if((com & (1<<2))==0) d6_0; else d6_1;
if((com & (1<<3))==0) d7_0; else d7_1;
rs_1;
e_1;
delayus(5);
e_0;
HAL_Delay(5);
}

//DESLOCA CURSOR PARA POSIÇÃO X Y
void lcd_goto(uint8_t x, uint8_t y){
int pos;
if(y>=0 && y<=1){
if(y==0) pos=0x80;
if(y==1) pos=0xc0;
if(x>=0 && x<=15){
pos+=x;
}
}
lcd_wrcom(pos);
}

//LIMPA O DISPLAY
void lcd_clear(){
lcd_wrcom(0x01);
}

//BACKLIGHT DO DISPLAY
void lcd_backlight(uint8_t backlight){
if(backlight==0) bl_off; else bl_on;
}

void lcd_progchar(void){
int i;
lcd_wrcom(0x40);
for(i=0;i<24;i++){
lcd_wrchar(tabChar[i]);
}
lcd_wrcom(0x80);
}

//INICIALIZAÇÃO DO DISPLAY
void lcd_init(uint8_t cursor){
lcd_wrcom4(3);
lcd_wrcom4(3);
lcd_wrcom4(3);
lcd_wrcom4(2);
lcd_wrcom(0x28);
lcd_wrcom(cursor_off);
lcd_wrcom(0x06);
lcd_wrcom(0x01);
bl_on;
lcd_progchar();
}

//FUNÇÃO PRINTF
int __io_putchar(int ch){
if(onde==no_lcd){
if (ch != '\n') lcd_wrchar(ch);
}
if(onde==na_serial2){
HAL_UART_Transmit(&huart2, (uint8_t*) &ch, 1, 10);
}
return ch;
}

int valor_celula(void){
if(celula_carga==0) return 1; else return 0; //precisa ser com conversor AD
}


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
  MX_USART2_UART_Init();
  MX_I2C1_Init();
  MX_RTC_Init();
  /* USER CODE BEGIN 2 */

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
 RTC_TimeTypeDef relogio;
   RTC_DateTypeDef calendario;
   uint8_t h,m,s,dia,mes,ano;
   relogio.Hours=17;
   relogio.Minutes=30;
   relogio.Seconds=00;

   calendario.Date=24;
   calendario.Month=11;
   calendario.Year=23;

   lcd_init(cursor_on);

   HAL_RTC_Init(&hrtc);
   HAL_RTC_WaitForSynchro(&hrtc);
   HAL_RTC_SetTime(&hrtc, &relogio, RTC_FORMAT_BIN);
   HAL_RTC_SetDate(&hrtc, &calendario, RTC_FORMAT_BIN);

   HAL_UART_Init(&huart2);

   int mostrar_mensagem = 1;

   do{
   mostrar_mensagem=1;
   if(!valor_celula()){
   do{
   if(mostrar_mensagem){
   onde=no_lcd;
   lcd_clear();
     printf("INSIRA UMA UNIDADE\n");
     onde=na_serial2;
   printf("INSIRA UMA UNIDADE\n");

   mostrar_mensagem = 0;
     }
   }while(!valor_celula());
   }

   onde=no_lcd;
   lcd_clear();
   printf("CADASTRE UM NOME\n");
   lcd_goto(0,1);
   printf("CADASTRE UM NOME\n");

   onde=na_serial2;
   printf("INSIRA UMA SENHA DE 4 DIGITOS\n");

   ch=p=0;
   do{
   do{
   HAL_UART_Receive(&huart2,(uint8_t*) &ch,1,10);
   }while((ch==0)&&(cartao_inserido()));
   senha[p++]=ch;
   }while((p<=3)&&(cartao_inserido()));
   senha[p]=0;
   }while((p<=3)&&(!cartao_inserido()));

   onde=no_lcd;
   lcd_clear();
   lcd_goto(0,0);
   printf("SENHA\n");
   lcd_goto(0,1);
   printf("CADASTRADA!\n");

   onde=na_serial2;
   printf("\nSENHA CADASTRADA\n");

   HAL_Delay(3000);

   if(cartao_inserido()){
   onde=no_lcd;
   lcd_clear();
   lcd_goto(0,0);
   printf("REMOVA O CARTAO\n");

   onde=na_serial2;
   printf("REMOVA O CARTAO\n");

   do{
   HAL_Delay(10);
   }while(cartao_inserido());
   }

   int maquina=0;

   HAL_RTC_GetTime(&hrtc, &relogio, RTC_FORMAT_BIN);
   HAL_RTC_GetDate(&hrtc, &calendario, RTC_FORMAT_BIN);

   h=relogio.Hours;
   m=relogio.Minutes;
   s=relogio.Seconds;

   dia=calendario.Date;
   mes=calendario.Month;
   ano=calendario.Year;

   onde=na_serial2;
   printf("\n%02d/%02d/%02d %02d:%02d:%02d\n\n",dia,mes,ano,h,m,s);

   lcd_clear();


   while (1)
     {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
     HAL_RTC_GetTime(&hrtc, &relogio, RTC_FORMAT_BIN);
     HAL_RTC_GetDate(&hrtc, &calendario, RTC_FORMAT_BIN);

     h=relogio.Hours;
     m=relogio.Minutes;
     s=relogio.Seconds;

     dia=calendario.Date;
     mes=calendario.Month;
     ano=calendario.Year;

     if(maquina==0){
     if(cartao_inserido()){
     lcd_clear();
     maquina=1;
     }else{
     onde=no_lcd;
     lcd_goto(8,0);
     printf("%02d:%02d:%02d\n",h,m,s);
     lcd_goto(8,1);
     printf("%02d/%02d/%02d\n",dia,mes,ano);


     }

     }else if(maquina==1){
     onde=no_lcd;
     lcd_goto(0,0);
     printf("DIGITE A SENHA:\n");

     onde=na_serial2;
     printf("DIGITE A SENHA:\n");

     ch=p=0;
     do{
     do{
     HAL_UART_Receive(&huart2,(uint8_t*) &ch,1,10);
     if(!cartao_inserido()){
     lcd_clear();
     maquina=0;
     break;
     }
     }while(ch==0);
    if(!cartao_inserido()){
    lcd_clear();
    maquina=0;
    break;
    }
      senha_digitada[p++]=ch;
       }while(p<=3);
     senha_digitada[p]=0;

     if(cartao_inserido()){
     lcd_clear();
     if(strcmp(senha,senha_digitada)!=0){
     onde=no_lcd;
     lcd_goto(0,0);
     printf("SENHA INCORRETA\n");

     onde=na_serial2;
     printf("\nSENHA INCORRETA\n");

     HAL_Delay(2000);
     lcd_clear();
     maquina=0;


     onde=na_serial2;
     printf("\nSENHA CORRETA\n");
     HAL_Delay(2000);
     maquina=2;
     lcd_clear();
     mostrar_mensagem=1;
     }
     }
     }else if(maquina==2){
     if(cartao_inserido()){
     servo_abrir();
     onde=no_lcd;
     lcd_goto(0,0);
     printf("REMOVA O CARTAO\n");

     if(mostrar_mensagem){
     onde=na_serial2;
     printf("REMOVA O CARTAO\n");
     mostrar_mensagem=0;
     }
     }else{
     maquina=3;
     }
     }else if(maquina==3){
     onde=no_lcd;
     lcd_clear();
     lcd_goto(0,0);
     printf("PORTA FECHARA\n");
     lcd_goto(0,1);
     printf("EM 5 SEGUNDOS.\n");

     onde=na_serial2;
     printf("PORTA FECHARA EM 5 SEGUNDOS\n");

     HAL_Delay(5000);
     servo_fechar();
     maquina=0;
     lcd_clear();
     }
     if(cartao_inserido()){
     onde=no_lcd;
     lcd_clear();
     lcd_goto(0,0);
     printf("REMOVA O\n");
     lcd_goto(0,1);
     printf("CARTAO\n");

     onde=na_serial2;
     printf("REMOVA O CARTAO\n");

     }

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
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI|RCC_OSCILLATORTYPE_HSI48
                              |RCC_OSCILLATORTYPE_LSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSI48State = RCC_HSI48_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
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
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART2|RCC_PERIPHCLK_I2C1
                              |RCC_PERIPHCLK_RTC;
  PeriphClkInit.Usart2ClockSelection = RCC_USART2CLKSOURCE_PCLK1;
  PeriphClkInit.I2c1ClockSelection = RCC_I2C1CLKSOURCE_HSI;
  PeriphClkInit.RTCClockSelection = RCC_RTCCLKSOURCE_LSI;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.Timing = 0x2000090E;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Analogue filter
  */
  if (HAL_I2CEx_ConfigAnalogFilter(&hi2c1, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Digital filter
  */
  if (HAL_I2CEx_ConfigDigitalFilter(&hi2c1, 0) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief RTC Initialization Function
  * @param None
  * @retval None
  */
static void MX_RTC_Init(void)
{

  /* USER CODE BEGIN RTC_Init 0 */

  /* USER CODE END RTC_Init 0 */

  /* USER CODE BEGIN RTC_Init 1 */

  /* USER CODE END RTC_Init 1 */

  /** Initialize RTC Only
  */
  hrtc.Instance = RTC;
  hrtc.Init.HourFormat = RTC_HOURFORMAT_24;
  hrtc.Init.AsynchPrediv = 127;
  hrtc.Init.SynchPrediv = 255;
  hrtc.Init.OutPut = RTC_OUTPUT_DISABLE;
  hrtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
  hrtc.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
  if (HAL_RTC_Init(&hrtc) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN RTC_Init 2 */

  /* USER CODE END RTC_Init 2 */

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
  huart2.Init.BaudRate = 38400;
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
  HAL_GPIO_WritePin(GPIOA, LD2_Pin|GPIO_PIN_8|GPIO_PIN_9, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_10|GPIO_PIN_4, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_RESET);

  /*Configure GPIO pin : B1_Pin */
  GPIO_InitStruct.Pin = B1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : PC3 */
  GPIO_InitStruct.Pin = GPIO_PIN_3;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : LD2_Pin PA8 PA9 */
  GPIO_InitStruct.Pin = LD2_Pin|GPIO_PIN_8|GPIO_PIN_9;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : PB10 PB4 */
  GPIO_InitStruct.Pin = GPIO_PIN_10|GPIO_PIN_4;
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
