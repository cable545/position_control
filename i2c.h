#ifndef _I2C_H
#define _I2C_H

typedef struct
{
  uint32_t I2C_ClockSpeed;          /*!< Specifies the clock frequency.
                                         This parameter must be set to a value lower than 400kHz */

  uint16_t I2C_Mode;                /*!< Specifies the I2C mode.
                                         This parameter can be a value of I2C_mode */

  uint16_t I2C_DutyCycle;           /*!< Specifies the I2C fast mode duty cycle.
                                         This parameter can be a value of I2C_duty_cycle_in_fast_mode */

  uint16_t I2C_OwnAddress1;         /*!< Specifies the first device own address.
                                         This parameter can be a 7-bit or 10-bit address. */

  uint16_t I2C_Ack;                 /*!< Enables or disables the acknowledgement.
                                         This parameter can be a value of I2C_acknowledgement */

  uint16_t I2C_AcknowledgedAddress; /*!< Specifies if 7-bit or 10-bit address is acknowledged.
                                         This parameter can be a value of @ref I2C_acknowledged_address */
}I2C_InitTypeDef;

/**
  *	I2C_mode
  */
#define I2C_Mode_I2C                    ((uint16_t)0x0000)
#define I2C_Mode_SMBusDevice            ((uint16_t)0x0002)  
#define I2C_Mode_SMBusHost              ((uint16_t)0x000A)

/**
  * I2C_duty_cycle_in_fast_mode 
  */
#define I2C_DutyCycle_16_9              ((uint16_t)0x4000) /*!< I2C fast mode Tlow/Thigh = 16/9 */
#define I2C_DutyCycle_2                 ((uint16_t)0xBFFF) /*!< I2C fast mode Tlow/Thigh = 2 */

/**
  * I2C_acknowledgement
  */
#define I2C_Ack_Enable                  ((uint16_t)0x0400)
#define I2C_Ack_Disable                 ((uint16_t)0x0000)

/**
  * I2C_acknowledged_address 
  */
#define I2C_AcknowledgedAddress_7bit    ((uint16_t)0x4000)
#define I2C_AcknowledgedAddress_10bit   ((uint16_t)0xC000)

/** 
  * SR2 register flags  
  */
#define I2C_FLAG_DUALF                  ((uint32_t)0x00800000)
#define I2C_FLAG_SMBHOST                ((uint32_t)0x00400000)
#define I2C_FLAG_SMBDEFAULT             ((uint32_t)0x00200000)
#define I2C_FLAG_GENCALL                ((uint32_t)0x00100000)
#define I2C_FLAG_TRA                    ((uint32_t)0x00040000)
#define I2C_FLAG_BUSY                   ((uint32_t)0x00020000)
#define I2C_FLAG_MSL                    ((uint32_t)0x00010000)

/** 
	* I2C_transfer_direction 
  */
#define  I2C_Direction_Transmitter      ((uint8_t)0x00)
#define  I2C_Direction_Receiver         ((uint8_t)0x01)

/** @defgroup I2C_interrupts_definition 
  * @{
  */

#define I2C_IT_BUF                      ((uint16_t)0x0400)
#define I2C_IT_EVT                      ((uint16_t)0x0200)
#define I2C_IT_ERR                      ((uint16_t)0x0100)

/**
 ===============================================================================
               I2C Master Events (Events grouped in order of communication)
 ===============================================================================
 */

/** 
  * Communication start
  * 
  * After sending the START condition (I2C_GenerateSTART() function) the master 
  * has to wait for this event. It means that the Start condition has been correctly 
  * released on the I2C bus (the bus is free, no other devices is communicating).
  * 
  */
/* --EV5 */
#define  I2C_EVENT_MASTER_MODE_SELECT                      ((uint32_t)0x00030001)  /* BUSY, MSL and SB flag */

/** 
  * Address Acknowledge
  * 
  * After checking on EV5 (start condition correctly released on the bus), the 
  * master sends the address of the slave(s) with which it will communicate 
  * (I2C_Send7bitAddress() function, it also determines the direction of the communication: 
  * Master transmitter or Receiver). Then the master has to wait that a slave acknowledges 
  * his address. If an acknowledge is sent on the bus, one of the following events will 
  * be set:
  * 
  *  1) In case of Master Receiver (7-bit addressing): the I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED 
  *     event is set.
  *  
  *  2) In case of Master Transmitter (7-bit addressing): the I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED 
  *     is set
  *  
  *  3) In case of 10-Bit addressing mode, the master (just after generating the START 
  *  and checking on EV5) has to send the header of 10-bit addressing mode (I2C_SendData() 
  *  function). Then master should wait on EV9. It means that the 10-bit addressing 
  *  header has been correctly sent on the bus. Then master should send the second part of 
  *  the 10-bit address (LSB) using the function I2C_Send7bitAddress(). Then master 
  *  should wait for event EV6. 
  *     
  */

/* --EV6 */
#define  I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED        ((uint32_t)0x00070082)  /* BUSY, MSL, ADDR, TXE and TRA flags */
#define  I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED           ((uint32_t)0x00030002)  /* BUSY, MSL and ADDR flags */

/** 
  * Communication events
  * 
  * If a communication is established (START condition generated and slave address 
  * acknowledged) then the master has to check on one of the following events for 
  * communication procedures:
  *  
  * 1) Master Receiver mode: The master has to wait on the event EV7 then to read 
  *    the data received from the slave (I2C_ReceiveData() function).
  * 
  * 2) Master Transmitter mode: The master has to send data (I2C_SendData() 
  *    function) then to wait on event EV8 or EV8_2.
  *    These two events are similar: 
  *     - EV8 means that the data has been written in the data register and is 
  *       being shifted out.
  *     - EV8_2 means that the data has been physically shifted out and output 
  *       on the bus.
  *     In most cases, using EV8 is sufficient for the application.
  *     Using EV8_2 leads to a slower communication but ensure more reliable test.
  *     EV8_2 is also more suitable than EV8 for testing on the last data transmission 
  *     (before Stop condition generation).
  *     
  *  In case the  user software does not guarantee that this event EV7 is 
  *  managed before the current byte end of transfer, then user may check on EV7 
  *  and BTF flag at the same time (ie. (I2C_EVENT_MASTER_BYTE_RECEIVED | I2C_FLAG_BTF)).
  *  In this case the communication may be slower.
  * 
  */

/* Master RECEIVER mode -----------------------------*/ 
/* --EV7 */
#define  I2C_EVENT_MASTER_BYTE_RECEIVED                    ((uint32_t)0x00030040)  /* BUSY, MSL and RXNE flags */

/* Master TRANSMITTER mode --------------------------*/
/* --EV8 */
#define I2C_EVENT_MASTER_BYTE_TRANSMITTING                 ((uint32_t)0x00070080) /* TRA, BUSY, MSL, TXE flags */
/* --EV8_2 */
#define  I2C_EVENT_MASTER_BYTE_TRANSMITTED                 ((uint32_t)0x00070084)  /* TRA, BUSY, MSL, TXE and BTF flags */

/* --EV9 */
#define  I2C_EVENT_MASTER_MODE_ADDRESS10                   ((uint32_t)0x00030008)  /* BUSY, MSL and ADD10 flags */

#define FLAG_TIMEOUT						0x10000
#define LONG_TIMEOUT						(100 * FLAG_TIMEOUT)
#define I2C_SPEED 							400000

#define I2C_1										1
#define I2C_2										2
#define I2C_3										3

class I2C
{
public:
	//static void I2C::userDefCallback(char methodString[], char deviceString[]);

	/**
		* Initializes the I2Cx peripheral according to the specified 
		* 	parameters in the I2C_InitStruct.
		*           
		* To use the I2C at 400 KHz (in fast mode), the PCLK1 frequency 
		* 	(I2C peripheral input clock) must be a multiple of 10 MHz.  
		*           
		* I2Cx: where x can be 1, 2 or 3 to select the I2C peripheral.
		* I2C_InitStruct: pointer to a I2C_InitTypeDef structure that contains 
		* 	the configuration information for the specified I2C peripheral.
		*/
	static void init(I2C_TypeDef* I2Cx, I2C_InitTypeDef* I2C_InitStruct);
	
	/**
		* Fills each I2C_InitStruct member with its default value.
		* I2C_InitStruct: pointer to an I2C_InitTypeDef structure which will be initialized.
		*/
	static void structInit(I2C_InitTypeDef* I2C_InitStruct);

	/**
		* Enables or disables the specified I2C peripheral.
		* I2Cx: where x can be 1, 2 or 3 to select the I2C peripheral.
		* NewState: new state of the I2Cx peripheral. 
		* 	This parameter can be: ENABLE or DISABLE.
		*/
	static void cmd(I2C_TypeDef* I2Cx, FunctionalState NewState);

	/**
		* Enables or disables the specified I2C interrupts.
		* I2Cx: where x can be 1, 2 or 3 to select the I2C peripheral.
		* I2C_IT: specifies the I2C interrupts sources to be enabled or disabled. 
		* 	This parameter can be any combination of the following values:
		*   	I2C_IT_BUF: Buffer interrupt mask
		*     I2C_IT_EVT: Event interrupt mask
		*     I2C_IT_ERR: Error interrupt mask
		* NewState: new state of the specified I2C interrupts.
		* 	This parameter can be: ENABLE or DISABLE.
		*/
	static void intConfig(I2C_TypeDef* I2Cx, uint16_t I2C_IT, FunctionalState NewState);
	
	/**
		* Generates I2Cx communication START condition.
		* I2Cx: where x can be 1, 2 or 3 to select the I2C peripheral.
		* NewState: new state of the I2C START condition generation.
		* 	This parameter can be: ENABLE or DISABLE.
		*/
	static void generateStart(I2C_TypeDef* I2Cx, FunctionalState NewState);

	/**
		* Generates I2Cx communication STOP condition.
		* I2Cx: where x can be 1, 2 or 3 to select the I2C peripheral.
		* NewState: new state of the I2C STOP condition generation.
		* 	This parameter can be: ENABLE or DISABLE.
		*/
	static void generateStop(I2C_TypeDef* I2Cx, FunctionalState NewState);

	/**
		* Transmits the address byte to select the slave device.
		* I2Cx: where x can be 1, 2 or 3 to select the I2C peripheral.
		* Address: specifies the slave address which will be transmitted
		* I2C_Direction: specifies whether the I2C device will be a Transmitter
		* 	or a Receiver. 
		* 	This parameter can be one of the following values
		* 		I2C_Direction_Transmitter: Transmitter mode
		* 		I2C_Direction_Receiver: Receiver mode
		*/
	static void send7bitAddress(I2C_TypeDef* I2Cx, uint8_t Address, uint8_t I2C_Direction);
	
	/**
		* Enables or disables the specified I2C acknowledge feature.
		* I2Cx: where x can be 1, 2 or 3 to select the I2C peripheral.
		* NewState: new state of the I2C Acknowledgement.
		* 	This parameter can be: ENABLE or DISABLE.
		*/
	static void acknowledgeConfig(I2C_TypeDef* I2Cx, FunctionalState NewState);
	
	/**
		* Enables or disables the specified I2C software reset.
		* When software reset is enabled, the I2C IOs are released (this can
		* 	be useful to recover from bus errors).  
		* I2Cx: where x can be 1, 2 or 3 to select the I2C peripheral.
		* 	NewState: new state of the I2C software reset.
		*   This parameter can be: ENABLE or DISABLE.
		*/
	static void softwareResetCmd(I2C_TypeDef* I2Cx, FunctionalState NewState);
	
	/**
		* Checks whether the last I2Cx Event is equal to the one passed
		*         as parameter.
		* I2Cx: where x can be 1, 2 or 3 to select the I2C peripheral.
		* I2C_EVENT: specifies the event to be checked. 
		*          This parameter can be one of the following values:
		*            I2C_EVENT_SLAVE_TRANSMITTER_ADDRESS_MATCHED: EV1
		*            I2C_EVENT_SLAVE_RECEIVER_ADDRESS_MATCHED: EV1
		*            I2C_EVENT_SLAVE_TRANSMITTER_SECONDADDRESS_MATCHED: EV1
		*            I2C_EVENT_SLAVE_RECEIVER_SECONDADDRESS_MATCHED: EV1
		*            I2C_EVENT_SLAVE_GENERALCALLADDRESS_MATCHED: EV1
		*            I2C_EVENT_SLAVE_BYTE_RECEIVED: EV2
		*            (I2C_EVENT_SLAVE_BYTE_RECEIVED | I2C_FLAG_DUALF): EV2
		*            (I2C_EVENT_SLAVE_BYTE_RECEIVED | I2C_FLAG_GENCALL): EV2
		*            I2C_EVENT_SLAVE_BYTE_TRANSMITTED: EV3
		*            (I2C_EVENT_SLAVE_BYTE_TRANSMITTED | I2C_FLAG_DUALF): EV3
		*            (I2C_EVENT_SLAVE_BYTE_TRANSMITTED | I2C_FLAG_GENCALL): EV3
		*            I2C_EVENT_SLAVE_ACK_FAILURE: EV3_2
		*            I2C_EVENT_SLAVE_STOP_DETECTED: EV4
		*            I2C_EVENT_MASTER_MODE_SELECT: EV5
		*            I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED: EV6     
		*            I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED: EV6
		*            I2C_EVENT_MASTER_BYTE_RECEIVED: EV7
		*            I2C_EVENT_MASTER_BYTE_TRANSMITTING: EV8
		*            I2C_EVENT_MASTER_BYTE_TRANSMITTED: EV8_2
		*            I2C_EVENT_MASTER_MODE_ADDRESS10: EV9
		*     
		* For detailed description of Events, please refer to section I2C_Events
		* in stm32f4xx_i2c.h file.
		*    
		* retval An ErrorStatus enumeration value:
		*           - SUCCESS: Last event is equal to the I2C_EVENT
		*           - ERROR: Last event is different from the I2C_EVENT
		*/
	static ErrorStatus checkEvent(I2C_TypeDef* I2Cx, uint32_t I2C_EVENT);
	
	/**
		*  Checks whether the specified I2C flag is set or not.
		*  I2Cx: where x can be 1, 2 or 3 to select the I2C peripheral.
		*  I2C_FLAG: specifies the flag to check. 
		*          This parameter can be one of the following values:
		*            I2C_FLAG_DUALF: Dual flag (Slave mode)
		*            I2C_FLAG_SMBHOST: SMBus host header (Slave mode)
		*            I2C_FLAG_SMBDEFAULT: SMBus default header (Slave mode)
		*            I2C_FLAG_GENCALL: General call header flag (Slave mode)
		*            I2C_FLAG_TRA: Transmitter/Receiver flag
		*            I2C_FLAG_BUSY: Bus busy flag
		*            I2C_FLAG_MSL: Master/Slave flag
		*            I2C_FLAG_SMBALERT: SMBus Alert flag
		*            I2C_FLAG_TIMEOUT: Timeout or Tlow error flag
		*            I2C_FLAG_PECERR: PEC error in reception flag
		*            I2C_FLAG_OVR: Overrun/Underrun flag (Slave mode)
		*            I2C_FLAG_AF: Acknowledge failure flag
		*            I2C_FLAG_ARLO: Arbitration lost flag (Master mode)
		*            I2C_FLAG_BERR: Bus error flag
		*            I2C_FLAG_TXE: Data register empty flag (Transmitter)
		*            I2C_FLAG_RXNE: Data register not empty (Receiver) flag
		*            I2C_FLAG_STOPF: Stop detection flag (Slave mode)
		*            I2C_FLAG_ADD10: 10-bit header sent flag (Master mode)
		*            I2C_FLAG_BTF: Byte transfer finished flag
		*            I2C_FLAG_ADDR: Address sent flag (Master mode) "ADSL"
		*                           Address matched flag (Slave mode)"ENDAD"
		*            I2C_FLAG_SB: Start bit flag (Master mode)
		* retval The new state of I2C_FLAG (SET or RESET).
		*/
	static FlagStatus getFlagStatus(I2C_TypeDef* I2Cx, uint32_t I2C_FLAG);
	
  /********************* Low Level Data transfers functions *****************/

	/**
		* Sends a data byte through the I2Cx peripheral.
		* I2Cx: where x can be 1, 2 or 3 to select the I2C peripheral.
		* Data: Byte to be transmitted..
		*/
	static void sendData(I2C_TypeDef* I2Cx, uint8_t Data);
	
	/**
		* Returns the most recent received data by the I2Cx peripheral.
		* I2Cx: where x can be 1, 2 or 3 to select the I2C peripheral.
		* retval: The value of the received data.
		*/
	static uint8_t receiveData(I2C_TypeDef* I2Cx);
	
	/********************* High Level control and transfers functions *****************/
	
	/**
		* Mit I2C_start wird die Start-Condition auf dem Bus erzeugt und die Adresse des Slaves gesendet,
		* damit befindet sich der STM32 im Master Mode. Durch direction wird angegeben, ob nun Bytes
		* gesendet oder empfangen werden.
		*/
	static bool start(I2C_TypeDef* I2Cx, uint8_t address, uint8_t direction);

	/**
		* Mit I2C_restart wird eine restart-condition erzeugt, um z.B bei einem Lesevorgang nach senden
		* des zu lesenden Registers direkt mit dem Lesen des Registers fortgeführt werden kann,
		* ohne den Bus frei zu geben.
		*/
	static bool restart(I2C_TypeDef * I2Cx, uint8_t address, uint8_t direction);

	/**
		* waits of the finish of the last transmit from the last write process
		* and writes data in shift register
		*/
	static bool write(I2C_TypeDef* I2Cx, uint8_t data);

	/**
		* I2C_read_ack liest ein Byte vom Bus und sendet ein ack, beendet damit also nicht das Empfangen
		*/
	static bool readAck(I2C_TypeDef* I2Cx, uint8_t* data);

	/** 
		* I2C_read_nack liest ein Byte vom Bus und sendet ein NACK, beendet also den Empfangszyklus
		*/
	static bool readNack(I2C_TypeDef* I2Cx, uint8_t* data);

	/*
	 * I2C_stop wartet auf das Senden des letzten Bytes und erzeugt dann die Stop-Condition,
	 * beendet damit einen Schreibzyklus und gibt den Bus frei. 
	 */
	static bool stop(I2C_TypeDef* I2Cx);
	
	/*
	 * scans all addresses (0 - 127) to check if there is any device on the given interface
	 */
	static void scan(I2C_TypeDef* I2Cx);
	
	/********************* High Level transfers wrapper functions *****************/
	
	static bool writeBytes(I2C_TypeDef* I2Cx, uint8_t deviceAddr, uint8_t regAddr, uint8_t* value, uint32_t numBytes);
	
	static bool writeByte(I2C_TypeDef* I2Cx, uint8_t deviceAddr, uint8_t regAddr, uint8_t byte);
	
	static bool readBytes(I2C_TypeDef* I2Cx, uint8_t deviceAddr, uint8_t regAddr, uint8_t* bytes, uint32_t numBytes);
	
	static bool readByte(I2C_TypeDef* I2Cx, uint8_t deviceAddr, uint8_t regAddr, uint8_t* byte);
	
};


#endif
