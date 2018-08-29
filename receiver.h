#ifndef _RECEIVER_H
#define _RECEIVER_H

#define RECEIVER_GPIO_AF_FUNCTION			GPIO_AF_TIM1

#define RECEIVER_RCC_APB_TIMER_Periph	RCC_APB2Periph_TIM1
#define RECEIVER_TIMER 								TIM1
#define RECEIVER_TIMER_CHANNEL				TIM_Channel_1
#define RECEIVER_TIMER_GPIO_PORT			GPIOA
#define RECEIVER_TIMER_PIN						GPIO_Pin_8

/* depends on the timer module clock
 * for timer 1, 8, 9, 10, 11 (180 Mhz clock) -> prescaler 180
 * for timer 2, 3, 4, 5, 6, 7, 12, 13, 14 (90 Mhz clock) -> prescaler 90
 */
#define RECEIVER_TIMER_PRESCALER			180 

class Receiver
{
	private:
		static void gpioInit();
	
	public:
		static void init();
	
	static uint32_t capturredValue[4];
};

#endif
