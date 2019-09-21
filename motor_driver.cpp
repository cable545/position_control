#include "main.h"

MotorDriver& MotorDriver::instance()
{
	static MotorDriver _instance;
	
	return _instance;
}

void MotorDriver::init(TIM_TypeDef* timerPointer)
{
	this->timerPointer = timerPointer;
	this->limitOutput = LIMIT_OUTPUT == 1 ? true : false;
	this->initGPIO();
	this->initTimer();
}

void MotorDriver::initTimer()
{
	initGPIO();
	Timer::enableClock(timerPointer);
	
	/*
	To get TIM3 counter clock at 1 MHz, the prescaler is computed as follows:
	TIMxCLK depends on the chosen timer module
	
       Prescaler = (TIMxCLK / TIMx counter clock) - 1
       Prescaler = ((SystemCoreClock / 2) /1 MHz) - 1
                                              
    To get TIM3 output clock at 30 KHz, the period (ARR)) is computed as follows:
       ARR = (TIM3 counter clock / TIM3 output clock) - 1
           = 665
	*/
	
	// compute prescaler value
	if(timerPointer == TIM1 || timerPointer == TIM8 ||
		 timerPointer == TIM9 || timerPointer == TIM10 ||
		 timerPointer == TIM11)
	{
		timerPointer->PSC = (SystemCoreClock / MOT_DRIV_TIM_CLK) - 1;
	}
	else
		timerPointer->PSC = ((SystemCoreClock / 2) / MOT_DRIV_TIM_CLK) - 1;
	
	// auto reload value
	timerPointer->ARR = MOT_DRIV_TIM_AUTO_RELOAD_VALUE;
	
	// PWM2 output mode channel 1
	timerPointer->CCMR1 |= TIM_CCMR1_OC1M_0 | TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1M_2;
	// PWM2 output mode channel 2
	timerPointer->CCMR1 |= TIM_CCMR1_OC2M_0 | TIM_CCMR1_OC2M_1 | TIM_CCMR1_OC2M_2;
	// PWM2 output mode channel 3
	timerPointer->CCMR2 |= TIM_CCMR2_OC3M_0 | TIM_CCMR2_OC3M_1 | TIM_CCMR2_OC3M_2;
	// PWM2 output mode channel 4
	timerPointer->CCMR2 |= TIM_CCMR2_OC4M_0 | TIM_CCMR2_OC4M_1 | TIM_CCMR2_OC4M_2;
	
	// Duty Cycle 75 % means 1.5ms
	timerPointer->CCR1 = PWM_MIN;
	timerPointer->CCR2 = PWM_MIN;
	timerPointer->CCR3 = PWM_MIN;
	timerPointer->CCR4 = PWM_MIN;
	
	// switch polarity and enable timer channel 4
	timerPointer->CCER |= TIM_CCER_CC1P | TIM_CCER_CC1E |
												TIM_CCER_CC2P | TIM_CCER_CC2E |
												TIM_CCER_CC3P | TIM_CCER_CC3E |
												TIM_CCER_CC4P | TIM_CCER_CC4E;

	// start the timer
	timerPointer->CR1 |= TIM_CR1_CEN;
}

void MotorDriver::initGPIO()
{
	TM_GPIO_InitAlternate(
		MOT_DRIV_TIM_GPIOC_PORT, 
		MOT_DRIV_TIM_GPIOC_CH2_PIN | MOT_DRIV_TIM_GPIOC_CH3_PIN | MOT_DRIV_TIM_GPIOC_CH4_PIN,
		TM_GPIO_OType_PP,
		TM_GPIO_PuPd_UP,
		TM_GPIO_Speed_High,
		MOT_DRIV_GPIO_AF_FUNCTION
	);
	
	TM_GPIO_InitAlternate(
		MOT_DRIV_TIM_GPIOA_PORT, 
		MOT_DRIV_TIM_GPIOA_CH1_PIN,
		TM_GPIO_OType_PP,
		TM_GPIO_PuPd_UP,
		TM_GPIO_Speed_High,
		MOT_DRIV_GPIO_AF_FUNCTION
	);
}

void MotorDriver::setPulse(uint32_t pulse, uint8_t motors)
{
	if(pulse >= PWM_MAX && this->limitOutput == true) pulse = PWM_MAX;
	
	if((motors & MOTOR_A) == MOTOR_A) timerPointer->CCR1 = pulse;
	if((motors & MOTOR_B) == MOTOR_B) timerPointer->CCR2 = pulse;
	if((motors & MOTOR_C) == MOTOR_C) timerPointer->CCR3 = pulse;
	if((motors & MOTOR_D) == MOTOR_D) timerPointer->CCR4 = pulse;
}

