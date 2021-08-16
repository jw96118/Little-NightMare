#include "stdafx.h"
#include "ColObject.h"
#include "Collider.h"
#include "Export_Function.h"

CColObject::CColObject(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CGameObject(pGraphicDev)
{
}

CColObject::~CColObject(void)
{
}




HRESULT CColObject::Ready_GameObject(void)
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	return S_OK;
}

HRESULT CColObject::LateReady_GameObject(void)
{

	return S_OK;
}

_int CColObject::Update_GameObject(const _float& fTimeDelta)
{
	Engine::CGameObject::Update_GameObject(fTimeDelta);

	//m_pRendererCom->Add_RenderGroup(Engine::RENDER_NONALPHA, this);

	return 0;
}

void CColObject::Render_GameObject(void)
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, &m_pTransformCom->m_matWorld);
	Engine::Render_BoxMesh();
	_matrix		matWorld;
	m_pTransformCom->Get_WorldMatrix(&matWorld);
	if (m_vecColliderCom.size() > 0)
	{
		m_vecColliderCom[0]->Render_Collider(m_eColType, &matWorld);
	}
}

HRESULT CColObject::Add_Component(void)
{
	Engine::CComponent*		pComponent = nullptr;

	pComponent = m_pTransformCom = Engine::CTransform::Create();
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_DYNAMIC].emplace(L"Com_Transform", pComponent);

	pComponent = m_pRendererCom = Engine::Get_Renderer();
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	pComponent->AddRef();
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Renderer", pComponent);

	return S_OK;
}

CColObject* CColObject::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CColObject* pInstance = new CColObject(pGraphicDev);
	if (FAILED(pInstance->Ready_GameObject()))
		Engine::Safe_Release(pInstance);
	return pInstance;
}
void CColObject::Set_TestCol()
{
	Engine::CComponent*		pComponent = nullptr;
	m_vecColliderCom.clear();
	pComponent = Engine::CCollider::Create(m_pGraphicDev,
		m_pTransformCom->Get_Info(Engine::INFO_POS),
		0.f,
		0.f, true);
	m_vecColliderCom.push_back(dynamic_cast<Engine::CCollider*>(pComponent));
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Collider", pComponent);

}

void  CColObject::SetColType(Engine::COLLTYPE colType)
{
	m_eColType = colType;
}
Engine::CCollider* CColObject::Get_BoxCollider()
{
	return m_vecColliderCom[0];
}
void CColObject::Free(void)
{
	Engine::CGameObject::Free();
}

