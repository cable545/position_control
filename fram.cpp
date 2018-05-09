#include "main.h"
		
void FRAM::gpioInit()
{
	// enables the I2C1 clock
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);
	
	// configurates GPIO ports for I2C SCL and SDA
	TM_GPIO_InitAlternate(
		FRAM_I2C_GPIO_PORT, 
		FRAM_I2C_SCL_GPIO_PIN | FRAM_I2C_SDA_GPIO_PIN,
		TM_GPIO_OType_OD,
		TM_GPIO_PuPd_NOPULL,
		TM_GPIO_Speed_Fast,
		GPIO_AF_I2C1
	);
}

void FRAM::init()
{
	I2C_InitTypeDef I2C_InitStructure;
	
	gpioInit();
	
	I2C::structInit(&I2C_InitStructure);
	I2C_InitStructure.I2C_Ack = I2C_Ack_Disable;
	I2C_InitStructure.I2C_ClockSpeed = I2C_SPEED;
	
  I2C::cmd(FRAM_I2C, ENABLE);
//	I2C::intConfig(FRAM_I2C, I2C_IT_EVT | I2C_IT_BUF, DISABLE);
	I2C::init(FRAM_I2C, &I2C_InitStructure);
}

uint8_t FRAM::readByte(uint16_t framAddr)
{
	uint8_t value;
	
	readBlock(framAddr, &value, 1);
	
	return value;
}

void FRAM::writeByte(uint16_t framAddr, uint8_t value)
{
	writeBlock(framAddr, &value, 1);
}

void FRAM::readBlock(uint16_t framAddr, uint8_t* value, uint32_t numBytes)
{
	I2C::start(FRAM_I2C, i2cAddr, I2C_Direction_Transmitter);
	I2C::write(FRAM_I2C, (uint8_t)(framAddr >> 8));
	I2C::write(FRAM_I2C, (uint8_t)(framAddr & 0xFF));
	I2C::restart(FRAM_I2C, i2cAddr, I2C_Direction_Receiver);
	
	for(uint32_t index = 0; index < numBytes; index++)
	{
		index == (numBytes - 1) ? I2C::readNack(FRAM_I2C, &value[index]) : I2C::readAck(FRAM_I2C, &value[index]);
	}
}

void FRAM::writeBlock(uint16_t framAddr, uint8_t* value, uint32_t numBytes)
{
	I2C::start(FRAM_I2C, i2cAddr, I2C_Direction_Transmitter);
	I2C::write(FRAM_I2C, (uint8_t)(framAddr >> 8));
	I2C::write(FRAM_I2C, (uint8_t)(framAddr & 0xFF));
	
	for(uint32_t index = 0; index < numBytes; index++) I2C::write(FRAM_I2C, value[index]);
	
	I2C::stop(FRAM_I2C);
}
