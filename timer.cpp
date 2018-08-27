#include "main.h"

void Timer::delay(uint32_t ms)
{
	uint32_t start = millis();
	
	while(millis() - start <= ms);
}

uint32_t Timer::millis(void)
{
	return RECEIVER_TIMER->CNT / 2;
}

void Timer::test()
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStruct;
	enableClock(RECEIVER_TIMER);
	
	structInit(&TIM_TimeBaseStruct);
	TIM_TimeBaseStruct.TIM_Prescaler = 45000;
	
	timeBaseInit(RECEIVER_TIMER, &TIM_TimeBaseStruct);
  enable(RECEIVER_TIMER);
}

/**
  * Fills each TIM_TimeBaseInitStruct member with its default value.
  * TIM_TimeBaseInitStruct : pointer to a TIM_TimeBaseInitTypeDef structure which will be initialized.
  */
void Timer::structInit(TIM_TimeBaseInitTypeDef* TIM_TimeBaseInitStruct)
{
  TIM_TimeBaseInitStruct->TIM_Period = 0xFFFFFFFF;
  TIM_TimeBaseInitStruct->TIM_Prescaler = 0x0000;
  TIM_TimeBaseInitStruct->TIM_ClockDivision = TIM_CKD_DIV1;
  TIM_TimeBaseInitStruct->TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseInitStruct->TIM_RepetitionCounter = 0x0000;
}

/**
  * Enables the clock from the given timer module.
  */
void Timer::enableClock(TIM_TypeDef* TIMx)
{
  if(TIMx == TIM1)
  {
		RCC->APB2ENR |= RCC_APB2Periph_TIM1;
  }
  else if(TIMx == TIM2) 
  {
		RCC->APB1ENR |= RCC_APB1Periph_TIM2;
  }  
  else if(TIMx == TIM3)
  {
		RCC->APB1ENR |= RCC_APB1Periph_TIM3;
  }
  else if(TIMx == TIM4)
  {
		RCC->APB1ENR |= RCC_APB1Periph_TIM4;
  }
  else if(TIMx == TIM5)
  {
		RCC->APB1ENR |= RCC_APB1Periph_TIM5;
  }
  else if(TIMx == TIM6)  
  {
		RCC->APB1ENR |= RCC_APB1Periph_TIM6;
  }
  else if(TIMx == TIM7)
  {
		RCC->APB1ENR |= RCC_APB1Periph_TIM7;
  }
  else if(TIMx == TIM8)
  {
		RCC->APB2ENR |= RCC_APB2Periph_TIM8;
  }
  else if(TIMx == TIM9)
  {
		RCC->APB2ENR |= RCC_APB2Periph_TIM9;
  }  
  else if(TIMx == TIM10)
  {      
		RCC->APB2ENR |= RCC_APB2Periph_TIM10;
  }  
  else if(TIMx == TIM11) 
  {     
		RCC->APB2ENR |= RCC_APB2Periph_TIM11;
  }  
  else if(TIMx == TIM12)
  {      
		RCC->APB1ENR |= RCC_APB1Periph_TIM12;
  }  
  else if(TIMx == TIM13) 
  {       
		RCC->APB1ENR |= RCC_APB1Periph_TIM13;
  }  
  else
  { 
    if(TIMx == TIM14) 
    {     
			RCC->APB1ENR |= RCC_APB1Periph_TIM14;
    }   
  }
}

/**
  * Initializes the TIMx Time Base Unit peripheral according to the specified
	* parameters in the TIM_TimeBaseInitStruct.
  * TIMx: where x can be  1 to 14 to select the TIM peripheral.
  * TIM_TimeBaseInitStruct: pointer to a TIM_TimeBaseInitTypeDef structure
	*	that contains the configuration information for the specified TIM peripheral.
  */
void Timer::timeBaseInit(TIM_TypeDef* TIMx, TIM_TimeBaseInitTypeDef* TIM_TimeBaseInitStruct)
{
  uint16_t tmpcr1 = 0;

  tmpcr1 = TIMx->CR1;  

  if((TIMx == TIM1) || (TIMx == TIM8)||
     (TIMx == TIM2) || (TIMx == TIM3)||
     (TIMx == TIM4) || (TIMx == TIM5)) 
  {
    /* Select the Counter Mode */
    tmpcr1 &= (uint16_t)(~(TIM_CR1_DIR | TIM_CR1_CMS));
    tmpcr1 |= (uint32_t)TIM_TimeBaseInitStruct->TIM_CounterMode;
  }
 
  if((TIMx != TIM6) && (TIMx != TIM7))
  {
    /* Set the clock division */
    tmpcr1 &=  (uint16_t)(~TIM_CR1_CKD);
    tmpcr1 |= (uint32_t)TIM_TimeBaseInitStruct->TIM_ClockDivision;
  }

  TIMx->CR1 = tmpcr1;

  /* Set the Autoreload value */
  TIMx->ARR = TIM_TimeBaseInitStruct->TIM_Period ;
 
  /* Set the Prescaler value */
  TIMx->PSC = TIM_TimeBaseInitStruct->TIM_Prescaler;
    
  if((TIMx == TIM1) || (TIMx == TIM8))  
  {
    /* Set the Repetition Counter value */
    TIMx->RCR = TIM_TimeBaseInitStruct->TIM_RepetitionCounter;
  }

  /* Generate an update event to reload the Prescaler 
     and the repetition counter(only for TIM1 and TIM8) value immediately */
  TIMx->EGR = TIM_PSCReloadMode_Immediate;          
}

/**
  * Enables the specified TIM peripheral.
  * TIMx: where x can be 1 to 14 to select the TIMx peripheral.
  * NewState: new state of the TIMx peripheral.
  */
void Timer::enable(TIM_TypeDef* TIMx)
{
  TIMx->CR1 |= TIM_CR1_CEN;
}

/**
  * Disables the specified TIM peripheral.
  * TIMx: where x can be 1 to 14 to select the TIMx peripheral.
  */
void Timer::disable(TIM_TypeDef* TIMx)
{
	TIMx->CR1 &= (uint16_t)~TIM_CR1_CEN;
}

/**
  * Fills each TIM_ICInitStruct member with its default value.
  * icInitStruct: pointer to a TIM_ICInitTypeDef structure which will be initialized.
  */
void Timer::icStructInit(TIM_ICInitTypeDef* icInitStruct)
{
  icInitStruct->TIM_Channel = TIM_Channel_1;
  icInitStruct->TIM_ICPolarity = TIM_ICPolarity_Rising;
  icInitStruct->TIM_ICSelection = TIM_ICSelection_DirectTI;
  icInitStruct->TIM_ICPrescaler = TIM_ICPSC_DIV1;
  icInitStruct->TIM_ICFilter = 0x00;
}

/**
  * Initializes the TIM peripheral according to the specified parameters in the icInitStruct.
  * TIMx: where x can be 1 to 14 except 6 and 7, to select the TIM peripheral.
  * icInitStruct: pointer to a TIM_ICInitTypeDef structure that contains the configuration information for the specified TIM peripheral.
  */
void Timer::icInit(TIM_TypeDef* TIMx, TIM_ICInitTypeDef* icInitStruct)
{
  if(icInitStruct->TIM_Channel == TIM_Channel_1)
  {
    ch1Config(TIMx, icInitStruct->TIM_ICPolarity, icInitStruct->TIM_ICSelection, icInitStruct->TIM_ICFilter);
    setIC1Prescaler(TIMx, icInitStruct->TIM_ICPrescaler);
  }
  else if(icInitStruct->TIM_Channel == TIM_Channel_2)
  {
    ch2Config(TIMx, icInitStruct->TIM_ICPolarity, icInitStruct->TIM_ICSelection, icInitStruct->TIM_ICFilter);
    setIC2Prescaler(TIMx, icInitStruct->TIM_ICPrescaler);
  }
  else if(icInitStruct->TIM_Channel == TIM_Channel_3)
  {
    ch3Config(TIMx,  icInitStruct->TIM_ICPolarity, icInitStruct->TIM_ICSelection, icInitStruct->TIM_ICFilter);
    setIC3Prescaler(TIMx, icInitStruct->TIM_ICPrescaler);
  }
  else
  {
    ch4Config(TIMx, icInitStruct->TIM_ICPolarity, icInitStruct->TIM_ICSelection, icInitStruct->TIM_ICFilter);
    setIC4Prescaler(TIMx, icInitStruct->TIM_ICPrescaler);
  }
}

/**
  * Sets the TIMx Input Capture 1 prescaler.
  * TIMx: where x can be 1 to 14 except 6 and 7, to select the TIM peripheral.
  * TIM_ICPSC: specifies the Input Capture1 prescaler new value. This parameter can be one of the following values:
  *   TIM_ICPSC_DIV1: no prescaler
  *   TIM_ICPSC_DIV2: capture is done once every 2 events
  *   TIM_ICPSC_DIV4: capture is done once every 4 events
  *   TIM_ICPSC_DIV8: capture is done once every 8 events
  */
void Timer::setIC1Prescaler(TIM_TypeDef* TIMx, uint16_t TIM_ICPSC)
{
  /* Reset the IC1PSC Bits */
  TIMx->CCMR1 &= (uint16_t)~TIM_CCMR1_IC1PSC;

  /* Set the IC1PSC value */
  TIMx->CCMR1 |= TIM_ICPSC;
}

/**
  * Sets the TIMx Input Capture 2 prescaler.
  * TIMx: where x can be 1, 2, 3, 4, 5, 8, 9 or 12 to select the TIM peripheral.
  * TIM_ICPSC: specifies the Input Capture2 prescaler new value. This parameter can be one of the following values:
	*		TIM_ICPSC_DIV1: no prescaler
  *   TIM_ICPSC_DIV2: capture is done once every 2 events
  *   TIM_ICPSC_DIV4: capture is done once every 4 events
  *   TIM_ICPSC_DIV8: capture is done once every 8 events
  */
void Timer::setIC2Prescaler(TIM_TypeDef* TIMx, uint16_t TIM_ICPSC)
{
  /* Reset the IC2PSC Bits */
  TIMx->CCMR1 &= (uint16_t)~TIM_CCMR1_IC2PSC;

  /* Set the IC2PSC value */
  TIMx->CCMR1 |= (uint16_t)(TIM_ICPSC << 8);
}

/**
  * Sets the TIMx Input Capture 3 prescaler.
  * TIMx: where x can be 1, 2, 3, 4, 5 or 8 to select the TIM peripheral.
  * TIM_ICPSC: specifies the Input Capture3 prescaler new value. This parameter can be one of the following values:
  *		TIM_ICPSC_DIV1: no prescaler
  *   TIM_ICPSC_DIV2: capture is done once every 2 events
  *   TIM_ICPSC_DIV4: capture is done once every 4 events
  *   TIM_ICPSC_DIV8: capture is done once every 8 events
  */
void Timer::setIC3Prescaler(TIM_TypeDef* TIMx, uint16_t TIM_ICPSC)
{
  /* Reset the IC3PSC Bits */
  TIMx->CCMR2 &= (uint16_t)~TIM_CCMR2_IC3PSC;

  /* Set the IC3PSC value */
  TIMx->CCMR2 |= TIM_ICPSC;
}

/**
  * Sets the TIMx Input Capture 4 prescaler.
  * TIMx: where x can be 1, 2, 3, 4, 5 or 8 to select the TIM peripheral.
  * TIM_ICPSC: specifies the Input Capture4 prescaler new value. This parameter can be one of the following values:
  * 	TIM_ICPSC_DIV1: no prescaler
  *   TIM_ICPSC_DIV2: capture is done once every 2 events
  *   TIM_ICPSC_DIV4: capture is done once every 4 events
  *   TIM_ICPSC_DIV8: capture is done once every 8 events
  */
void Timer::setIC4Prescaler(TIM_TypeDef* TIMx, uint16_t TIM_ICPSC)
{  
  /* Reset the IC4PSC Bits */
  TIMx->CCMR2 &= (uint16_t)~TIM_CCMR2_IC4PSC;

  /* Set the IC4PSC value */
  TIMx->CCMR2 |= (uint16_t)(TIM_ICPSC << 8);
}

/**
  * Configure the TI1 as Input.
  * TIMx: where x can be 1, 2, 3, 4, 5, 8, 9, 10, 11, 12, 13 or 14 to select the TIM peripheral.
  * TIM_ICPolarity : The Input Polarity.
  * This parameter can be one of the following values:
  * TIM_ICPolarity_Rising
  * TIM_ICPolarity_Falling
  * TIM_ICPolarity_BothEdge  
  * 
	* TIM_ICSelection: specifies the input to be used.
	* This parameter can be one of the following values:
  * TIM_ICSelection_DirectTI: TIM Input 1 is selected to be connected to IC1.
  * TIM_ICSelection_IndirectTI: TIM Input 1 is selected to be connected to IC2.
  * TIM_ICSelection_TRC: TIM Input 1 is selected to be connected to TRC.
  * 
	* TIM_ICFilter: Specifies the Input Capture Filter. This parameter must be a value between 0x00 and 0x0F.
  */
void Timer::ch1Config(TIM_TypeDef* TIMx, uint16_t TIM_ICPolarity, uint16_t TIM_ICSelection, uint16_t TIM_ICFilter)
{
  uint16_t tmpccmr1 = 0, tmpccer = 0;

  /* Disable the Channel 1: Reset the CC1E Bit */
  TIMx->CCER &= (uint16_t)~TIM_CCER_CC1E;
  tmpccmr1 = TIMx->CCMR1;
  tmpccer = TIMx->CCER;

  /* Select the Input and set the filter */
  tmpccmr1 &= ((uint16_t)~TIM_CCMR1_CC1S) & ((uint16_t)~TIM_CCMR1_IC1F);
  tmpccmr1 |= (uint16_t)(TIM_ICSelection | (uint16_t)(TIM_ICFilter << (uint16_t)4));

  /* Select the Polarity and set the CC1E Bit */
  tmpccer &= (uint16_t)~(TIM_CCER_CC1P | TIM_CCER_CC1NP);
  tmpccer |= (uint16_t)(TIM_ICPolarity | (uint16_t)TIM_CCER_CC1E);

  /* Write to TIMx CCMR1 and CCER registers */
  TIMx->CCMR1 = tmpccmr1;
  TIMx->CCER = tmpccer;
}

/**
  * Configure the TI2 as Input.
  * TIMx: where x can be 1, 2, 3, 4, 5, 8, 9 or 12 to select the TIM peripheral.
  * TIM_ICPolarity : The Input Polarity.
  * This parameter can be one of the following values:
  * TIM_ICPolarity_Rising
  * TIM_ICPolarity_Falling
  * TIM_ICPolarity_BothEdge   
  * 
	* TIM_ICSelection: specifies the input to be used.
  * This parameter can be one of the following values:
  * TIM_ICSelection_DirectTI: TIM Input 2 is selected to be connected to IC2.
  * TIM_ICSelection_IndirectTI: TIM Input 2 is selected to be connected to IC1.
  * TIM_ICSelection_TRC: TIM Input 2 is selected to be connected to TRC.
  * 
	* TIM_ICFilter: Specifies the Input Capture Filter. This parameter must be a value between 0x00 and 0x0F.
  */
void Timer::ch2Config(TIM_TypeDef* TIMx, uint16_t TIM_ICPolarity, uint16_t TIM_ICSelection, uint16_t TIM_ICFilter)
{
  uint16_t tmpccmr1 = 0, tmpccer = 0, tmp = 0;

  /* Disable the Channel 2: Reset the CC2E Bit */
  TIMx->CCER &= (uint16_t)~TIM_CCER_CC2E;
  tmpccmr1 = TIMx->CCMR1;
  tmpccer = TIMx->CCER;
  tmp = (uint16_t)(TIM_ICPolarity << 4);

  /* Select the Input and set the filter */
  tmpccmr1 &= ((uint16_t)~TIM_CCMR1_CC2S) & ((uint16_t)~TIM_CCMR1_IC2F);
  tmpccmr1 |= (uint16_t)(TIM_ICFilter << 12);
  tmpccmr1 |= (uint16_t)(TIM_ICSelection << 8);

  /* Select the Polarity and set the CC2E Bit */
  tmpccer &= (uint16_t)~(TIM_CCER_CC2P | TIM_CCER_CC2NP);
  tmpccer |=  (uint16_t)(tmp | (uint16_t)TIM_CCER_CC2E);

  /* Write to TIMx CCMR1 and CCER registers */
  TIMx->CCMR1 = tmpccmr1 ;
  TIMx->CCER = tmpccer;
}

/**
  * Configure the TI3 as Input.
  * TIMx: where x can be 1, 2, 3, 4, 5 or 8 to select the TIM peripheral.
  * TIM_ICPolarity : The Input Polarity.
  * This parameter can be one of the following values:
  * TIM_ICPolarity_Rising
  * TIM_ICPolarity_Falling
  * TIM_ICPolarity_BothEdge         
  * 
	* TIM_ICSelection: specifies the input to be used.
  * This parameter can be one of the following values:
  * TIM_ICSelection_DirectTI: TIM Input 3 is selected to be connected to IC3.
  * TIM_ICSelection_IndirectTI: TIM Input 3 is selected to be connected to IC4.
  * TIM_ICSelection_TRC: TIM Input 3 is selected to be connected to TRC.
  * 
	* TIM_ICFilter: Specifies the Input Capture Filter. This parameter must be a value between 0x00 and 0x0F.
  */
void Timer::ch3Config(TIM_TypeDef* TIMx, uint16_t TIM_ICPolarity, uint16_t TIM_ICSelection, uint16_t TIM_ICFilter)
{
  uint16_t tmpccmr2 = 0, tmpccer = 0, tmp = 0;

  /* Disable the Channel 3: Reset the CC3E Bit */
  TIMx->CCER &= (uint16_t)~TIM_CCER_CC3E;
  tmpccmr2 = TIMx->CCMR2;
  tmpccer = TIMx->CCER;
  tmp = (uint16_t)(TIM_ICPolarity << 8);

  /* Select the Input and set the filter */
  tmpccmr2 &= ((uint16_t)~TIM_CCMR1_CC1S) & ((uint16_t)~TIM_CCMR2_IC3F);
  tmpccmr2 |= (uint16_t)(TIM_ICSelection | (uint16_t)(TIM_ICFilter << (uint16_t)4));

  /* Select the Polarity and set the CC3E Bit */
  tmpccer &= (uint16_t)~(TIM_CCER_CC3P | TIM_CCER_CC3NP);
  tmpccer |= (uint16_t)(tmp | (uint16_t)TIM_CCER_CC3E);

  /* Write to TIMx CCMR2 and CCER registers */
  TIMx->CCMR2 = tmpccmr2;
  TIMx->CCER = tmpccer;
}

/**
  * Configure the TI4 as Input.
  * TIMx: where x can be 1, 2, 3, 4, 5 or 8 to select the TIM peripheral.
  * TIM_ICPolarity : The Input Polarity.
  * This parameter can be one of the following values:
  * TIM_ICPolarity_Rising
  * TIM_ICPolarity_Falling
  * TIM_ICPolarity_BothEdge     
  * 
	* TIM_ICSelection: specifies the input to be used.
  * This parameter can be one of the following values:
  * TIM_ICSelection_DirectTI: TIM Input 4 is selected to be connected to IC4.
  * TIM_ICSelection_IndirectTI: TIM Input 4 is selected to be connected to IC3.
  * TIM_ICSelection_TRC: TIM Input 4 is selected to be connected to TRC.
  * 
	* TIM_ICFilter: Specifies the Input Capture Filter. This parameter must be a value between 0x00 and 0x0F.
  */
void Timer::ch4Config(TIM_TypeDef* TIMx, uint16_t TIM_ICPolarity, uint16_t TIM_ICSelection, uint16_t TIM_ICFilter)
{
  uint16_t tmpccmr2 = 0, tmpccer = 0, tmp = 0;

  /* Disable the Channel 4: Reset the CC4E Bit */
  TIMx->CCER &= (uint16_t)~TIM_CCER_CC4E;
  tmpccmr2 = TIMx->CCMR2;
  tmpccer = TIMx->CCER;
  tmp = (uint16_t)(TIM_ICPolarity << 12);

  /* Select the Input and set the filter */
  tmpccmr2 &= ((uint16_t)~TIM_CCMR1_CC2S) & ((uint16_t)~TIM_CCMR1_IC2F);
  tmpccmr2 |= (uint16_t)(TIM_ICSelection << 8);
  tmpccmr2 |= (uint16_t)(TIM_ICFilter << 12);

  /* Select the Polarity and set the CC4E Bit */
  tmpccer &= (uint16_t)~(TIM_CCER_CC4P | TIM_CCER_CC4NP);
  tmpccer |= (uint16_t)(tmp | (uint16_t)TIM_CCER_CC4E);

  /* Write to TIMx CCMR2 and CCER registers */
  TIMx->CCMR2 = tmpccmr2;
  TIMx->CCER = tmpccer ;
}

/**
  * Gets the TIMx Input Capture 1 value.
  * TIMx: where x can be 1 to 14 except 6 and 7, to select the TIM peripheral.
  * returns capture compare 1 register value.
  */
uint32_t Timer::getCapture1(TIM_TypeDef* TIMx)
{
  return TIMx->CCR1;
}

/**
  * Gets the TIMx Input Capture 2 value.
  * TIMx: where x can be 1, 2, 3, 4, 5, 8, 9 or 12 to select the TIM  peripheral.
  * returns capture compare 2 register value.
  */
uint32_t Timer::getCapture2(TIM_TypeDef* TIMx)
{
  return TIMx->CCR2;
}

/**
  * Gets the TIMx Input Capture 3 value.
  * TIMx: where x can be 1, 2, 3, 4, 5 or 8 to select the TIM peripheral.
  * returns capture compare 3 register value.
  */
uint32_t Timer::getCapture3(TIM_TypeDef* TIMx)
{
  return TIMx->CCR3;
}

/**
  * Gets the TIMx Input Capture 4 value.
  * TIMx: where x can be 1, 2, 3, 4, 5 or 8 to select the TIM peripheral.
  * returns capture compare 4 register value.
  */
uint32_t Timer::getCapture4(TIM_TypeDef* TIMx)
{
  return TIMx->CCR4;
}


/*===============================================================================
          ##### Interrupts, DMA and flags management functions #####
 ===============================================================================*/  

/**
  * Enables the specified TIM interrupts.
  * TIMx: where x can be 1 to 14 to select the TIMx peripheral.
  * TIM_IT: specifies the TIM interrupts sources to be enabled or disabled.
  * 	This parameter can be any combination of the following values:
  *   	TIM_IT_Update: TIM update Interrupt source
  *     TIM_IT_CC1: TIM Capture Compare 1 Interrupt source
  *     TIM_IT_CC2: TIM Capture Compare 2 Interrupt source
  *     TIM_IT_CC3: TIM Capture Compare 3 Interrupt source
  *     TIM_IT_CC4: TIM Capture Compare 4 Interrupt source
  *     TIM_IT_COM: TIM Commutation Interrupt source
  *     TIM_IT_Trigger: TIM Trigger Interrupt source
  *     TIM_IT_Break: TIM Break Interrupt source
  *  
  * For TIM6 and TIM7 only the parameter TIM_IT_Update can be used
  * For TIM9 and TIM12 only one of the following parameters can be used: TIM_IT_Update,
  * 	TIM_IT_CC1, TIM_IT_CC2 or TIM_IT_Trigger. 
  * For TIM10, TIM11, TIM13 and TIM14 only one of the following parameters can
  * 	be used: TIM_IT_Update or TIM_IT_CC1   
  * TIM_IT_COM and TIM_IT_Break can be used only with TIM1 and TIM8 
  */
void Timer::enableInterrupt(TIM_TypeDef* TIMx, uint16_t TIM_IT)
{  
	TIMx->DIER |= TIM_IT;
}

/**
  * Disables the specified TIM interrupts.
  * TIMx: where x can be 1 to 14 to select the TIMx peripheral.
  * TIM_IT: specifies the TIM interrupts sources to be enabled or disabled.
  * 	This parameter can be any combination of the following values:
  *   	TIM_IT_Update: TIM update Interrupt source
  *     TIM_IT_CC1: TIM Capture Compare 1 Interrupt source
  *     TIM_IT_CC2: TIM Capture Compare 2 Interrupt source
  *     TIM_IT_CC3: TIM Capture Compare 3 Interrupt source
  *     TIM_IT_CC4: TIM Capture Compare 4 Interrupt source
  *     TIM_IT_COM: TIM Commutation Interrupt source
  *     TIM_IT_Trigger: TIM Trigger Interrupt source
  *     TIM_IT_Break: TIM Break Interrupt source
  *  
  * For TIM6 and TIM7 only the parameter TIM_IT_Update can be used
  * For TIM9 and TIM12 only one of the following parameters can be used: TIM_IT_Update,
  * 	TIM_IT_CC1, TIM_IT_CC2 or TIM_IT_Trigger. 
  * For TIM10, TIM11, TIM13 and TIM14 only one of the following parameters can
  * 	be used: TIM_IT_Update or TIM_IT_CC1   
  * TIM_IT_COM and TIM_IT_Break can be used only with TIM1 and TIM8 
  */
void Timer::disableInterrupt(TIM_TypeDef* TIMx, uint16_t TIM_IT)
{  
	TIMx->DIER &= (uint16_t)~TIM_IT;
}

/**
  * Checks whether the TIM interrupt has occurred or not.
  * TIMx: where x can be 1 to 14 to select the TIM peripheral.
  * TIM_IT: specifies the TIM interrupt source to check.
  * 	This parameter can be one of the following values:
  *   	TIM_IT_Update: TIM update Interrupt source
  *     TIM_IT_CC1: TIM Capture Compare 1 Interrupt source
  *     TIM_IT_CC2: TIM Capture Compare 2 Interrupt source
  *     TIM_IT_CC3: TIM Capture Compare 3 Interrupt source
  *     TIM_IT_CC4: TIM Capture Compare 4 Interrupt source
  *     TIM_IT_COM: TIM Commutation Interrupt source
  *     TIM_IT_Trigger: TIM Trigger Interrupt source
  *     TIM_IT_Break: TIM Break Interrupt source
  *
  * TIM7 can generate only an update interrupt.
  * TIM_IT_Break are used only with TIM1 and TIM8.
  */
ITStatus Timer::getInterruptStatus(TIM_TypeDef* TIMx, uint16_t TIM_IT)
{
  ITStatus bitstatus = RESET;  
  uint16_t itstatus = 0x0, itenable = 0x0;
   
  itstatus = TIMx->SR & TIM_IT;
  
  itenable = TIMx->DIER & TIM_IT;
  if ((itstatus != (uint16_t)RESET) && (itenable != (uint16_t)RESET))
  {
    bitstatus = SET;
  }
  else
  {
    bitstatus = RESET;
  }
  return bitstatus;
}

/**
  * Clears the TIMx's interrupt pending bits.
  * TIMx: where x can be 1 to 14 to select the TIM peripheral.
  * TIM_IT: specifies the pending bit to clear.
  * 	This parameter can be any combination of the following values:
  *   	TIM_IT_Update: TIM1 update Interrupt source
  *     TIM_IT_CC1: TIM Capture Compare 1 Interrupt source
  *     TIM_IT_CC2: TIM Capture Compare 2 Interrupt source
  *     TIM_IT_CC3: TIM Capture Compare 3 Interrupt source
  *     TIM_IT_CC4: TIM Capture Compare 4 Interrupt source
  *     TIM_IT_COM: TIM Commutation Interrupt source
  *     TIM_IT_Trigger: TIM Trigger Interrupt source
  *     TIM_IT_Break: TIM Break Interrupt source
  *
  * TIM6 and TIM7 can generate only an update interrupt.
  * TIM_IT_COM and TIM_IT_Break are used only with TIM1 and TIM8.
  */
void Timer::clearInterruptPendingBit(TIM_TypeDef* TIMx, uint16_t TIM_IT)
{
  TIMx->SR = (uint16_t)~TIM_IT;
}


