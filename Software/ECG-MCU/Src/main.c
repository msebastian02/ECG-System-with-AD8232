#include "main.h"
#include "stm32f4xx_hal.h"
#include "fatfs.h"
#include "string.h"
#include "stdio.h"
#include "boxes.h"

ADC_HandleTypeDef hadc1;
SD_HandleTypeDef hsd;
UART_HandleTypeDef huart2;
DMA_HandleTypeDef hdma_usart2_rx;
DMA_HandleTypeDef hdma_usart2_tx;
GPIO_PinState Pulsador;

HAL_StatusTypeDef HAL_UART_DMA_Tx_Stop(UART_HandleTypeDef *huart);

//MCU Config
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_ADC1_Init(void);
static void MX_SDIO_SD_Init(void);
static void MX_USART2_UART_Init(void);
//DATA PROCESS
uint16_t leerADC(void);
void guardardatos(char* a, char* archivo);
void crearSD(char* archivo);
void guardarSD(char* datos,char* archivo);
void parpadeo(void);
void validarESP(void);
//AES
void AES_Encrypt(unsigned char* message, unsigned char* key);
void KeyExpansionCore(unsigned char* in, unsigned char i);
void KeyExpansion(unsigned char* inputKey, unsigned char* expandedKeys);
void SubBytes(unsigned char* state);
void ShiftRows(unsigned char* state);
void MixColumns(unsigned char* state);
void AddRoundKey(unsigned char* state, unsigned char* roundKey);
void PrintHex(unsigned char* x,int y,char* z);

int main(void)
{
	int  contador=0;
	uint16_t datosADC;
	char datos[1001];
	char MSB;
	char LSB;
	char time[10];
	char archivo[20];
  HAL_Init();
  SystemClock_Config();
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_ADC1_Init();
  MX_SDIO_SD_Init();
  MX_USART2_UART_Init();
  MX_FATFS_Init();
	
	//HAL_UART_Receive(&huart2,(uint8_t*)time,sizeof(time),HAL_MAX_DELAY);
	//strcat(archivo,time);
	//strcat(archivo,".txt");
	crearSD("result.txt");
	HAL_Delay(5000);
	
	//TESTS
	/*
	for (int i=0;i<=0;i++){
		char test[]="Este es un texto de prueba para la encripcion de datos con algoritmo AES del proyecto de grado Diseño e implementacion de un sistema de adquisicion de señales electrocardiograficas con almacenamiento remoto de datos encriptados desarrollado por Juan Sebastian Munar Aldana y Carlos Eduardo Moreno Doria de la facultad de Ingenieria Electronica de la universidad Santo Tomas en Bogota, Colombia. Asesorada por Jaime Vitola";
		guardarSD(test,"test3.txt");
		guardardatos(test,"encrypt1.txt");
		HAL_UART_Transmit(&huart2,(uint8_t*)test,sizeof(test),HAL_MAX_DELAY);
		HAL_Delay(100);
	}*/
	
	for (int cantidad=0;cantidad<=2;cantidad++){
		while (contador <= 1000){
			
			HAL_GPIO_TogglePin(GPIOC,GPIO_PIN_6);
			datosADC=leerADC();
			MSB =(datosADC>>8)&0xff;
			datos[contador]=MSB;
			LSB = (datosADC&0xff);
			datos[contador+1]=LSB;
			if (contador==1000){
				guardardatos(datos,"result.txt");
			}		
			contador+=2;
			HAL_Delay(2);
		}
		HAL_GPIO_TogglePin(GPIOC,GPIO_PIN_6);
		HAL_Delay(1000);
		contador = 0;		
	}
	
	
  while (1)
  {
		parpadeo();
	}
}

void crearSD(char* archivo){
	FATFS myFATFS;
	FIL myFILE;
	
	if(f_mount(&myFATFS,"",1)== FR_OK){
		if(f_open( &myFILE, archivo, FA_CREATE_ALWAYS | FA_WRITE ) == FR_OK)
			parpadeo();
			
		f_close(&myFILE);
		f_mount(0, "", 1);
	}
}

void guardarSD(char* datos,char* archivo){
	FATFS myFATFS;
	FIL myFILE;
	
	if(f_mount(&myFATFS,"",1)== FR_OK){
		if(f_open( &myFILE, archivo, FA_OPEN_EXISTING | FA_WRITE | FA_READ) == FR_OK)
			f_lseek(&myFILE, f_size(&myFILE));
				if(f_puts(datos, &myFILE) > 0)
					parpadeo();
		f_close(&myFILE);
		//f_mount(0, "", 1);
	}
}

void parpadeo(){
	for(int a=0;a<=1;a++){
		HAL_GPIO_TogglePin(GPIOC,GPIO_PIN_6);
		HAL_Delay(50);
		HAL_GPIO_TogglePin(GPIOC,GPIO_PIN_6);
		HAL_Delay(50);
	}
}

void guardardatos(char* a, char* archivo)
{
	char message[1001];	
  unsigned char key[16] = {0X50,0X41,0X53,0X53,0X57,0X4F,0X52,0X44,0X31,0X32,0X33,0X34,0X35,0X36,0X37,0X38};
												//		P		A		S		S		W		O		R		D		1		2		3		4		5		6		7		8

	for(int k=0; k<=sizeof(message);k++)
		message[k]=0;
	
	for(int f=0; f<=sizeof(message);f++)
		message[f]=a[f];
	
	int originalLen = sizeof(message);
	int lenOfPaddedMessage = originalLen;

	if (lenOfPaddedMessage % 16 != 0){
		lenOfPaddedMessage = (lenOfPaddedMessage / 16 + 1) * 16;
	}
	
	unsigned char paddedMessage [lenOfPaddedMessage];
	char cifrado[lenOfPaddedMessage*2];

	for (int i = 0; i < lenOfPaddedMessage; i++){
		if (i >= originalLen)
			paddedMessage[i] = 0;
		else 
			paddedMessage[i] = message[i];
	}
	
	for(int i=0;i<lenOfPaddedMessage;i+=16){
		AES_Encrypt(paddedMessage+i,key);
		if(i==(lenOfPaddedMessage-16)){
			PrintHex(paddedMessage,lenOfPaddedMessage,cifrado);
			if(sizeof(cifrado)==2016){
				guardarSD(cifrado,archivo);
				HAL_UART_Transmit(&huart2,(uint8_t*)cifrado,sizeof(cifrado),HAL_MAX_DELAY);
			}
		}
	}
}

uint16_t leerADC(void)
{
	uint16_t a;
	HAL_ADC_Start(&hadc1);
	if(HAL_ADC_PollForConversion(&hadc1, 5)==HAL_OK)
		a= HAL_ADC_GetValue(&hadc1);
	else
		a=0;
	HAL_ADC_Stop(&hadc1);
	return a;
}

/** System Clock Configuration
*/
void SystemClock_Config(void)
{

  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;

    /**Configure the main internal regulator output voltage 
    */
  __HAL_RCC_PWR_CLK_ENABLE();

  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = 16;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 16;
  RCC_OscInitStruct.PLL.PLLN = 192;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Configure the Systick interrupt time 
    */
  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

    /**Configure the Systick 
    */
  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

  /* SysTick_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

/* ADC1 init function */
static void MX_ADC1_Init(void)
{

  ADC_ChannelConfTypeDef sConfig;

    /**Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion) 
    */
  hadc1.Instance = ADC1;
  hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV2;
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
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time. 
    */
  sConfig.Channel = ADC_CHANNEL_5;
  sConfig.Rank = 1;
  sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
	parpadeo();
	HAL_UART_DMA_Tx_Stop(&huart2);
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	HAL_GPIO_TogglePin(GPIOC,GPIO_PIN_6);
}

HAL_StatusTypeDef HAL_UART_DMA_Tx_Stop(UART_HandleTypeDef *huart)
{
  uint32_t dmarequest = 0x00U;
  dmarequest = HAL_IS_BIT_SET(huart->Instance->CR3, USART_CR3_DMAT);
  if((huart->gState == HAL_UART_STATE_BUSY_TX) && dmarequest)
  {
    CLEAR_BIT(huart->Instance->CR3, USART_CR3_DMAT);

    /* Abort the UART DMA Tx channel */
    if(huart->hdmatx != NULL)
    {
      HAL_DMA_Abort(huart->hdmatx);
    }
    //UART_EndTxTransfer(huart);
		  /* Disable TXEIE and TCIE interrupts */
		CLEAR_BIT(huart->Instance->CR1, (USART_CR1_TXEIE | USART_CR1_TCIE));
		huart->gState = HAL_UART_STATE_READY;
		
		return HAL_OK;
  }
	else return HAL_ERROR;

}

/* SDIO init function */
static void MX_SDIO_SD_Init(void)
{

  hsd.Instance = SDIO;
  hsd.Init.ClockEdge = SDIO_CLOCK_EDGE_RISING;
  hsd.Init.ClockBypass = SDIO_CLOCK_BYPASS_DISABLE;
  hsd.Init.ClockPowerSave = SDIO_CLOCK_POWER_SAVE_DISABLE;
  hsd.Init.BusWide = SDIO_BUS_WIDE_1B;
  hsd.Init.HardwareFlowControl = SDIO_HARDWARE_FLOW_CONTROL_DISABLE;
  hsd.Init.ClockDiv = 0;

}

/* USART2 init function */
static void MX_USART2_UART_Init(void)
{

  huart2.Instance = USART2;
  huart2.Init.BaudRate = 9600;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

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
  /* DMA1_Stream6_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Stream6_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Stream6_IRQn);

}

/** Configure pins as 
        * Analog 
        * Input 
        * Output
        * EVENT_OUT
        * EXTI
*/
static void MX_GPIO_Init(void)
{

  GPIO_InitTypeDef GPIO_InitStruct;

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, GPIO_PIN_SET);
  /*Configure GPIO pin : PC6 */
  GPIO_InitStruct.Pin = GPIO_PIN_6;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pin : PC9 */
  GPIO_InitStruct.Pin = GPIO_PIN_9;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI9_5_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);

}



void PrintHex(unsigned char* x, int y, char* z)
{
	char temp[1];
	for(int i=0; i< y; i++)
	{
		if(x[i]/16 < 10){
			temp[0] = ((x[i]/16) + '0');
			strcat((char*)z, temp);
		}
		if(x[i]/16 >= 10){
			temp[0] = ((x[i]/16 - 10) + 'A');
			strcat((char*)z, temp);
		}
		if(x[i]%16 < 10){
			temp[0]= ((x[i]%16) + '0');
			strcat((char*)z, temp);
		}
		if(x[i]%16 >= 10){
			temp[0] = ((x[i]%16 - 10) + 'A');
			strcat((char*)z, temp);
		}
  }
	
}

void KeyExpansionCore(unsigned char* in, unsigned char i)
{
	//Rotate Left
	unsigned int* q = (unsigned int*)in;
	*q = (*q >> 8) | ((*q & 0xff) << 24);

	//S-BOX 4Bytes
	in[0] = s_box[in[0]];
	in[1] = s_box[in[1]];
	in[2] = s_box[in[2]];
	in[3] = s_box[in[3]];

	//RCon
	in[0] ^= rcon[i];
}
void KeyExpansion(unsigned char* inputKey, unsigned char* expandedKeys)
{
	for (int i = 0; i < 16; i++)
		expandedKeys[i] = inputKey[i];

	int bytesGenerated = 16;
	int rconIteration = 1;
	unsigned char temp[4];

	while (bytesGenerated < 176)
	{
		for (int i = 0; i < 4; i++)
			temp[i] = expandedKeys[i + bytesGenerated - 4];

		if (bytesGenerated % 16 == 0)
		{
			KeyExpansionCore(temp, rconIteration);
			rconIteration++;
		}

		for (unsigned char a = 0; a < 4; a++)
		{
			expandedKeys[bytesGenerated]= expandedKeys[bytesGenerated - 16] ^ temp[a];
			bytesGenerated++;
		}
	}
}
void SubBytes(unsigned char* state)
{
	for (int i = 0; i < 16; i++)
		state[i] = s_box[state[i]];
}
void ShiftRows(unsigned char* state)
{
	unsigned char tmp[16];

	tmp[0] = state[0];
	tmp[1] = state[5];
	tmp[2] = state[10];
	tmp[3] = state[15];

	tmp[4] = state[4];
	tmp[5] = state[9];
	tmp[6] = state[14];
	tmp[7] = state[3];

	tmp[8] = state[8];
	tmp[9] = state[13];
	tmp[10] = state[2];
	tmp[11] = state[7];

	tmp[12] = state[12];
	tmp[13] = state[1];
	tmp[14] = state[6];
	tmp[15] = state[11];

	for (int i = 0; i < 16; i++)
		state[i] = tmp[i];
}
void MixColumns(unsigned char* state)
{
	unsigned char tmp[16];

	tmp[0] = (unsigned char)(mul2[state[0]] ^ mul3[state[1]] ^ state[2] ^ state[3]);
	tmp[1] = (unsigned char)(state[0] ^ mul2[state[1]] ^ mul3[state[2]] ^ state[3]);
	tmp[2] = (unsigned char)(state[0] ^ state[1] ^ mul2[state[2]] ^ mul3[state[3]]);
	tmp[3] = (unsigned char)(mul3[state[0]] ^ state[1] ^ state[2] ^ mul2[state[3]]);

	tmp[4] = (unsigned char)(mul2[state[4]] ^ mul3[state[5]] ^ state[6] ^ state[7]);
	tmp[5] = (unsigned char)(state[4] ^ mul2[state[5]] ^ mul3[state[6]] ^ state[7]);
	tmp[6] = (unsigned char)(state[4] ^ state[5] ^ mul2[state[6]] ^ mul3[state[7]]);
	tmp[7] = (unsigned char)(mul3[state[4]] ^ state[5] ^ state[6] ^ mul2[state[7]]);

	tmp[8] = (unsigned char)(mul2[state[8]] ^ mul3[state[9]] ^ state[10] ^ state[11]);
	tmp[9] = (unsigned char)(state[8] ^ mul2[state[9]] ^ mul3[state[10]] ^ state[11]);
	tmp[10] = (unsigned char)(state[8] ^ state[9] ^ mul2[state[10]] ^ mul3[state[11]]);
	tmp[11] = (unsigned char)(mul3[state[8]] ^ state[9] ^ state[10] ^ mul2[state[11]]);

	tmp[12] = (unsigned char)(mul2[state[12]] ^ mul3[state[13]] ^ state[14] ^ state[15]);
	tmp[13] = (unsigned char)(state[12] ^ mul2[state[13]] ^ mul3[state[14]] ^ state[15]);
	tmp[14] = (unsigned char)(state[12] ^ state[13] ^ mul2[state[14]] ^ mul3[state[15]]);
	tmp[15] = (unsigned char)(mul3[state[12]] ^ state[13] ^ state[14] ^ mul2[state[15]]);

	for (int i = 0; i<16; i++)
		state[i] = tmp[i];

}
void AddRoundKey(unsigned char* state, unsigned char* roundKey)
{
	for (int i = 0; i < 16; i++)
		state[i] ^= roundKey[i];
}
void AES_Encrypt(unsigned char* message, unsigned char* key)
{
	unsigned char state[16];
	for (int i = 0; i < 16; i++)
		state[i] = message[i];

	int numberOfRounds = 9;
	unsigned char expandedKey[176];
	//Initial Round
	KeyExpansion(key,expandedKey);
	AddRoundKey(state,key);

	//Common Rounds
	for (int i = 0; i < numberOfRounds; i++)
	{
		SubBytes(state);
		ShiftRows(state);
		MixColumns(state);
		AddRoundKey(state,expandedKey+(16*(i+1)));
	}
	//Final Round
	SubBytes(state);
	ShiftRows(state);
	AddRoundKey(state,expandedKey+160);

	for (int i = 0; i < 16; i++)
	{
		message[i] = state[i];
	}
}

void _Error_Handler(char * file, int line)
{
  while(1) 
  {
  }
}

