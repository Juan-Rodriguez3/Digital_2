/* USER CODE BEGIN Header */
/*
 * ADC_TIMER_UART2VIAS_DAC
 * Proyecto que muestra el uso del ADC y DAC en el STM32F446RE. La lectura se hace
 * dentro de la rutina de interrupción de un temporizador para que sea a una frecuencia constante.
 * Las conversiones se envían por UART, para poder ser leídas en una computadora.
 * La lectura se escala para enviarla al DAC.
 * Además de esto, se utiliza una rutina de interrupción del UART para la recepción de datos
 * provenientes de una PC.
 *
 * PARA ASEGURAR UNA TRANSMISIÓN CORRECTA DE DATOS, EL UART SE CONFIGURÓ A 921600 baudios.
 * Asegúrese de configurar las terminales o puertos seriales a esta velocidad.
 *
 * Basado en los ejemplos ADC_IT y TIM_IT del curso MOOC - STM32CubeMX and STM32Cube HAL basics
 * Modificado por Pedro Joaquín Castillo
 *
 * Para hacer uso de este programa se debe conectar una señal al pin PA0 del Nucleo-F446RE.
 * La salida del DAC puede observarse en el pin PA4.
 * Para la recepción de coeficientes, puede utilizar el programa PS_proy_enviar_recibir.m
 * Consulten el diagrama esquemático de la placa NUCLEO para mayor detalle de la conexión
 * de los pines.
 */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
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
//Importante incluir estas librerías para evitar mensajes de alerta al momento del "build"
//Contienen funciones para enviar mensajes por puerto COM
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

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
ADC_HandleTypeDef hadc1;

DAC_HandleTypeDef hdac;

TIM_HandleTypeDef htim10;

UART_HandleTypeDef huart2;
DMA_HandleTypeDef hdma_usart2_rx;

/* USER CODE BEGIN PV */

#define COEF_SIZE 8		//Tamaño del coeficiente a recibir

uint16_t adc_value;  //Variable para almacenar las conversiones del ADC
char msg[30]; //Buffer para el mensaje a enviar por UART (debe ser del tamaño adecuado para que quepan todos
//caracteres a enviar)
uint8_t rec[COEF_SIZE + 1]; //Buffer para la recepción de datos

//FRECUENCIA DE TIMER
//OJO: LA CONFIGURACIÓN ACTUAL PERMITE UNA freq_timer MÁXIMA DE 10kHz.
//Sin embargo, no se recomienda sobrepasar los 3kHz.
//SI SE DESEA UNA FRECUENCIA MÁS ALTA SE DEBE CAMBIAR LA CONFIGURACIÓN DEL TIM10
uint16_t freq_timer = 1000; //En Hz
uint32_t timer_clk; //Variable para configurar el TIM10
uint32_t pclk; //Variable para configurar el TIM10

float num_recibido = 1.0; //Número recibido por UART
float x_n = 0.0, y_n = 0.0; // En este ejemplo no es necesario inicializar a 0
							// estas variables. Se hace para ilustrar cómo inicializar.

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_ADC1_Init(void);
static void MX_DAC_Init(void);
static void MX_TIM10_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

//Callback para la interrupción de TIM10
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim){
	HAL_ADC_Start(&hadc1);  //Inicializa la conversión ADC1
	HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY); //Se espera a que se complete la conversión
	adc_value = HAL_ADC_GetValue(&hadc1); //Se asigna el valor en la variable correspondiente
	sprintf(msg, "%d\n", adc_value); //Se prepara el mensaje...
	HAL_UART_Transmit_IT(&huart2,(uint8_t*)msg, strlen(msg)); //...y se envía por UART
	//En la línea de arriba se usa la opción con IT para evitar "ocupar" al procesador enviando el mensaje

	x_n = 1.0*adc_value; //Con el 1.0, de una vez se convierte a float
	y_n = num_recibido*x_n;  //Esto ilustra cómo se podría usar lo recibido por UART.

	// Lo siguiente es para asegurarse de que el valor que se envíe al DAC esté en
	// el intervalo adecuado (el DAC es de 12 bits)
	if(y_n > 4095.0){
		y_n = 4095.0;
	}
	else if(y_n < 0.0){
		y_n = 0.0;
	}

	//Se envía un valor de 12 bits alineado a la derecha al DAC (note el casting a entero que se le aplica  y_n
	HAL_DAC_SetValue(&hdac, DAC_CHANNEL_1, DAC_ALIGN_12B_R, (uint16_t)y_n);
}

//Callback para la interrupción de UART2 al momento de recibir toda la data
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart){
	rec[COEF_SIZE-1] = '\0'; //Se agrega un nulo para convertir a string
	num_recibido = atof((char*)rec);  //Se convierte a float

	//El UART se configura nuevamente para poder recibir la siguiente recepción
	HAL_UART_Receive_DMA(&huart2,rec,COEF_SIZE);

	// Para el proyecto: podrían usar un arreglo de floats para ir guardando los
	// coeficientes de la ecuación de diferencias. Necesitarían un contador de
	// coeficientes (variable global), para tener control de cuántos se han recibido.
	// El número de coeficientes depende del orden del filtro diseñado. Como se
	// explicó, pueden asumir un orden máximo para los filtros que se ejecutan
	// en el Nucleo-F446RE (ej.: orden 4). Y pueden ajustar sus programas de Matlab y del
	// Nucleo-F446RE para siempre enviar y recibir el mismo número de coeficientes. Si se
	// diseña un filtro de orden menor al máximo, los coeficientes "faltantes" los
	// igualarían a cero.
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
  MX_DMA_Init();
  MX_USART2_UART_Init();
  MX_ADC1_Init();
  MX_DAC_Init();
  MX_TIM10_Init();
  /* USER CODE BEGIN 2 */

  HAL_DAC_Start(&hdac, DAC_CHANNEL_1); //Inicializa el Canal 1 del DAC
  HAL_UART_Receive_DMA(&huart2, rec, COEF_SIZE); //Se configura la recepción del UART con DMA
  HAL_TIM_Base_Start_IT(&htim10); //Esto inicializa el TIM10 y habilita la interrupción
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	  //El controlador se queda esperando las interrupciones de TIM10 y del UART
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
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 16;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  RCC_OscInitStruct.PLL.PLLR = 2;
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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief ADC1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC1_Init(void)
{

  /* USER CODE BEGIN ADC1_Init 0 */

  /* USER CODE END ADC1_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC1_Init 1 */

  /* USER CODE END ADC1_Init 1 */

  /** Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion)
  */
  hadc1.Instance = ADC1;
  hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4;
  hadc1.Init.Resolution = ADC_RESOLUTION_12B;
  hadc1.Init.ScanConvMode = DISABLE;
  hadc1.Init.ContinuousConvMode = DISABLE;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.NbrOfConversion = 1;
  hadc1.Init.DMAContinuousRequests = DISABLE;
  hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
  */
  sConfig.Channel = ADC_CHANNEL_0;
  sConfig.Rank = 1;
  sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC1_Init 2 */

  /* USER CODE END ADC1_Init 2 */

}

/**
  * @brief DAC Initialization Function
  * @param None
  * @retval None
  */
static void MX_DAC_Init(void)
{

  /* USER CODE BEGIN DAC_Init 0 */

  /* USER CODE END DAC_Init 0 */

  DAC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN DAC_Init 1 */

  /* USER CODE END DAC_Init 1 */

  /** DAC Initialization
  */
  hdac.Instance = DAC;
  if (HAL_DAC_Init(&hdac) != HAL_OK)
  {
    Error_Handler();
  }

  /** DAC channel OUT1 config
  */
  sConfig.DAC_Trigger = DAC_TRIGGER_NONE;
  sConfig.DAC_OutputBuffer = DAC_OUTPUTBUFFER_ENABLE;
  if (HAL_DAC_ConfigChannel(&hdac, &sConfig, DAC_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN DAC_Init 2 */

  /* USER CODE END DAC_Init 2 */

}

/**
  * @brief TIM10 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM10_Init(void)
{

  /* USER CODE BEGIN TIM10_Init 0 */
	//Se obtiene la frecuencia del CLK que controla el TIM10
	pclk = HAL_RCC_GetPCLK2Freq();
	if ((RCC->CFGR & RCC_CFGR_PPRE2) != RCC_CFGR_PPRE2_DIV1) {
		timer_clk = pclk * 2;  // Si el divisor del CLK es diferente a 1, se duplica la frecuencia del reloj
	} else {
		timer_clk = pclk; //Si el divisor es 1, se mantiene igual
	}

	htim10.Init.Prescaler = 8399;
	//OJO: Gracias a la siguiente línea de código es modificar la frecuencia del timer al sólo cambiar freq_timer.
	//Note que la línea que configura el período está comentada en la sección de configuración del TIM10 (más abajo).
	//Si se genera nuevo código, debe comentar la línea en donde se asigna el valor del htim10.Init.Period en la
	//configuración del TIM10.
	htim10.Init.Period = ((timer_clk / (htim10.Init.Prescaler + 1)) / freq_timer) - 1;

  /* USER CODE END TIM10_Init 0 */

  /* USER CODE BEGIN TIM10_Init 1 */

  /* USER CODE END TIM10_Init 1 */
  htim10.Instance = TIM10;
  htim10.Init.Prescaler = 8399;
  htim10.Init.CounterMode = TIM_COUNTERMODE_UP;
  //htim10.Init.Period = 65535;
  htim10.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim10.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim10) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM10_Init 2 */

  /* USER CODE END TIM10_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

	//NOTE QUE EL BAUDAJE DEL UART2 SE CONFIGURÓ A 921600
  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 921600;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Stream5_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Stream5_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Stream5_IRQn);

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
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : B1_Pin */
  GPIO_InitStruct.Pin = B1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : LD2_Pin */
  GPIO_InitStruct.Pin = LD2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LD2_GPIO_Port, &GPIO_InitStruct);

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
