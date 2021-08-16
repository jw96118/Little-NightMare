#include "stdafx.h" 
#include "TempObject.h"
#include "Export_Function.h"

CTempObject::CTempObject(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CGameObject(pGraphicDev)
{

}

CTempObject::~CTempObject(void)
{
}

HRESULT CTempObject::Ready_GameObject(void)
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	return S_OK;
}

_int CTempObject::Update_GameObject(const _float& fTimeDelta)
{
	Engine::CGameObject::Update_GameObject(fTimeDelta);

	m_pRendererCom->Add_RenderGroup(Engine::RENDER_PRIORITY, this);

	return 0;
}

void CTempObject::Render_GameObject(void)
{
	_matrix matProj;
	m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &matProj);

	m_pGraphicDev->SetTransform(D3DTS_WORLD, &m_pTransformCom->m_matWorld);
	m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, TRUE);
	//m_pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	//m_pGraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	//m_pGraphicDev->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_LESS);
	//m_pGraphicDev->SetRenderState(D3DRS_ALPHAREF,0x00000088);
	if (m_pTextureCom != nullptr)
	{
		m_pTextureCom->Render_Texture(m_iTexCount);
	}
	else
	{
		m_pGraphicDev->SetTexture(0,nullptr);
	}
	if (FAILED(SetUp_Material()))
		return;
	m_pBufferCom->Render_Buffer();

	//m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, FALSE);
	//m_pGraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	//m_pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
}



HRESULT CTempObject::Add_Component(void)
{
	Engine::CComponent*		pComponent = nullptr;

	pComponent = m_pBufferCom = dynamic_cast<Engine::CLightTex*>(Engine::Clone(RESOURCE_STATIC, L"Buffer_LightTex"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Buffer", pComponent);

	pComponent = m_pTransformCom = Engine::CTransform::Create();
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_DYNAMIC].emplace(L"Com_Transform", pComponent);

	pComponent = m_pRendererCom = Engine::Get_Renderer();
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	pComponent->AddRef();
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Renderer", pComponent);

	return S_OK;
}

HRESULT CTempObject::SetUp_Material(void)
{
	D3DMATERIAL9		tMtrlInfo;
	ZeroMemory(&tMtrlInfo, sizeof(D3DMATERIAL9));

	tMtrlInfo.Diffuse = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
	//tMtrlInfo.Specular = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
	//tMtrlInfo.Ambient = D3DXCOLOR(1.0f, 1.f, 1.f, 1.f);
	//tMtrlInfo.Emissive = D3DXCOLOR(0.f, 0.f, 0.f, 1.f);
	//tMtrlInfo.Power = 1.f;

	m_pGraphicDev->SetMaterial(&tMtrlInfo);

	return S_OK;
}




CTempObject* CTempObject::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CTempObject* pInstance = new CTempObject(pGraphicDev);
	if (FAILED(pInstance->Ready_GameObject()))
		Engine::Safe_Release(pInstance);

	
	return pInstance;
}



void CTempObject::Free(void)
{

	Engine::CGameObject::Free();

}

