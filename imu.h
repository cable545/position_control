#ifndef _IMU_H
#define _IMU_H

#define IMU_I2C 									I2C1
#define IMU_I2C_GPIO_PORT					GPIOB
#define IMU_I2C_SCL_GPIO_PIN			GPIO_Pin_8
#define IMU_I2C_SDA_GPIO_PIN			GPIO_Pin_9

class ImuSettings;

class Imu
{
public:
		
	static Imu* createImu(ImuSettings* settings);
		
	Imu(ImuSettings* settings);

	virtual const char* name() = 0;
	virtual int init() = 0;                              // set up the IMU

protected:
	
	bool m_calibrationMode;	// true if cal mode so don't use cal data!
	bool m_calibrationValid; 
	bool m_gyroBiasValid;  
	ImuSettings* m_settings;

	uint32_t m_sampleRate;											// samples per second
	uint64_t m_sampleInterval;									// interval between samples in microseonds
};

#endif
