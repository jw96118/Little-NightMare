#include "stdafx.h"
#include "FirstPersonCamera.h"
#include "Player.h"
#include "Export_Function.h"
#include <iostream>
CFirstPersonCamera::CFirstPersonCamera(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CCamera(pGraphicDev)
	, m_vMoveDir(0.f, 0.f, 0.f)
	, m_fRotationX(0.f)
	, m_fRotationY(0.f)
{

}

CFirstPersonCamera::~CFirstPersonCamera(void)
{
}

const _vec3 & CFirstPersonCamera::Get_Eye() const
{
	return m_vEye;
}

const _vec3 & CFirstPersonCamera::Get_At() const
{
	return m_vAt;
}

const _vec3 & CFirstPersonCamera::Get_MoveDir() const
{
	return m_vMoveDir;
}

const _vec3 & CFirstPersonCamera::Get_LookDir() const
{

	return m_vLookDir;
}

const _float & CFirstPersonCamera::Get_RotationX() const
{
	return m_fRotationX;
}

const _float & CFirstPersonCamera::Get_RotationY() const
{
	return m_fRotationY;
}

void CFirstPersonCamera::Set_At(const _vec3 & vAt)
{
	m_vAt = vAt;
}

HRESULT CFirstPersonCamera::Ready_GameObject(const _float& fFovY, const _float& fAspect, const _float& fNear, const _float& fFar)
{
	m_fSpeed = 5.f;
	m_fFovY = fFovY;
	m_fAspect = fAspect;
	m_fNear = fNear;
	m_fFar = fFar;
	m_vUp = _vec3(0.f, 1.f, 0.f);
	FAILED_CHECK_RETURN(Engine::CCamera::Ready_GameObject(), E_FAIL);
	Engine::CComponent* pComponent = m_pTransformCom = Engine::CTransform::Create();
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_DYNAMIC].emplace(L"Com_Transform", pComponent);

	return S_OK;
}

HRESULT CFirstPersonCamera::LateReady_GameObject()
{
	m_pPlayerObj = dynamic_cast<CPlayer*>(Engine::Get_GameObject(L"02.GameLogic", L"Dyn_Player"));

	return S_OK;
}

_int CFirstPersonCamera::Update_GameObject(const _float& fTimeDelta)
{
	Mouse_Move(fTimeDelta);

	_int iExit = Engine::CCamera::Update_GameObject(fTimeDelta);
	m_pTransformCom->Set_Pos(m_vEye);
	return NO_EVENT;
}

void CFirstPersonCamera::Move_View(const _vec3 & vMove)
{
	m_vEye += vMove;
	m_vAt += vMove;

	D3DXMatrixLookAtLH(&m_matView, &m_vEye, &m_vAt, &m_vUp);
	m_pGraphicDev->SetTransform(D3DTS_VIEW, &m_matView);
}


void CFirstPersonCamera::Mouse_Move(const _float & fTimeDelta)
{
	_vec3 vPlayerPos;
	_float vNaviY = 0.f;
	_float CamY = 2.f;

	_vec3 tempAt = m_vAt;


	if (m_pPlayerObj->GetAniflag())
		vPlayerPos = m_pPlayerObj->Get_AniPos();
	else
		vPlayerPos = *m_pPlayerObj->GetTransformCom()->Get_Info(Engine::INFO_POS);

	vNaviY = m_pPlayerObj->Get_NaviY();
	_vec3 tempDir = vPlayerPos - tempAt;
	//D3DXVec3Normalize(&tempDir,&tempDir);
	tempAt += tempDir * fTimeDelta * m_fSpeed * 0.75f;
	if (vPlayerPos.x <= 4.f&& vPlayerPos.z >= 0.f)
	{
		m_vEye = { 4.f, vNaviY + CamY,vPlayerPos.z - 9.f };
		//m_vAt = { vPlayerPos.x, vNaviY, vPlayerPos.z };
		m_vAt = { tempAt };
	}
	else if ((vPlayerPos.x <= 4.f) && vPlayerPos.z < 0.f)
	{
		m_vEye = { 4.f, vNaviY + CamY,vPlayerPos.z - 9.f };
		//m_vAt = { vPlayerPos.x, vNaviY, -0.f };
		m_vAt = { tempAt };
	}
	else if (vPlayerPos.x <= 31.f)
	{
		m_vEye = { vPlayerPos.x, vNaviY + CamY,vPlayerPos.z - 9.f };
		//m_vAt = { vPlayerPos.x, vNaviY, -0.f };
		m_vAt = { tempAt };
	}
	else if (vPlayerPos.x >= 31.f && vPlayerPos.x <= 37.f)
	{
		_float MoveX = m_vEye.x - fTimeDelta * m_fSpeed;

		if (m_PreX == 36.f && vPlayerPos.x <= 34.5f)
		{
			if (MoveX < 31.f)
			{
				MoveX = 31.f;
				m_PreX = 31.f;
			}
		}
		else
			MoveX = m_vEye.x;
		m_vEye = { MoveX, vNaviY + CamY,vPlayerPos.z - 9.f };
		//m_vAt = { vPlayerPos.x, vNaviY, -0.f };
		m_vAt = { tempAt };
	}
	else if (vPlayerPos.x > 37.f && vPlayerPos.x < 41.f)
	{
		_float MoveX = m_vEye.x + fTimeDelta * m_fSpeed;
		if (MoveX > vPlayerPos.x)
		{
			MoveX = vPlayerPos.x;
		}
		m_PreX = 36.f;
		m_vEye = { MoveX, vNaviY + CamY,vPlayerPos.z - 9.f };
		//m_vAt = { vPlayerPos.x, vNaviY, -0.f };
		m_vAt = { tempAt };
	}
	else if (vPlayerPos.x >= 41.f && vPlayerPos.x < 42.5f)
	{
		_float MeveY = m_vEye.y + fTimeDelta * m_fSpeed * 0.5f;
		if (MeveY >= vPlayerPos.y + CamY)
			MeveY = vPlayerPos.y + CamY;
		m_vEye = { vPlayerPos.x,MeveY,vPlayerPos.z - 9.f };
		//m_vAt = { vPlayerPos.x, vPlayerPos.y, -0.f };
		m_vAt = { tempAt };
	}
	else if (vPlayerPos.x >= 42.5f && vPlayerPos.x < 50.5f)
	{
		m_vEye = { vPlayerPos.x,vNaviY + CamY,vPlayerPos.z - 9.f };
		//m_vAt = { vPlayerPos.x, vNaviY, -0.f };
		m_vAt = { tempAt };
	}
	else if (vPlayerPos.x >= 50.5f && vPlayerPos.x < 54.8f)
	{
		m_vEye = { m_vEye.x,vNaviY + CamY,vPlayerPos.z - 9.f };
		//m_vAt = { vPlayerPos.x, vNaviY, -0.f };
		m_vAt = { tempAt };
	}
	else if (vPlayerPos.x >= 54.8f && vPlayerPos.x < 55.5f)
	{
		_float MoveX = m_vEye.x + fTimeDelta * m_fSpeed;
		if (MoveX > vPlayerPos.x)
		{
			MoveX = vPlayerPos.x;
		}
		m_PreX = 52.5f;
		m_vEye = { MoveX, vNaviY + CamY,vPlayerPos.z - 9.f };
		//m_vAt = { vPlayerPos.x, vNaviY, -0.f };
		m_vAt = { tempAt };
	}
	else if (vPlayerPos.x >= 55.5f && vPlayerPos.x < 73.5f && !m_pPlayerObj->Get_Falling())
	{
		m_vEye = { vPlayerPos.x, vNaviY + CamY,vPlayerPos.z - 9.f };
		//m_vAt = { vPlayerPos.x, vNaviY, -0.f };
		m_vAt = { tempAt };
	}
	else if (vPlayerPos.x >= 55.5f && vPlayerPos.x < 73.5f && m_pPlayerObj->Get_Falling())
	{
		m_vEye = { vPlayerPos.x,vPlayerPos.y + CamY,vPlayerPos.z - 9.f };
		//m_vAt = { vPlayerPos.x, vPlayerPos.y, -0.f };
		m_vAt = { tempAt };
	}
	else if (vPlayerPos.x >= 73.5f && vPlayerPos.x < 78.8f)
	{
		_float MoveX = m_vEye.x - fTimeDelta * m_fSpeed;
		_float MoveZ = m_vEye.z - fTimeDelta * m_fSpeed* 0.5f;
		if (m_PreX == 78.8f && vPlayerPos.x <= 75.5f)
		{
			if (MoveX < 73.5f)
			{
				MoveX = 73.5f;
				m_PreX = 73.5f;
			}
			if (MoveZ < vPlayerPos.z - 9.f)
				MoveZ = vPlayerPos.z - 9.f;
		}
		else
		{
			MoveX = m_vEye.x;
			MoveZ = vPlayerPos.z - 9.f;
		}

		m_vEye = { MoveX,vNaviY + CamY, MoveZ };
		//m_vAt = { vPlayerPos.x, vPlayerPos.y, -0.f };
		m_vAt = { tempAt };
	}
	else if (vPlayerPos.x >= 78.8f && vPlayerPos.x <= 92.f)
	{
		_float MoveX = m_vEye.x + fTimeDelta * m_fSpeed * 2.f;
		_float MoveZ = m_vEye.z + fTimeDelta * m_fSpeed * 0.5f;
		if (m_vEye.x < 0)
		{
			MoveX = vPlayerPos.x;
			MoveZ = vPlayerPos.z - 9.f;
		}
		if (MoveX > vPlayerPos.x)
			MoveX = vPlayerPos.x;
		if (MoveZ >= vPlayerPos.z - 6.5f)
			MoveZ = vPlayerPos.z - 6.5f;

		m_PreX = 78.8f;
		m_vEye = { MoveX, vPlayerPos.y + CamY,MoveZ };
		m_vAt = { tempAt };
	}
	else if (vPlayerPos.x >= 92.f && vPlayerPos.x <= 97.5f)
	{
		_float MoveX = m_vEye.x - fTimeDelta * m_fSpeed;
		_float MoveZ = m_vEye.z - fTimeDelta * m_fSpeed* 0.5f;
		if (m_PreX == 95.5f && vPlayerPos.x <= 93.f)
		{
			if (MoveX < 93.f)
			{
				MoveX = 93.f;
				m_PreX = 93.f;
			}
			if (MoveZ < vPlayerPos.z - 9.f)
				MoveZ = vPlayerPos.z - 9.f;
		}
		else
		{
			MoveX = m_vEye.x;
			MoveZ = vPlayerPos.z - 6.5f;
		}
		m_vEye = { MoveX, vPlayerPos.y + CamY,MoveZ };
		m_vAt = { tempAt };
	}
	else if (vPlayerPos.x >= 97.5f && vPlayerPos.x <= 110.f)
	{
		_float MoveX = m_vEye.x + fTimeDelta * m_fSpeed * 2.f;
		_float MoveZ = m_vEye.z + fTimeDelta * m_fSpeed * 0.5f;
		if (m_vEye.x < 0)
		{
			MoveX = vPlayerPos.x;
			MoveZ = vPlayerPos.z - 9.f;
		}
		if (MoveX > vPlayerPos.x)
			MoveX = vPlayerPos.x;
		if (MoveZ >= vPlayerPos.z - 6.5f)
			MoveZ = vPlayerPos.z - 6.5f;

		m_PreX = 97.5f;
		m_vEye = { MoveX, vPlayerPos.y + CamY,MoveZ };
		m_vAt = { tempAt };
	}
	else if (vPlayerPos.x >= 110.f && vPlayerPos.x <= 112.5f)
	{
		_float MoveX = m_vEye.x - fTimeDelta * m_fSpeed;
		_float MoveZ = m_vEye.z - fTimeDelta * m_fSpeed* 0.5f;
		_float MeveY = m_vEye.y + fTimeDelta * m_fSpeed * 0.5f;
		if (m_PreX == 115.f && vPlayerPos.x <= 112.f)
		{
			if (MoveX < 112.f)
			{
				MoveX = 112.f;
				m_PreX = 112.f;
			}
			if (MoveZ < vPlayerPos.z - 9.f)
				MoveZ = vPlayerPos.z - 9.f;
			if (MeveY >= vPlayerPos.y + CamY)
				MeveY = vPlayerPos.y + CamY;
		}
		else
		{
			MoveX = m_vEye.x;
			MoveZ = vPlayerPos.z - 6.5f;

			if (MeveY >= vPlayerPos.y + CamY)
				MeveY = vPlayerPos.y + CamY;
		}
		m_vEye = { MoveX,MeveY,MoveZ };
		m_vAt = { tempAt };
	}
	else if (vPlayerPos.x >= 112.5f && vPlayerPos.x <= 120.f)
	{
		_float MoveX = m_vEye.x + fTimeDelta * m_fSpeed * 2.f;
		_float MoveZ = m_vEye.z + fTimeDelta * m_fSpeed * 0.5f;
		if (m_vEye.x < 0)
		{
			MoveX = vPlayerPos.x;
			MoveZ = vPlayerPos.z - 9.f;
		}
		if (MoveX > vPlayerPos.x)
			MoveX = vPlayerPos.x;
		if (MoveZ >= vPlayerPos.z - 6.5f)
			MoveZ = vPlayerPos.z - 6.5f;

		m_PreX = 115.f;
		m_vEye = { MoveX, vPlayerPos.y + CamY,MoveZ };
		m_vAt = { tempAt };
	}
	else if (vPlayerPos.x >= 120.f && vPlayerPos.x <= 134.f)
	{
		if (!m_pPlayerObj->Get_JionterCatch())
		{
			m_vEye = { vPlayerPos.x, vPlayerPos.y + CamY,vPlayerPos.z - 6.5f };
			m_vAt = { tempAt };
		}
	}

	else if (vPlayerPos.x >= 134.f && vPlayerPos.x <= 135.8f)
	{
		_float MoveX = m_vEye.x - fTimeDelta * m_fSpeed;
		_float MoveZ = m_vEye.z - fTimeDelta * m_fSpeed* 0.5f;
		_float MeveY = m_vEye.y + fTimeDelta * m_fSpeed * 0.5f;
		if (m_PreX == 135.8f && vPlayerPos.x <= 134.5f)
		{
			if (MoveX < 134.5f)
			{
				MoveX = 134.5f;
				m_PreX = 134.5f;
			}
			if (MoveZ < vPlayerPos.z - 9.f)
				MoveZ = vPlayerPos.z - 9.f;
			if (MeveY >= vPlayerPos.y + CamY)
				MeveY = vPlayerPos.y + CamY;
		}
		else
		{
			MoveX = m_vEye.x;
			if (MoveZ <= vPlayerPos.z - 6.5f)
				MoveZ = vPlayerPos.z - 6.5f;
			if (MeveY >= vPlayerPos.y + CamY)
				MeveY = vPlayerPos.y + CamY;
		}
		m_vEye = { MoveX,MeveY,MoveZ };
		m_vAt = { tempAt };
	}
	else if (vPlayerPos.x >= 135.8f && vPlayerPos.x <= 161.5f)
	{
		_float MoveX = m_vEye.x + fTimeDelta * m_fSpeed * 2.f;
		_float MeveY = m_vEye.y - fTimeDelta * m_fSpeed * 0.5f;
		_float MoveZ = m_vEye.z + fTimeDelta * m_fSpeed * 0.5f;

		if (m_vEye.x < 0)
		{
			MoveX = vPlayerPos.x;
			MoveZ = vPlayerPos.z - 9.f;
		}
		if (MoveX > vPlayerPos.x)
			MoveX = vPlayerPos.x;
		if (MoveZ >= vPlayerPos.z - 4.f)
			MoveZ = vPlayerPos.z - 4.f;
		if (MeveY <= vPlayerPos.y + CamY - 1.f)
			MeveY = vPlayerPos.y + CamY - 1.f;
		m_PreX = 135.8f;
		if (!m_pPlayerObj->Get_JionterCatch())
		{
			m_vEye = { MoveX, MeveY,MoveZ };
			m_vAt = { tempAt };
		}
	}
	else if (vPlayerPos.x >= 137.f && vPlayerPos.x <= 161.5f)
	{
		_float MoveX = m_vEye.x + fTimeDelta * m_fSpeed * 2.f;
		_float MeveY = m_vEye.y - fTimeDelta * m_fSpeed * 0.5f;
		_float MoveZ = m_vEye.z + fTimeDelta * m_fSpeed * 0.5f;


		MoveX = vPlayerPos.x;
		MeveY = vPlayerPos.y + CamY - 1.f;
		MoveZ = vPlayerPos.z - 4.f;

		if (!m_pPlayerObj->Get_JionterCatch())
		{
			m_vEye = { MoveX, MeveY,MoveZ };
			m_vAt = { tempAt };
		}
	}
	else if (vPlayerPos.x >= 161.5f && vPlayerPos.x <= 162.6f)
	{
		_float MoveX = m_vEye.x - fTimeDelta * m_fSpeed;
		_float MoveZ = m_vEye.z - fTimeDelta * m_fSpeed* 0.5f;
		_float MeveY = m_vEye.y + fTimeDelta * m_fSpeed * 0.5f;


		MoveX = m_vEye.x;
		MoveZ = vPlayerPos.z - 4.f;
		MeveY = vPlayerPos.y + CamY - 1.f;

		m_vEye = { MoveX,MeveY,MoveZ };
		m_vAt = { tempAt };
	}
	else if (vPlayerPos.x >= 162.6f && vPlayerPos.x <= 185.f)
	{
		_float MoveX = m_vEye.x + fTimeDelta * m_fSpeed ;
		_float MeveY = m_vEye.y + fTimeDelta * m_fSpeed * 0.5f;
		_float MoveZ = m_vEye.z + fTimeDelta * m_fSpeed * 0.5f;

		if (m_vEye.x < 0)
		{
			MoveX = vPlayerPos.x;
			MoveZ = vPlayerPos.z - 9.f;
		}
		if (MoveX >= vPlayerPos.x)
			MoveX = vPlayerPos.x;
		if (MoveZ >= vPlayerPos.z - 6.5f)
			MoveZ = vPlayerPos.z - 6.5f;
		if (MeveY >= vPlayerPos.y + CamY)
			MeveY = vPlayerPos.y + CamY;
		m_PreX = 162.6f;
		if (!m_pPlayerObj->Get_JionterCatch())
		{
			m_vEye = { MoveX, MeveY,MoveZ };
			m_vAt = { tempAt };
		}
	}
	else if (vPlayerPos.x >= 185.f && vPlayerPos.x <= 188.f)
	{
		_float MoveX = m_vEye.x - fTimeDelta * m_fSpeed;
		_float MoveZ = m_vEye.z - fTimeDelta * m_fSpeed* 0.5f;
		_float MeveY = m_vEye.y + fTimeDelta * m_fSpeed * 0.5f;


		MoveX = m_vEye.x;
		MoveZ = vPlayerPos.z - 6.5f;
		MeveY = vPlayerPos.y + CamY;

		m_vEye = { MoveX,MeveY,MoveZ };
		m_vAt = { tempAt };
	}
	else if (vPlayerPos.x >= 188.f)
	{
		_float MoveX = m_vEye.x + fTimeDelta * m_fSpeed * 1.5f;
		_float MeveY = m_vEye.y + fTimeDelta * m_fSpeed * 0.5f;
		_float MoveZ = m_vEye.z + fTimeDelta * m_fSpeed * 0.5f;

		if (m_vEye.x < 0)
		{
			MoveX = vPlayerPos.x;
			MoveZ = vPlayerPos.z - 9.f;
		}
		if (MoveX >= 192.f)
			MoveX = 192.f;
		if (MoveZ >= 4.5f)
			MoveZ = 4.5f;
		if (MeveY >= vNaviY + 5.f)
			MeveY = vNaviY + 5.f;

		if (!m_pPlayerObj->Get_JionterCatch())
		{
			m_vEye = { MoveX, MeveY,MoveZ };
			m_vAt = { tempAt };
		}
	}





	if (m_vEye.z < -9.f)
	{
		m_vEye.z = -9.f;
	}
}

CFirstPersonCamera* CFirstPersonCamera::Create(LPDIRECT3DDEVICE9 pGraphicDev, const _float& fFovY, const _float& fAspect, const _float& fNear, const _float& fFar)
{
	CFirstPersonCamera*	pInstance = new CFirstPersonCamera(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject(fFovY, fAspect, fNear, fFar)))
		Engine::Safe_Release(pInstance);

	return pInstance;
}

void CFirstPersonCamera::Free(void)
{
	Engine::CCamera::Free();
}

