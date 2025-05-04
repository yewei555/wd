/*
 * ov2640.c
 *
 *  Created on: May 3, 2025
 *      Author: 艾斯
 */
#include "ov2640.h"
#include "i2c.h"

#define OV2640_I2C_ADDRESS 0x60 // OV2640 7位地址 0x30，左移1位为 0x60

/* OV2640 JPEG 配置寄存器 */
static const uint8_t ov2640_jpeg_config[][2] = {
    {0xFF, 0x01}, // 切换到寄存器组 1
    {0x12, 0x80}, // 复位
    {0xFF, 0x00}, // 切换到寄存器组 0
    {0xDA, 0x08}, // JPEG 模式
    {0xD7, 0x03}, // JPEG 启用
    {0xDF, 0x02}, // JPEG 配置
    {0x33, 0xA0}, // 输出格式
    {0x3C, 0x00}, // 帧控制
    {0xE1, 0x77}, // JPEG 压缩
    {0x00, 0x00}  // 结束标志
};

void OV2640_Init(void) {
    HAL_Delay(100); // 等待摄像头稳定
    OV2640_JPEGConfig();
}

void OV2640_JPEGConfig(void) {
    uint8_t reg, val;
    for (int i = 0; ov2640_jpeg_config[i][0] != 0x00; i++) {
        reg = ov2640_jpeg_config[i][0];
        val = ov2640_jpeg_config[i][1];
        HAL_I2C_Mem_Write(&hi2c1, OV2640_I2C_ADDRESS, reg, 1, &val, 1, HAL_MAX_DELAY);
    }
}

