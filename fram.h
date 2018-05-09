#ifndef _FRAM_H
#define _FRAM_H

#define MB85RC_DEFAULT_ADDRESS      0x50 /* 1010 + A2 + A1 + A0 = 0x50 default */

#define FRAM_I2C 								I2C1
#define FRAM_I2C_GPIO_PORT			GPIOB

#define FRAM_I2C_SCL_GPIO_PIN		GPIO_Pin_8
#define FRAM_I2C_SDA_GPIO_PIN		GPIO_Pin_9

class FRAM
{
	private:
		static void gpioInit();
	
		static const uint8_t i2cAddr = MB85RC_DEFAULT_ADDRESS << 1;
	
	public:
		static void init();
		static uint8_t readByte(uint16_t framAddr);
		static void writeByte(uint16_t framAddr, uint8_t value);
		static void readBlock(uint16_t framAddr, uint8_t* value, uint32_t numBytes);
		static void writeBlock(uint16_t framAddr, uint8_t* value, uint32_t numBytes);
};

#endif
