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
	virtual bool lowLevelInit() = 0;
	virtual int32_t init() = 0;                              		// set up the IMU
	virtual bool read() = 0;																		// get a sample

	inline const Vector& getGyro() {return m_gyro;}            	// gets gyro rates in radians/sec
	inline const Vector& getAccel() {return m_accel;}          	// get accel data in gs
	inline const Vector& getCompass() {return m_compass;}      	// gets compass data in uT
	inline uint32_t getTimestamp() {return m_timestamp;} 				// and the timestamp for it

	inline bool gyroBiasValid() {return m_gyroBiasValid;}
protected:
	void gyroBiasInit();												// sets up gyro bias calculation
	void handleGyroBias();											// adjust gyro for bias
	void calibrateAverageCompass();							// calibrate and smooth compass
	bool m_calibrationMode;											// true if cal mode so don't use cal data!
	bool m_calibrationValid; 
	
	Vector m_gyro;                              // the gyro readings
	Vector m_accel;                             // the accel readings
	Vector m_compass;                           // the compass readings
	uint32_t m_timestamp;                       // the timestamp

	ImuSettings* m_settings;										// the settings object pointer

	uint32_t m_sampleRate;											// samples per second
	uint64_t m_sampleInterval;									// interval between samples in microseonds

	float m_gyroAlpha;                          // gyro bias learning rate
	uint32_t m_gyroSampleCount;                 // number of gyro samples used
	bool m_gyroBiasValid;												// true if the recorded gyro bias is valid
	Vector m_gyroBias;                          // the recorded gyro bias

	Vector m_previousAccel;                     // previous step accel for gyro learning
	
	float m_compassCalOffset[3];
	float m_compassCalScale[3];
	Vector m_compassAverage;										// a running average to smooth the mag outputs
};

#endif
