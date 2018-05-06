#include "main.h"

ImuSettings::ImuSettings()
{
	m_imuType = -1;
	m_i2cSlaveAddress = 0;
	
#ifdef MPU9250_68
	//  MPU9250 defaults
	
	m_MPU9250GyroAccelSampleRate = 80;
  m_MPU9250CompassSampleRate = 40;
	m_MPU9250GyroLpf = MPU9250_GYRO_LPF_41;
	m_MPU9250AccelLpf = MPU9250_ACCEL_LPF_41;
	m_MPU9250GyroFsr = MPU9250_GYROFSR_1000;
	m_MPU9250AccelFsr = MPU9250_ACCELFSR_8;
	m_i2cSlaveAddress = MPU9250_ADDRESS0;
#endif
}
