#include "Platform.h"
#include "Vector3.h"

Vector3 Vector3::Cross(const Vector3& v) const
{
	float result_x = (y * v.z) - (z * v.y);
	float result_y = (z * v.x) - (x * v.z);
	float result_z = (x * v.y) - (y * v.x);
	return Vector3(result_x, result_y, result_z);
}

float Vector3::GetRoll(float tightness) const
{
	float pos = fabsf(GetPitch() / (PI / 2.0f));
	float roll_t = (pos - (1.0f - tightness)) / tightness;
	float roll = Bound< float >(roll_t, 0.0f, 1.0f) * (PI / 2.0f);
	if (fabsf(GetYaw()) > (PI / 2.0f))
	{
		roll = -roll;
	}
	return roll;
}

Vector3 Vector3::operator*(const Matrix44& m) const
{
	Vector3 result;
	for (int i = 0; i < 3; i++)
	{
		result[i] = (x * m[i][0]) + (y * m[i][1]) + (z * m[i][2]) + m[i][3];
	}
	return result;
}

Vector3& Vector3::operator*=(const Matrix44& m)
{
	*this = *this * m;
	return *this;
}
