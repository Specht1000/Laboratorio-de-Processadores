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

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//DEFINES DOS PINOS DE COMANDO DO LCD

#define e_0 GPIOC->BRR  = (1<<7)
#define e_1 GPIOC->BSRR = (1<<7)

#define rs_0 GPIOA->BRR  = (1<<9)
#define rs_1 GPIOA->BSRR = (1<<9)

#define d4_0 GPIOB->BRR  = (1<<5)
#define d4_1 GPIOB->BSRR = (1<<5)

#define d5_0 GPIOB->BRR  = (1<<4)
#define d5_1 GPIOB->BSRR = (1<<4)

#define d6_0 GPIOB->BRR  = (1<<10)
#define d6_1 GPIOB->BSRR = (1<<10)

#define d7_0 GPIOA->BRR  = (1<<8)
#define d7_1 GPIOA->BSRR = (1<<8)

#define bl_on GPIOB->BRR  = (1<<6)
#define bl_off GPIOB->BSRR = (1<<6)

//DEFINES DE FUNÇÕES PRINCIPAIS DO LCD
#define cursor_off 0x0c
#define cursor_on 0x0e
#define cursor_blink 0x0f

//CARACTER ESPECIAL
unsigned char tabchar[16] = {0x0e, 0x01, 0x04, 0x04, 0x06, 0x10, 0x0e, 0x00, 0x0e, 0x04, 0x0e, 0x15, 0x17, 0x11, 0x0e, 0x00};

//CONTROLE POSICAOIÇÕES CURSOR
unsigned char posicoes [10] = {0x88, 0x89, 0x8e, 0x8f, 0xc8, 0xc9, 0xcb, 0xcc, 0xce, 0xcf};

//CONTROLE DO RELOGIO
int r_d, r_u, t1, t2, h_d, h_u, m_d, m_u, s_d, s_u, relogio, contador,hora, minuto, segundo,posicao,tecla,estado;

//CONTROLE DO RELE
char rele1, rele2, cont;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//FUNÇÕES GERAIS:

//FUNÇÃO PARA TEMPO DOS COMANDOS DO LCD

void udelay (void){
	int tempo = 7;
	while (tempo --);
}

void delayus (int tempo){

	while(tempo--)udelay();
}

//ESCREVE 2 DIGITOS

void lcd_wrnumero(uint8_t numero){
	if(numero>9){
		lcd_wrchar(48 + numero/10);
		lcd_wrchar(48 + numero%10);
		}
	else{
		lcd_wrchar(48);
		lcd_wrchar(48 + numero);
	}
}

//PRINTF

	int __io_putchar(int ch){
		if (ch != '\n') lcd_wrchar(ch);
		return ch;
	}

//LEITURA DOS BOTÕES

void ler_botoes (void){
	HAL_ADC_Start(&hadc);
	HAL_ADC_PollForConversion(&hadc, 1);
	tecla = HAL_ADC_GetValue(&hadc);
	HAL_ADC_Stop(&hadc);
	HAL_Delay(10);
	}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


//EXERCÍCIO 1:

//1.1(FUNÇÕES BÁSICAS):

	//LCD_LEITURA E ESCRITA COMANDO DE 8 BITS
void lcd_wrcom(uint8_t com){
	if((com & (1<<4))== 0) d4_0; else d4_1;
	if((com & (1<<5))== 0) d5_0; else d5_1;
	if((com & (1<<6))== 0) d6_0; else d6_1;
	if((com & (1<<7))== 0) d7_0; else d7_1;

	rs_0;
	e_1;
	delayus(5);
	e_0;
	HAL_Delay(5);

	if((com & (1<<0))== 0) d4_0; else d4_1;
	if((com & (1<<1))== 0) d5_0; else d5_1;
	if((com & (1<<2))== 0) d6_0; else d6_1;
	if((com & (1<<3))== 0) d7_0; else d7_1;

	rs_0;
	e_1;
	delayus(5);
	e_0;
	HAL_Delay(5);
}

	//LCD_LEITURA E ESCRITA COMANDO DE 4 BITS

void lcd_wrcom4(uint8_t com4){
	if((com4 & (1<<0))== 0) d4_0; else d4_1;
	if((com4 & (1<<1))== 0) d5_0; else d5_1;
	if((com4 & (1<<2))== 0) d6_0; else d6_1;
	if((com4 & (1<<3))== 0) d7_0; else d7_1;

	rs_0;
	e_1;
	delayus(5);
	e_0;
	HAL_Delay(5);
}

//1.2(FUNÇÕES BÁSICAS):

	//ENVIA CARACTERES

void lcd_wrchar(uint8_t com){
	if((com & (1<<4))== 0) d4_0; else d4_1;
	if((com & (1<<5))== 0) d5_0; else d5_1;
	if((com & (1<<6))== 0) d6_0; else d6_1;
	if((com & (1<<7))== 0) d7_0; else d7_1;

	rs_1;
	e_1;
	delayus(5);
	e_0;
	HAL_Delay(5);

	if((com & (1<<0))== 0) d4_0; else d4_1;
	if((com & (1<<1))== 0) d5_0; else d5_1;
	if((com & (1<<2))== 0) d6_0; else d6_1;
	if((com & (1<<3))== 0) d7_0; else d7_1;

	rs_1;
	e_1;
	delayus(5);
	e_0;
	HAL_Delay(5);
}

//1.3(FUNÇÕES BÁSICAS):

	//INICIA DISPLAY

void lcd_init(uint8_t cursor){
	lcd_wrcom4(3);
	lcd_wrcom4(3);
	lcd_wrcom4(3);
	lcd_wrcom4(2);
	lcd_wrcom(0x28);
	lcd_wrcom(cursor);
	lcd_wrcom(0x06);
	lcd_wrcom(0x01);
	lcd_progchar();
	lcd_backlight(0);
}

//1.4(FUNÇÕES PARA POSICAOICIONAMENTO DO CURSOR):

//DESLOCA CURSOR

void lcd_goto(uint8_t x, uint8_t y){
	int posicao;
	if(y>=0 && y<=1){
		if(y==0) posicao=0x80;
		if(y==1) posicao=0xc0;
			if(x>=0 && x<=15) {
				posicao+=x;
			}
	}
	lcd_wrcom(posicao);
}

//1.5(FUNÇÕES PARA POSICAOICIONAMENTO DO CURSOR):

//CARACTERES ESPECIAIS

void lcd_progchar(void){
	lcd_wrcom(0x40);

	for(int i=0; i<16; i++){
		lcd_wrchar(tabchar[i]);
	}

	lcd_wrcom(0x80);
}


//1.6(FUNÇÕES PARA POSICAOICIONAMENTO DO CURSOR):

//STRINGS

void lcd_wrstr(char *str){
	while(*str){
		lcd_wrchar(*str);
				str++;
	}
}

//1.7(FUNÇÕES PARA POSICAOICIONAMENTO DO CURSOR):

//CLEAR
	void lcd_clear (){
		lcd_wrcom(0x01);
	}

//1.8(FUNÇÃO BLACKLIGHT)


void lcd_backlight (uint8_t backlight){
	if(backlight==0) bl_off; else bl_on;
}

//1.9(FUNÇÕES RELE)

void rele1_on(void){
	GPIOC->BSRR=(1<<2);
}

void rele1_off(void){
	GPIOC->BRR=(1<<2);
}

void rele2_on(void){
	GPIOC->BSRR=(1<<3);
}

void rele2_off(void){
	GPIOC->BRR=(1<<3);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//EXERCICIO 2:

//2.1 ESCREVER O RELOGIO NO CANTO INFERIOR DIREITO DO DISPLAY


	int compara_horas (int h_d,int m_d, int s_d, int h_u, int m_u, int s_u){
		if(h_d>h_u){
			return 1;
				}else if (h_d==h_u){
						if (m_d>m_u){
							return 1;
				}else if (m_d==m_u){
						if(s_d>s_u){
							return 1;
				}else if (s_d==s_u){
							return 1;
				}

				}

				}
			return 0;
		}

void atualiza_tela(void){
		if(estado==2){
			segundo++;
			if(segundo>59){
				segundo = 0;
				minuto ++;
				if(minuto>59){
					minuto = 0;
					hora ++;
				if(hora>23) hora=0;
			}
		}
		}else{
				relogio = r_d*10+r_u;
				contador = t1*10+t2;
				hora = h_d*10+h_u;
				minuto = m_d*10+m_u;
				segundo = s_d*10+s_u;
			}
	if (compara_horas(hora,minuto,segundo,18,0,0) && compara_horas(22,59,59,hora,minuto,segundo)){
		rele1 = 'X';
		lcd_backlight(1);
		rele1_on();
	}else {
		rele1='.';
		lcd_backlight(0);
		rele1_off();
	}
	if (compara_horas(hora,minuto,segundo,relogio,0,0) && compara_horas(relogio,contador,0,hora,minuto,segundo)){
			rele2 = 'X';
			rele2_on();
	}else{
		rele2='.';
		rele2_off();
	}

	lcd_goto(0,0);
	printf("L[%c]\n",rele1);
	lcd_goto(7,0);
	lcd_wrchar(0);
	printf("%02d\n", relogio);
	lcd_goto(13,0);
	lcd_wrchar(1);
	printf("%02d\n", contador);
	lcd_goto(0,1);
	printf("C[%c]\n", rele2);
	lcd_goto(7, 1);
	printf("%c%02d:%02d:%02d:\n",cont,hora,minuto,segundo);
	lcd_wrcom(posicoes[posicao]);
		if(estado==2){
			HAL_Delay(700);
		}else{
			HAL_Delay(10);
			}
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
  MX_ADC_Init();
  /* USER CODE BEGIN 2 */

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */

  ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  //INICIALIZANDO CODIGO NO DISPLAY

  lcd_init(cursor_on);
  HAL_ADC_Init(&hadc);
  r_d=r_u=t1=t2=h_d=h_u=m_d=m_u=s_d=s_u=posicao=estado=0;
  relogio = r_d*10+r_u;
  contador = t1*10+t2;
  minuto = m_d*10+t2;
  segundo = s_d*10+s_u;
  rele1=rele2= '.';
  cont = ' ';

  atualiza_tela();

  ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

	  ler_botoes();

	  if(estado==0){
		 if(tecla<100){
			  posicao++;
		  if(posicao > 9)posicao = 0;
		    estado=1;
		    atualiza_tela();
		 	 }else if ((tecla>2200)&&(tecla<2500)){
		 		 posicao--;
		 		 if(posicao < 0) posicao = 9;
		 		 estado=1;
		 		 atualiza_tela();

		 	 }else if((tecla > 400)&&(tecla < 700)){
		 		 switch(posicao){
		 		 case 0:
		 			 r_d++;
		 			 if(r_u<=3){if(r_d > 2)r_d =0;}
		 			 else if(r_d > 1)r_d = 0;
		 			 break;
		 		 case 1:
		 			r_u++;
		 			if(r_d==2){if(r_u > 3)r_u =0;}
		 			else if(r_u > 9)r_u = 0;
		 			break;
		 		 case 2:
		 			t1++;
		 			if(t1 > 5)t1 = 0;
		 			break;
		 		 case 3:
		 		 	t2++;
		 			if(t2 > 9)t2 = 0;
		 			break;
		 		 case 4:
		 			h_d++;
		 			if(h_u<=3){if(h_d > 2)h_d =0;}
		 			else if(h_d > 1)h_d = 0;
		 			break;
		 		case 5:
		 			h_u++;
		 			if(h_d==2){if(h_u > 3)h_u =0;}
		 			else if(h_u > 9)h_u = 0;
		 			break;
		 		case 6:
		 			m_d++;
		 			if(m_d > 5)m_d = 0;
		 			break;
		 		case 7:
		 			m_u++;
		 			if(m_u > 9)m_u = 0;
		 			break;
		 		case 8:
		 			s_d++;
		 			if(s_d > 5)s_d = 0;
		 			break;
		 		case 9:
		 			s_u++;
		 			if(s_u > 9)s_u = 0;
		 			break;
		 		default:
					break;
		 		 }
		 		 estado=1;
		 		 atualiza_tela();

		 		 }else if((tecla > 1300)&&(tecla < 1600)){
					switch(posicao){

					case 0:
						r_d--;
						if(r_u<=3){if(r_d < 0)r_d =2;}
						else if(r_d < 0)r_d = 1;
						break;
					 case 1:
						r_u--;
						if(r_d==2){if(r_u < 0)r_u = 3;}
						else if(r_u < 0)r_u = 9;
						break;
					case 2:
						t1--;
						if(t1 < 0)t1 = 5;
						break;
					case 3:
						t2--;
						if(t2 < 0)t2 = 9;
						break;
					case 4:
						h_d--;
						if(h_u<=3){if(h_d < 0)h_d = 2;}
						else if(h_d < 0)h_d = 1;
						break;
					case 5:
						h_u--;
						if(h_d==2){if(h_u < 0)h_u = 3;}
						else if(h_u < 0)h_u = 9;
						break;
					case 6:
						m_d--;
						if(m_d < 0)m_d = 5;
						break;
					case 7:
						m_u--;
						if(m_u < 0)m_u = 9;
						break;
					case 8:
						s_d--;
						if(s_d < 0)s_d = 5;
						break;
					case 9:
						s_u--;
						if(s_u < 0)s_u = 9;
						break;
					default:
						break;
						}
					estado=1;
				    atualiza_tela();
				}else if((tecla>3500)&&(tecla<3700)){
					cont = '.';
					atualiza_tela();
					estado = 2;
					while(tecla < 3800)ler_botoes ();
				}
	  	  	  }else if(estado == 1){
	  	  		  if(tecla>3800){
	  	  			  estado = 0;
	  	  		  }
	  	  	  }else if(estado == 2){
	  	  		  atualiza_tela();
	  	  		 if((tecla>3500)&&(tecla<3700)){
	  	  			 cont = ' ';
	  	  			 h_d = hora/10;
	  	  			 h_u = hora%10;
	  	  			 m_d = minuto/10;
	  	  			 m_u = minuto%10;
	  	  			 s_d = segundo/10;
	  	  			 s_u = segundo%10;
	  	  			 estado = 0;
	  	  			 posicao = 0;
	  	  			 atualiza_tela();
	  	  			 while(tecla<3800) ler_botoes();
	  	  		 }
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
  sConfig.Channel = ADC_CHANNEL_0;
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
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_7, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, LD2_Pin|GPIO_PIN_8|GPIO_PIN_9, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_10|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6, GPIO_PIN_RESET);

  /*Configure GPIO pin : B1_Pin */
  GPIO_InitStruct.Pin = B1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : PC2 PC3 PC7 */
  GPIO_InitStruct.Pin = GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_7;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : LD2_Pin PA8 PA9 */
  GPIO_InitStruct.Pin = LD2_Pin|GPIO_PIN_8|GPIO_PIN_9;
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

  /*Configure GPIO pins : PC10 PC11 PC12 */
  GPIO_InitStruct.Pin = GPIO_PIN_10|GPIO_PIN_11|GPIO_PIN_12;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
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

