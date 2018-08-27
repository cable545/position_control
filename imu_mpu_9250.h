#ifndef _IMU_MPU_9250_H
#define _IMU_MPU_9250_H

//  MPU9250 I2C Slave Addresses

#define MPU9250_ADDRESS0            (0x68 << 1)
#define MPU9250_ADDRESS1            0x69
#define MPU9250_ID                  0x71

#define AK8963_ADDRESS              (0x0c << 1) 

// Register Map

#define AK8963_WHO_AM_I							0x00 // should return 0x48

#define MPU9250_SMPLRT_DIV					0x19
#define MPU9250_GYRO_LPF            0x1a
#define MPU9250_GYRO_CONFIG         0x1b
#define MPU9250_ACCEL_CONFIG        0x1c
#define MPU9250_ACCEL_LPF           0x1d
#define MPU9250_FIFO_EN             0x23
#define MPU9250_I2C_MST_CTRL        0x24
#define MPU9250_I2C_SLV0_ADDR       0x25
#define MPU9250_I2C_SLV0_REG        0x26
#define MPU9250_I2C_SLV0_CTRL       0x27
#define MPU9250_I2C_SLV1_ADDR       0x28
#define MPU9250_I2C_SLV1_REG        0x29
#define MPU9250_I2C_SLV1_CTRL       0x2a
#define MPU9250_I2C_SLV2_ADDR       0x2b
#define MPU9250_I2C_SLV2_REG        0x2c
#define MPU9250_I2C_SLV2_CTRL       0x2d
#define MPU9250_I2C_SLV4_CTRL       0x34
#define MPU9250_INT_PIN_CFG         0x37
#define MPU9250_INT_ENABLE          0x38
#define MPU9250_INT_STATUS          0x3a
#define MPU9250_ACCEL_XOUT_H        0x3b
#define MPU9250_GYRO_XOUT_H         0x43
#define MPU9250_EXT_SENS_DATA_00    0x49
#define MPU9250_I2C_SLV1_DO         0x64
#define MPU9250_I2C_MST_DELAY_CTRL  0x67
#define MPU9250_USER_CTRL           0x6a
#define MPU9250_PWR_MGMT_1          0x6b
#define MPU9250_PWR_MGMT_2          0x6c
#define MPU9250_FIFO_COUNT_H        0x72
#define MPU9250_FIFO_R_W            0x74
#define MPU9250_WHO_AM_I						0x75 // Should return 0x71

//  sample rate defines (applies to gyros and accels, not mags)

#define MPU9250_SAMPLERATE_MIN      5                       // 5 samples per second is the lowest
#define MPU9250_SAMPLERATE_MAX      1000                    // 1000 samples per second is the absolute maximum

//  compass rate defines

#define MPU9250_COMPASSRATE_MIN     1                       // 1 samples per second is the lowest
#define MPU9250_COMPASSRATE_MAX     100                     // 100 samples per second is maximum

//  Gyro LPF options

#define MPU9250_GYRO_LPF_8800       0x11                    // 8800Hz, 0.64mS delay
#define MPU9250_GYRO_LPF_3600       0x10                    // 3600Hz, 0.11mS delay
#define MPU9250_GYRO_LPF_250        0x00                    // 250Hz, 0.97mS delay
#define MPU9250_GYRO_LPF_184        0x01                    // 184Hz, 2.9mS delay
#define MPU9250_GYRO_LPF_92         0x02                    // 92Hz, 3.9mS delay
#define MPU9250_GYRO_LPF_41         0x03                    // 41Hz, 5.9mS delay
#define MPU9250_GYRO_LPF_20         0x04                    // 20Hz, 9.9mS delay
#define MPU9250_GYRO_LPF_10         0x05                    // 10Hz, 17.85mS delay
#define MPU9250_GYRO_LPF_5          0x06                    // 5Hz, 33.48mS delay

//  Gyro FSR options

#define MPU9250_GYROFSR_250         0                       // +/- 250 degrees per second
#define MPU9250_GYROFSR_500         8                       // +/- 500 degrees per second
#define MPU9250_GYROFSR_1000        0x10                    // +/- 1000 degrees per second
#define MPU9250_GYROFSR_2000        0x18                    // +/- 2000 degrees per second

//  Accel FSR options

#define MPU9250_ACCELFSR_2          0                       // +/- 2g
#define MPU9250_ACCELFSR_4          8                       // +/- 4g
#define MPU9250_ACCELFSR_8          0x10                    // +/- 8g
#define MPU9250_ACCELFSR_16         0x18                    // +/- 16g

//  Accel LPF options

#define MPU9250_ACCEL_LPF_1046      0x08                    // 1046Hz, 0.50mS delay
#define MPU9250_ACCEL_LPF_218       0x01                    // 218.1Hz, 1.88mS delay
#define MPU9250_ACCEL_LPF_99        0x02                    // 99Hz, 2.88mS delay
#define MPU9250_ACCEL_LPF_45        0x03                    // 44.8Hz, 4.88mS delay
#define MPU9250_ACCEL_LPF_21        0x04                    // 21.2Hz, 8.87mS delay
#define MPU9250_ACCEL_LPF_10        0x05                    // 10.2Hz, 16.83mS delay
#define MPU9250_ACCEL_LPF_5         0x06                    // 5.05Hz, 32.48mS delay
#define MPU9250_ACCEL_LPF_420       0x07                    // 420Hz, 1.38mS delay

//  AK8963 compass registers

#define AK8963_DEVICEID             0x48                    // the device ID
#define AK8963_ST1                  0x02                    // status 1
#define AK8963_CNTL                 0x0a                    // control reg
#define AK8963_ASAX                 0x10                    // start of the fuse ROM data

//  FIFO transfer size

#define MPU9250_FIFO_CHUNK_SIZE     12                      // gyro and accels take 12 bytes

class ImuMpu9250 : public Imu
{
public:
	ImuMpu9250(ImuSettings* settings);
	~ImuMpu9250();

	bool setGyroLpf(uint8_t lpf);
	bool setAccelLpf(uint8_t lpf);
	bool setSampleRate(uint32_t rate);
	bool setCompassRate(int32_t rate);
	bool setGyroFsr(uint8_t fsr);
	bool setAccelFsr(uint8_t fsr);

	virtual const char* name(){return "MPU_9250";}
	virtual int32_t init();
	virtual bool lowLevelInit();
	virtual bool read();
	
private:
	bool setGyroConfig();
	bool setAccelConfig();
	bool setSampleRate();
	bool bypassOn();
	bool bypassOff();
	bool setCompassRate();
	bool resetFifo();

	bool m_firstTime;							// if first sample

	uint8_t m_slaveAddr;					// I2C address of MPU9250

	uint8_t m_gyroLpf;						// gyro low pass filter setting
	uint8_t m_accelLpf;						// accel low pass filter setting
	int32_t m_compassRate;				// compass sample rate in Hz
	uint8_t m_gyroFsr;						// gyro full scale range
	uint8_t m_accelFsr;						// accelerometer full scale range

	float m_gyroScale;
	float m_accelScale;

	float m_compassAdjust[3];			// the compass fuse ROM values converted for use
};

#endif
