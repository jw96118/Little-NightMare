#include "stdafx.h"
#include "StaticCamera.h"

#include "Export_Function.h"

CStaticCamera::CStaticCamera(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CCamera(pGraphicDev)
	, m_pTargetInfo(nullptr)
	, m_fDistance(0.f)
	, m_fAngle(0.f)
{

}

CStaticCamera::~CStaticCamera(void)
{

}

void CStaticCamera::SetUp_Target(Engine::CTransform* pTargetInfo)
{
	m_pTargetInfo = pTargetInfo;
}

HRESULT CStaticCamera::Ready_GameObject(const _vec3* pEye,
										const _vec3* pAt,
										const _vec3* pUp,
										const _float& fFovY,
										const _float& fAspect,
										const _float& fNear,
										const _float& fFar)
{
	m_vEye = *pEye;
	m_vAt = *pAt;
	m_vUp = *pUp;

	m_fFovY = fFovY;
	m_fAspect = fAspect;
	m_fNear = fNear;
	m_fFar = fFar;

	m_fDistance = 10.f;

	FAILED_CHECK_RETURN(Engine::CCamera::Ready_GameObject(), E_FAIL);

	return S_OK;
}

_int CStaticCamera::Update_GameObject(const _float& fTimeDelta)
{
	Target_Renewal(fTimeDelta);
	Key_Input(fTimeDelta);

	_int iExit = Engine::CCamera::Update_GameObject(fTimeDelta);

	return iExit;
}

void CStaticCamera::Key_Input(const _float& fTimeDelta)
{
	if (Engine::Get_DIKeyState(DIK_Q) & 0x80)
		m_fAngle += D3DXToRadian(45.f) * fTimeDelta;

	if (Engine::Get_DIKeyState(DIK_A) & 0x80)
		m_fAngle -= D3DXToRadian(45.f) * fTimeDelta;

	if (Engine::Get_DIKeyState(DIK_O) & 0x80)
		m_fDistance += fTimeDelta * 20.f;

	if (Engine::Get_DIKeyState(DIK_P) & 0x80)
		m_fDistance -= fTimeDelta * 20.f;
}

void CStaticCamera::Target_Renewal(const _float& fTimeDelta)
{
	m_vEye = m_pTargetInfo->m_vInfo[Engine::INFO_LOOK] * -1.f;
	D3DXVec3Normalize(&m_vEye, &m_vEye);

	m_vEye *= m_fDistance;

	_vec3		vRight;
	memcpy(&vRight, &m_pTargetInfo->m_matWorld.m[0][0], sizeof(_vec3));

	_matrix		matRotAxis;
	D3DXMatrixRotationAxis(&matRotAxis, &vRight, m_fAngle);
	D3DXVec3TransformNormal(&m_vEye, &m_vEye, &matRotAxis);

	m_vEye += m_pTargetInfo->m_vInfo[Engine::INFO_POS];
	m_vAt = m_pTargetInfo->m_vInfo[Engine::INFO_POS];
}

CStaticCamera* CStaticCamera::Create(LPDIRECT3DDEVICE9 pGraphicDev, 
									Engine::CTransform* pTargetInfo,
									const _vec3* pEye,
									const _vec3* pAt,
									const _vec3* pUp,
									const _float& fFovY,
									const _float& fAspect,
									const _float& fNear,
									const _float& fFar)
{
	CStaticCamera*	pInstance = new CStaticCamera(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject(pEye, pAt, pUp, fFovY, fAspect, fNear, fFar)))
		Engine::Safe_Release(pInstance);
	
	pInstance->SetUp_Target(pTargetInfo);

	return pInstance;
}

void CStaticCamera::Free(void)
{
	Engine::CCamera::Free();
}

