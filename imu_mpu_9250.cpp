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
	
	// reset th mpu9250
	
	if(!I2C::writeByte(IMU_I2C, m_slaveAddr, MPU9250_PWR_MGMT_1, 0x80)) return -1;
	
	// delay(100);
	
	// is it necessary
	// if(!I2C::writeByte(IMU_I2C, m_slaveAddr, MPU9250_PWR_MGMT_1, 0x00)) return -4;
	if(!I2C::readByte(IMU_I2C, m_slaveAddr, MPU9250_WHO_AM_I, &result)) return -5;
	if(result != MPU9250_ID) return -6;
	
	// now configure the various components
	
	if(!setGyroConfig()) return -7;
  if(!setAccelConfig()) return -8;
	if(!setSampleRate()) return -9;
	
	//  now configure compass
	
	if(!bypassOn()) return -11;
	
	// get fuse ROM data
	
	if(!I2C::writeByte(IMU_I2C, AK8963_ADDRESS, AK8963_CNTL, 0))
	{
		bypassOff();
		return -12;
	}
	
	if(!I2C::writeByte(IMU_I2C, AK8963_ADDRESS, AK8963_CNTL, 0x0f))
	{
		bypassOff();
		return -13;
	}

  if(!I2C::readBytes(IMU_I2C, AK8963_ADDRESS, AK8963_ASAX, asa, 3))
	{
		bypassOff();
		return -14;
	}
	
	//  convert asa to usable scale factor
	
	m_compassAdjust[0] = ((float)asa[0] - 128.0f) / 256.0f + 1.0f;
	m_compassAdjust[1] = ((float)asa[1] - 128.0f) / 256.0f + 1.0f;
  m_compassAdjust[2] = ((float)asa[2] - 128.0f) / 256.0f + 1.0f;
	
	if(!I2C::writeByte(IMU_I2C, AK8963_ADDRESS, AK8963_CNTL, 0))
	{
		bypassOff();
		return -15;
	}
	
	if(!bypassOff()) return -16;
	
	//  now set up MPU9250 to talk to the compass chip
	
	if(!I2C::writeByte(IMU_I2C, m_slaveAddr, MPU9250_I2C_MST_CTRL, 0x40)) return -17;
	if(!I2C::writeByte(IMU_I2C, m_slaveAddr, MPU9250_I2C_SLV0_ADDR, 0x80 | AK8963_ADDRESS)) return -18;
	if(!I2C::writeByte(IMU_I2C, m_slaveAddr, MPU9250_I2C_SLV0_REG, AK8963_ST1)) return -19;
	if(!I2C::writeByte(IMU_I2C, m_slaveAddr, MPU9250_I2C_SLV0_CTRL, 0x88)) return -20;
	if(!I2C::writeByte(IMU_I2C, m_slaveAddr, MPU9250_I2C_SLV1_ADDR, AK8963_ADDRESS)) return -21;
	if(!I2C::writeByte(IMU_I2C, m_slaveAddr, MPU9250_I2C_SLV1_REG, AK8963_CNTL)) return -22;
	if(!I2C::writeByte(IMU_I2C, m_slaveAddr, MPU9250_I2C_SLV1_CTRL, 0x81)) return -23;
	if(!I2C::writeByte(IMU_I2C, m_slaveAddr, MPU9250_I2C_SLV1_DO, 0x1)) return -24;
	if(!I2C::writeByte(IMU_I2C, m_slaveAddr, MPU9250_I2C_MST_DELAY_CTRL, 0x3)) return -25;
	
	if(!setCompassRate()) return -27;
	
	//  enable the sensors

  if(!I2C::writeByte(IMU_I2C, m_slaveAddr, MPU9250_PWR_MGMT_1, 1)) return -28;
	if(!I2C::writeByte(IMU_I2C, m_slaveAddr, MPU9250_PWR_MGMT_2, 0)) return -29;
	
	//  select the data to go into the FIFO and enable
	
	if(!resetFifo()) return -30;
	
	gyroBiasInit();
	
	return 1;
}

bool ImuMpu9250::setSampleRate()
{
	if(m_sampleRate > 1000) return true;                                        // SMPRT not used above 1000Hz
	
  I2C::writeByte(IMU_I2C, m_slaveAddr, MPU9250_SMPLRT_DIV, (uint8_t)(1000 / m_sampleRate - 1));
	
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
		case MPU9250_ACCEL_LPF_1046:
		case MPU9250_ACCEL_LPF_420:
		case MPU9250_ACCEL_LPF_218:
		case MPU9250_ACCEL_LPF_99:
		case MPU9250_ACCEL_LPF_45:
    case MPU9250_ACCEL_LPF_21:
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

bool ImuMpu9250::setGyroConfig()
{
	uint8_t gyroConfig = m_gyroFsr + ((m_gyroLpf >> 3) & 3); // check output, not sure if the shift is correct
  uint8_t gyroLpf = m_gyroLpf & 7;
	
	if(!I2C::writeByte(IMU_I2C, m_slaveAddr, MPU9250_GYRO_CONFIG, gyroConfig)) return false;
	if(!I2C::writeByte(IMU_I2C, m_slaveAddr, MPU9250_GYRO_LPF, gyroLpf)) return false;
  
	return true;
}

bool ImuMpu9250::setAccelConfig()
{
	if(!I2C::writeByte(IMU_I2C, m_slaveAddr, MPU9250_ACCEL_CONFIG, m_accelFsr)) return false;
	if(!I2C::writeByte(IMU_I2C, m_slaveAddr, MPU9250_ACCEL_LPF, m_accelLpf)) return false;
	
	return true;
}

bool ImuMpu9250::bypassOn()
{
	uint8_t userControl;
	
	if(!I2C::readByte(IMU_I2C, m_slaveAddr, MPU9250_USER_CTRL, &userControl)) return false;
	
	userControl &= ~0x20;
	userControl |= 2;
	
	if(!I2C::writeByte(IMU_I2C, m_slaveAddr, MPU9250_USER_CTRL, userControl)) return false;
//	delay(50);
	if(!I2C::writeByte(IMU_I2C, m_slaveAddr, MPU9250_INT_PIN_CFG, 0x82)) return false;
//	delay(50);

	return true;
}

bool ImuMpu9250::bypassOff()
{
	uint8_t userControl;
	
	if(!I2C::readByte(IMU_I2C, m_slaveAddr, MPU9250_USER_CTRL, &userControl)) return false;
	
	userControl |= 0x20;
	
	if(!I2C::writeByte(IMU_I2C, m_slaveAddr, MPU9250_USER_CTRL, userControl)) return false;
//	delay(50);
	if(!I2C::writeByte(IMU_I2C, m_slaveAddr, MPU9250_INT_PIN_CFG, 0x80)) return false;
//	delay(50);
	
	return true;
}

bool ImuMpu9250::setCompassRate()
{
	uint32_t rate;
	
	rate = m_sampleRate / m_compassRate - 1;
	
	if(rate > 31) rate = 31;
	
	if(!I2C::writeByte(IMU_I2C, m_slaveAddr, MPU9250_I2C_SLV4_CTRL, rate)) return false;
	
	return true;
}

bool ImuMpu9250::resetFifo()
{
	if(!I2C::writeByte(IMU_I2C, m_slaveAddr, MPU9250_INT_ENABLE, 0)) return false;
	if(!I2C::writeByte(IMU_I2C, m_slaveAddr, MPU9250_FIFO_EN, 0)) return false;
	if(!I2C::writeByte(IMU_I2C, m_slaveAddr, MPU9250_USER_CTRL, 0)) return false;
	if(!I2C::writeByte(IMU_I2C, m_slaveAddr, MPU9250_USER_CTRL, 0x04)) return false;
	if(!I2C::writeByte(IMU_I2C, m_slaveAddr, MPU9250_USER_CTRL, 0x60)) return false;
	
//	delay(50);
	
	if(!I2C::writeByte(IMU_I2C, m_slaveAddr, MPU9250_INT_ENABLE, 1)) return false;
	if(!I2C::writeByte(IMU_I2C, m_slaveAddr, MPU9250_FIFO_EN, 0x78)) return false;
	
	return true;
}

