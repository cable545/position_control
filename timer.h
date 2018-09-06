#ifndef _TIMER_H
#define _TIMER_H

/* 
 * TIM Time Base Init structure definition  
 * This structure is used with all TIMx except for TIM6 and TIM7.  
 */
typedef struct
{
  uint16_t TIM_Prescaler;         /*!< Specifies the prescaler value used to divide the TIM clock.
                                       This parameter can be a number between 0x0000 and 0xFFFF */

  uint16_t TIM_CounterMode;       /*!< Specifies the counter mode.
                                       This parameter can be a value of @ref TIM_Counter_Mode */

  uint32_t TIM_Period;            /*!< Specifies the period value to be loaded into the active
                                       Auto-Reload Register at the next update event.
                                       This parameter must be a number between 0x0000 and 0xFFFF.  */ 

  uint16_t TIM_ClockDivision;     /*!< Specifies the clock division.
                                      This parameter can be a value of @ref TIM_Clock_Division_CKD */

  uint8_t TIM_RepetitionCounter;  /*!< Specifies the repetition counter value. Each time the RCR downcounter
                                       reaches zero, an update event is generated and counting restarts
                                       from the RCR value (N).
                                       This means in PWM mode that (N+1) corresponds to:
                                          - the number of PWM periods in edge-aligned mode
                                          - the number of half PWM period in center-aligned mode
                                       This parameter must be a number between 0x00 and 0xFF. 
                                       @note This parameter is valid only for TIM1 and TIM8. */
} TIM_TimeBaseInitTypeDef;

/* 
 * TIM Input Capture Init structure definition  
 */
typedef struct
{

  uint16_t TIM_Channel;      /*!< Specifies the TIM channel.
                                  This parameter can be a value of @ref TIM_Channel */

  uint16_t TIM_ICPolarity;   /*!< Specifies the active edge of the input signal.
                                  This parameter can be a value of @ref TIM_Input_Capture_Polarity */

  uint16_t TIM_ICSelection;  /*!< Specifies the input.
                                  This parameter can be a value of @ref TIM_Input_Capture_Selection */

  uint16_t TIM_ICPrescaler;  /*!< Specifies the Input Capture Prescaler.
                                  This parameter can be a value of @ref TIM_Input_Capture_Prescaler */

  uint16_t TIM_ICFilter;     /*!< Specifies the input capture filter.
                                  This parameter can be a number between 0x0 and 0xF */
} TIM_ICInitTypeDef;

/* 
 * TIM_Channel 
 */
#define TIM_Channel_1                      ((uint16_t)0x0000)
#define TIM_Channel_2                      ((uint16_t)0x0004)
#define TIM_Channel_3                      ((uint16_t)0x0008)
#define TIM_Channel_4                      ((uint16_t)0x000C)

/* 
 * TIM_Clock_Division_CKD 
 */
#define TIM_CKD_DIV1                       ((uint16_t)0x0000)
#define TIM_CKD_DIV2                       ((uint16_t)0x0100)
#define TIM_CKD_DIV4                       ((uint16_t)0x0200)

/* 
 * TIM_Counter_Mode 
 */
#define TIM_CounterMode_Up                 ((uint16_t)0x0000)
#define TIM_CounterMode_Down               ((uint16_t)0x0010)
#define TIM_CounterMode_CenterAligned1     ((uint16_t)0x0020)
#define TIM_CounterMode_CenterAligned2     ((uint16_t)0x0040)
#define TIM_CounterMode_CenterAligned3     ((uint16_t)0x0060)

/* 
 * TIM_Input_Capture_Polarity 
 */
#define  TIM_ICPolarity_Rising             ((uint16_t)0x0000)
#define  TIM_ICPolarity_Falling            ((uint16_t)0x0002)
#define  TIM_ICPolarity_BothEdge           ((uint16_t)0x000A)

/* 
 * TIM_Input_Capture_Selection 
 */
#define TIM_ICSelection_DirectTI           ((uint16_t)0x0001) /*!< TIM Input 1, 2, 3 or 4 is selected to be 
                                                                   connected to IC1, IC2, IC3 or IC4, respectively */
#define TIM_ICSelection_IndirectTI         ((uint16_t)0x0002) /*!< TIM Input 1, 2, 3 or 4 is selected to be
                                                                   connected to IC2, IC1, IC4 or IC3, respectively. */
#define TIM_ICSelection_TRC                ((uint16_t)0x0003) /*!< TIM Input 1, 2, 3 or 4 is selected to be connected to TRC. */

/* 
 * TIM_Input_Capture_Prescaler 
 */
#define TIM_ICPSC_DIV1                     ((uint16_t)0x0000) /*!< Capture performed each time an edge is detected on the capture input. */
#define TIM_ICPSC_DIV2                     ((uint16_t)0x0004) /*!< Capture performed once every 2 events. */
#define TIM_ICPSC_DIV4                     ((uint16_t)0x0008) /*!< Capture performed once every 4 events. */
#define TIM_ICPSC_DIV8                     ((uint16_t)0x000C) /*!< Capture performed once every 8 events. */

/* 
 * TIM_Prescaler_Reload_Mode 
 */
#define TIM_PSCReloadMode_Update           ((uint16_t)0x0000)
#define TIM_PSCReloadMode_Immediate        ((uint16_t)0x0001)

/* 
 * TIM_interrupt_sources 
 */
#define TIM_IT_Update                      ((uint16_t)0x0001)
#define TIM_IT_CC1                         ((uint16_t)0x0002)
#define TIM_IT_CC2                         ((uint16_t)0x0004)
#define TIM_IT_CC3                         ((uint16_t)0x0008)
#define TIM_IT_CC4                         ((uint16_t)0x0010)
#define TIM_IT_COM                         ((uint16_t)0x0020)
#define TIM_IT_Trigger                     ((uint16_t)0x0040)
#define TIM_IT_Break                       ((uint16_t)0x0080)

/*
 * TIM_DMA_sources 
 */
#define TIM_DMA_Update                     ((uint16_t)0x0100)
#define TIM_DMA_CC1                        ((uint16_t)0x0200)
#define TIM_DMA_CC2                        ((uint16_t)0x0400)
#define TIM_DMA_CC3                        ((uint16_t)0x0800)
#define TIM_DMA_CC4                        ((uint16_t)0x1000)
#define TIM_DMA_COM                        ((uint16_t)0x2000)
#define TIM_DMA_Trigger                    ((uint16_t)0x4000)

/*
 * TIM_DMA_Base_address 
 */
#define TIM_DMABase_CR1                    ((uint16_t)0x0000)
#define TIM_DMABase_CR2                    ((uint16_t)0x0001)
#define TIM_DMABase_SMCR                   ((uint16_t)0x0002)
#define TIM_DMABase_DIER                   ((uint16_t)0x0003)
#define TIM_DMABase_SR                     ((uint16_t)0x0004)
#define TIM_DMABase_EGR                    ((uint16_t)0x0005)
#define TIM_DMABase_CCMR1                  ((uint16_t)0x0006)
#define TIM_DMABase_CCMR2                  ((uint16_t)0x0007)
#define TIM_DMABase_CCER                   ((uint16_t)0x0008)
#define TIM_DMABase_CNT                    ((uint16_t)0x0009)
#define TIM_DMABase_PSC                    ((uint16_t)0x000A)
#define TIM_DMABase_ARR                    ((uint16_t)0x000B)
#define TIM_DMABase_RCR                    ((uint16_t)0x000C)
#define TIM_DMABase_CCR1                   ((uint16_t)0x000D)
#define TIM_DMABase_CCR2                   ((uint16_t)0x000E)
#define TIM_DMABase_CCR3                   ((uint16_t)0x000F)
#define TIM_DMABase_CCR4                   ((uint16_t)0x0010)
#define TIM_DMABase_BDTR                   ((uint16_t)0x0011)
#define TIM_DMABase_DCR                    ((uint16_t)0x0012)
#define TIM_DMABase_OR                     ((uint16_t)0x0013)

/*
 * TIM_DMA_Burst_Length 
 */
#define TIM_DMABurstLength_1Transfer       ((uint16_t)0x0000)
#define TIM_DMABurstLength_2Transfers      ((uint16_t)0x0100)
#define TIM_DMABurstLength_3Transfers      ((uint16_t)0x0200)
#define TIM_DMABurstLength_4Transfers      ((uint16_t)0x0300)
#define TIM_DMABurstLength_5Transfers      ((uint16_t)0x0400)
#define TIM_DMABurstLength_6Transfers      ((uint16_t)0x0500)
#define TIM_DMABurstLength_7Transfers      ((uint16_t)0x0600)
#define TIM_DMABurstLength_8Transfers      ((uint16_t)0x0700)
#define TIM_DMABurstLength_9Transfers      ((uint16_t)0x0800)
#define TIM_DMABurstLength_10Transfers     ((uint16_t)0x0900)
#define TIM_DMABurstLength_11Transfers     ((uint16_t)0x0A00)
#define TIM_DMABurstLength_12Transfers     ((uint16_t)0x0B00)
#define TIM_DMABurstLength_13Transfers     ((uint16_t)0x0C00)
#define TIM_DMABurstLength_14Transfers     ((uint16_t)0x0D00)
#define TIM_DMABurstLength_15Transfers     ((uint16_t)0x0E00)
#define TIM_DMABurstLength_16Transfers     ((uint16_t)0x0F00)
#define TIM_DMABurstLength_17Transfers     ((uint16_t)0x1000)
#define TIM_DMABurstLength_18Transfers     ((uint16_t)0x1100)

class Timer
{
	public:
		static void delay(uint32_t ms);
		static uint32_t millis();
		static void test();
		static void structInit(TIM_TimeBaseInitTypeDef* TIM_TimeBaseInitStruct);
		static void enableClock(TIM_TypeDef* TIMx);
		static void timeBaseInit(TIM_TypeDef* TIMx, TIM_TimeBaseInitTypeDef* TIM_TimeBaseInitStruct);
		static void enable(TIM_TypeDef* TIMx);
		static void disable(TIM_TypeDef* TIMx);
	
		static void icStructInit(TIM_ICInitTypeDef* icInitStruct);
		static void icInit(TIM_TypeDef* TIMx, TIM_ICInitTypeDef* icInitStruct);
		static void setIC1Prescaler(TIM_TypeDef* TIMx, uint16_t TIM_ICPSC);
		static void setIC2Prescaler(TIM_TypeDef* TIMx, uint16_t TIM_ICPSC);
		static void setIC3Prescaler(TIM_TypeDef* TIMx, uint16_t TIM_ICPSC);
		static void setIC4Prescaler(TIM_TypeDef* TIMx, uint16_t TIM_ICPSC);
	
		static uint32_t getCapture1(TIM_TypeDef* TIMx);
		static uint32_t getCapture2(TIM_TypeDef* TIMx);
		static uint32_t getCapture3(TIM_TypeDef* TIMx);
		static uint32_t getCapture4(TIM_TypeDef* TIMx);
	
		static void enableInterrupt(TIM_TypeDef* TIMx, uint16_t TIM_IT);
		static void disableInterrupt(TIM_TypeDef* TIMx, uint16_t TIM_IT);
		static ITStatus getInterruptStatus(TIM_TypeDef* TIMx, uint16_t TIM_IT);
		static void clearInterruptPendingBit(TIM_TypeDef* TIMx, uint16_t TIM_IT);
		
		static void configDMA(TIM_TypeDef* TIMx, uint16_t TIM_DMABase, uint16_t TIM_DMABurstLength);
		static void enableDMA(TIM_TypeDef* TIMx, uint16_t TIM_DMASource);
		static void disableDMA(TIM_TypeDef* TIMx, uint16_t TIM_DMASource);
	
	private:
		static void ch1Config(TIM_TypeDef* TIMx, uint16_t TIM_ICPolarity, uint16_t TIM_ICSelection, uint16_t TIM_ICFilter);
		static void ch2Config(TIM_TypeDef* TIMx, uint16_t TIM_ICPolarity, uint16_t TIM_ICSelection, uint16_t TIM_ICFilter);
		static void ch3Config(TIM_TypeDef* TIMx, uint16_t TIM_ICPolarity, uint16_t TIM_ICSelection, uint16_t TIM_ICFilter);
		static void ch4Config(TIM_TypeDef* TIMx, uint16_t TIM_ICPolarity, uint16_t TIM_ICSelection, uint16_t TIM_ICFilter);
};

#endif
