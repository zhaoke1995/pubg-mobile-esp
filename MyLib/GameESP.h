#pragma once

#include "D3DMenu.h"
#include "ProcManager.h"
#include "MemoryManager.h"

extern DxManager *pDxm;
extern D3DMenu* pMenu;
extern MemoryManager *pMM;
extern DWORD dwGamePid;
extern DWORD dwJuzhenAddr;

void ESPWork();
DWORD WINAPI ThreadUpdateData(LPVOID p);
DWORD WINAPI ThreadUpdateData2(LPVOID p);
DWORD WINAPI ThreadUpdateData3(LPVOID p);

struct FRotator
{
	float Pitch;
	float Yaw;
	float Roll;

	FRotator()
	{
	}
	FRotator(float flPitch, float flYaw, float flRoll)
	{
		Pitch = flPitch;
		Yaw = flYaw;
		Roll = flRoll;
	}


	double Length()
	{
		return sqrt(Pitch * Pitch + Yaw * Yaw + Roll * Roll);
	}

	FRotator Clamp()
	{
		if (Pitch > 180)
			Pitch -= 360;
		else if (Pitch < -180)
			Pitch += 360;
		if (Yaw > 180)
			Yaw -= 360;
		else if (Yaw < -180)
			Yaw += 360;
		if (Pitch < -89)
			Pitch = -89;
		if (Pitch > 89)
			Pitch = 89;
		while (Yaw < -180.0f)
			Yaw += 360.0f;
		while (Yaw > 180.0f)
			Yaw -= 360.0f;
		Roll = 0;
		return *this;
	}

	FRotator operator-(FRotator angB)
	{

		return FRotator(Pitch - angB.Pitch, Yaw - angB.Yaw, Roll - angB.Roll);
	}

	FRotator operator+(FRotator angB)
	{

		return FRotator(Pitch + angB.Pitch, Yaw + angB.Yaw, Roll + angB.Roll);
	}
};


//Vector3
class Vector3
{
public:
	Vector3() : x(0.f), y(0.f), z(0.f)
	{

	}

	Vector3(float _x, float _y, float _z) : x(_x), y(_y), z(_z)
	{

	}
	~Vector3()
	{

	}

	float x;
	float y;
	float z;

	inline float Dot(Vector3 v)
	{
		return x * v.x + y * v.y + z * v.z;
	}

	inline float Distance(Vector3 v)
	{
		return float(sqrtf(powf(v.x - x, 2.0) + powf(v.y - y, 2.0) + powf(v.z - z, 2.0)));
	}
	inline float Length() {
		return sqrt(x*x + y * y + z * z);
	}

	FRotator ToFRotator()
	{
		FRotator rot;
		float RADPI = (float)(180 / M_PI);
		rot.Yaw = (float)(atan2(y, x) * RADPI);
		rot.Pitch = (float)atan2(z, sqrt((x * x) + (y * y))) * RADPI;
		rot.Roll = 0;

		return rot;
	}


	Vector3 operator+(Vector3 v)
	{
		return Vector3(x + v.x, y + v.y, z + v.z);
	}

	Vector3 operator-(Vector3 v)
	{
		return Vector3(x - v.x, y - v.y, z - v.z);
	}
	Vector3 operator/(float Scale) {
		const float RScale = 1.f / Scale;
		return Vector3(x * RScale, y * RScale, z * RScale);
	}
	//Vector3& Vector3::operator*=(const Vector3& v) {
	//	x *= v.x;
	//	y *= v.y;
	//	z *= v.z;
	//	return *this;
	//}

};
