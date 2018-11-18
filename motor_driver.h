#ifndef _MOTOR_DRIVER_H
#define _MOTOR_DRIVER_H

/***************** constants for hardware initialization *******************/

#define MOT_DRIV_GPIO_AF_FUNCTION				GPIO_AF_TIM3
#define MOT_DRIV_APB_TIM_Periph					RCC_APB1Periph_TIM3
#define MOT_DRIV_TIM										TIM3
#define MOT_DRIV_TIM_GPIOA_PORT					GPIOA
#define MOT_DRIV_TIM_GPIOA_CH1_PIN			GPIO_Pin_6
#define MOT_DRIV_TIM_GPIOC_PORT					GPIOC
#define MOT_DRIV_TIM_GPIOC_CH2_PIN			GPIO_Pin_7
#define MOT_DRIV_TIM_GPIOC_CH3_PIN			GPIO_Pin_8
#define MOT_DRIV_TIM_GPIOC_CH4_PIN			GPIO_Pin_9

#define MOT_DRIV_TIM_CLK								1000000
#define MOT_DRIV_TIM_AUTO_RELOAD_VALUE	(MOT_DRIV_TIM_CLK / 500.0f) - 1

#define PWM_1_0_MS											1000
#define PWM_1_5_MS											1500
#define PWM_2_0_MS											2000

#define PWM_MIN													PWM_1_0_MS
#define PWM_MAX													PWM_1_5_MS

#define LIMIT_OUTPUT										1

#define MOTOR_A 												0x1
#define MOTOR_B 												0x2
#define MOTOR_C 												0x4
#define MOTOR_D 												0x8

//#define MOTOR_DRIVER_2_TIMER_GPIO_PORT
//#define MOTOR_DRIVER_3_TIMER_GPIO_PORT
//#define MOTOR_DRIVER_4_TIMER_GPIO_PORT

class MotorDriver
{
	private:
		TIM_TypeDef* timerPointer;
		bool limitOutput;
		
		void initTimer();
		void initGPIO();
	
	public:
		MotorDriver(TIM_TypeDef* timerPointer);
		void init();
		void setPulse(uint32_t pulse, uint8_t motors);
};

#endif
