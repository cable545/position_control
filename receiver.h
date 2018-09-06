#ifndef _RECEIVER_H
#define _RECEIVER_H

/***************** constants for application software *******************/

// a ic value of CHANNEL_VALUE_COUNT_MIN means a signal length of 1,0 ms
#define CHANNEL_VALUE_COUNT_MIN 1000

// a ic value of CHANNEL_VALUE_MIDDLE means a signal length of 1,5 ms
#define CHANNEL_VALUE_MIDDLE 		1500

// a ic value of CHANNEL_VALUE_COUNT_MAX means a signal length of 2,0 ms
#define CHANNEL_VALUE_COUNT_MAX 2000

// the number of control channels
#define RECEIVER_CHANNEL_CNT		4

// the number of sum-signal values
#define SUM_SIGNAL_VALUES_CNT		RECEIVER_CHANNEL_CNT + 1

#define CAPTURE_VALUE_TARGET_CNT SUM_SIGNAL_VALUES_CNT


/***************** constants for hardware initialization *******************/

#define RECEIVER_RCC_APB_DMA_Periph		RCC_AHB1Periph_DMA2
#define RECEIVER_DMA_STREAM						DMA2_Stream1
#define RECEIVER_DMA_CHANNEL					DMA_Channel_6

#define RECEIVER_GPIO_AF_FUNCTION			GPIO_AF_TIM1
#define RECEIVER_RCC_APB_TIMER_Periph	RCC_APB2Periph_TIM1
#define RECEIVER_TIMER 								TIM1
#define RECEIVER_TIMER_CHANNEL				TIM_Channel_1
#define RECEIVER_TIMER_GPIO_PORT			GPIOA
#define RECEIVER_TIMER_PIN						GPIO_Pin_8

/* 
 * depends on the timer module clock
 * for timer 1, 8, 9, 10, 11 (180 Mhz clock) -> prescaler 180
 * for timer 2, 3, 4, 5, 6, 7, 12, 13, 14 (90 Mhz clock) -> prescaler 90
 */
#define RECEIVER_TIMER_PRESCALER			180

// size for the buffer where the captured values will be stored
#define BUFFER_SIZE 									CAPTURE_VALUE_TARGET_CNT * SUM_SIGNAL_VALUES_CNT

typedef union {
	uint32_t value;
	uint8_t bytes[4];
} ReceiverValue;

class Receiver
{
	private:
		static void initGPIO();
		static void initInterrupt();
		static void initDMAInterrupt();
		static uint32_t initTimer(bool enableInterrupt, bool enableDMA);
		static void initDMA();
		static bool isControlSignalValue(uint32_t value);
		static bool isSyncSignalValue(uint32_t value);
	
	public:
		static uint32_t init(bool enableInterrupt, bool enableDMA);
		static void getCapturedValues();
	
		static uint32_t capturedValue[CAPTURE_VALUE_TARGET_CNT];
		static uint32_t capturedValues[CAPTURE_VALUE_TARGET_CNT];
		static uint32_t values[RECEIVER_CHANNEL_CNT];
		static bool bufferFull;
};

#endif
