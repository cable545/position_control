#ifndef _IMU_SETTINGS_H
#define _IMU_SETTINGS_H

class ImuSettings
{
public:
	ImuSettings();

	int32_t m_imuType;                                        // type code of imu in use
	uint8_t m_i2cSlaveAddress;                        // I2C slave address of the imu
	int m_pressureType;                                     // type code of pressure sensor in use
	unsigned char m_i2cPressureAddress;

#if defined(MPU9250_68) || defined(MPU9250_69)
	//  MPU9250

	uint32_t m_MPU9250GyroAccelSampleRate;                       // the sample rate (samples per second) for gyro and accel
	uint32_t m_MPU9250CompassSampleRate;                         // same for the compass
	int32_t m_MPU9250GyroLpf;                                   // low pass filter code for the gyro
	int32_t m_MPU9250AccelLpf;                                  // low pass filter code for the accel
	int32_t m_MPU9250GyroFsr;                                   // FSR code for the gyro
	int32_t m_MPU9250AccelFsr;                                  // FSR code for the accel
#endif
};

#endif
