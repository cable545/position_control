#ifndef __MAIN_H
#define __MAIN_H

#include <stdio.h>
#include <stdarg.h>
#include <math.h>
#include <string.h>
#include "stm32f4xx.h"
#include "rcc.h"
#include "usart.h"
#include "gpio.h"
#include "i2c.h"
#include "debug.h"
#include "fram.h"
#include "maths.h"
#include "system.h"
#include "buzzer.h"
#include "timer.h"
#include "interrupt.h"
#include "dma.h"

#include "imu_defs.h"
#include "imu_settings.h"
#include "imu.h"
#include "fusion.h"

#include "receiver.h"

#include "unity_run.h"
#include "unity/unity.h"
#include <setjmp.h>

#define EXIT_SUCCESS 									0
#define	I2C_START_ERROR								1
#define	I2C_READ_ACK_ERROR						2
#define	I2C_READ_NACK_ERROR						3
#define I2C_WRITE_ERROR								4
#define	I2C_STOP_ERROR								5
#define BUZZER_NOT_INITIALIZED				6
#define RECEIVER_INT_AND_DMA_ENABLED	7

extern const char* RETURN_VALUES[];

#if defined(MPU9250_68) || defined(MPU9250_69)
#include "imu_mpu_9250.h"
#endif

#ifdef BNO080_IMU
#include "imu_bno080.h"
#endif

#endif
