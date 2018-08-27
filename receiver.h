#ifndef _RECEIVER_H
#define _RECEIVER_H

#define RECEIVER_GPIO_AF_FUNCTION			GPIO_AF_TIM1

#define RECEIVER_RCC_APB_TIMER_Periph	RCC_APB2Periph_TIM1
#define RECEIVER_TIMER 								TIM1
#define RECEIVER_TIMER_CHANNEL				TIM_Channel_1
#define RECEIVER_TIMER_GPIO_PORT			GPIOA
#define RECEIVER_TIMER_PIN						GPIO_Pin_8

class Receiver
{
	private:
		static void gpioInit();
	
	public:
		static void init();
	
	static uint32_t capturredValue;
};

#endif
