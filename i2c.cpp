#include "main.h"

/* Private define ------------------------------------------------------------*/
#define CR1_CLEAR_MASK    ((uint16_t)0xFBF5)      /*<! I2C registers Masks */
#define FLAG_MASK         ((uint32_t)0x00FFFFFF)  /*<! I2C FLAG mask */
#define ITEN_MASK         ((uint32_t)0x07000000)  /*<! I2C Interrupt Enable mask */

uint32_t timeout = LONG_TIMEOUT;

void I2C::init(I2C_TypeDef* I2Cx, I2C_InitTypeDef* I2C_InitStruct)
{
  uint16_t tmpreg = 0, freqrange = 0;
  uint16_t result = 0x04;
  uint32_t pclk1 = 8000000;
  RCC_ClocksTypeDef  rcc_clocks;

/*---------------------------- I2Cx CR2 Configuration ------------------------*/
  /* Get the I2Cx CR2 value */
  tmpreg = I2Cx->CR2;
  /* Clear frequency FREQ[5:0] bits */
  tmpreg &= (uint16_t)~((uint16_t)I2C_CR2_FREQ);
  /* Get pclk1 frequency value */
  RCC_GetClocksFreq(&rcc_clocks);
  pclk1 = rcc_clocks.PCLK1_Frequency;
  /* Set frequency bits depending on pclk1 value */
  freqrange = (uint16_t)(pclk1 / 1000000);
  tmpreg |= freqrange;
  /* Write to I2Cx CR2 */
  I2Cx->CR2 = tmpreg;

/*---------------------------- I2Cx CCR Configuration ------------------------*/
  /* Disable the selected I2C peripheral to configure TRISE */
  I2Cx->CR1 &= (uint16_t)~((uint16_t)I2C_CR1_PE);
  /* Reset tmpreg value */
  /* Clear F/S, DUTY and CCR[11:0] bits */
  tmpreg = 0;

  /* Configure speed in standard mode */
  if(I2C_InitStruct->I2C_ClockSpeed <= 100000)
  {
    /* Standard mode speed calculate */
    result = (uint16_t)(pclk1 / (I2C_InitStruct->I2C_ClockSpeed << 1));
    /* Test if CCR value is under 0x4*/
    if(result < 0x04)
    {
      /* Set minimum allowed value */
      result = 0x04;  
    }
    /* Set speed value for standard mode */
    tmpreg |= result;	  
    /* Set Maximum Rise Time for standard mode */
    I2Cx->TRISE = freqrange + 1; 
  }
  /* Configure speed in fast mode */
  /* To use the I2C at 400 KHz (in fast mode), the PCLK1 frequency (I2C peripheral
     input clock) must be a multiple of 10 MHz */
  else /*(I2C_InitStruct->I2C_ClockSpeed <= 400000)*/
  {
    if(I2C_InitStruct->I2C_DutyCycle == I2C_DutyCycle_2)
    {
      /* Fast mode speed calculate: Tlow/Thigh = 2 */
      result = (uint16_t)(pclk1 / (I2C_InitStruct->I2C_ClockSpeed * 3));
    }
    else /*I2C_InitStruct->I2C_DutyCycle == I2C_DutyCycle_16_9*/
    {
      /* Fast mode speed calculate: Tlow/Thigh = 16/9 */
      result = (uint16_t)(pclk1 / (I2C_InitStruct->I2C_ClockSpeed * 25));
      /* Set DUTY bit */
      result |= I2C_DutyCycle_16_9;
    }

    /* Test if CCR value is under 0x1*/
    if((result & I2C_CCR_CCR) == 0)
    {
      /* Set minimum allowed value */
      result |= (uint16_t)0x0001;  
    }
    /* Set speed value and set F/S bit for fast mode */
    tmpreg |= (uint16_t)(result | I2C_CCR_FS);
    /* Set Maximum Rise Time for fast mode */
    I2Cx->TRISE = (uint16_t)(((freqrange * (uint16_t)300) / (uint16_t)1000) + (uint16_t)1);  
  }

  /* Write to I2Cx CCR */
  I2Cx->CCR = tmpreg;
  /* Enable the selected I2C peripheral */
  I2Cx->CR1 |= I2C_CR1_PE;

/*---------------------------- I2Cx CR1 Configuration ------------------------*/
  /* Get the I2Cx CR1 value */
  tmpreg = I2Cx->CR1;
  /* Clear ACK, SMBTYPE and  SMBUS bits */
  tmpreg &= CR1_CLEAR_MASK;
  /* Configure I2Cx: mode and acknowledgement */
  /* Set SMBTYPE and SMBUS bits according to I2C_Mode value */
  /* Set ACK bit according to I2C_Ack value */
  tmpreg |= (uint16_t)((uint32_t)I2C_InitStruct->I2C_Mode | I2C_InitStruct->I2C_Ack);
  /* Write to I2Cx CR1 */
  I2Cx->CR1 = tmpreg;

/*---------------------------- I2Cx OAR1 Configuration -----------------------*/
  /* Set I2Cx Own Address1 and acknowledged address */
  I2Cx->OAR1 = (I2C_InitStruct->I2C_AcknowledgedAddress | I2C_InitStruct->I2C_OwnAddress1);
}

void I2C::structInit(I2C_InitTypeDef* I2C_InitStruct)
{
/*---------------- Reset I2C init structure parameters values ----------------*/
  /* initialize the I2C_ClockSpeed member */
  I2C_InitStruct->I2C_ClockSpeed = 5000;
  /* Initialize the I2C_Mode member */
  I2C_InitStruct->I2C_Mode = I2C_Mode_I2C;
  /* Initialize the I2C_DutyCycle member */
  I2C_InitStruct->I2C_DutyCycle = I2C_DutyCycle_2;
  /* Initialize the I2C_OwnAddress1 member */
  I2C_InitStruct->I2C_OwnAddress1 = 0;
  /* Initialize the I2C_Ack member */
  I2C_InitStruct->I2C_Ack = I2C_Ack_Disable;
  /* Initialize the I2C_AcknowledgedAddress member */
  I2C_InitStruct->I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
}

void I2C::cmd(I2C_TypeDef* I2Cx, FunctionalState NewState)
{
  if(NewState != DISABLE)
  {
    /* Enable the selected I2C peripheral */
    I2Cx->CR1 |= I2C_CR1_PE;
  }
  else
  {
    /* Disable the selected I2C peripheral */
    I2Cx->CR1 &= (uint16_t)~((uint16_t)I2C_CR1_PE);
  }
}

void I2C::intConfig(I2C_TypeDef* I2Cx, uint16_t I2C_IT, FunctionalState NewState)
{
  if(NewState != DISABLE)
  {
    /* Enable the selected I2C interrupts */
    I2Cx->CR2 |= I2C_IT;
  }
  else
  {
    /* Disable the selected I2C interrupts */
    I2Cx->CR2 &= (uint16_t)~I2C_IT;
  }
}

void I2C::generateStart(I2C_TypeDef* I2Cx, FunctionalState NewState)
{
  if(NewState != DISABLE)
  {
    /* Generate a START condition */
    I2Cx->CR1 |= I2C_CR1_START;
  }
  else
  {
    /* Disable the START condition generation */
    I2Cx->CR1 &= (uint16_t)~((uint16_t)I2C_CR1_START);
  }
}

void I2C::generateStop(I2C_TypeDef* I2Cx, FunctionalState NewState)
{
  if(NewState != DISABLE)
  {
    /* Generate a STOP condition */
    I2Cx->CR1 |= I2C_CR1_STOP;
  }
  else
  {
    /* Disable the STOP condition generation */
    I2Cx->CR1 &= (uint16_t)~((uint16_t)I2C_CR1_STOP);
  }
}

void I2C::send7bitAddress(I2C_TypeDef* I2Cx, uint8_t Address, uint8_t I2C_Direction)
{
  /* Test on the direction to set/reset the read/write bit */
  if(I2C_Direction != I2C_Direction_Transmitter)
  {
    /* Set the address bit0 for read */
    Address |= I2C_OAR1_ADD0;
  }
  else
  {
    /* Reset the address bit0 for write */
    Address &= (uint8_t)~((uint8_t)I2C_OAR1_ADD0);
  }
  /* Send the address */
  I2Cx->DR = Address;
}

void I2C::acknowledgeConfig(I2C_TypeDef* I2Cx, FunctionalState NewState)
{
  if(NewState != DISABLE)
  {
    /* Enable the acknowledgement */
    I2Cx->CR1 |= I2C_CR1_ACK;
  }
  else
  {
    /* Disable the acknowledgement */
    I2Cx->CR1 &= (uint16_t)~((uint16_t)I2C_CR1_ACK);
  }
}

void I2C::softwareResetCmd(I2C_TypeDef* I2Cx, FunctionalState NewState)
{
  if(NewState != DISABLE)
  {
    /* Peripheral under reset */
    I2Cx->CR1 |= I2C_CR1_SWRST;
  }
  else
  {
    /* Peripheral not under reset */
    I2Cx->CR1 &= (uint16_t)~((uint16_t)I2C_CR1_SWRST);
  }
}


ErrorStatus I2C::checkEvent(I2C_TypeDef* I2Cx, uint32_t I2C_EVENT)
{
  uint32_t lastevent = 0;
  uint32_t flag1 = 0, flag2 = 0;
  ErrorStatus status = ERROR;

  /* Read the I2Cx status register */
  flag1 = I2Cx->SR1;
  flag2 = I2Cx->SR2;
  flag2 = flag2 << 16;

  /* Get the last event value from I2C status register */
  lastevent = (flag1 | flag2) & FLAG_MASK;

  /* Check whether the last event contains the I2C_EVENT */
  if((lastevent & I2C_EVENT) == I2C_EVENT)
  {
    /* SUCCESS: last event is equal to I2C_EVENT */
    status = SUCCESS;
  }
  else
  {
    /* ERROR: last event is different from I2C_EVENT */
    status = ERROR;
  }
  /* Return status */
  return status;
}

FlagStatus I2C::getFlagStatus(I2C_TypeDef* I2Cx, uint32_t I2C_FLAG)
{
  FlagStatus bitstatus = RESET;
  __IO uint32_t i2creg = 0, i2cxbase = 0;

  /* Get the I2Cx peripheral base address */
  i2cxbase = (uint32_t)I2Cx;
  
  /* Read flag register index */
  i2creg = I2C_FLAG >> 28;
  
  /* Get bit[23:0] of the flag */
  I2C_FLAG &= FLAG_MASK;
  
  if(i2creg != 0)
  {
    /* Get the I2Cx SR1 register address */
    i2cxbase += 0x14;
  }
  else
  {
    /* Flag in I2Cx SR2 Register */
    I2C_FLAG = (uint32_t)(I2C_FLAG >> 16);
    /* Get the I2Cx SR2 register address */
    i2cxbase += 0x18;
  }
  
  if(((*(__IO uint32_t *)i2cxbase) & I2C_FLAG) != (uint32_t)RESET)
  {
    /* I2C_FLAG is set */
    bitstatus = SET;
  }
  else
  {
    /* I2C_FLAG is reset */
    bitstatus = RESET;
  }
  
  /* Return the I2C_FLAG status */
  return  bitstatus;
}

void I2C::sendData(I2C_TypeDef* I2Cx, uint8_t Data)
{
  /* Write in the DR register the data to be sent */
  I2Cx->DR = Data;
}

uint8_t I2C::receiveData(I2C_TypeDef* I2Cx)
{
  /* Return the data in the DR register */
  return (uint8_t)I2Cx->DR;
}

bool I2C::start(I2C_TypeDef* I2Cx, uint8_t address, uint8_t direction)
{
	timeout = LONG_TIMEOUT;
	// wait until I2C1 is not busy any more
	while(I2C::getFlagStatus(I2Cx, I2C_FLAG_BUSY))
	{
		if(--timeout == 0) return false;
	}
 
	// Send I2C1 START condition
	generateStart(I2Cx, ENABLE);
 
	timeout = FLAG_TIMEOUT;
	// wait for I2C1 EV5 --> Slave has acknowledged start condition
  while(!I2C::checkEvent(I2Cx, I2C_EVENT_MASTER_MODE_SELECT))
	{
		if(--timeout == 0) return false;
	}
 
	// Send slave Address for write
	send7bitAddress(I2Cx, address, direction);
 
	timeout = FLAG_TIMEOUT;
	/* wait for I2Cx EV6, check if
	 * either Slave has acknowledged Master transmitter or
	 * Master receiver mode, depending on the transmission
	 * direction
	 */
	if(direction == I2C_Direction_Transmitter)
	{
		while(!I2C::checkEvent(I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
		{
			if(--timeout == 0) return false;
		}
	}
	else if(direction == I2C_Direction_Receiver)
	{
		while(!I2C::checkEvent(I2Cx, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED))
		{
			if(--timeout == 0) return false;
		}
	}
	
	return true;
}

bool I2C::restart(I2C_TypeDef * I2Cx, uint8_t address, uint8_t direction)
{
	timeout = FLAG_TIMEOUT;
	
	while(!I2C::checkEvent(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTING))
	{
		if(--timeout == 0) return false;
	}
    
	I2Cx->CR1 |= I2C_CR1_START;
 
	timeout = FLAG_TIMEOUT;
	while(!I2C::checkEvent(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
	{
		if(--timeout == 0) return false;
	}
		
	timeout = FLAG_TIMEOUT;
	
	while(!I2C::checkEvent(I2Cx, I2C_EVENT_MASTER_MODE_SELECT))
	{
		if(--timeout == 0) return false;
	}
 
	// Send slave Address for write
	I2C::send7bitAddress(I2Cx, address, direction);
 
	timeout = FLAG_TIMEOUT;
	/* wait for I2Cx EV6, check if
	 * either Slave has acknowledged Master transmitter or
	 * Master receiver mode, depending on the transmission
	 * direction
	 */
	if(direction == I2C_Direction_Transmitter)
	{
		while(!I2C::checkEvent(I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
		{
			if(--timeout == 0) return false;
		}
	}
	else if(direction == I2C_Direction_Receiver)
	{
		while(!I2C::checkEvent(I2Cx, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED))
		{
			if(--timeout == 0) return false;
		}
	}
	
	return true;
}

bool I2C::write(I2C_TypeDef* I2Cx, uint8_t data)
{
	timeout = FLAG_TIMEOUT;
	// wait for I2C1 EV8 --> last byte is still being transmitted (last byte in SR, buffer empty), next byte can already be written
	while(!I2C::checkEvent(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTING))
	{
		if(--timeout == 0) return false;
	}
	
	I2C::sendData(I2Cx, data);
	
	return true;
}

bool I2C::readAck(I2C_TypeDef* I2Cx, uint8_t* data)
{
	// enable acknowledge of received data
	I2C::acknowledgeConfig(I2Cx, ENABLE);
	
	timeout = FLAG_TIMEOUT;
	// wait until one byte has been received
	while(!I2C::checkEvent(I2Cx, I2C_EVENT_MASTER_BYTE_RECEIVED))
	{
		if(--timeout == 0) return false;
	}
	
	// read data from I2C data register and return data byte
	*data = I2C::receiveData(I2Cx);
	
	return true;
}

bool I2C::readNack(I2C_TypeDef* I2Cx, uint8_t* data)
{
	// disable acknowledge of received data
	// nack also generates stop condition after last byte received
	// see reference manual for more info
	I2C::acknowledgeConfig(I2Cx, DISABLE);
	I2C::generateStop(I2Cx, ENABLE);
	
	timeout = FLAG_TIMEOUT;
	// wait until one byte has been received
	while(!I2C::checkEvent(I2Cx, I2C_EVENT_MASTER_BYTE_RECEIVED))
	{
		if(--timeout == 0) return false;
	}
	
	// read data from I2C data register and return data byte
	*data = I2C::receiveData(I2Cx);
	
	return true;
}

bool I2C::stop(I2C_TypeDef* I2Cx)
{
	timeout = FLAG_TIMEOUT;
	
  while(!I2C::checkEvent(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
	{
		if(--timeout == 0) return false;
	}
	
	// Send I2C1 STOP Condition after last byte has been transmitted
	I2C::generateStop(I2Cx, ENABLE);
	
	timeout = FLAG_TIMEOUT;
	// wait for I2C1 EV8_2 --> byte has been transmitted
	while(!I2C::checkEvent(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
	{
		if(--timeout == 0) return false;
	}
	
	return true;
}

void I2C::scan(I2C_TypeDef* I2Cx)
{
	uint8_t address;
	uint32_t nDevices = 0, result;
	
	for(address = 1; address < 127; address++)
	{
		result = 1;
		
		timeout = LONG_TIMEOUT;
		// wait until I2C1 is not busy any more
		while(I2C::getFlagStatus(I2Cx, I2C_FLAG_BUSY))
		{
			timeout--;
			if(timeout <= 0)
			{
				result = 1;
				break;
			}
		}
 
		// Send I2C1 START condition
		generateStart(I2Cx, ENABLE);
		
		timeout = FLAG_TIMEOUT;
		// wait for I2C1 EV5
		while(!I2C::checkEvent(I2Cx, I2C_EVENT_MASTER_MODE_SELECT))
		{
			timeout--;
			if(timeout <= 0)
			{
				result = 3;
				break;
			}
		}
		
		// Send slave Address for write
		send7bitAddress(I2Cx, address, I2C_Direction_Transmitter);
 
		Debug::print("huhuhu\n");
		
		timeout = FLAG_TIMEOUT;
		/* 
		 * wait for I2Cx EV6, check if Slave has acknowledged Master transmitter
		 */
		while(true)
		{
			Debug::print("%i\n", timeout);
			
			timeout--;
			
			if(I2C::checkEvent(I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
			{
				result = 0;
				break;
			}
			else if(timeout <= 0)
			{
				result = 2;
				break;
			}
		}
		
		Debug::print("%i\n", result);
		
		// Send I2C1 STOP Condition after last byte has been transmitted
		I2C::generateStop(I2Cx, ENABLE);

		if(result == 0)
		{
			Debug::print("I2C device found at address 0x");
			if(address < 16) Debug::print("0");
			Debug::print("%x !\n", address);
			
			nDevices++;
		}
		else if(result == 4)
		{
			Debug::print("Unknown error at address 0x");
			if(address < 16) Debug::print("0");
			Debug::print("%x\n", address);
		}
	}
	
	if(nDevices == 0)
		Debug::print("No I2C devices found\n");
	else
		Debug::print("done\n");
}

bool I2C::writeBytes(I2C_TypeDef* I2Cx, uint8_t deviceAddr, uint8_t regAddr, uint8_t* bytes, uint32_t numBytes)
{
	uint32_t index;
	
	if(!I2C::start(I2Cx, deviceAddr, I2C_Direction_Transmitter)) return false;
	if(!I2C::write(I2Cx, regAddr)) return false;
	
	for(index = 0; index < numBytes; index++)
	{
		if(!I2C::write(I2Cx, bytes[index])) return false;
	}
	
	if(!I2C::stop(I2Cx)) return false;
	
	return true;
}

bool I2C::writeByte(I2C_TypeDef* I2Cx, uint8_t deviceAddr, uint8_t regAddr, uint8_t byte)
{
	return I2C::writeBytes(I2Cx, deviceAddr, regAddr, &byte, 1);
}

bool I2C::readBytes(I2C_TypeDef* I2Cx, uint8_t deviceAddr, uint8_t regAddr, uint8_t* bytes, uint32_t numBytes)
{
	uint32_t index;
	
	if(!I2C::start(I2Cx, deviceAddr, I2C_Direction_Transmitter)) return false;
	if(!I2C::write(I2Cx, regAddr)) return false;
	if(!I2C::restart(I2Cx, deviceAddr, I2C_Direction_Receiver)) return false;
	
	for(index = 0; index < numBytes; index++)
	{
		if(index == (numBytes - 1))
		{
			if(!I2C::readNack(I2Cx, &bytes[index])) return false;
		}
		else
		{
			if(!I2C::readAck(I2Cx, &bytes[index])) return false;
		}
	}
	
	return true;
}

bool I2C::readByte(I2C_TypeDef* I2Cx, uint8_t deviceAddr, uint8_t regAddr, uint8_t* byte)
{
	return I2C::readBytes(I2Cx, deviceAddr, regAddr, byte, 1);
}



