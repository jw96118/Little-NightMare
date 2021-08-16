
#include "Camera.h"
#include "Pipeline.h"

USING(Engine)

Engine::CCamera::CCamera(LPDIRECT3DDEVICE9 pGraphicDev)
	: CGameObject(pGraphicDev)
	, m_bMainCamera(false)
{

}

Engine::CCamera::~CCamera(void)
{

}

const _matrix* Engine::CCamera::Get_ViewSpace(void)
{ 
	return &m_matView; 
}
const _matrix* Engine::CCamera::Get_Projection(void)
{ 
	return &m_matProj; 
}

const _bool& Engine::CCamera::Get_IsMainCamera(void) const
{
	return m_bMainCamera;
}

HRESULT Engine::CCamera::Ready_GameObject(void)
{
	D3DXMatrixLookAtLH(&m_matView, &m_vEye, &m_vAt, &m_vUp);
	//CPipeline::MakeViewSpaceMatrix(&m_matView, &m_vEye, &m_vAt, &m_vUp);
	m_pGraphicDev->SetTransform(D3DTS_VIEW, &m_matView);

	D3DXMatrixPerspectiveFovLH(&m_matProj, m_fFovY, m_fAspect, m_fNear, m_fFar);
	//CPipeline::MakeProjectionMatrix(&m_matProj, m_fFovY, m_fAspect, m_fNear, m_fFar);
	m_pGraphicDev->SetTransform(D3DTS_PROJECTION, &m_matProj);

	return S_OK;
}

_int Engine::CCamera::Update_GameObject(const _float& fTimeDelta)
{
	D3DXMatrixLookAtLH(&m_matView, &m_vEye, &m_vAt, &m_vUp);
	//CPipeline::MakeViewSpaceMatrix(&m_matView, &m_vEye, &m_vAt, &m_vUp);
	m_pGraphicDev->SetTransform(D3DTS_VIEW, &m_matView);

	return 0;
}

void Engine::CCamera::Free(void)
{
	CGameObject::Free();
}


