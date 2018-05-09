#include "main.h"

int main(void)
{
	/* USART 2 init */
	USART_InitTypeDef usart2InitStruct;
	usart2InitStruct.USART_BaudRate = 115200;
  usart2InitStruct.USART_WordLength = USART_WordLength_8b;
  usart2InitStruct.USART_StopBits = USART_StopBits_1;
  usart2InitStruct.USART_Parity = USART_Parity_No ;
  usart2InitStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  usart2InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
	TM_GPIO_InitAlternate(GPIOA, GPIO_Pin_2 | GPIO_Pin_3, TM_GPIO_OType_PP, TM_GPIO_PuPd_NOPULL, TM_GPIO_Speed_High, GPIO_AF_USART2);
	USART_Init(USART2, &usart2InitStruct);
	USART_Cmd(USART2, ENABLE);
	/* USART 2 init end */
	
//	FRAM::init();
	
	if(ImuMpu9250::lowLevelInit())
		Debug::print("Init ist duch\n");
	else
		Debug::print("Init ist nicht durch\n");
	
	I2C::writeByte(IMU_I2C, MPU9250_ADDRESS0, MPU9250_PWR_MGMT_1, 0x00); // Clear sleep mode bit (6), enable all sensors
	
//	for(int i = 5000000; i > 0; i--);
//	
//	uint8_t value = 'A', value2;
	
	while(true)
	{
//		FRAM::writeByte(0x00, &value);
//		
//		value2 = FRAM::readByte(0x0000);
//		
//		Debug::print("%c", value2);
		
//		if(I2C::readByte(IMU_I2C, AK8963_ADDRESS, AK8963_WHO_AM_I) == 0x48)
//			Debug::print("AK8963 ist da\n");
//		else
//			Debug::print("AK8963 ist nicht da\n");
//		
//		if(I2C::readByte(IMU_I2C, MPU9250_ADDRESS0, MPU9250_WHO_AM_I) == 0x71)
//			Debug::print("MPU9250 ist da\n");
//		else
//			Debug::print("MPU9250 ist nicht da\n");
	
//		Debug::print("hossa\n");
//		I2C::scan(IMU_I2C);
	}
}
