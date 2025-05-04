/*
 * dcmi.c
 *
 *  Created on: May 3, 2025
 *      Author: 艾斯
 */
#include "dcmi.h"
#include "main.h" // 包含 Error_Handler 和 IMAGE_BUFFER_SIZE
#include "cmsis_os.h" // 包含 osSemaphoreId_t
#include "stm32h7xx_hal_dma.h" // 包含 HAL_DMA_GetCounter
#include "stdio.h" // 包含 printf
#include "usart.h"

extern osSemaphoreId_t CaptureDoneSemHandle;
extern uint8_t image_buffer[];
extern volatile uint32_t image_size;

DCMI_HandleTypeDef hdcmi;
DMA_HandleTypeDef hdma_dcmi;

void MX_DCMI_Init(void) {
    hdcmi.Instance = DCMI;
    hdcmi.Init.SynchroMode = DCMI_SYNCHRO_HARDWARE;
    hdcmi.Init.PCKPolarity = DCMI_PCKPOLARITY_RISING;
    hdcmi.Init.VSPolarity = DCMI_VSPOLARITY_LOW;
    hdcmi.Init.HSPolarity = DCMI_HSPOLARITY_LOW;
    hdcmi.Init.CaptureRate = DCMI_CR_ALL_FRAME;
    hdcmi.Init.ExtendedDataMode = DCMI_EXTEND_DATA_8B;
    hdcmi.Init.JPEGMode = DCMI_JPEG_ENABLE;
    if (HAL_DCMI_Init(&hdcmi) != HAL_OK) {
        Error_Handler();
    }

    __HAL_LINKDMA(&hdcmi, DMA_Handle, hdma_dcmi);
    hdma_dcmi.Instance = DMA2_Stream1;
    hdma_dcmi.Init.Request = DMA_REQUEST_DCMI;
    hdma_dcmi.Init.Direction = DMA_PERIPH_TO_MEMORY;
    hdma_dcmi.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_dcmi.Init.MemInc = DMA_MINC_ENABLE;
    hdma_dcmi.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
    hdma_dcmi.Init.MemDataAlignment = DMA_MDATAALIGN_WORD;
    hdma_dcmi.Init.Mode = DMA_NORMAL;
    hdma_dcmi.Init.Priority = DMA_PRIORITY_HIGH;
    hdma_dcmi.Init.FIFOMode = DMA_FIFOMODE_ENABLE;
    hdma_dcmi.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_FULL;
    hdma_dcmi.Init.MemBurst = DMA_MBURST_SINGLE;
    hdma_dcmi.Init.PeriphBurst = DMA_PBURST_SINGLE;
    if (HAL_DMA_Init(&hdma_dcmi) != HAL_OK) {
        Error_Handler();
    }

    HAL_NVIC_SetPriority(DCMI_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(DCMI_IRQn);
}

void HAL_DCMI_FrameEventCallback(DCMI_HandleTypeDef *hdcmi) {
    HAL_DCMI_Stop(hdcmi);
    image_size = IMAGE_BUFFER_SIZE - __HAL_DMA_GET_COUNTER(hdcmi->DMA_Handle) * 4;
    printf("Capture complete, size: %lu bytes\n", image_size);
    osSemaphoreRelease(CaptureDoneSemHandle);
    HAL_UART_Transmit(&huart1, image_buffer, image_size, HAL_MAX_DELAY);
    printf("Image uploaded\n");
}
