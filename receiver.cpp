#include "main.h"

uint32_t Receiver::capturedValue[CAPTURE_VALUE_TARGET_CNT];
uint32_t Receiver::capturedValues[CAPTURE_VALUE_TARGET_CNT];
TargetValues Receiver::targetValues;
uint32_t Receiver::values[RECEIVER_CHANNEL_CNT];
bool Receiver::bufferFull;

extern "C"
{
	static uint32_t sumSignalIndex = 0;
	
  void TIM1_CC_IRQHandler()
	{
		if(Timer::getInterruptStatus(RECEIVER_TIMER, TIM_IT_CC1) == SET)
		{
			uint32_t tmp;
			
			Timer::clearInterruptPendingBit(RECEIVER_TIMER, TIM_IT_CC1);
			RECEIVER_TIMER->CNT = 0x0;
			tmp = Timer::getCapture1(RECEIVER_TIMER);
			
			if(tmp > 2000)
			{
				sumSignalIndex = 0;
			}
			else
			{
				Receiver::capturedValue[sumSignalIndex++] = tmp;
			}
		}
	}
	
	void DMA1_Stream0_IRQHandler()
	{
		if(DMA::getInterruptStatus(RECEIVER_DMA_STREAM, RECEIVER_DMA_IRQ_SC) == SET)
		{
			DMA::clearInterruptPendingBit(RECEIVER_DMA_STREAM, RECEIVER_DMA_IRQ_SC);
			Receiver::bufferFull = true;
			RECEIVER_TIMER->CNT = 0;
		}
	}
}

void Receiver::initGPIO()
{
	TM_GPIO_InitAlternate(
		RECEIVER_TIMER_GPIO_PORT, 
		RECEIVER_TIMER_PIN,
		TM_GPIO_OType_PP,
		TM_GPIO_PuPd_NOPULL,
		TM_GPIO_Speed_High,
		RECEIVER_GPIO_AF_FUNCTION
	);
}

void Receiver::initInterrupt()
{
	NVIC_InitTypeDef NVIC_InitStruct;
	NVIC_InitStruct.NVIC_IRQChannel = TIM1_CC_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStruct.NVIC_IRQChannelSubPriority = 1;
  NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
  Interrupt::init(&NVIC_InitStruct);
}

void Receiver::initDMAInterrupt()
{
	NVIC_InitTypeDef NVIC_InitStruct;
	NVIC_InitStruct.NVIC_IRQChannel = RECEIVER_DMA_IRQ_NR;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStruct.NVIC_IRQChannelSubPriority = 1;
  NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
  Interrupt::init(&NVIC_InitStruct);
}

uint32_t Receiver::initTimer(bool enableInterrupt, bool enableDMA)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStruct;
	TIM_ICInitTypeDef TIM_ICInitStruct;
	
	Timer::enableClock(RECEIVER_TIMER);
	
	Timer::structInit(&TIM_TimeBaseStruct);
	TIM_TimeBaseStruct.TIM_Prescaler = RECEIVER_TIMER_PRESCALER;
	Timer::timeBaseInit(RECEIVER_TIMER, &TIM_TimeBaseStruct);
	
	Timer::icStructInit(&TIM_ICInitStruct);
	TIM_ICInitStruct.TIM_Channel = RECEIVER_TIMER_CHANNEL;
	TIM_ICInitStruct.TIM_ICPolarity = TIM_ICPolarity_Falling;
	TIM_ICInitStruct.TIM_ICSelection = TIM_ICSelection_DirectTI;
	TIM_ICInitStruct.TIM_ICPrescaler = TIM_ICPSC_DIV1;
	
	Timer::icInit(RECEIVER_TIMER, &TIM_ICInitStruct);
	
	Timer::enable(RECEIVER_TIMER);
	
	if(enableInterrupt && enableDMA)
	{
		return RECEIVER_INT_AND_DMA_ENABLED;
	}
	else if(enableInterrupt)
	{
		initInterrupt();
		Timer::enableInterrupt(RECEIVER_TIMER, TIM_IT_CC1);
	}
	else if(enableDMA)
	{
		Timer::configDMA(RECEIVER_TIMER, TIM_DMABase_CCR1, TIM_DMABurstLength_1Transfer);
		Timer::enableDMA(RECEIVER_TIMER, TIM_DMA_CC1);
	}
	
	return EXIT_SUCCESS;
}


void Receiver::initDMA()
{
	DMA_InitTypeDef DMA_InitStruct;
	RCC_AHB1PeriphClockCmd(RECEIVER_RCC_APB_DMA_Periph, ENABLE);
	
	DMA::enableDisable(RECEIVER_DMA_STREAM, DISABLE);
	DMA::deInit(RECEIVER_DMA_STREAM);
	
	DMA::structInit(&DMA_InitStruct);
	
	DMA_InitStruct.DMA_Channel = RECEIVER_DMA_CHANNEL;
	DMA_InitStruct.DMA_PeripheralBaseAddr = (uint32_t) &RECEIVER_TIMER->CCR1;
	DMA_InitStruct.DMA_Memory0BaseAddr = (uint32_t) &Receiver::capturedValues;
	
	DMA_InitStruct.DMA_DIR = DMA_DIR_PeripheralToMemory;
	DMA_InitStruct.DMA_BufferSize = BUFFER_SIZE;
	DMA_InitStruct.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStruct.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStruct.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Word;
	DMA_InitStruct.DMA_MemoryDataSize = DMA_PeripheralDataSize_Word;
	DMA_InitStruct.DMA_Mode = DMA_Mode_Circular;
	
	DMA_InitStruct.DMA_Priority = DMA_Priority_High;
	DMA_InitStruct.DMA_FIFOMode = DMA_FIFOMode_Disable;
	DMA_InitStruct.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;
	DMA_InitStruct.DMA_MemoryBurst = DMA_MemoryBurst_Single;
	DMA_InitStruct.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
	
	DMA::init(RECEIVER_DMA_STREAM, &DMA_InitStruct);
	
	DMA::enableDisableInterrupt(RECEIVER_DMA_STREAM, DMA_IT_TC, ENABLE);
	DMA::enableDisable(RECEIVER_DMA_STREAM, ENABLE);
}

uint32_t Receiver::init(bool enableInterrupt, bool enableDMA)
{
	uint32_t result;
	
	initGPIO();
	
	result = initTimer(enableInterrupt, enableDMA);
	if(result != EXIT_SUCCESS) return result;
	
	if(enableDMA) initDMA();
	
	initDMAInterrupt();
	
	return EXIT_SUCCESS;
}

bool Receiver::processCapturedValues()
{
	uint32_t tmpValues[CAPTURE_VALUE_TARGET_CNT] = {0};
	uint32_t syncSignalIndex = 0;
	uint32_t offset;
	
	if(bufferFull)
	{
		for(uint32_t i = 0; i < CAPTURE_VALUE_TARGET_CNT; i++)
		{
			if(i == 0)
				tmpValues[i] = Receiver::capturedValues[i];
			else
				tmpValues[i] = Receiver::capturedValues[i] - Receiver::capturedValues[i - 1];
			
			if(Receiver::isSyncSignalValue(tmpValues[i])) syncSignalIndex = i;
		}
		
		if(syncSignalIndex == 0 || syncSignalIndex == CAPTURE_VALUE_TARGET_CNT - 1)
		{
			offset = (syncSignalIndex == 0) ? 1 : 0;
			
			for(uint32_t i = 0; i < RECEIVER_CHANNEL_CNT; i++) Receiver::values[i] = tmpValues[i + offset];
		}
		else
		{
			for(uint32_t i = 0, j = syncSignalIndex + 1; i < RECEIVER_CHANNEL_CNT; j++)
			{
				if(j >= CAPTURE_VALUE_TARGET_CNT) j = 0;
				Receiver::values[i++] = tmpValues[j];
			}
		}
		
		Receiver::fillTargetValues();
		
		bufferFull = false;
		
		return true;
	}
	else
	{
		return false;
	}
}

bool Receiver::isControlSignalValue(uint32_t value)
{
	return value > CHANNEL_VALUE_COUNT_MIN && value < CHANNEL_VALUE_COUNT_MAX;
}

bool Receiver::isSyncSignalValue(uint32_t value)
{
	return value > CHANNEL_VALUE_COUNT_MAX;
}

void Receiver::fillTargetValues()
{
	Receiver::targetValues.elevator = Receiver::values[0];
	Receiver::targetValues.roll = Maths::map(Receiver::values[1], CHANNEL_VALUE_COUNT_MIN, CHANNEL_VALUE_COUNT_MAX, ROLL_ANGLE_MIN, ROLL_ANGLE_MAX);
	Receiver::targetValues.pitch = Maths::map(Receiver::values[2], CHANNEL_VALUE_COUNT_MIN, CHANNEL_VALUE_COUNT_MAX, PITCH_ANGLE_MIN, PITCH_ANGLE_MAX);
	Receiver::targetValues.yaw = Maths::map(Receiver::values[3], CHANNEL_VALUE_COUNT_MIN, CHANNEL_VALUE_COUNT_MAX, YAW_RATE_MIN, YAW_RATE_MAX);
}
