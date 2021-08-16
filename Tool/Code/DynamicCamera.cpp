#include "pch.h"
#include "DynamicCamera.h"

#include "Export_Function.h"

CDynamicCamera::CDynamicCamera(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CCamera(pGraphicDev)
	, m_bClick(false)
	, m_bMouseFix(true)
{

}

CDynamicCamera::~CDynamicCamera(void)
{

}

HRESULT CDynamicCamera::Ready_GameObject(const _vec3* pEye, const _vec3* pAt, const _vec3* pUp, const _float& fFovY, const _float& fAspect, const _float& fNear, const _float& fFar, const _bool& bMainCamera)
{
	m_vEye = *pEye;
	m_vAt = *pAt;
	m_vUp = *pUp;

	m_fFovY = fFovY;
	m_fAspect = fAspect;
	m_fNear = fNear;
	m_fFar = fFar;

	m_fSpeed = 0.5f;

	m_bMainCamera = bMainCamera;

	FAILED_CHECK_RETURN(Engine::CCamera::Ready_GameObject(), E_FAIL);

	m_pKeyMgr = Engine::Get_KeyMgr();

	return S_OK;
}

_int CDynamicCamera::Update_GameObject(const _float& fTimeDelta)
{
	m_pKeyMgr->Update();
	Key_Input(fTimeDelta);

	if (m_pKeyMgr->KeyPressing(Engine::KEY_TAB))
	{
		Mouse_Move(fTimeDelta);
		Mouse_Fix();
	}


	_int iExit = Engine::CCamera::Update_GameObject(fTimeDelta);

	return iExit;
}

void CDynamicCamera::Key_Input(const _float& fTimeDelta)
{
	_matrix	matCamWorld;
	D3DXMatrixInverse(&matCamWorld, NULL, &m_matView);

	if (!m_pKeyMgr->KeyPressing(Engine::KEY_LCTRL))
	{
		if (Engine::Get_DIMouseWheel() > 0 || m_pKeyMgr->KeyPressing(Engine::KEY_W))
		{

			_vec3	vLook;
			memcpy(&vLook, &matCamWorld.m[2][0], sizeof(_vec3));

			_vec3 vLength = *D3DXVec3Normalize(&vLook, &vLook) * m_fSpeed;

			m_vEye += vLength;
			m_vAt += vLength;
		}

		else if (Engine::Get_DIMouseWheel() < 0 || m_pKeyMgr->KeyPressing(Engine::KEY_S))
		{
			_vec3	vLook;
			memcpy(&vLook, &matCamWorld.m[2][0], sizeof(_vec3));

			_vec3 vLength = *D3DXVec3Normalize(&vLook, &vLook) * m_fSpeed;

			m_vEye -= vLength;
			m_vAt -= vLength;
		}

		if (m_pKeyMgr->KeyPressing(Engine::KEY_A))
		{
			_vec3	vRight;
			memcpy(&vRight, &matCamWorld.m[0][0], sizeof(_vec3));

			_vec3 vLength = *D3DXVec3Normalize(&vRight, &vRight) * m_fSpeed;

			m_vEye -= vLength;
			m_vAt -= vLength;
		}

		if (m_pKeyMgr->KeyPressing(Engine::KEY_D))
		{
			_vec3	vRight;
			memcpy(&vRight, &matCamWorld.m[0][0], sizeof(_vec3));

			_vec3 vLength = *D3DXVec3Normalize(&vRight, &vRight) * m_fSpeed;

			m_vEye += vLength;
			m_vAt += vLength;
		}
	}
	//if (Engine::Get_DIKeyState(DIK_A) & 0x80)
	//{
	//	_vec3	vRight;
	//	memcpy(&vRight, &matCamWorld.m[0][0], sizeof(_vec3));

	//	_vec3 vLength = *D3DXVec3Normalize(&vRight, &vRight) * m_fSpeed * fTimeDelta;

	//	m_vEye -= vLength;
	//	m_vAt -= vLength;
	//}

	//if (Engine::Get_DIKeyState(DIK_D) & 0x80)
	//{
	//	_vec3	vRight;
	//	memcpy(&vRight, &matCamWorld.m[0][0], sizeof(_vec3));

	//	_vec3 vLength = *D3DXVec3Normalize(&vRight, &vRight) * m_fSpeed * fTimeDelta;

	//	m_vEye += vLength;
	//	m_vAt += vLength;
	//}

	//if (Engine::Get_DIKeyState(DIK_TAB) & 0x80)
	//{
	//	if (true == m_bClick)
	//		return;

	//	m_bClick = true;

	//	if (true == m_bMouseFix)
	//		m_bMouseFix = false;

	//	else
	//		m_bMouseFix = true;

	//}
	//else
	//	m_bClick = false;

	//if (false == m_bMouseFix)
	//	return;
}

void CDynamicCamera::Mouse_Move(const _float& fTimeDelta)
{
	_long	dwMouseMove = 0;

	_matrix	matCamWorld;
	D3DXMatrixInverse(&matCamWorld, NULL, &m_matView);

	if (dwMouseMove = Engine::Get_DIMouseMove(Engine::DIMS_Y))
	{
		_vec3	vRight;
		memcpy(&vRight, &matCamWorld.m[0][0], sizeof(_vec3));

		_vec3	vLook = m_vAt - m_vEye;

		_matrix		matRotAxis;
		D3DXMatrixRotationAxis(&matRotAxis, &vRight, D3DXToRadian(dwMouseMove / 10.f));

		D3DXVec3TransformNormal(&vLook, &vLook, &matRotAxis);

		m_vAt = m_vEye + vLook;
	}

	if (dwMouseMove = Engine::Get_DIMouseMove(Engine::DIMS_X))
	{
		_vec3	vUp = _vec3(0.f, 1.f, 0.f);

		_vec3	vLook = m_vAt - m_vEye;

		_matrix		matRotAxis;
		D3DXMatrixRotationAxis(&matRotAxis, &vUp, D3DXToRadian(dwMouseMove / 10.f));

		D3DXVec3TransformNormal(&vLook, &vLook, &matRotAxis);

		m_vAt = m_vEye + vLook;
	}

}

void CDynamicCamera::Mouse_Fix(void)
{
	POINT		ptMouse{ WINCX >> 1, WINCY >> 1 };

	ClientToScreen(g_hWnd, &ptMouse);
	SetCursorPos(ptMouse.x, ptMouse.y);
}

CDynamicCamera* CDynamicCamera::Create(LPDIRECT3DDEVICE9 pGraphicDev,
	const _vec3* pEye, const _vec3* pAt, const _vec3* pUp,
	const _float& fFovY, const _float& fAspect,
	const _float& fNear, const _float& fFar,
	const _bool& bMainCamera)
{
	CDynamicCamera*	pInstance = new CDynamicCamera(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject(pEye, pAt, pUp, fFovY, fAspect, fNear, fFar, bMainCamera)))
		Engine::Safe_Release(pInstance);

	return pInstance;
}

void CDynamicCamera::Free(void)
{
	Engine::CCamera::Free();
}

