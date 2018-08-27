#include "main.h"

void Maths::convertToVector(uint8_t* rawData, Vector& vector, float scale, bool bigEndian)
{
	if(bigEndian)
	{
		vector.setX((float)((int16_t)(((uint16_t)rawData[0] << 8) | (uint16_t)rawData[1])) * scale);
		vector.setY((float)((int16_t)(((uint16_t)rawData[2] << 8) | (uint16_t)rawData[3])) * scale);
		vector.setZ((float)((int16_t)(((uint16_t)rawData[4] << 8) | (uint16_t)rawData[5])) * scale);
	}
	else
	{
		vector.setX((float)((int16_t)(((uint16_t)rawData[1] << 8) | (uint16_t)rawData[0])) * scale);
		vector.setY((float)((int16_t)(((uint16_t)rawData[3] << 8) | (uint16_t)rawData[2])) * scale);
		vector.setZ((float)((int16_t)(((uint16_t)rawData[5] << 8) | (uint16_t)rawData[4])) * scale);
	}
}

void Maths::display(const char* label, const Vector& vector)
{
	Debug::print("%s\n", label);
	Debug::print(" x:"); Debug::print("%f\n", vector.x());
  Debug::print(" y:"); Debug::print("%f\n", vector.y());
	Debug::print(" z:"); Debug::print("%f\n", vector.z());
}

//-------------------------------- Vector class --------------------------------------//

Vector::Vector()
{
	zero();
}

Vector::Vector(float x, float y, float z)
{
	m_data[0] = x;
	m_data[1] = y;
	m_data[2] = z;
}

Vector& Vector::operator =(const Vector& vector)
{
	if(this == &vector) return *this;

	m_data[0] = vector.m_data[0];
	m_data[1] = vector.m_data[1];
	m_data[2] = vector.m_data[2];

	return *this;
}


const Vector& Vector::operator +=(Vector& vector)
{
	for(uint32_t i = 0; i < 3; i++) m_data[i] += vector.m_data[i];
	
	return *this;
}

const Vector& Vector::operator -=(Vector& vector)
{
	for(uint32_t i = 0; i < 3; i++) m_data[i] -= vector.m_data[i];
	
	return *this;
}

void Vector::zero()
{
	for(uint32_t i = 0; i < 3; i++) m_data[i] = 0;
}

float Vector::length()
{
	return (float)sqrt(m_data[0] * m_data[0] + m_data[1] * m_data[1] + m_data[2] * m_data[2]);
}

float Vector::squareLength()
{
	return m_data[0] * m_data[0] + m_data[1] * m_data[1] + m_data[2] * m_data[2];
}

float Vector::dotProduct(const Vector& a, const Vector& b)
{
	return a.x() * b.x() + a.y() * b.y() + a.z() * b.z();
}

void Vector::crossProduct(const Vector& a, const Vector& b, Vector& d)
{
	d.setX(a.y() * b.z() - a.z() * b.y());
	d.setY(a.z() * b.x() - a.x() * b.z());
	d.setZ(a.x() * b.y() - a.y() * b.x());
}

void Vector::accelToEuler(Vector& rollPitchYaw) const
{
	Vector normAccel = *this;
	
	normAccel.normalize();
	
	rollPitchYaw.setX(atan2(normAccel.y(), normAccel.z()));
	rollPitchYaw.setY(-atan2(normAccel.x(), sqrt(normAccel.y() * normAccel.y() + normAccel.z() * normAccel.z())));
	rollPitchYaw.setZ(0);
}


void Vector::accelToQuaternion(Quaternion& qPose) const
{
	Vector normAccel = *this;
	Vector vec;
	Vector z(0, 0, 1.0);

	normAccel.normalize();

	float angle = acos(Vector::dotProduct(z, normAccel));
	Vector::crossProduct(normAccel, z, vec);
	vec.normalize();

	qPose.fromAngleVector(angle, vec);
}

void Vector::normalize()
{
	float length = (float)sqrt(m_data[0] * m_data[0] + m_data[1] * m_data[1] + m_data[2] * m_data[2]);

	if((length == 0) || (length == 1)) return;

	m_data[0] /= length;
	m_data[1] /= length;
	m_data[2] /= length;
}

//-------------------------------- Quaternion class --------------------------------------//

Quaternion::Quaternion()
{
	zero();
}

Quaternion::Quaternion(float scalar, float x, float y, float z)
{
	m_data[0] = scalar;
	m_data[1] = x;
	m_data[2] = y;
	m_data[3] = z;
}

Quaternion& Quaternion::operator =(const Quaternion& quat)
{
	if(this == &quat) return *this;
	
	m_data[0] = quat.m_data[0];
	m_data[1] = quat.m_data[1];
	m_data[2] = quat.m_data[2];
	m_data[3] = quat.m_data[3];
	
	return *this;
}

Quaternion& Quaternion::operator +=(const Quaternion& quat)
{
	for (int i = 0; i < 4; i++) m_data[i] += quat.m_data[i];
	
	return *this;
}

Quaternion& Quaternion::operator -=(const Quaternion& quat)
{
	for (int i = 0; i < 4; i++) m_data[i] -= quat.m_data[i];
	
	return *this;
}

Quaternion& Quaternion::operator -=(const float val)
{
	for(int i = 0; i < 4; i++) m_data[i] -= val;

	return *this;
}

Quaternion& Quaternion::operator *=(const Quaternion& qb)
{
	Quaternion qa;
	
	qa = *this;
	
	m_data[0] = qa.scalar() * qb.scalar() - qa.x() * qb.x() - qa.y() * qb.y() - qa.z() * qb.z();
	m_data[1] = qa.scalar() * qb.x() + qa.x() * qb.scalar() + qa.y() * qb.z() - qa.z() * qb.y();
	m_data[2] = qa.scalar() * qb.y() - qa.x() * qb.z() + qa.y() * qb.scalar() + qa.z() * qb.x();
	m_data[3] = qa.scalar() * qb.z() + qa.x() * qb.y() - qa.y() * qb.x() + qa.z() * qb.scalar();
	
	return *this;
}

Quaternion& Quaternion::operator *=(const float val)
{
	m_data[0] *= val;
	m_data[1] *= val;
	m_data[2] *= val;
	m_data[3] *= val;
	
	return *this;
}

const Quaternion Quaternion::operator *(const Quaternion& qb) const
{
	Quaternion result = *this;
	result *= qb;
	
	return result;
}

const Quaternion Quaternion::operator *(const float val) const
{
	Quaternion result = *this;
	result *= val;
	
	return result;
}

const Quaternion Quaternion::operator -(const Quaternion& qb) const
{
	Quaternion result = *this;
	result -= qb;
	
	return result;
}

const Quaternion Quaternion::operator -(const float val) const
{
	Quaternion result = *this;
	result -= val;
	
	return result;
}


void Quaternion::zero()
{
	for(int i = 0; i < 4; i++) m_data[i] = 0;
}

void Quaternion::normalize()
{
	float length = sqrt(m_data[0] * m_data[0] + m_data[1] * m_data[1] + m_data[2] * m_data[2] + m_data[3] * m_data[3]);
		
	if((length == 0) || (length == 1)) return;
	
	m_data[0] /= length;
	m_data[1] /= length;
	m_data[2] /= length;
	m_data[3] /= length;
}

void Quaternion::toEuler(Vector& vec)
{
	vec.setX(atan2(2.0f * (m_data[2] * m_data[3] + m_data[0] * m_data[1]), 1 - 2.0f * (m_data[1] * m_data[1] + m_data[2] * m_data[2])));
	
	vec.setY(asin(2.0f * (m_data[0] * m_data[2] - m_data[1] * m_data[3])));
	
	vec.setZ(atan2(2.0f * (m_data[1] * m_data[2] + m_data[0] * m_data[3]), 1 - 2.0f * (m_data[2] * m_data[2] + m_data[3] * m_data[3])));
}

void Quaternion::fromEuler(Vector& vec)
{
	float cosX2 = cos(vec.x() / 2.0f);
	float sinX2 = sin(vec.x() / 2.0f);
	float cosY2 = cos(vec.y() / 2.0f);
	float sinY2 = sin(vec.y() / 2.0f);
	float cosZ2 = cos(vec.z() / 2.0f);
	float sinZ2 = sin(vec.z() / 2.0f);
	
	m_data[0] = cosX2 * cosY2 * cosZ2 + sinX2 * sinY2 * sinZ2;
	m_data[1] = sinX2 * cosY2 * cosZ2 - cosX2 * sinY2 * sinZ2;
	m_data[2] = cosX2 * sinY2 * cosZ2 + sinX2 * cosY2 * sinZ2;
	m_data[3] = cosX2 * cosY2 * sinZ2 - sinX2 * sinY2 * cosZ2;
	
	normalize();
}

Quaternion Quaternion::conjugate() const
{
	Quaternion q;
	q.setScalar(m_data[0]);
	q.setX(-m_data[1]);
	q.setY(-m_data[2]);
	q.setZ(-m_data[3]);
	
	return q;
}

void Quaternion::toAngleVector(float& angle, Vector& vec)
{
	float halfTheta;
	float sinHalfTheta;
	
	halfTheta = acos(m_data[0]);
	sinHalfTheta = sin(halfTheta);
	
	if(sinHalfTheta == 0)
	{
		vec.setX(1.0);
		vec.setY(0);
		vec.setZ(0);
	}
	else
	{
		vec.setX(m_data[1] / sinHalfTheta);
		vec.setY(m_data[1] / sinHalfTheta);
		vec.setZ(m_data[1] / sinHalfTheta);
	}
	
	angle = 2.0f * halfTheta;
}

void Quaternion::fromAngleVector(const float& angle, const Vector& vec)
{
	float sinHalfTheta = sin(angle / 2.0f);
	m_data[0] = cos(angle / 2.0f);
	m_data[1] = vec.x() * sinHalfTheta;
	m_data[2] = vec.y() * sinHalfTheta;
	m_data[3] = vec.z() * sinHalfTheta;
}

