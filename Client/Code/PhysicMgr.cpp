#include "stdafx.h"
#include "PhysicMgr.h"


float CPhysicMgr::ConvertGravity(const D3DXVECTOR3* pIn, const float fPower, const float acc)
{
	if (nullptr == pIn)
	{
		return 0.f;
	}

	static const float fGravity = 9.8f;
	static const float fHalf = 0.5f;
	static const float fPI = 3.141592f;
	
	const float fUpForce = fPower * sinf(90 * fPI / 180.f);
	const float fDownForce = powf(acc, 2.f) * fGravity * fHalf;

	return fUpForce - fDownForce;
}
