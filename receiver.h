#ifndef _RECEIVER_H
#define _RECEIVER_H

/***************** constants for application software *******************/

// a ic value of CHANNEL_VALUE_COUNT_MIN means a signal length of 1,0 ms
#define CHANNEL_VALUE_COUNT_MIN				1000

// a ic value of CHANNEL_VALUE_MIDDLE means a signal length of 1,5 ms
#define CHANNEL_VALUE_MIDDLE					1500

// a ic value of CHANNEL_VALUE_COUNT_MAX means a signal length of 2,0 ms
#define CHANNEL_VALUE_COUNT_MAX				2000

// the number of control channels
#define RECEIVER_CHANNEL_CNT					4

// the number of sum-signal values which comes from the GR-16 modul
// in one period
#define SUM_SIGNAL_VALUES_CNT					RECEIVER_CHANNEL_CNT + 1

// the number of values which should be captured 
#define CAPTURE_VALUE_TARGET_CNT			SUM_SIGNAL_VALUES_CNT

#define ROLL_ANGLE_MIN  							-35
#define ROLL_ANGLE_MAX  							35

#define PITCH_ANGLE_MIN  							-35
#define PITCH_ANGLE_MAX  							35

#define YAW_RATE_MIN 									-150
#define YAW_RATE_MAX 									150

/***************** constants for hardware initialization *******************/

#define RECEIVER_RCC_APB_DMA_Periph		RCC_AHB1Periph_DMA1
#define RECEIVER_DMA_STREAM						DMA1_Stream0
#define RECEIVER_DMA_CHANNEL					DMA_Channel_2
#define RECEIVER_DMA_IRQ_NR						DMA1_Stream0_IRQn
#define RECEIVER_DMA_IRQ_SC						DMA_IT_TCIF0

#define RECEIVER_GPIO_AF_FUNCTION			GPIO_AF_TIM4
#define RECEIVER_RCC_APB_TIMER_Periph	RCC_APB1Periph_TIM4
#define RECEIVER_TIMER 								TIM4
#define RECEIVER_TIMER_CHANNEL				TIM_Channel_1
#define RECEIVER_TIMER_GPIO_PORT			GPIOB
#define RECEIVER_TIMER_PIN						GPIO_Pin_6

/* 
 * depends on the timer module clock
 * for timer 1, 8, 9, 10, 11 (180 Mhz clock) -> prescaler 180
 * for timer 2, 3, 4, 5, 6, 7, 12, 13, 14 (90 Mhz clock) -> prescaler 90
 */
#define RECEIVER_TIMER_PRESCALER			90

// size for the buffer where the captured values will be stored
#define BUFFER_SIZE 									CAPTURE_VALUE_TARGET_CNT * SUM_SIGNAL_VALUES_CNT

typedef struct
{
	uint32_t elevator;
	uint32_t roll;
	uint32_t pitch;
	uint32_t yaw;
} TargetValues;

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
		static void fillTargetValues();
	
	public:
		static uint32_t init(bool enableInterrupt, bool enableDMA);
		static bool processCapturedValues();
	
		static uint32_t capturedValue[CAPTURE_VALUE_TARGET_CNT];
		static uint32_t capturedValues[CAPTURE_VALUE_TARGET_CNT];
		static TargetValues targetValues;
		static uint32_t values[RECEIVER_CHANNEL_CNT];
		static bool bufferFull;
};

#endif
