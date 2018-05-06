#ifndef __MAIN_H
#define __MAIN_H

#include <stdio.h>
#include <stdarg.h>
#include <math.h>
#include "stm32f4xx.h"
#include "rcc.h"
#include "usart.h"
#include "gpio.h"
#include "i2c.h"
#include "test.h"
#include "debug.h"
#include "fram.h"
#include "maths.h"

#include "imu_defs.h"
#include "imu_settings.h"
#include "imu.h"

#if defined(MPU9250_68) || defined(MPU9250_69)
#include "imu_mpu_9250.h"
#endif

#endif
