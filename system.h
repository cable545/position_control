#ifndef _SYSTEM_H
#define _SYSTEM_H

//#define SYSTEM_DEBUG_ENABLED

/*-----------------------------Systick Timer-------------------------*/

bool startSysTickTimer(void);
void delayMicroseconds(uint32_t us);
void delay(uint32_t ms);

uint32_t micros(void);
uint32_t microsISR(void);
uint32_t millis(void);

/*-----------------Data Watchpoint and Trace Unit(DWT)---------------*/

void delayMS_DWT(uint32_t ms);
void delayUS_DWT(uint32_t us);

void DWT_Enable(void);
void DWT_CycCounterEn(void);
void DWT_CycCounterDis(void);
uint32_t DWTCycCounterRead(void);
void DWT_CycCounterClear(void);

void DWT_CycCounterStart(void);
uint32_t DWT_CycCounterRead(void);
float DWT_CycCounterReadMs(void);

#endif
