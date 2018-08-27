#include "main.h"

#ifdef USE_SLERP
//  The slerp power valule controls the influence of the measured state to correct the predicted state
//  0 = measured state ignored (just gyros), 1 = measured state overrides predicted state.
//  In between 0 and 1 mixes the two conditions

#define SLERP_POWER (float)0.02;

#else
//  The QVALUE affects the gyro response.

#define QVALUE	(float)0.001

//  The RVALUE controls the influence of the accels and compass.
//  The bigger the value, the more sluggish the response.

#define RVALUE	(float)0.0005
#endif

Fusion::Fusion()
{
#ifdef USE_SLERP
	m_slerpPower = SLERP_POWER;
#else
	m_Q = QVALUE;
	m_R = RVALUE;
#endif
	m_enableGyro = true;
	m_enableAccel = true;
	m_enableCompass = true;
	reset();
}

Fusion::~Fusion()
{
}

void Fusion::reset()
{
	m_firstTime = true;
	m_fusionPose = Vector();
	m_fusionQPose.fromEuler(m_fusionPose);
	m_measuredPose = Vector();
	m_measuredQPose.fromEuler(m_measuredPose);
}

void Fusion::newImuData(const Vector& gyro, const Vector& accel, const Vector& compass, uint32_t timestamp)
{
	Vector fusionGyro;
	
	if(m_firstTime)
	{
		m_lastFusionTime = timestamp;
		calculatePose(accel, compass);

    // initialize the poses
		m_fusionQPose.fromEuler(m_measuredPose);
		m_fusionPose = m_measuredPose;
		m_firstTime = false;
	}
	else
	{
		m_timeDelta = (float)(timestamp - m_lastFusionTime) / (float)1000;
		m_lastFusionTime = timestamp;
		
		if(m_timeDelta <= 0) return;
		
		calculatePose(accel, compass);

		// predict();
		
		float x2, y2, z2;
		float qs, qx, qy,qz;
		
		qs = m_fusionQPose.scalar();
		qx = m_fusionQPose.x();
		qy = m_fusionQPose.y();
		qz = m_fusionQPose.z();
		
		if(m_enableGyro)
			fusionGyro = gyro;
		else
			fusionGyro = Vector();

		x2 = fusionGyro.x() / 2.0f;
		y2 = fusionGyro.y() / 2.0f;
		z2 = fusionGyro.z() / 2.0f;
		
		// Predict new state

		m_fusionQPose.setScalar(qs + (-x2 * qx - y2 * qy - z2 * qz) * m_timeDelta);
		m_fusionQPose.setX(qx + (x2 * qs + z2 * qy - y2 * qz) * m_timeDelta);
		m_fusionQPose.setY(qy + (y2 * qs - z2 * qx + x2 * qz) * m_timeDelta);
		m_fusionQPose.setZ(qz + (z2 * qs + y2 * qx - x2 * qy) * m_timeDelta);

		// update();

#ifdef USE_SLERP
		if(m_enableCompass || m_enableAccel)
		{
			// calculate rotation delta

			m_rotationDelta = m_fusionQPose.conjugate() * m_measuredQPose;
			m_rotationDelta.normalize();

			// take it to the power (0 to 1) to give the desired amount of correction

			float theta = acos(m_rotationDelta.scalar());

			float sinPowerTheta = sin(theta * m_slerpPower);
			float cosPowerTheta = cos(theta * m_slerpPower);

			m_rotationUnitVector.setX(m_rotationDelta.x());
			m_rotationUnitVector.setY(m_rotationDelta.y());
			m_rotationUnitVector.setZ(m_rotationDelta.z());
			m_rotationUnitVector.normalize();

			m_rotationPower.setScalar(cosPowerTheta);
			m_rotationPower.setX(sinPowerTheta * m_rotationUnitVector.x());
			m_rotationPower.setY(sinPowerTheta * m_rotationUnitVector.y());
			m_rotationPower.setZ(sinPowerTheta * m_rotationUnitVector.z());
			m_rotationPower.normalize();

			//  multiple this by predicted value to get result

			m_fusionQPose *= m_rotationPower;
		}
#else
		if(m_enableCompass || m_enableAccel)
			m_stateQError = m_measuredQPose - m_fusionQPose;
		else
			m_stateQError = RTQuaternion();

		// make new state estimate

		float qt = m_Q * m_timeDelta;

		m_fusionQPose += m_stateQError * (qt / (qt + m_R));
#endif

		m_fusionQPose.normalize();

		m_fusionQPose.toEuler(m_fusionPose);
	}
}

void Fusion::calculatePose(const Vector& accel, const Vector& mag)
{
	Quaternion m;
	Quaternion q;
	
	bool compassValid = (mag.x() != 0) || (mag.y() != 0) || (mag.z() != 0);
	
	if (m_enableAccel)
		accel.accelToEuler(m_measuredPose);
	else
		m_measuredPose = m_fusionPose;
	
	if(m_enableCompass && compassValid)
	{
		float cosX2 = cos(m_measuredPose.x() / 2.0f);
		float sinX2 = sin(m_measuredPose.x() / 2.0f);
		float cosY2 = cos(m_measuredPose.y() / 2.0f);
		float sinY2 = sin(m_measuredPose.y() / 2.0f);

		q.setScalar(cosX2 * cosY2);
		q.setX(sinX2 * cosY2);
		q.setY(cosX2 * sinY2);
		q.setZ( - sinX2 * sinY2);

		//   normalize();

		m.setScalar(0);
		m.setX(mag.x());
		m.setY(mag.y());
		m.setZ(mag.z());

		m = q * m * q.conjugate();
		m_measuredPose.setZ(-atan2(m.y(), m.x()));
	}
	else
	{
		m_measuredPose.setZ(m_fusionPose.z());
  }
	
	m_measuredQPose.fromEuler(m_measuredPose);

	//  check for quaternion aliasing. If the quaternion has the wrong sign
	//  the kalman filter will be very unhappy.

	int32_t maxIndex = -1;
	float maxVal = -1000;
	
	for(int32_t i = 0; i < 4; i++)
	{
		if(fabs(m_measuredQPose.data(i)) > maxVal)
		{
			maxVal = fabs(m_measuredQPose.data(i));
      maxIndex = i;
		}
	}

	//  if the biggest component has a different sign in the measured and kalman poses,
	//  change the sign of the measured pose to match.

	if(((m_measuredQPose.data(maxIndex) < 0) && (m_fusionQPose.data(maxIndex) > 0)) || ((m_measuredQPose.data(maxIndex) > 0) && (m_fusionQPose.data(maxIndex) < 0)))
	{
		m_measuredQPose.setScalar(-m_measuredQPose.scalar());
		m_measuredQPose.setX(-m_measuredQPose.x());
		m_measuredQPose.setY(-m_measuredQPose.y());
		m_measuredQPose.setZ(-m_measuredQPose.z());
		m_measuredQPose.toEuler(m_measuredPose);
	}
}

