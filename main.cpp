#include "main.h"

const char* RETURN_VALUES[] = {
	"EXIT_SUCCESS",
	"I2C_START_ERROR",
	"I2C_READ_ACK_ERROR",
	"I2C_READ_NACK_ERROR",
	"I2C_WRITE_ERROR",
	"I2C_STOP_ERROR"
};

static void initImu(void);
static void readImuData(ImuData* actualImuData);

//#define TEST_RUN

#ifdef TEST_RUN

int main(void)
{
	return run();
}

#else

#ifdef BNO080_IMU
IMU_BNO080 imu;
#endif

#if defined(MPU9250_68) || defined(MPU9250_69)
Imu* imu;
#endif


TargetValues_t* targetValues;

int main(void)
{
	ImuData actualImuData;
	
	USART1_Init();
	USART2_Init();
	
	Debug::println("Initialized USART");
	
	startSysTickTimer();
//	Buzzer::init(false);
	
//	FRAM::init();
	
	initImu();
//	Receiver& receiver = Receiver::instance();

//	if(receiver.init(false, true) != EXIT_SUCCESS)
//		Debug::println("failed to initialize receiver");
//	
//	MotorDriver& driver = MotorDriver::instance();
//	driver.init(MOT_DRIV_TIM);
	
	Debug::println("Start while loop");
	
	while(true)
	{
//		if(receiver.isBufferFull())
//		{
//			receiver.processCapturedValues();
//			targetValues = receiver.getTargetValues();
//		}
		readImuData(&actualImuData);
		
		
		
//		driver.setPulse(1750, MOTOR_A | MOTOR_D | MOTOR_C | MOTOR_B);
//		driver.setPulse(1250, MOTOR_A | MOTOR_D);

		
//			Debug::print("Orientation: ");
//			Debug::print("%i ", (int)(euler.x() * RAD_DEG));
//			Debug::print("%i ", (int)(euler.y() * RAD_DEG));
//			Debug::print("%i\r", (int)(euler.z() * RAD_DEG));
		
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

static void initImu(void)
{
#ifdef BNO080_IMU
	if(imu.begin(true))
		imu.enableRotationVector(3);
	else
		Debug::println("BNO080_IMU init not successfull");
#endif

#if defined(MPU9250_68) || defined(MPU9250_69)
	ImuSettings settings;
	Fusion fusion;
	
	int32_t errcode;
	
	imu = Imu::createImu(&settings);                        // create the imu object
	if((errcode = imu->init()) < 0)
		Debug::println("Failed to init IMU: %i", errcode);
	
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
}

static void readImuData(ImuData* actualImuData)
{
#ifdef BNO080_IMU
	Vector eulerVector;
	
	if(imu.dataAvailable())
	{
		Quaternion q = Quaternion(
			imu.getQuatReal(),
			imu.getQuatI(),
			imu.getQuatJ(),
			imu.getQuatK()
		);
		
		q.toEuler(eulerVector);
		
		actualImuData->rollAngle = eulerVector.x() * RAD_DEG;
		actualImuData->pitchAngle = eulerVector.y() * RAD_DEG;
		actualImuData->yawAngle = eulerVector.z() * RAD_DEG;
		actualImuData->gyroX = imu.getGyroX();
		actualImuData->gyroY = imu.getGyroY();
		actualImuData->gyroZ = imu.getGyroZ();
		actualImuData->rollRaw = Maths::map(actualImuData->rollAngle, ROLL_ANGLE_MIN, ROLL_ANGLE_MAX, CHANNEL_VALUE_COUNT_MIN, CHANNEL_VALUE_COUNT_MAX);
		actualImuData->pitchRaw = Maths::map(actualImuData->pitchAngle, PITCH_ANGLE_MIN, PITCH_ANGLE_MAX, CHANNEL_VALUE_COUNT_MIN, CHANNEL_VALUE_COUNT_MAX);
		
		Debug::print("%i ", actualImuData->rollAngle);
		Debug::print("%i ", actualImuData->pitchAngle);
		Debug::println("%i ", actualImuData->yawAngle);
	}
#endif
		
#if defined(MPU9250_68) || defined(MPU9250_69)
	imu->read();
	fusion.newImuData(imu->getGyro(), imu->getAccel(), imu->getCompass(), imu->getTimestamp());
	
	actualImuData->rollAngle = fusion.getFusionPose().x();
	actualImuData->pitchAngle = fusion.getFusionPose().y();
	actualImuData->yawAngle = fusion.getFusionPose().z();
	actualImuData->gyroX = imu->getGyro().x();
	actualImuData->gyroY = imu->getGyro().y();
	actualImuData->gyroZ = imu->getGyro().z();
	actualImuData->rollRaw = Maths::map(actualImuData->rollAngle, ROLL_ANGLE_MIN, ROLL_ANGLE_MAX, CHANNEL_VALUE_COUNT_MIN, CHANNEL_VALUE_COUNT_MAX);
	actualImuData->pitchRaw = Maths::map(actualImuData->pitchAngle, PITCH_ANGLE_MIN, PITCH_ANGLE_MAX, CHANNEL_VALUE_COUNT_MIN, CHANNEL_VALUE_COUNT_MAX);
#endif
}

#endif
