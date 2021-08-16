#include "stdafx.h"
#include "SkyBox.h"

#include "Export_Function.h"

CSkyBox::CSkyBox(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CGameObject(pGraphicDev)
	, m_vDir(0.f, 0.f, 0.f)
	, m_fSpeed(0.f)
{

}

CSkyBox::~CSkyBox(void)
{

}

HRESULT CSkyBox::Ready_GameObject(void)
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	m_fSpeed = 20.f;

	m_pTransformCom->Set_Scale(40.f, 40.f, 40.f);

	return S_OK;
}

_int CSkyBox::Update_GameObject(const _float& fTimeDelta)
{
	m_pRendererCom->Add_RenderGroup(Engine::RENDER_PRIORITY, this);

	return NO_EVENT;
}

_int CSkyBox::LateUpdate_GameObject(const _float & fTimeDelta)
{
	_matrix	matCamWorld;
	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matCamWorld);

	D3DXMatrixInverse(&matCamWorld, NULL, &matCamWorld);

	m_pTransformCom->Set_Pos(matCamWorld._41, matCamWorld._42 + 3.f, matCamWorld._43);

	Engine::CGameObject::Update_GameObject(fTimeDelta);

	return NO_EVENT;
}

void CSkyBox::Render_GameObject(void)
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, &m_pTransformCom->m_matWorld);

	m_pTextureCom->Render_Texture(m_iRenderId);

	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);
	m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);

	m_pBufferCom->Render_Buffer();

	m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

HRESULT CSkyBox::Add_Component(void)
{
	Engine::CComponent*		pComponent = nullptr;

	pComponent = m_pBufferCom = dynamic_cast<Engine::CCubeTex*>(Engine::Clone(RESOURCE_STATIC, L"Buffer_CubeTex"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Buffer", pComponent);
	
	pComponent = m_pTextureCom = dynamic_cast<Engine::CTexture*>(Engine::Clone(RESOURCE_STAGE, L"Texture_Cube"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Texture", pComponent);
	
	pComponent = m_pTransformCom = Engine::CTransform::Create();
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_DYNAMIC].emplace(L"Com_Transform", pComponent);

	pComponent = m_pRendererCom = Engine::Get_Renderer();
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	pComponent->AddRef();
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Renderer", pComponent);
	
	return S_OK;
}



CSkyBox* CSkyBox::Create(LPDIRECT3DDEVICE9 pGraphicDev, _int iRenderId)
{
	CSkyBox*	pInstance = new CSkyBox(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject()))
		Engine::Safe_Release(pInstance);
	pInstance->m_iRenderId = iRenderId;
	return pInstance;
}

void CSkyBox::Free(void)
{
	Engine::CGameObject::Free();
}

