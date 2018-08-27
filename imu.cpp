#include "main.h"

//  this sets the learning rate for compass running average calculation

#define COMPASS_ALPHA                   0.2f

//  this defines the gyro noise level

#define IMU_FUZZY_GYRO_ZERO           0.20f

#define IMU_FUZZY_GYRO_ZERO_SQUARED   (IMU_FUZZY_GYRO_ZERO * IMU_FUZZY_GYRO_ZERO)

//  this defines the accelerometer noise level

#define IMU_FUZZY_ACCEL_ZERO          0.05f

#define IMU_FUZZY_ACCEL_ZERO_SQUARED   (IMU_FUZZY_ACCEL_ZERO * IMU_FUZZY_ACCEL_ZERO)

Imu* Imu::createImu(ImuSettings* settings)
{
#if defined(MPU9250_68) || defined(MPU9250_69)
	return new ImuMpu9250(settings);
#endif
}

Imu::Imu(ImuSettings* settings)
{
	m_settings = settings;
	
	m_calibrationMode = false;
  m_calibrationValid = false;
	m_gyroBiasValid = false;
}

/*
Imu::~Imu()
{}
*/

void Imu::gyroBiasInit()
{
	m_gyroAlpha = 2.0f / m_sampleRate;
	m_gyroSampleCount = 0;
}

//  Note - code assumes that this is the first thing called after axis swapping
//  for each specific IMU chip has occurred.

void Imu::handleGyroBias()
{
	if(!m_gyroBiasValid)
	{
		Vector deltaAccel = m_previousAccel;
		deltaAccel -= m_accel;   // compute difference
		m_previousAccel = m_accel;

    if((deltaAccel.squareLength() < IMU_FUZZY_ACCEL_ZERO_SQUARED) && (m_gyro.squareLength() < IMU_FUZZY_GYRO_ZERO_SQUARED))
		{
			// what we are seeing on the gyros should be bias only so learn from this
			m_gyroBias.setX((1.0f - m_gyroAlpha) * m_gyroBias.x() + m_gyroAlpha * m_gyro.x());
			m_gyroBias.setY((1.0f - m_gyroAlpha) * m_gyroBias.y() + m_gyroAlpha * m_gyro.y());
			m_gyroBias.setZ((1.0f - m_gyroAlpha) * m_gyroBias.z() + m_gyroAlpha * m_gyro.z());

			if(m_gyroSampleCount < (5 * m_sampleRate))
			{
				m_gyroSampleCount++;
				
				if(m_gyroSampleCount == (5 * m_sampleRate)) m_gyroBiasValid = true;
			}
		}
	}
	
	m_gyro -= m_gyroBias;
}

void Imu::calibrateAverageCompass()
{
	//  calibrate if required
	if(!m_calibrationMode && m_calibrationValid)
	{
		m_compass.setX((m_compass.x() - m_compassCalOffset[0]) * m_compassCalScale[0]);
		m_compass.setY((m_compass.y() - m_compassCalOffset[1]) * m_compassCalScale[1]);
		m_compass.setZ((m_compass.z() - m_compassCalOffset[2]) * m_compassCalScale[2]);
	}
	
	//  update running average
  m_compassAverage.setX(m_compass.x() * COMPASS_ALPHA + m_compassAverage.x() * (1.0f - COMPASS_ALPHA));
	m_compassAverage.setY(m_compass.y() * COMPASS_ALPHA + m_compassAverage.y() * (1.0f - COMPASS_ALPHA));
	m_compassAverage.setZ(m_compass.z() * COMPASS_ALPHA + m_compassAverage.z() * (1.0f - COMPASS_ALPHA));

	m_compass = m_compassAverage;
}

