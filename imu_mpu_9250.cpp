#include "main.h"

ImuMpu9250::ImuMpu9250(ImuSettings* settings) : Imu(settings)
{}

ImuMpu9250::~ImuMpu9250()
{}

bool ImuMpu9250::lowLevelInit()
{
	I2C_InitTypeDef I2C_InitStructure;
	uint8_t gpioAltFunctionSel;
	
	if(IMU_I2C == I2C1)
	{
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);
		gpioAltFunctionSel = GPIO_AF_I2C1;
	}
	else if(IMU_I2C == I2C2)
	{
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C2, ENABLE);
		gpioAltFunctionSel = GPIO_AF_I2C2;
	}
	else if(IMU_I2C == I2C3)
	{
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C3, ENABLE);
		gpioAltFunctionSel = GPIO_AF_I2C3;
	}
	else
	{
		return false;
	}
	
	// configurates GPIO ports for I2C SCL and SDA
	TM_GPIO_InitAlternate(
		IMU_I2C_GPIO_PORT, 
		IMU_I2C_SCL_GPIO_PIN | IMU_I2C_SDA_GPIO_PIN,
		TM_GPIO_OType_OD,
		TM_GPIO_PuPd_UP,
		TM_GPIO_Speed_Fast,
		gpioAltFunctionSel
	);
	
	I2C::structInit(&I2C_InitStructure);
	I2C_InitStructure.I2C_Ack = I2C_Ack_Disable;
	I2C_InitStructure.I2C_ClockSpeed = I2C_SPEED;
	
	I2C::init(IMU_I2C, &I2C_InitStructure);
	I2C::intConfig(IMU_I2C, I2C_IT_EVT | I2C_IT_BUF, DISABLE);
  I2C::cmd(IMU_I2C, ENABLE);
	
	return true;
}

int ImuMpu9250::init()
{
	uint8_t result;
	uint8_t asa[3];
	
	m_firstTime = true;
	
	//  configure IMU
	
	m_slaveAddr = m_settings->m_i2cSlaveAddress;
	
	setSampleRate(m_settings->m_MPU9250GyroAccelSampleRate);
	setCompassRate(m_settings->m_MPU9250CompassSampleRate);
	setGyroLpf(m_settings->m_MPU9250GyroLpf);
	setAccelLpf(m_settings->m_MPU9250AccelLpf);
	setGyroFsr(m_settings->m_MPU9250GyroFsr);
	setAccelFsr(m_settings->m_MPU9250AccelFsr);
	
//	setCalibrationData();
	
//	I2C::writeByte(IMU_I2C, m_slaveAddr, MPU9250_SMPRT_DIV, &tmpSampleRate);
	
	return 0;
}

bool ImuMpu9250::setSampleRate()
{
	uint8_t tmpSampleRate;
	
	if(m_sampleRate > 1000) return true;                                        // SMPRT not used above 1000Hz
	
	tmpSampleRate = (uint8_t)(1000 / m_sampleRate - 1);
	
  I2C::writeByte(IMU_I2C, m_slaveAddr, MPU9250_SMPRT_DIV, &tmpSampleRate);
	
	return true;
}

bool ImuMpu9250::setSampleRate(uint32_t rate)
{
	if((rate < MPU9250_SAMPLERATE_MIN) || (rate > MPU9250_SAMPLERATE_MAX)) return false;
		
	m_sampleRate = rate;
	m_sampleInterval = (unsigned long)1000 / m_sampleRate;
    
	if(m_sampleInterval == 0) m_sampleInterval = 1;
    
	return true;
}

bool ImuMpu9250::setCompassRate(int32_t rate)
{
	if((rate < MPU9250_COMPASSRATE_MIN) || (rate > MPU9250_COMPASSRATE_MAX)) return false;
  
	m_compassRate = rate;
  return true;
}

bool ImuMpu9250::setGyroLpf(uint8_t lpf)
{
	switch(lpf)
	{
    case MPU9250_GYRO_LPF_8800:
    case MPU9250_GYRO_LPF_3600:
    case MPU9250_GYRO_LPF_250:
    case MPU9250_GYRO_LPF_184:
    case MPU9250_GYRO_LPF_92:
    case MPU9250_GYRO_LPF_41:
    case MPU9250_GYRO_LPF_20:
    case MPU9250_GYRO_LPF_10:
    case MPU9250_GYRO_LPF_5:
			m_gyroLpf = lpf;
			return true;

    default: return false;
	}
}

bool ImuMpu9250::setAccelLpf(uint8_t lpf)
{
	switch(lpf)
	{
		case MPU9250_ACCEL_LPF_1130:
		case MPU9250_ACCEL_LPF_460:
		case MPU9250_ACCEL_LPF_184:
		case MPU9250_ACCEL_LPF_92:
		case MPU9250_ACCEL_LPF_41:
    case MPU9250_ACCEL_LPF_20:
    case MPU9250_ACCEL_LPF_10:
    case MPU9250_ACCEL_LPF_5:
			m_accelLpf = lpf;
			return true;

    default: return false;
	}
}

bool ImuMpu9250::setGyroFsr(uint8_t fsr)
{
	switch(fsr)
	{
		case MPU9250_GYROFSR_250:
			m_gyroFsr = fsr;
			m_gyroScale = PI_f / (131.0f * 180.0f);
			return true;
		
		case MPU9250_GYROFSR_500:
			m_gyroFsr = fsr;
			m_gyroScale = PI_f / (62.5f * 180.0f);
			return true;
		
		case MPU9250_GYROFSR_1000:
			m_gyroFsr = fsr;
			m_gyroScale = PI_f / (32.8f * 180.0f);
			return true;
		
		case MPU9250_GYROFSR_2000:
			m_gyroFsr = fsr;
			m_gyroScale = PI_f / (16.4f * 180.0f);
			return true;
		
		default: return false;
    }
}

bool ImuMpu9250::setAccelFsr(uint8_t fsr)
{
	switch(fsr)
	{
		case MPU9250_ACCELFSR_2:
			m_accelFsr = fsr;
			m_accelScale = 1.0f / 16384.0f;
			return true;
		
		case MPU9250_ACCELFSR_4:
			m_accelFsr = fsr;
			m_accelScale = 1.0f / 8192.0f;
			return true;
		
		case MPU9250_ACCELFSR_8:
			m_accelFsr = fsr;
			m_accelScale = 1.0f / 4096.0f;
			return true;
		
		case MPU9250_ACCELFSR_16:
			m_accelFsr = fsr;
			m_accelScale = 1.0f / 2048.0f;
			return true;
		
		default: return false;
	}
}

