#include "main.h"

const char* RETURN_VALUES[] = {
	"EXIT_SUCCESS",
	"I2C_START_ERROR",
	"I2C_READ_ACK_ERROR",
	"I2C_READ_NACK_ERROR",
	"I2C_WRITE_ERROR",
	"I2C_STOP_ERROR"
};

//#define TEST_RUN

#ifdef TEST_RUN

int main(void)
{
	return run();
}

#else




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
	
	if(!startSysTickTimer()) Debug::print("Systick timer init failed\n");
	
	delay(100);
	
//	Buzzer::initBuzzer(true);
//	FRAM::init();
	
#if defined(MPU9250_68) || defined(MPU9250_69)
	Imu* imu;
	ImuSettings settings;
	Fusion fusion;   
	
	int32_t errcode;
#endif

#ifdef BNO080_IMU
	IMU_BNO080 imu;
	
	if(imu.begin())
	{
		Debug::print("BNO080_IMU init successfull\n");
		
		imu.enableRotationVector(3);
	}
	else
	{
		Debug::print("BNO080_IMU init not successfull\n");
	}
#endif

#if defined(MPU9250_68) || defined(MPU9250_69)	
	imu = Imu::createImu(&settings);                        // create the imu object
	if((errcode = imu->init()) < 0) Debug::print("Failed to init IMU: %i\n", errcode);
	
	// Slerp power controls the fusion and can be between 0 and 1
	// 0 means that only gyros are used, 1 means that only accels/compass are used
	// In-between gives the fusion mix.

	fusion.setSlerpPower(0.07);

	// use of sensors in the fusion algorithm can be controlled here
	// change any of these to false to disable that sensor

	fusion.setGyroEnable(true);
	fusion.setAccelEnable(true);
	fusion.setCompassEnable(true);
#endif

//	Buzzer::buzzerBeepXTimes(3);

	Debug::print("Start\n");

	Receiver::init();
	
	Debug::print("receiver initialized\n");

	while(true)
	{
#ifdef BNO080_IMU
//		imu.start
		
//		if(imu.dataAvailable())
//		{
//			float x = imu.getQuatI();
//			float y = imu.getQuatJ();
//			float z = imu.getQuatK();
//			float real = imu.getQuatReal();
//			Vector euler;
//			
//			Quaternion q = Quaternion(real, x, y, z);
//			q.toEuler(euler);
//			Debug::print("Orientation: ");
//			Debug::print("%i ", (int)(euler.x() * RAD_DEG));
//			Debug::print("%i ", (int)(euler.y() * RAD_DEG));
//			Debug::print("%i\r", (int)(euler.z() * RAD_DEG));
//		}
//		
//		delay(3);
#endif

		
#if defined(MPU9250_68) || defined(MPU9250_69)

		while(imu->read())
		{
			fusion.newImuData(imu->getGyro(), imu->getAccel(), imu->getCompass(), imu->getTimestamp());
			Debug::print("Orientation: ");
			Debug::print("%i ", (int)(fusion.getFusionPose().x() * RAD_DEG));
			Debug::print("%i ", (int)(fusion.getFusionPose().y() * RAD_DEG));
			Debug::print("%i\r", (int)(fusion.getFusionPose().z() * RAD_DEG));
			Maths::display("", imu->getAccel());
			Maths::display("Gyro", imu->getGyro());
			Maths::display("Mag", imu->getCompass());
		}
#endif
		
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

#endif
