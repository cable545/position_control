#ifndef _MATHS_H
#define _MATHS_H

#define DEG_RAD 0.017453292519943f
#define RAD_DEG 57.29577951308232f

#define PI_f 3.14159265358979323846f

class Vector;
class Quaternion;

class Maths
{
public:
	//  Takes signed 16 bit data from a char array and converts it to a vector of scaled floats
	static void convertToVector(uint8_t* rawData, Vector& vector, float scale, bool bigEndian);

	static void display(const char* label, const Vector& vector);

	static int32_t map(int32_t x, int32_t inMin, int32_t inMax, int32_t outMin, int32_t outMax);
};

class Vector
{
public:
	Vector();
	Vector(float x, float y, float z);

	const Vector& operator +=(Vector& vector);
	const Vector& operator -=(Vector& vector);

	Vector& operator =(const Vector& vector);

	float squareLength();
	void zero();

	inline float x() const { return m_data[0]; }
	inline float y() const { return m_data[1]; }
	inline float z() const { return m_data[2]; }
	inline float data(const int i) const { return m_data[i]; }

	inline void setX(const float val) { m_data[0] = val; }
	inline void setY(const float val) { m_data[1] = val; }
	inline void setZ(const float val) { m_data[2] = val; }
	inline void setData(const int i, float val) { m_data[i] = val; }

	float length();
	void normalize();

//	const char *display();
//	const char *displayDegrees();

	static float dotProduct(const Vector& a, const Vector& b);
	static void crossProduct(const Vector& a, const Vector& b, Vector& d);

	void accelToEuler(Vector& rollPitchYaw) const;
	void accelToQuaternion(Quaternion& qPose) const;

private:
    float m_data[3];
};

class Quaternion
{
public:
	Quaternion();
  Quaternion(float scalar, float x, float y, float z);

	Quaternion& operator +=(const Quaternion& quat);
	Quaternion& operator -=(const Quaternion& quat);
	Quaternion& operator *=(const Quaternion& qb);
	Quaternion& operator *=(const float val);
	Quaternion& operator -=(const float val);

	Quaternion& operator =(const Quaternion& quat);
	const Quaternion operator *(const Quaternion& qb) const;
	const Quaternion operator *(const float val) const;
	const Quaternion operator -(const Quaternion& qb) const;
	const Quaternion operator -(const float val) const;

	void normalize();
	void toEuler(Vector& vec);
	void fromEuler(Vector& vec);
	Quaternion conjugate() const;
	void toAngleVector(float& angle, Vector& vec);
	void fromAngleVector(const float& angle, const Vector& vec);
	
	void zero();
	const char *display();

	inline float scalar() const {return m_data[0];}
	inline float x() const {return m_data[1];}
	inline float y() const {return m_data[2];}
	inline float z() const {return m_data[3];}
	inline float data(const int i) const {return m_data[i];}

	inline void setScalar(const float val) {m_data[0] = val;}
	inline void setX(const float val) {m_data[1] = val;}
	inline void setY(const float val) {m_data[2] = val;}
	inline void setZ(const float val) {m_data[3] = val;}
	inline void setData(const int i, float val) {m_data[i] = val;}

private:
	float m_data[4];
};

#endif
