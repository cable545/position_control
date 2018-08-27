#include "main.h"

uint32_t Receiver::capturredValue;

extern "C"
{
   void TIM1_CC_IRQHandler()
	{
		if(Timer::getInterruptStatus(RECEIVER_TIMER, TIM_IT_CC1) == SET)
		{
			Timer::clearInterruptPendingBit(RECEIVER_TIMER, TIM_IT_CC1);
			RECEIVER_TIMER->CNT = 0x0;
			Receiver::capturredValue = Timer::getCapture1(RECEIVER_TIMER);
			
			
			Debug::print("%i\n", Receiver::capturredValue);	
		}
	}
}

void Receiver::gpioInit()
{
	// configurates GPIO port
	TM_GPIO_InitAlternate(
		RECEIVER_TIMER_GPIO_PORT, 
		RECEIVER_TIMER_PIN,
		TM_GPIO_OType_PP,
		TM_GPIO_PuPd_NOPULL,
		TM_GPIO_Speed_High,
		RECEIVER_GPIO_AF_FUNCTION
	);
}

void Receiver::init()
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStruct;
	TIM_ICInitTypeDef TIM_ICInitStruct;
	NVIC_InitTypeDef NVIC_InitStruct;
	
	// enables the TIMER clock
	Timer::enableClock(RECEIVER_TIMER);
	gpioInit();
	
	NVIC_InitStruct.NVIC_IRQChannel = TIM1_CC_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStruct.NVIC_IRQChannelSubPriority = 1;
  NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
  Interrupt::init(&NVIC_InitStruct);
	
	Timer::structInit(&TIM_TimeBaseStruct);
	TIM_TimeBaseStruct.TIM_Prescaler = 45000;
	
	Timer::icStructInit(&TIM_ICInitStruct);
	TIM_ICInitStruct.TIM_Channel = RECEIVER_TIMER_CHANNEL;
	TIM_ICInitStruct.TIM_ICPolarity = TIM_ICPolarity_Falling;
	TIM_ICInitStruct.TIM_ICSelection = TIM_ICSelection_DirectTI;
	TIM_ICInitStruct.TIM_ICPrescaler = TIM_ICPSC_DIV1;
	
	Timer::icInit(RECEIVER_TIMER, &TIM_ICInitStruct);
	
	Timer::enable(RECEIVER_TIMER);
	
	Timer::enableInterrupt(RECEIVER_TIMER, TIM_IT_CC1);
}
