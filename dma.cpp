#include "main.h"

/* Masks Definition */
#define TRANSFER_IT_ENABLE_MASK (uint32_t)(DMA_SxCR_TCIE | DMA_SxCR_HTIE | \
                                           DMA_SxCR_TEIE | DMA_SxCR_DMEIE)

#define DMA_Stream0_IT_MASK     (uint32_t)(DMA_LISR_FEIF0 | DMA_LISR_DMEIF0 | \
                                           DMA_LISR_TEIF0 | DMA_LISR_HTIF0 | \
                                           DMA_LISR_TCIF0)

#define DMA_Stream1_IT_MASK     (uint32_t)(DMA_Stream0_IT_MASK << 6)
#define DMA_Stream2_IT_MASK     (uint32_t)(DMA_Stream0_IT_MASK << 16)
#define DMA_Stream3_IT_MASK     (uint32_t)(DMA_Stream0_IT_MASK << 22)
#define DMA_Stream4_IT_MASK     (uint32_t)(DMA_Stream0_IT_MASK | (uint32_t)0x20000000)
#define DMA_Stream5_IT_MASK     (uint32_t)(DMA_Stream1_IT_MASK | (uint32_t)0x20000000)
#define DMA_Stream6_IT_MASK     (uint32_t)(DMA_Stream2_IT_MASK | (uint32_t)0x20000000)
#define DMA_Stream7_IT_MASK     (uint32_t)(DMA_Stream3_IT_MASK | (uint32_t)0x20000000)
#define TRANSFER_IT_MASK        (uint32_t)0x0F3C0F3C
#define HIGH_ISR_MASK           (uint32_t)0x20000000
#define RESERVED_MASK           (uint32_t)0x0F7D0F7D  

/*
 * Fills each DMA_InitStruct member with its default value.
 * DMA_InitStruct : pointer to a DMA_InitTypeDef structure which will be initialized.
 */
void DMA::structInit(DMA_InitTypeDef* DMA_InitStruct)
{
  /*-------------- Reset DMA init structure parameters values ----------------*/
  /* Initialize the DMA_Channel member */
  DMA_InitStruct->DMA_Channel = 0;

  /* Initialize the DMA_PeripheralBaseAddr member */
  DMA_InitStruct->DMA_PeripheralBaseAddr = 0;

  /* Initialize the DMA_Memory0BaseAddr member */
  DMA_InitStruct->DMA_Memory0BaseAddr = 0;

  /* Initialize the DMA_DIR member */
  DMA_InitStruct->DMA_DIR = DMA_DIR_PeripheralToMemory;

  /* Initialize the DMA_BufferSize member */
  DMA_InitStruct->DMA_BufferSize = 0;

  /* Initialize the DMA_PeripheralInc member */
  DMA_InitStruct->DMA_PeripheralInc = DMA_PeripheralInc_Disable;

  /* Initialize the DMA_MemoryInc member */
  DMA_InitStruct->DMA_MemoryInc = DMA_MemoryInc_Disable;

  /* Initialize the DMA_PeripheralDataSize member */
  DMA_InitStruct->DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;

  /* Initialize the DMA_MemoryDataSize member */
  DMA_InitStruct->DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;

  /* Initialize the DMA_Mode member */
  DMA_InitStruct->DMA_Mode = DMA_Mode_Normal;

  /* Initialize the DMA_Priority member */
  DMA_InitStruct->DMA_Priority = DMA_Priority_Low;

  /* Initialize the DMA_FIFOMode member */
  DMA_InitStruct->DMA_FIFOMode = DMA_FIFOMode_Disable;

  /* Initialize the DMA_FIFOThreshold member */
  DMA_InitStruct->DMA_FIFOThreshold = DMA_FIFOThreshold_1QuarterFull;

  /* Initialize the DMA_MemoryBurst member */
  DMA_InitStruct->DMA_MemoryBurst = DMA_MemoryBurst_Single;

  /* Initialize the DMA_PeripheralBurst member */
  DMA_InitStruct->DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
}

/*
 * Enables or disables the specified DMAy Streamx.
 * DMAy_Streamx: where y can be 1 or 2 to select the DMA and x can be 0
 * 	to 7 to select the DMA Stream.
 *
 * This function may be used to perform Pause-Resume operation. When a
 * transfer is ongoing, calling this function to disable the Stream will
 * cause the transfer to be paused. All configuration registers and the
 * number of remaining data will be preserved. When calling again this
 * function to re-enable the Stream, the transfer will be resumed from
 * the point where it was paused.          
 *    
 * After configuring the DMA Stream (DMA_Init() function) and enabling the
 * stream, it is recommended to check (or wait until) the DMA Stream is
 * effectively enabled. A Stream may remain disabled if a configuration 
 * parameter is wrong.
 * After disabling a DMA Stream, it is also recommended to check (or wait
 * until) the DMA Stream is effectively disabled. If a Stream is disabled 
 * while a data transfer is ongoing, the current data will be transferred
 * and the Stream will be effectively disabled only after the transfer of
 * this single data is finished.            
 */
 void DMA::enableDisable(DMA_Stream_TypeDef* DMAy_Streamx, FunctionalState NewState)
{
  if(NewState != DISABLE)
  {
    DMAy_Streamx->CR |= (uint32_t)DMA_SxCR_EN;
  }
  else
  {
    DMAy_Streamx->CR &= ~(uint32_t)DMA_SxCR_EN;
  }
}

/*
 * Initializes the DMAy Streamx according to the specified parameters in 
 * 	the DMA_InitStruct structure.
 * Before calling this function, it is recommended to check that the Stream 
 *  is actually disabled using the function DMA_GetCmdStatus().  
 * DMAy_Streamx: where y can be 1 or 2 to select the DMA and x can be 0
 *  to 7 to select the DMA Stream.
 * DMA_InitStruct: pointer to a DMA_InitTypeDef structure that contains
 *	the configuration information for the specified DMA Stream.  
 */
void DMA::init(DMA_Stream_TypeDef* DMAy_Streamx, DMA_InitTypeDef* DMA_InitStruct)
{
  uint32_t tmpreg = 0;

  /*------------------------- DMAy Streamx CR Configuration ------------------*/
  /* Get the DMAy_Streamx CR value */
  tmpreg = DMAy_Streamx->CR;

  /* Clear CHSEL, MBURST, PBURST, PL, MSIZE, PSIZE, MINC, PINC, CIRC and DIR bits */
  tmpreg &= ((uint32_t)~(DMA_SxCR_CHSEL | DMA_SxCR_MBURST | DMA_SxCR_PBURST | \
                         DMA_SxCR_PL | DMA_SxCR_MSIZE | DMA_SxCR_PSIZE | \
                         DMA_SxCR_MINC | DMA_SxCR_PINC | DMA_SxCR_CIRC | \
                         DMA_SxCR_DIR));

  /* Configure DMAy Streamx: */
  /* Set CHSEL bits according to DMA_CHSEL value */
  /* Set DIR bits according to DMA_DIR value */
  /* Set PINC bit according to DMA_PeripheralInc value */
  /* Set MINC bit according to DMA_MemoryInc value */
  /* Set PSIZE bits according to DMA_PeripheralDataSize value */
  /* Set MSIZE bits according to DMA_MemoryDataSize value */
  /* Set CIRC bit according to DMA_Mode value */
  /* Set PL bits according to DMA_Priority value */
  /* Set MBURST bits according to DMA_MemoryBurst value */
  /* Set PBURST bits according to DMA_PeripheralBurst value */
  tmpreg |= DMA_InitStruct->DMA_Channel | DMA_InitStruct->DMA_DIR |
            DMA_InitStruct->DMA_PeripheralInc | DMA_InitStruct->DMA_MemoryInc |
            DMA_InitStruct->DMA_PeripheralDataSize | DMA_InitStruct->DMA_MemoryDataSize |
            DMA_InitStruct->DMA_Mode | DMA_InitStruct->DMA_Priority |
            DMA_InitStruct->DMA_MemoryBurst | DMA_InitStruct->DMA_PeripheralBurst;

  /* Write to DMAy Streamx CR register */
  DMAy_Streamx->CR = tmpreg;

  /*------------------------- DMAy Streamx FCR Configuration -----------------*/
  /* Get the DMAy_Streamx FCR value */
  tmpreg = DMAy_Streamx->FCR;

  /* Clear DMDIS and FTH bits */
  tmpreg &= (uint32_t)~(DMA_SxFCR_DMDIS | DMA_SxFCR_FTH);

  /* Configure DMAy Streamx FIFO: 
    Set DMDIS bits according to DMA_FIFOMode value 
    Set FTH bits according to DMA_FIFOThreshold value */
  tmpreg |= DMA_InitStruct->DMA_FIFOMode | DMA_InitStruct->DMA_FIFOThreshold;

  /* Write to DMAy Streamx CR */
  DMAy_Streamx->FCR = tmpreg;

  /*------------------------- DMAy Streamx NDTR Configuration ----------------*/
  /* Write to DMAy Streamx NDTR register */
  DMAy_Streamx->NDTR = DMA_InitStruct->DMA_BufferSize;

  /*------------------------- DMAy Streamx PAR Configuration -----------------*/
  /* Write to DMAy Streamx PAR */
  DMAy_Streamx->PAR = DMA_InitStruct->DMA_PeripheralBaseAddr;

  /*------------------------- DMAy Streamx M0AR Configuration ----------------*/
  /* Write to DMAy Streamx M0AR */
  DMAy_Streamx->M0AR = DMA_InitStruct->DMA_Memory0BaseAddr;
}



/*
 * Deinitialize the DMAy Streamx registers to their default reset values.
 * DMAy_Streamx: where y can be 1 or 2 to select the DMA and x can be 0
 * 	to 7 to select the DMA Stream.
 */
void DMA::deInit(DMA_Stream_TypeDef* DMAy_Streamx)
{
  /* Disable the selected DMAy Streamx */
  DMAy_Streamx->CR &= ~((uint32_t)DMA_SxCR_EN);

  /* Reset DMAy Streamx control register */
  DMAy_Streamx->CR  = 0;
  
  /* Reset DMAy Streamx Number of Data to Transfer register */
  DMAy_Streamx->NDTR = 0;
  
  /* Reset DMAy Streamx peripheral address register */
  DMAy_Streamx->PAR  = 0;
  
  /* Reset DMAy Streamx memory 0 address register */
  DMAy_Streamx->M0AR = 0;

  /* Reset DMAy Streamx memory 1 address register */
  DMAy_Streamx->M1AR = 0;

  /* Reset DMAy Streamx FIFO control register */
  DMAy_Streamx->FCR = (uint32_t)0x00000021; 

  /* Reset interrupt pending bits for the selected stream */
  if(DMAy_Streamx == DMA1_Stream0)
  {
    /* Reset interrupt pending bits for DMA1 Stream0 */
    DMA1->LIFCR = DMA_Stream0_IT_MASK;
  }
  else if(DMAy_Streamx == DMA1_Stream1)
  {
    /* Reset interrupt pending bits for DMA1 Stream1 */
    DMA1->LIFCR = DMA_Stream1_IT_MASK;
  }
  else if(DMAy_Streamx == DMA1_Stream2)
  {
    /* Reset interrupt pending bits for DMA1 Stream2 */
    DMA1->LIFCR = DMA_Stream2_IT_MASK;
  }
  else if(DMAy_Streamx == DMA1_Stream3)
  {
    /* Reset interrupt pending bits for DMA1 Stream3 */
    DMA1->LIFCR = DMA_Stream3_IT_MASK;
  }
  else if(DMAy_Streamx == DMA1_Stream4)
  {
    /* Reset interrupt pending bits for DMA1 Stream4 */
    DMA1->HIFCR = DMA_Stream4_IT_MASK;
  }
  else if(DMAy_Streamx == DMA1_Stream5)
  {
    /* Reset interrupt pending bits for DMA1 Stream5 */
    DMA1->HIFCR = DMA_Stream5_IT_MASK;
  }
  else if(DMAy_Streamx == DMA1_Stream6)
  {
    /* Reset interrupt pending bits for DMA1 Stream6 */
    DMA1->HIFCR = (uint32_t)DMA_Stream6_IT_MASK;
  }
  else if(DMAy_Streamx == DMA1_Stream7)
  {
    /* Reset interrupt pending bits for DMA1 Stream7 */
    DMA1->HIFCR = DMA_Stream7_IT_MASK;
  }
  else if(DMAy_Streamx == DMA2_Stream0)
  {
    /* Reset interrupt pending bits for DMA2 Stream0 */
    DMA2->LIFCR = DMA_Stream0_IT_MASK;
  }
  else if(DMAy_Streamx == DMA2_Stream1)
  {
    /* Reset interrupt pending bits for DMA2 Stream1 */
    DMA2->LIFCR = DMA_Stream1_IT_MASK;
  }
  else if(DMAy_Streamx == DMA2_Stream2)
  {
    /* Reset interrupt pending bits for DMA2 Stream2 */
    DMA2->LIFCR = DMA_Stream2_IT_MASK;
  }
  else if(DMAy_Streamx == DMA2_Stream3)
  {
    /* Reset interrupt pending bits for DMA2 Stream3 */
    DMA2->LIFCR = DMA_Stream3_IT_MASK;
  }
  else if(DMAy_Streamx == DMA2_Stream4)
  {
    /* Reset interrupt pending bits for DMA2 Stream4 */
    DMA2->HIFCR = DMA_Stream4_IT_MASK;
  }
  else if(DMAy_Streamx == DMA2_Stream5)
  {
    /* Reset interrupt pending bits for DMA2 Stream5 */
    DMA2->HIFCR = DMA_Stream5_IT_MASK;
  }
  else if(DMAy_Streamx == DMA2_Stream6)
  {
    /* Reset interrupt pending bits for DMA2 Stream6 */
    DMA2->HIFCR = DMA_Stream6_IT_MASK;
  }
  else 
  {
    if(DMAy_Streamx == DMA2_Stream7)
    {
      /* Reset interrupt pending bits for DMA2 Stream7 */
      DMA2->HIFCR = DMA_Stream7_IT_MASK;
    }
  }
}

/*
 * Enables or disables the specified DMAy Streamx interrupts.
 * DMAy_Streamx: where y can be 1 or 2 to select the DMA and x can be 0
 *  to 7 to select the DMA Stream.
 * DMA_IT: specifies the DMA interrupt sources to be enabled or disabled. 
 *  This parameter can be any combination of the following values:
 *   DMA_IT_TC:  Transfer complete interrupt mask
 *   DMA_IT_HT:  Half transfer complete interrupt mask
 *   DMA_IT_TE:  Transfer error interrupt mask
 *   DMA_IT_FE:  FIFO error interrupt mask
 * NewState: new state of the specified DMA interrupts.
 *  This parameter can be: ENABLE or DISABLE.
 */
void DMA::enableDisableInterrupt(DMA_Stream_TypeDef* DMAy_Streamx, uint32_t DMA_IT, FunctionalState NewState)
{
  /* Check if the DMA_IT parameter contains a FIFO interrupt */
  if((DMA_IT & DMA_IT_FE) != 0)
  {
    if(NewState != DISABLE)
    {
      /* Enable the selected DMA FIFO interrupts */
      DMAy_Streamx->FCR |= (uint32_t)DMA_IT_FE;
    }    
    else 
    {
      /* Disable the selected DMA FIFO interrupts */
      DMAy_Streamx->FCR &= ~(uint32_t)DMA_IT_FE;  
    }
  }

  /* Check if the DMA_IT parameter contains a Transfer interrupt */
  if(DMA_IT != DMA_IT_FE)
  {
    if(NewState != DISABLE)
    {
      /* Enable the selected DMA transfer interrupts */
      DMAy_Streamx->CR |= (uint32_t)(DMA_IT  & TRANSFER_IT_ENABLE_MASK);
    }
    else
    {
      /* Disable the selected DMA transfer interrupts */
      DMAy_Streamx->CR &= ~(uint32_t)(DMA_IT & TRANSFER_IT_ENABLE_MASK);
    }    
  }
}

/*
 * Checks whether the specified DMAy Streamx interrupt has occurred or not.
 * DMAy_Streamx: where y can be 1 or 2 to select the DMA and x can be 0
 *  to 7 to select the DMA Stream.
 * DMA_IT: specifies the DMA interrupt source to check.
 *  This parameter can be one of the following values:
 *   DMA_IT_TCIFx:  Streamx transfer complete interrupt
 *   DMA_IT_HTIFx:  Streamx half transfer complete interrupt
 *   DMA_IT_TEIFx:  Streamx transfer error interrupt
 *   DMA_IT_DMEIFx: Streamx direct mode error interrupt
 *   DMA_IT_FEIFx:  Streamx FIFO error interrupt
 *    Where x can be 0 to 7 to select the DMA Stream.
 */
ITStatus DMA::getInterruptStatus(DMA_Stream_TypeDef* DMAy_Streamx, uint32_t DMA_IT)
{
  ITStatus bitstatus = RESET;
  DMA_TypeDef* DMAy;
  uint32_t tmpreg = 0, enablestatus = 0;

  /* Determine the DMA to which belongs the stream */
  if(DMAy_Streamx < DMA2_Stream0)
  {
    /* DMAy_Streamx belongs to DMA1 */
    DMAy = DMA1; 
  } 
  else 
  {
    /* DMAy_Streamx belongs to DMA2 */
    DMAy = DMA2; 
  }

  /* Check if the interrupt enable bit is in the CR or FCR register */
  if((DMA_IT & TRANSFER_IT_MASK) != (uint32_t)RESET)
  {
    /* Get the interrupt enable position mask in CR register */
    tmpreg = (uint32_t)((DMA_IT >> 11) & TRANSFER_IT_ENABLE_MASK);   
    
    /* Check the enable bit in CR register */
    enablestatus = (uint32_t)(DMAy_Streamx->CR & tmpreg);
  }
  else 
  {
    /* Check the enable bit in FCR register */
    enablestatus = (uint32_t)(DMAy_Streamx->FCR & DMA_IT_FE); 
  }
 
  /* Check if the interrupt pending flag is in LISR or HISR */
  if((DMA_IT & HIGH_ISR_MASK) != (uint32_t)RESET)
  {
    /* Get DMAy HISR register value */
    tmpreg = DMAy->HISR ;
  }
  else
  {
    /* Get DMAy LISR register value */
    tmpreg = DMAy->LISR ;
  } 

  /* mask all reserved bits */
  tmpreg &= (uint32_t)RESERVED_MASK;

  /* Check the status of the specified DMA interrupt */
  if(((tmpreg & DMA_IT) != (uint32_t)RESET) && (enablestatus != (uint32_t)RESET))
  {
    /* DMA_IT is set */
    bitstatus = SET;
  }
  else
  {
    /* DMA_IT is reset */
    bitstatus = RESET;
  }

  /* Return the DMA_IT status */
  return  bitstatus;
}

/*
  * Clears the DMAy Streamx's interrupt pending bits.
  * DMAy_Streamx: where y can be 1 or 2 to select the DMA and x can be 0
  *  to 7 to select the DMA Stream.
  * DMA_IT: specifies the DMA interrupt pending bit to clear.
  *  This parameter can be any combination of the following values:
  *   DMA_IT_TCIFx:  Streamx transfer complete interrupt
  *   DMA_IT_HTIFx:  Streamx half transfer complete interrupt
  *   DMA_IT_TEIFx:  Streamx transfer error interrupt
  *   DMA_IT_DMEIFx: Streamx direct mode error interrupt
  *   DMA_IT_FEIFx:  Streamx FIFO error interrupt
  *    Where x can be 0 to 7 to select the DMA Stream.
  */
void DMA::clearInterruptPendingBit(DMA_Stream_TypeDef* DMAy_Streamx, uint32_t DMA_IT)
{
	DMA_TypeDef* DMAy;

  /* Determine the DMA to which belongs the stream */
  if(DMAy_Streamx < DMA2_Stream0)
  {
    /* DMAy_Streamx belongs to DMA1 */
    DMAy = DMA1; 
  } 
  else 
  {
    /* DMAy_Streamx belongs to DMA2 */
    DMAy = DMA2; 
  }

  /* Check if LIFCR or HIFCR register is targeted */
  if((DMA_IT & HIGH_ISR_MASK) != (uint32_t)RESET)
  {
    /* Set DMAy HIFCR register clear interrupt bits */
    DMAy->HIFCR = (uint32_t)(DMA_IT & RESERVED_MASK);
  }
  else 
  {
    /* Set DMAy LIFCR register clear interrupt bits */
    DMAy->LIFCR = (uint32_t)(DMA_IT & RESERVED_MASK);
  }   
}
