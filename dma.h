#ifndef _DMA_H
#define _DMA_H

/* 
 * DMA Init structure definition
 */
typedef struct
{
  uint32_t DMA_Channel;            /*!< Specifies the channel used for the specified stream. 
                                        This parameter can be a value of @ref DMA_channel */
 
  uint32_t DMA_PeripheralBaseAddr; /*!< Specifies the peripheral base address for DMAy Streamx. */

  uint32_t DMA_Memory0BaseAddr;    /*!< Specifies the memory 0 base address for DMAy Streamx. 
                                        This memory is the default memory used when double buffer mode is
                                        not enabled. */

  uint32_t DMA_DIR;                /*!< Specifies if the data will be transferred from memory to peripheral, 
                                        from memory to memory or from peripheral to memory.
                                        This parameter can be a value of @ref DMA_data_transfer_direction */

  uint32_t DMA_BufferSize;         /*!< Specifies the buffer size, in data unit, of the specified Stream. 
                                        The data unit is equal to the configuration set in DMA_PeripheralDataSize
                                        or DMA_MemoryDataSize members depending in the transfer direction. */

  uint32_t DMA_PeripheralInc;      /*!< Specifies whether the Peripheral address register should be incremented or not.
                                        This parameter can be a value of @ref DMA_peripheral_incremented_mode */

  uint32_t DMA_MemoryInc;          /*!< Specifies whether the memory address register should be incremented or not.
                                        This parameter can be a value of @ref DMA_memory_incremented_mode */

  uint32_t DMA_PeripheralDataSize; /*!< Specifies the Peripheral data width.
                                        This parameter can be a value of @ref DMA_peripheral_data_size */

  uint32_t DMA_MemoryDataSize;     /*!< Specifies the Memory data width.
                                        This parameter can be a value of @ref DMA_memory_data_size */

  uint32_t DMA_Mode;               /*!< Specifies the operation mode of the DMAy Streamx.
                                        This parameter can be a value of @ref DMA_circular_normal_mode
                                        @note The circular buffer mode cannot be used if the memory-to-memory
                                              data transfer is configured on the selected Stream */

  uint32_t DMA_Priority;           /*!< Specifies the software priority for the DMAy Streamx.
                                        This parameter can be a value of @ref DMA_priority_level */

  uint32_t DMA_FIFOMode;          /*!< Specifies if the FIFO mode or Direct mode will be used for the specified Stream.
                                        This parameter can be a value of @ref DMA_fifo_direct_mode
                                        @note The Direct mode (FIFO mode disabled) cannot be used if the 
                                               memory-to-memory data transfer is configured on the selected Stream */

  uint32_t DMA_FIFOThreshold;      /*!< Specifies the FIFO threshold level.
                                        This parameter can be a value of @ref DMA_fifo_threshold_level */

  uint32_t DMA_MemoryBurst;        /*!< Specifies the Burst transfer configuration for the memory transfers. 
                                        It specifies the amount of data to be transferred in a single non interruptable 
                                        transaction. This parameter can be a value of @ref DMA_memory_burst 
                                        @note The burst mode is possible only if the address Increment mode is enabled. */

  uint32_t DMA_PeripheralBurst;    /*!< Specifies the Burst transfer configuration for the peripheral transfers. 
                                        It specifies the amount of data to be transferred in a single non interruptable 
                                        transaction. This parameter can be a value of @ref DMA_peripheral_burst
                                        @note The burst mode is possible only if the address Increment mode is enabled. */  
}DMA_InitTypeDef;

/*
 * DMA_channel 
 */
#define DMA_Channel_0                     ((uint32_t)0x00000000)
#define DMA_Channel_1                     ((uint32_t)0x02000000)
#define DMA_Channel_2                     ((uint32_t)0x04000000)
#define DMA_Channel_3                     ((uint32_t)0x06000000)
#define DMA_Channel_4                     ((uint32_t)0x08000000)
#define DMA_Channel_5                     ((uint32_t)0x0A000000)
#define DMA_Channel_6                     ((uint32_t)0x0C000000)
#define DMA_Channel_7                     ((uint32_t)0x0E000000)

/*
 * DMA_data_transfer_direction 
 */
#define DMA_DIR_PeripheralToMemory        ((uint32_t)0x00000000)
#define DMA_DIR_MemoryToPeripheral        ((uint32_t)0x00000040) 
#define DMA_DIR_MemoryToMemory            ((uint32_t)0x00000080)

/*
 * DMA_peripheral_incremented_mode 
 */ 
#define DMA_PeripheralInc_Enable          ((uint32_t)0x00000200)
#define DMA_PeripheralInc_Disable         ((uint32_t)0x00000000)

/*
 * DMA_memory_incremented_mode 
 */ 
#define DMA_MemoryInc_Enable              ((uint32_t)0x00000400)
#define DMA_MemoryInc_Disable             ((uint32_t)0x00000000)

/*
 * DMA_peripheral_data_size 
 */
#define DMA_PeripheralDataSize_Byte       ((uint32_t)0x00000000) 
#define DMA_PeripheralDataSize_HalfWord   ((uint32_t)0x00000800) 
#define DMA_PeripheralDataSize_Word       ((uint32_t)0x00001000)

/*
 * DMA_memory_data_size 
 */
#define DMA_MemoryDataSize_Byte           ((uint32_t)0x00000000) 
#define DMA_MemoryDataSize_HalfWord       ((uint32_t)0x00002000) 
#define DMA_MemoryDataSize_Word           ((uint32_t)0x00004000)

/*
 * DMA_circular_normal_mode 
 */
#define DMA_Mode_Normal                   ((uint32_t)0x00000000) 
#define DMA_Mode_Circular                 ((uint32_t)0x00000100)

/*
 * DMA_priority_level 
 */
#define DMA_Priority_Low                  ((uint32_t)0x00000000)
#define DMA_Priority_Medium               ((uint32_t)0x00010000) 
#define DMA_Priority_High                 ((uint32_t)0x00020000)
#define DMA_Priority_VeryHigh             ((uint32_t)0x00030000)

/*
 * DMA_fifo_direct_mode 
 */
#define DMA_FIFOMode_Disable              ((uint32_t)0x00000000) 
#define DMA_FIFOMode_Enable               ((uint32_t)0x00000004)

/*
 * DMA_fifo_threshold_level 
 */
#define DMA_FIFOThreshold_1QuarterFull    ((uint32_t)0x00000000)
#define DMA_FIFOThreshold_HalfFull        ((uint32_t)0x00000001) 
#define DMA_FIFOThreshold_3QuartersFull   ((uint32_t)0x00000002)
#define DMA_FIFOThreshold_Full            ((uint32_t)0x00000003)

/*
 * DMA_memory_burst 
 */
#define DMA_MemoryBurst_Single            ((uint32_t)0x00000000)
#define DMA_MemoryBurst_INC4              ((uint32_t)0x00800000)  
#define DMA_MemoryBurst_INC8              ((uint32_t)0x01000000)
#define DMA_MemoryBurst_INC16             ((uint32_t)0x01800000)

/*
 * DMA_peripheral_burst 
 */ 
#define DMA_PeripheralBurst_Single        ((uint32_t)0x00000000)
#define DMA_PeripheralBurst_INC4          ((uint32_t)0x00200000)  
#define DMA_PeripheralBurst_INC8          ((uint32_t)0x00400000)
#define DMA_PeripheralBurst_INC16         ((uint32_t)0x00600000)

/* 
 * DMA_interrupt_enable_definitions 
 */ 
#define DMA_IT_TC                         ((uint32_t)0x00000010) // Transfer complete
#define DMA_IT_HT                         ((uint32_t)0x00000008) // Half-transfer reached
#define DMA_IT_TE                         ((uint32_t)0x00000004) // Transfer error
#define DMA_IT_DME                        ((uint32_t)0x00000002) // Direct mode error
#define DMA_IT_FE                         ((uint32_t)0x00000080) // Fifo error (overrun, underrun or FIFO level error)

/*
 * DMA_interrupts_definitions 
 */ 
#define DMA_IT_FEIF0                      ((uint32_t)0x90000001)
#define DMA_IT_DMEIF0                     ((uint32_t)0x10001004)
#define DMA_IT_TEIF0                      ((uint32_t)0x10002008)
#define DMA_IT_HTIF0                      ((uint32_t)0x10004010)
#define DMA_IT_TCIF0                      ((uint32_t)0x10008020)
#define DMA_IT_FEIF1                      ((uint32_t)0x90000040)
#define DMA_IT_DMEIF1                     ((uint32_t)0x10001100)
#define DMA_IT_TEIF1                      ((uint32_t)0x10002200)
#define DMA_IT_HTIF1                      ((uint32_t)0x10004400)
#define DMA_IT_TCIF1                      ((uint32_t)0x10008800)
#define DMA_IT_FEIF2                      ((uint32_t)0x90010000)
#define DMA_IT_DMEIF2                     ((uint32_t)0x10041000)
#define DMA_IT_TEIF2                      ((uint32_t)0x10082000)
#define DMA_IT_HTIF2                      ((uint32_t)0x10104000)
#define DMA_IT_TCIF2                      ((uint32_t)0x10208000)
#define DMA_IT_FEIF3                      ((uint32_t)0x90400000)
#define DMA_IT_DMEIF3                     ((uint32_t)0x11001000)
#define DMA_IT_TEIF3                      ((uint32_t)0x12002000)
#define DMA_IT_HTIF3                      ((uint32_t)0x14004000)
#define DMA_IT_TCIF3                      ((uint32_t)0x18008000)
#define DMA_IT_FEIF4                      ((uint32_t)0xA0000001)
#define DMA_IT_DMEIF4                     ((uint32_t)0x20001004)
#define DMA_IT_TEIF4                      ((uint32_t)0x20002008)
#define DMA_IT_HTIF4                      ((uint32_t)0x20004010)
#define DMA_IT_TCIF4                      ((uint32_t)0x20008020)
#define DMA_IT_FEIF5                      ((uint32_t)0xA0000040)
#define DMA_IT_DMEIF5                     ((uint32_t)0x20001100)
#define DMA_IT_TEIF5                      ((uint32_t)0x20002200)
#define DMA_IT_HTIF5                      ((uint32_t)0x20004400)
#define DMA_IT_TCIF5                      ((uint32_t)0x20008800)
#define DMA_IT_FEIF6                      ((uint32_t)0xA0010000)
#define DMA_IT_DMEIF6                     ((uint32_t)0x20041000)
#define DMA_IT_TEIF6                      ((uint32_t)0x20082000)
#define DMA_IT_HTIF6                      ((uint32_t)0x20104000)
#define DMA_IT_TCIF6                      ((uint32_t)0x20208000)
#define DMA_IT_FEIF7                      ((uint32_t)0xA0400000)
#define DMA_IT_DMEIF7                     ((uint32_t)0x21001000)
#define DMA_IT_TEIF7                      ((uint32_t)0x22002000)
#define DMA_IT_HTIF7                      ((uint32_t)0x24004000)
#define DMA_IT_TCIF7                      ((uint32_t)0x28008000)

class DMA
{
	public:
		static void structInit(DMA_InitTypeDef* DMA_InitStruct);
		static void enableDisable(DMA_Stream_TypeDef* DMAy_Streamx, FunctionalState NewState);
		static void init(DMA_Stream_TypeDef* DMAy_Streamx, DMA_InitTypeDef* DMA_InitStruct);
		static void deInit(DMA_Stream_TypeDef* DMAy_Streamx);
		
		static void enableDisableInterrupt(DMA_Stream_TypeDef* DMAy_Streamx, uint32_t DMA_IT, FunctionalState NewState);
		static ITStatus getInterruptStatus(DMA_Stream_TypeDef* DMAy_Streamx, uint32_t DMA_IT);
		static void clearInterruptPendingBit(DMA_Stream_TypeDef* DMAy_Streamx, uint32_t DMA_IT);
};

#endif
