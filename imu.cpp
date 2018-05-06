#include "main.h"

#if defined(MPU9250_68) || defined(MPU9250_69)

Imu* Imu::createImu(ImuSettings* settings)
{
#if defined(MPU9250_68) || defined(MPU9250_69)
	return new ImuMpu9250(settings);
#endif
}

Imu::Imu(ImuSettings* settings)
{
	settings = settings;
	
	m_calibrationMode = false;
  m_calibrationValid = false;
	m_gyroBiasValid = false;
}

/*
Imu::~Imu()
{}
*/
#endif
