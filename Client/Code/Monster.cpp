#include "stdafx.h"
#include "Monster.h"

#include "Export_Function.h"

CMonster::CMonster(LPDIRECT3DDEVICE9 pGraphicDev, const _tchar* pLayerTag, wstring wstrObjName)
	: Engine::CGameObject(pGraphicDev),
	m_pLayerTag(pLayerTag),
	m_wstrObjName(wstrObjName)
{

}

CMonster::~CMonster(void)
{

}



void CMonster::Get_Damage(_float fDamage)
{
	m_tData.fCurHP -= fDamage;
	if (m_tData.fCurHP <= 0.f)
		m_tData.fCurHP = 0.f;
}

HRESULT CMonster::Ready_GameObject(void)
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	return S_OK;
}

_int CMonster::Update_GameObject(const _float& fTimeDelta)
{
	Engine::CGameObject::Update_GameObject(fTimeDelta);

	_matrix			matBill, matWorld, matView;

	m_pRendererCom->Add_RenderGroup(Engine::RENDER_NONALPHA, this);

	return NO_EVENT;
}

void CMonster::Render_GameObject(void)
{
}

HRESULT CMonster::LateReady_GameObject(void)
{
	return S_OK;
}

HRESULT CMonster::Add_Component(void)
{
	Engine::CComponent*		pComponent = nullptr;

	//pComponent = m_pBufferCom = dynamic_cast<Engine::CLightTex*>(Engine::Clone(RESOURCE_STATIC, L"Buffer_LightTex"));
	//NULL_CHECK_RETURN(pComponent, E_FAIL);
	//m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Buffer", pComponent);

	//pComponent = m_pTransformCom = Engine::CTransform::Create();
	//NULL_CHECK_RETURN(pComponent, E_FAIL);
	//m_mapComponent[Engine::ID_DYNAMIC].emplace(L"Com_Transform", pComponent);

	//pComponent = m_pRendererCom = Engine::Get_Renderer();
	//NULL_CHECK_RETURN(pComponent, E_FAIL);
	//pComponent->AddRef();
	//m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Renderer", pComponent);

	return S_OK;
}


void CMonster::Free(void)
{
	Engine::CGameObject::Free();
}



void CMonster::SetUp_OnTerrain(void)
{
	_vec3		vPos;
	m_pTransformCom->Get_Info(Engine::INFO_POS, &vPos);

	Engine::CTerrainTex*		pTerrainBufferCom = dynamic_cast<Engine::CTerrainTex*>
		(Engine::Get_Component(L"02.GameLogic", L"Terrain", L"Com_Buffer", Engine::ID_STATIC));
	NULL_CHECK(pTerrainBufferCom);

	_float	fHeight = m_pCalculatorCom->Compute_HeightOnTerrain(&vPos,
		pTerrainBufferCom->Get_VtxPos(),
		pTerrainBufferCom->Get_VtxCntX(),
		pTerrainBufferCom->Get_VtxCntZ());

	m_pTransformCom->Set_Pos(vPos.x, fHeight + 4.f, vPos.z);
}

void CMonster::Set_Pos(_float fX, _float fY, _float fZ)
{
	m_pTransformCom->m_vInfo[Engine::INFO_POS] = _vec3(fX, fY, fZ);
}

void CMonster::SetScale(_float fScale)
{
	m_pTransformCom->m_matWorld._11 *= fScale;
	m_pTransformCom->m_matWorld._22 *= fScale;
	m_pTransformCom->m_matWorld._33 *= fScale;

}


