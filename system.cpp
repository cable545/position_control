#include "main.h"

/*******************************************************************************
																		Systick Timer Functions
********************************************************************************/

// cycles per microsecond
static uint32_t usTicks = 0;

// current uptime for 1kHz systick timer. will rollover after 49 days. hopefully we won't care.
static volatile uint32_t sysTickUptime = 0;

static volatile int sysTickPending = 0;

bool startSysTickTimer(void)
{
	usTicks = 180;
	
	if(SysTick_Config(SystemCoreClock / 1000) == 0)
		return true;
	else
		return false;
}

extern "C" void SysTick_Handler(void)
{
#ifdef SYSTEM_DEBUG_ENABLED
	Debug::print("Timer Interrupt happened\n");
#endif
	
	__disable_irq();
	sysTickUptime++;
	sysTickPending = 0;
	(void)(SysTick->CTRL);
	__enable_irq();
}

// Return system uptime in microseconds (rollover in 70minutes)
uint32_t microsISR(void)
{
	register uint32_t ms, pending, cycle_cnt;
	
	__disable_irq();
	cycle_cnt = SysTick->VAL;
		
	if(SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk)
	{
		// Update pending.
		// Record it for multiple calls within the same rollover period
		// (Will be cleared when serviced).
		// Note that multiple rollovers are not considered.
			
		sysTickPending = 1;
			
		// Read VAL again to ensure the value is read after the rollover.
			
		cycle_cnt = SysTick->VAL;
	}
		
	ms = sysTickUptime;
	pending = sysTickPending;
	__enable_irq();
	
	return ((ms + pending) * 1000) + (usTicks * 1000 - cycle_cnt) / usTicks;
}

uint32_t micros(void)
{
	register uint32_t ms, cycle_cnt;
	
	// Call microsISR() in interrupt and elevated (non-zero) BASEPRI context
	
	if((SCB->ICSR & SCB_ICSR_VECTACTIVE_Msk) || (__get_BASEPRI())) return microsISR();
	
	do
	{
		ms = sysTickUptime;
		cycle_cnt = SysTick->VAL;
		
		/*
		 * If the SysTick timer expired during the previous instruction, we need to give it a little time for that
		 * interrupt to be delivered before we can recheck sysTickUptime:
		 */
		__asm volatile("\tnop\n");
	} while (ms != sysTickUptime);
	
	return (ms * 1000) + (usTicks * 1000 - cycle_cnt) / usTicks;
}

// Return system uptime in milliseconds (rollover in 49 days)
uint32_t millis(void)
{
	return sysTickUptime;
}

#if 1
void delayMicroseconds(uint32_t us)
{
	uint32_t now = micros();
	while(micros() - now < us);
}
#else
void delayMicroseconds(uint32_t us)
{
	uint32_t elapsed = 0;
	uint32_t lastCount = SysTick->VAL;
	
	for(;;)
	{
		register uint32_t current_count = SysTick->VAL;
		uint32_t elapsed_us;
		
		// measure the time elapsed since the last time we checked
		elapsed += current_count - lastCount;
		lastCount = current_count;
		
		// convert to microseconds
		elapsed_us = elapsed / usTicks;
		if (elapsed_us >= us) break;
		
		// reduce the delay by the elapsed time
		us -= elapsed_us;
		
		// keep fractional microseconds for the next iteration
		elapsed %= usTicks;
	}
}
#endif

void delay(uint32_t ms)
{
	while(ms--) delayMicroseconds(1000);
}

/*******************************************************************************
												Data Watchpoint and Trace Unit(DWT)
********************************************************************************/

/* 
 * ms delay function implemented for level 0 compiler optimisation
 * DWT_CycCounterStart have to be called before using the delay functions
 */
void delayMS_DWT(uint32_t ms)
{
	volatile uint32_t cycles = 182000 * ms;
	volatile uint32_t start = DWT->CYCCNT;
	do  {
	} while(DWT->CYCCNT - start < cycles);
}

void delayUS_DWT(uint32_t us)
{
	volatile uint32_t cycles = 182 * us;
	volatile uint32_t start = DWT->CYCCNT;
	do  {
	} while(DWT->CYCCNT - start < cycles);
}

void DWT_Enable(void)
{
	CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
}

void DWT_CycCounterEn(void)
{
	DWT->CTRL = 0x40000001;
}

void DWT_CycCounterDis(void)
{
	DWT->CTRL = 0x40000000;
}

uint32_t DWTCycCounterRead(void)
{
	return DWT->CYCCNT;
}

void DWT_CycCounterClear(void)
{
	DWT->CYCCNT = 0;
}

void DWT_CycCounterStart(void)
{
	DWT_Enable();
	DWT_CycCounterEn();
	DWT_CycCounterClear();
}

uint32_t DWT_CycCounterRead(void)
{
	return DWTCycCounterRead();
}

float DWT_CycCounterReadMs(void)
{
	return DWTCycCounterRead() / 180000.0f;
}


