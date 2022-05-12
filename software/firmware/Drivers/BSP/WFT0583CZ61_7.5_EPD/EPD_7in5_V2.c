/*****************************************************************************
 * | File      	:	EPD_7in5.c
 * | Author      :   Waveshare team
 * | Function    :   Electronic paper driver
 * | Info        :
 *----------------
 * |	This version:   V2.0
 * | Date        :   2018-11-09
 * | Info        :
 *****************************************************************************
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documnetation files(the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to  whom the Software is
# furished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS OR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.
#
 ******************************************************************************/
#include "stm32l4xx_hal.h"
#include "cmsis_os.h"

#include "EPD_7in5_V2.h"
#include "Debug.h"
#include "config.h"

static SPI_HandleTypeDef hspi1;
static DMA_HandleTypeDef hdma_spi1_tx;
static TaskHandle_t epd_thread_handler;

/**
 * GPIO read and write
 **/
#define DEV_Digital_Write(_pin, _value) HAL_GPIO_WritePin(_pin, _value == 0? GPIO_PIN_RESET:GPIO_PIN_SET)
#define DEV_Digital_Read(_pin) HAL_GPIO_ReadPin(_pin)

void DEV_SPI_WriteByte(uint8_t value);
void DEV_SPI_WriteBuffer(uint8_t *buffer, uint32_t length);

static void EPD_SendCommand(uint8_t Reg);
static void EPD_SendData(uint8_t Data);
static void EPD_SendBuffer(uint8_t *buffer, uint32_t count);
static void EPD_WaitUntilIdle(void) ;

#if EPD_CONFIG_LUT==1
static void EPD_ConfigLUT(const uint8_t* lut_vcom, const uint8_t* lut_ww, const uint8_t* lut_bw, const uint8_t* lut_wb, const uint8_t* lut_bb);
#endif

//int DEV_Module_Init(void);
//void DEV_Module_Exit(void);

uint8_t imageFramebuffer[(EPD_7IN5_V2_WIDTH * EPD_7IN5_V2_HEIGHT) / 8] __attribute__((section (".framebuffer")));

uint8_t Voltage_Frame_7IN5_V2[]={
		0x6, 0x3F, 0x3F, 0x11, 0x24, 0x7, 0x17,
};

#if EPD_CONFIG_LUT==1
const uint8_t LUT_VCOM_7IN5_V2[]={
		0x0,	0xF,	0xF,	0x0,	0x0,	0x1,
		0x0,	0xF,	0x1,	0xF,	0x1,	0x2,
		0x0,	0xF,	0xF,	0x0,	0x0,	0x1,
		0x0,	0x0,	0x0,	0x0,	0x0,	0x0,
		0x0,	0x0,	0x0,	0x0,	0x0,	0x0,
		0x0,	0x0,	0x0,	0x0,	0x0,	0x0,
		0x0,	0x0,	0x0,	0x0,	0x0,	0x0,
};						

const uint8_t LUT_WW_7IN5_V2[]={
		0x10,	0xF,	0xF,	0x0,	0x0,	0x1,
		0x84,	0xF,	0x1,	0xF,	0x1,	0x2,
		0x20,	0xF,	0xF,	0x0,	0x0,	0x1,
		0x0,	0x0,	0x0,	0x0,	0x0,	0x0,
		0x0,	0x0,	0x0,	0x0,	0x0,	0x0,
		0x0,	0x0,	0x0,	0x0,	0x0,	0x0,
		0x0,	0x0,	0x0,	0x0,	0x0,	0x0,
};

const uint8_t LUT_BW_7IN5_V2[]={
		0x10,	0xF,	0xF,	0x0,	0x0,	0x1,
		0x84,	0xF,	0x1,	0xF,	0x1,	0x2,
		0x20,	0xF,	0xF,	0x0,	0x0,	0x1,
		0x0,	0x0,	0x0,	0x0,	0x0,	0x0,
		0x0,	0x0,	0x0,	0x0,	0x0,	0x0,
		0x0,	0x0,	0x0,	0x0,	0x0,	0x0,
		0x0,	0x0,	0x0,	0x0,	0x0,	0x0,
};

const uint8_t LUT_WB_7IN5_V2[]={
		0x80,	0xF,	0xF,	0x0,	0x0,	0x1,
		0x84,	0xF,	0x1,	0xF,	0x1,	0x2,
		0x40,	0xF,	0xF,	0x0,	0x0,	0x1,
		0x0,	0x0,	0x0,	0x0,	0x0,	0x0,
		0x0,	0x0,	0x0,	0x0,	0x0,	0x0,
		0x0,	0x0,	0x0,	0x0,	0x0,	0x0,
		0x0,	0x0,	0x0,	0x0,	0x0,	0x0,
};

const uint8_t LUT_BB_7IN5_V2[]={
		0x80,	0xF,	0xF,	0x0,	0x0,	0x1,
		0x84,	0xF,	0x1,	0xF,	0x1,	0x2,
		0x40,	0xF,	0xF,	0x0,	0x0,	0x1,
		0x0,	0x0,	0x0,	0x0,	0x0,	0x0,
		0x0,	0x0,	0x0,	0x0,	0x0,	0x0,
		0x0,	0x0,	0x0,	0x0,	0x0,	0x0,
		0x0,	0x0,	0x0,	0x0,	0x0,	0x0,
};
#endif

/**
 * @brief SPI MSP Initialization
 * This function configures the hardware resources used in this example
 * @param hspi: SPI handle pointer
 * @retval None
 */
void HAL_SPI_MspInit(SPI_HandleTypeDef* hspi) {
	GPIO_InitTypeDef GPIO_InitStruct = {0};

	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_DMA1_CLK_ENABLE();

	/**SPI1 GPIO Configuration
    PA1     ------> SPI1_SCK
    PA7     ------> SPI1_MOSI
	 */
	GPIO_InitStruct.Pin = SPI_MISO_Pin|SPI_MOSI_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_InitStruct.Alternate = GPIO_AF5_SPI1;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	/* SPI1 DMA Init */
	/* SPI1_TX Init */
	hdma_spi1_tx.Instance = DMA1_Channel3;
	hdma_spi1_tx.Init.Request = DMA_REQUEST_1;
	hdma_spi1_tx.Init.Direction = DMA_MEMORY_TO_PERIPH;
	hdma_spi1_tx.Init.PeriphInc = DMA_PINC_DISABLE;
	hdma_spi1_tx.Init.MemInc = DMA_MINC_ENABLE;
	hdma_spi1_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
	hdma_spi1_tx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
	hdma_spi1_tx.Init.Mode = DMA_NORMAL;
	hdma_spi1_tx.Init.Priority = DMA_PRIORITY_MEDIUM;

	HAL_DMA_Init(&hdma_spi1_tx);

	__HAL_LINKDMA(hspi,hdmatx,hdma_spi1_tx);
}

/**
 * @brief SPI MSP De-Initialization
 * This function freeze the hardware resources used in this example
 * @param hspi: SPI handle pointer
 * @retval None
 */
void HAL_SPI_MspDeInit(SPI_HandleTypeDef* hspi) {
	/* Peripheral clock disable */
	__HAL_RCC_SPI1_CLK_DISABLE();

	/**SPI1 GPIO Configuration
    PA1     ------> SPI1_SCK
    PA7     ------> SPI1_MOSI
	 */
	HAL_GPIO_DeInit(GPIOA, GPIO_PIN_1|GPIO_PIN_7);

	/* SPI1 DMA DeInit */
	HAL_DMA_DeInit(hspi->hdmatx);
}

void DEV_SPI_WriteByte(uint8_t value) {
	HAL_SPI_Transmit(&hspi1, &value, 1, 100);
}

void DEV_SPI_WriteBuffer(uint8_t *buffer, uint32_t length) {
	//HAL_SPI_Transmit(&hspi1, buffer, length, 1000);
	HAL_SPI_Transmit_DMA(&hspi1, buffer, length);
}

/******************************************************************************
function :	Initialize I/O and SPI peripheral
parameter:
 ******************************************************************************/
EPD_Status EPD_HWInit(void) {
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	EPD_Status status = EPD_OK;

	/* GPIO Ports Clock Enable */
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_SPI1_CLK_ENABLE();

	__HAL_RCC_SPI1_FORCE_RESET();
	__HAL_RCC_SPI1_RELEASE_RESET();

	HAL_GPIO_DeInit(GPIOA, RST_Pin|DC_Pin|SPI_CS_Pin|SPI_MISO_Pin|SPI_MOSI_Pin);
	HAL_GPIO_DeInit(GPIOB, BUSY_Pin);

	/*Configure GPIO pin Output Level */
	//HAL_GPIO_WritePin(GPIOA, RST_Pin|DC_Pin, GPIO_PIN_RESET);

	/*Configure GPIO pin Output Level */
	//HAL_GPIO_WritePin(SPI_CS_GPIO_Port, SPI_CS_Pin, GPIO_PIN_SET);

	/*Configure GPIO pins : RST_Pin DC_Pin SPI_CS_Pin */
	GPIO_InitStruct.Pin = RST_Pin|DC_Pin|SPI_CS_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	/*Configure GPIO pins : BUSY_Pin*/
	GPIO_InitStruct.Pin = BUSY_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	/* Configure GPIO lines */
	DEV_Digital_Write(EPD_DC_PIN, 0);
	DEV_Digital_Write(EPD_CS_PIN, 1);
	//DEV_Digital_Write(EPD_RST_PIN, 1);
	DEV_Digital_Write(EPD_RST_PIN, 0);

	HAL_NVIC_SetPriority(EXTI1_IRQn, 5, 0);
	__HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_1);
	HAL_NVIC_EnableIRQ(EXTI1_IRQn);

	/* Configure SPI peripheral */
	hspi1.Instance = SPI1;
	hspi1.Init.Mode = SPI_MODE_MASTER;
	hspi1.Init.Direction = SPI_DIRECTION_1LINE;
	hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
	hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
	hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
	hspi1.Init.NSS = SPI_NSS_SOFT;
	hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
	hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
	hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
	hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
	hspi1.Init.CRCPolynomial = 7;
	hspi1.Init.CRCLength = SPI_CRC_LENGTH_DATASIZE;
	hspi1.Init.NSSPMode = SPI_NSS_PULSE_DISABLE;

	//HAL_SPI_DeInit(&hspi1);
	if (HAL_SPI_Init(&hspi1) != HAL_OK)
		status = EPD_ERROR_HW;

	HAL_SPI_MspInit(&hspi1);
	return status;
}

/******************************************************************************
function :	Initialize the e-Paper register
parameter:
 ******************************************************************************/
EPD_Status EPD_SWInit(void) {
	EPD_Status status = EPD_OK;

	if ((status = EPD_Reset())==EPD_OK) {
		EPD_SendCommand(0x01);  // power setting
		EPD_SendData(0x17);  // 1-0=11: internal power
		EPD_SendData(*(Voltage_Frame_7IN5_V2+6));  // VGH&VGL
		EPD_SendData(*(Voltage_Frame_7IN5_V2+1));  // VSH
		EPD_SendData(*(Voltage_Frame_7IN5_V2+2));  //  VSL
		EPD_SendData(*(Voltage_Frame_7IN5_V2+3));  //  VSHR

		EPD_SendCommand(0x82);  // VCOM DC Setting
		EPD_SendData(*(Voltage_Frame_7IN5_V2+4));  // VCOM

		EPD_SendCommand(0x06);  // Booster Setting
		EPD_SendData(0x27);
		EPD_SendData(0x27);
		EPD_SendData(0x2F);
		EPD_SendData(0x17);

		EPD_SendCommand(0x30);   // OSC Setting
		EPD_SendData(*(Voltage_Frame_7IN5_V2+0));  // 2-0=100: N=4  ; 5-3=111: M=7  ;  3C=50Hz     3A=100HZ

		EPD_SendCommand(0x04); //POWER ON
		//HAL_Delay(100);
		//vTaskDelay(msToTicks(100));
		EPD_WaitUntilIdle();

		EPD_SendCommand(0X00);			//PANNEL SETTING
		//EPD_SendData(0x3F);   //KW-3f   KWR-2F	BWROTP 0f	BWOTP 1f
		EPD_SendData(0x1F);

		EPD_SendCommand(0x61);        	//tres
		EPD_SendData(0x03);		//source 800
		EPD_SendData(0x20);
		EPD_SendData(0x01);		//gate 480
		EPD_SendData(0xE0);

		EPD_SendCommand(0X15);
		EPD_SendData(0x00);

		EPD_SendCommand(0X50);			//VCOM AND DATA INTERVAL SETTING
		EPD_SendData(0x10);
		EPD_SendData(0x00);

		EPD_SendCommand(0X60);			//TCON SETTING
		EPD_SendData(0x22);

		EPD_SendCommand(0x65);  // Resolution setting
		EPD_SendData(0x00);
		EPD_SendData(0x00);//800*480
		EPD_SendData(0x00);
		EPD_SendData(0x00);

#if EPD_CONFIG_LUT==1
		EPD_ConfigLUT(LUT_VCOM_7IN5_V2, LUT_WW_7IN5_V2, LUT_BW_7IN5_V2, LUT_WB_7IN5_V2, LUT_BB_7IN5_V2);
#endif
	}

	return status;
}

static void EPD_usDelay(uint32_t val) {
	volatile uint32_t tmp;

	for (tmp=val; tmp!=0; tmp--);
}

/******************************************************************************
function :	 Display hardware reset
parameter:
 ******************************************************************************/
EPD_Status EPD_Reset(void) {
	//DEV_Digital_Write(EPD_RST_PIN, 1);
	//HAL_Delay(200);
	//vTaskDelay(msToTicks(200));
	DEV_Digital_Write(EPD_RST_PIN, 0);
	//HAL_Delay(2);
	//vTaskDelay(msToTicks(100));
	EPD_usDelay(25); // ~ 50 µs
	DEV_Digital_Write(EPD_RST_PIN, 1);
	//HAL_Delay(200);
	//vTaskDelay(msToTicks(100));

	return EPD_OK;
}

/******************************************************************************
function :	Enter sleep mode
parameter:
 ******************************************************************************/
void EPD_Sleep(void) {
	EPD_SendCommand(0x02);  	//power off
	EPD_WaitUntilIdle();

	EPD_SendCommand(0x07);  	//deep sleep
	EPD_SendData(0xA5);
}

/******************************************************************************
function :	Shutdown (zero power) display
parameter:
 ******************************************************************************/
void EPD_ShutDown(void) {
	DEV_Digital_Write(EPD_DC_PIN, 0);
	DEV_Digital_Write(EPD_CS_PIN, 1);

	//close 5V
	DEV_Digital_Write(EPD_RST_PIN, 0);
}

/******************************************************************************
function :	send command
parameter:
     Reg : Command register
 ******************************************************************************/
static void EPD_SendCommand(uint8_t Reg)
{
	DEV_Digital_Write(EPD_DC_PIN, 0);
	DEV_Digital_Write(EPD_CS_PIN, 0);
	DEV_SPI_WriteByte(Reg);
	DEV_Digital_Write(EPD_CS_PIN, 1);
}

/******************************************************************************
function :	send data
parameter:
    Data : Write data
 ******************************************************************************/
static void EPD_SendData(uint8_t Data)
{
	DEV_Digital_Write(EPD_DC_PIN, 1);
	DEV_Digital_Write(EPD_CS_PIN, 0);
	DEV_SPI_WriteByte(Data);
	DEV_Digital_Write(EPD_CS_PIN, 1);
}

/******************************************************************************
function :	SendBuffer
parameter:
    Data : Write buffer
 ******************************************************************************/
static void EPD_SendBuffer(uint8_t *buffer, uint32_t count) {
	epd_thread_handler= xTaskGetCurrentTaskHandle();

	DEV_Digital_Write(EPD_DC_PIN, 1);
	DEV_Digital_Write(EPD_CS_PIN, 0);

	DEV_SPI_WriteBuffer(buffer, count);

	ulTaskNotifyTake( pdTRUE, msToTicks(500)); // timeout to 500ms
	//while (!__HAL_SPI_GET_FLAG(&hspi1, SPI_FLAG_TXE)); // Wait last char to be sent

	DEV_Digital_Write(EPD_CS_PIN, 1);
}

/******************************************************************************
function :	Wait until the busy_pin is LOW
parameter:
 ******************************************************************************/
static void EPD_WaitUntilIdle(void) {
	uint32_t ulNotificationValue;
	uint8_t end_busy=0;

	__HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_1);
	epd_thread_handler= xTaskGetCurrentTaskHandle();

	while (!end_busy) {
		ulNotificationValue= ulTaskNotifyTake( pdTRUE, msToTicks(100)); // timeout to 100ms
		if (ulNotificationValue!= 1) {
			//Check if busy is returned to high state
			if ((DEV_Digital_Read(EPD_BUSY_PIN) & 0x01))
				end_busy = 1;
		} else
			end_busy = 1;
	}

	epd_thread_handler = NULL;
}

#if EPD_CONFIG_LUT==1
static void EPD_ConfigLUT(const uint8_t* lut_vcom, const uint8_t* lut_ww, const uint8_t* lut_bw, const uint8_t* lut_wb, const uint8_t* lut_bb)
{
	uint8_t count;

	EPD_SendCommand(0x20); //VCOM
	for(count=0; count<42; count++)
		EPD_SendData(lut_vcom[count]);

	EPD_SendCommand(0x21); //LUTBW
	for(count=0; count<42; count++)
		EPD_SendData(lut_ww[count]);

	EPD_SendCommand(0x22); //LUTBW
	for(count=0; count<42; count++)
		EPD_SendData(lut_bw[count]);

	EPD_SendCommand(0x23); //LUTWB
	for(count=0; count<42; count++)
		EPD_SendData(lut_wb[count]);

	EPD_SendCommand(0x24); //LUTBB
	for(count=0; count<42; count++)
		EPD_SendData(lut_bb[count]);
}
#endif

/******************************************************************************
function :	Turn On Display
parameter:
 ******************************************************************************/
void EPD_RefreshDisplay(void) {
	EPD_SendCommand(0x12);			//DISPLAY REFRESH
	//HAL_Delay(100);	        //!!!The delay here is necessary, 200uS at least!!!
	EPD_WaitUntilIdle();
}

/******************************************************************************
function :	Get framebuffer address
parameter:
 ******************************************************************************/
uint8_t* EPD_GetFramebuffer() {
	return imageFramebuffer;
}

/******************************************************************************
function :	Sends the image buffer in RAM to e-Paper and displays (no inversion)
parameter:
 ******************************************************************************/
void EPD_WritePicture(const uint8_t *buffer, uint32_t length) {
	EPD_SendCommand(0x13);

	EPD_SendBuffer((uint8_t*) buffer, length);

	EPD_RefreshDisplay();
}

/**
 * @brief This function handles EXTI line1 interrupt (Display Busy).
 */
void EXTI1_IRQHandler(void) {
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_1);

	if (epd_thread_handler != NULL) {
		/* Notify the task that an event has been emitted. */
		vTaskNotifyGiveFromISR(epd_thread_handler, &xHigherPriorityTaskWoken );

		/* There are no more eventin progress, so no tasks to notify. */
		epd_thread_handler = NULL;

		/* If xHigherPriorityTaskWoken is now set to pdTRUE then a context switch
			    should be performed to ensure the interrupt returns directly to the highest
			    priority task.  The macro used for this purpose is dependent on the port in
			    use and may be called portEND_SWITCHING_ISR(). */
		portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
	}
}

/**
 * @brief This function handles DMA1 channel3 global interrupt.
 */
void DMA1_Channel3_IRQHandler(void) {
	HAL_DMA_IRQHandler(hspi1.hdmatx);
}

void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi)
{
	/* Turn LED3 on: Transfer in transmission/reception process is complete */
	//BSP_LED_On(LED3);
	//wTransferState = TRANSFER_COMPLETE;
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;

	if (epd_thread_handler != NULL) {
		/* Notify the task that an event has been emitted. */
		vTaskNotifyGiveFromISR(epd_thread_handler, &xHigherPriorityTaskWoken );

		/* There are no more eventin progress, so no tasks to notify. */
		epd_thread_handler = NULL;

		/* If xHigherPriorityTaskWoken is now set to pdTRUE then a context switch
				    should be performed to ensure the interrupt returns directly to the highest
				    priority task.  The macro used for this purpose is dependent on the port in
				    use and may be called portEND_SWITCHING_ISR(). */
		portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
	}
}

