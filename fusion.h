#ifndef _FUSION_H
#define _FUSION_H

#define USE_SLERP

class Fusion
{
public:
	Fusion();
	~Fusion();

	// resets the state but keeps any setting changes (such as enables)
	void reset();
	
	// should be called for subsequent updates
	// deltaTime is in units of seconds
	void newImuData(const Vector& gyro, const Vector& accel, const Vector& compass, uint32_t timestamp);

	// the following three functions control the influence of the gyro, accel and compass sensors
	void setGyroEnable(bool enable) {m_enableGyro = enable;}
	void setAccelEnable(bool enable) {m_enableAccel = enable;}
	void setCompassEnable(bool enable) {m_enableCompass = enable;}

#ifdef USE_SLERP
	// the following function can be called to set the SLERP power
	void setSlerpPower(float power) {m_slerpPower = power;}
#else
	// the following two functions can be called to customize the noise covariance
	void setQ(float Q) {m_Q = Q; reset();}
	void setR(float R) {if (R > 0) m_R = R; reset();}
#endif
	
	inline const Vector& getMeasuredPose() {return m_measuredPose;}
	inline const Quaternion& getMeasuredQPose() {return m_measuredQPose;}
	inline const Vector& getFusionPose() {return m_fusionPose;}
	inline const Quaternion& getFusionQPose() {return m_fusionQPose;}
	
private:
	void calculatePose(const Vector& accel, const Vector& mag); // generates pose from accels and heading

	float m_timeDelta;                                    // time between predictions
	Quaternion m_stateQError;                             // difference between stateQ and measuredQ


#ifdef USE_SLERP
	float m_slerpPower;                                   // a value 0 to 1 that controls measured state influence
	Quaternion m_rotationDelta;                           // amount by which measured state differs from predicted
	Quaternion m_rotationPower;                           // delta raised to the appopriate power
	Vector m_rotationUnitVector;                         	// the vector part of the rotation delta
#else
	float m_Q;                                            // process noise covariance
	float m_R;                                            // the measurement noise covariance
#endif

	Quaternion m_measuredQPose;       										// quaternion form of pose from measurement
	Vector m_measuredPose;																// vector form of pose from measurement
	Quaternion m_fusionQPose;                             // quaternion form of pose from fusion
	Vector m_fusionPose;                                 	// vector form of pose from fusion

	bool m_enableGyro;                                      // enables gyro as input
	bool m_enableAccel;                                     // enables accel as input
	bool m_enableCompass;                                   // enables compass a input	
	
	bool m_firstTime;                                       // if first time after reset
	uint32_t m_lastFusionTime;                         // for delta time calculation
};

#endif
