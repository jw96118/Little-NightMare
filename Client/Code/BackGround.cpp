#include "stdafx.h"
#include "BackGround.h"

#include "Export_Function.h"

CBackGround::CBackGround(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CGameObject(pGraphicDev)
	, m_pBufferCom(nullptr)
	, m_pTextureCom(nullptr)
	, m_pTransformCom(nullptr)
	, m_pRendererCom(nullptr)
	, m_fFrameCnt(0.f)
	, m_fFrameMax(0.f)
{

}

CBackGround::~CBackGround(void)
{

}

HRESULT CBackGround::Ready_GameObject(void)
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	//m_pTransformCom->m_vScale.x = 1.f;
	//m_pTransformCom->m_vScale.y = 1.f;

	m_fFrameMax = (_float)m_pTextureCom->Get_TextureFrameSize();

	const D3DXIMAGE_INFO& tImageInfo = m_pTextureCom->Get_ImageInfo();
	m_fWidth = (_float)tImageInfo.Width;
	m_fHeight = (_float)tImageInfo.Height;

	return S_OK;
}

_int CBackGround::Update_GameObject(const _float& fTimeDelta)
{
	m_fFrameCnt += m_fFrameMax * fTimeDelta * 0.5f;

	if (m_fFrameCnt >= m_fFrameMax)
		m_fFrameCnt = 0.f;

	Engine::CGameObject::Update_GameObject(fTimeDelta);	

	m_pRendererCom->Add_RenderGroup(Engine::RENDER_UI, this);

	return NO_EVENT;
}

void CBackGround::Render_GameObject(void)
{
	_matrix matView;
	D3DXMatrixScaling(&matView, m_fWidth, m_fHeight, 1.f);

	m_pGraphicDev->SetTransform(D3DTS_WORLD, &m_pTransformCom->m_matWorld);
	m_pGraphicDev->SetTransform(D3DTS_VIEW, &matView);
	
	m_pTextureCom->Render_Texture(0/*(_uint)m_fFrameCnt*/);
	m_pBufferCom->Render_Buffer();
}

HRESULT CBackGround::Add_Component(void)
{
	Engine::CComponent*	pComponent = nullptr;

	pComponent = m_pBufferCom = dynamic_cast<Engine::CRcTex*>(Engine::Clone(RESOURCE_STATIC, L"Buffer_Buffer_RcTex"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Buffer", pComponent);

	pComponent = m_pBufferLightCom = dynamic_cast<Engine::CLightTex*>(Engine::Clone(RESOURCE_STATIC, L"Buffer_LightTex"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Buffer", pComponent);

	pComponent = m_pTransformCom = Engine::CTransform::Create();
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_DYNAMIC].emplace(L"Com_Transform", pComponent);

	pComponent = m_pTextureCom = dynamic_cast<Engine::CTexture*>(Engine::Clone(RESOURCE_LOGO, L"Texture_MenuBG"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Texture", pComponent);

	pComponent = m_pRendererCom = Engine::Get_Renderer();
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	pComponent->AddRef();
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Renderer", pComponent);
	
	return S_OK;
}

CBackGround* CBackGround::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CBackGround*	pInstance = new CBackGround(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject()))
		Engine::Safe_Release(pInstance);

	return pInstance;
}

void CBackGround::Free(void)
{
	Engine::CGameObject::Free();
}

