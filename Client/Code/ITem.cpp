#include "stdafx.h"
#include "Item.h"
#include "Export_Function.h"
#include "CollisionMgr.h"
#include "Player.h"

CItem::CItem(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CGameObject(pGraphicDev), m_fAniCount(0.f),  m_pNowScene(nullptr)
{
	ZeroMemory(&m_tCollider, sizeof(m_tCollider));
}

CItem::~CItem(void)
{

}




HRESULT CItem::Ready_GameObject(void)
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	m_tCollider.fRadius = 0.75f;


	return S_OK;
}

HRESULT CItem::LateReady_GameObject(void)
{
	m_pTargetTransformCom = dynamic_cast<Engine::CTransform*>
		(Engine::Get_Component(L"02.GameLogic", L"Obj_Player", L"Com_Transform", Engine::ID_DYNAMIC));
	NULL_CHECK_RETURN(m_pTargetTransformCom, -1);


	return S_OK;
}

_int CItem::Update_GameObject(const _float& fTimeDelta)
{
	_vec3 vLength = *m_pTargetTransformCom->Get_Info(Engine::INFO_POS) - *m_pTransformCom->Get_Info(Engine::INFO_POS);

	if (D3DXVec3Length(&vLength) <= 1.0f)
	{
		Use_Item();
	}
	Engine::CGameObject::Update_GameObject(fTimeDelta);
	Update_Collider();


	_matrix			matBill, matWorld, matView;
	D3DXMatrixIdentity(&matBill);

	matWorld = *m_pTransformCom->Get_WorldMatrixPointer();
	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);

	matBill._11 = matView._11;
	matBill._13 = matView._13;
	matBill._31 = matView._31;
	matBill._33 = matView._33;

	D3DXMatrixInverse(&matBill, NULL, &matBill);
	m_pTransformCom->m_matWorld = matBill * matWorld;

	m_pRendererCom->Add_RenderGroup(Engine::RENDER_ALPHA, this);
	Animation(fTimeDelta);
	return NO_EVENT;
}

void CItem::Render_GameObject(void)
{
#ifdef _DEBUG
	Render_Collider();
#endif

	m_pGraphicDev->SetTransform(D3DTS_WORLD, &m_pTransformCom->m_matWorld);
	m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, TRUE);
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	m_pGraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	m_pGraphicDev->SetRenderState(D3DRS_ALPHAREF, 0x00000088);
	m_pGraphicDev->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);

	m_pTextureCom->Render_Texture(m_tFrame.fFrameCnt);
	m_pBufferCom->Render_Buffer();
	//m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, FALSE);
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

void CItem::Render_Collider()
{
	_matrix matWorld, matScale, matTrans;

	D3DXMatrixTranslation(&matTrans, m_tCollider.vCenter.x, m_tCollider.vCenter.y, m_tCollider.vCenter.z);
	D3DXMatrixScaling(&matScale, m_tCollider.fRadius, m_tCollider.fRadius, m_tCollider.fRadius);
	matWorld = matScale * matTrans;
	m_pGraphicDev->SetTransform(D3DTS_WORLD, &matWorld);
	Engine::Render_SphereMesh();
}

void CItem::Update_Collider(void)
{
	_vec3 vPos = { 0.f, 0.f, 0.f };
	m_pTransformCom->Get_Info(Engine::INFO_POS, &vPos);

	// Hit Collider
	m_tCollider.vCenter = vPos;
	m_tCollider.vCenter.y -= 0.25f;
}

void CItem::Set_Texture()
{
	Engine::CComponent*		pComponent = nullptr;
	switch (m_ItemType)
	{
	case Engine::NO_ITEM:
		break;
	case Engine::ITEM_HP:
		pComponent = m_pTextureCom = dynamic_cast<Engine::CTexture*>(Engine::Clone(RESOURCE_STAGE, L"Texture_Item_Hp"));
		NULL_CHECK(pComponent, E_FAIL);
		m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Texture", pComponent);
		m_tFrame.fFrameCnt = 0;
		m_tFrame.fMaxFrameCnt = 4.f;
		m_tFrame.fTimeScale = 1.f;
		break;
	case Engine::ITEM_MANA:
		pComponent = m_pTextureCom = dynamic_cast<Engine::CTexture*>(Engine::Clone(RESOURCE_STAGE, L"Texture_Item_Mana"));
		NULL_CHECK(pComponent, E_FAIL);
		m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Texture", pComponent);
		m_tFrame.fFrameCnt = 0;
		m_tFrame.fMaxFrameCnt = 4.f;
		m_tFrame.fTimeScale = 1.f;
		break;
	case Engine::ITEM_MINIGAME:
		pComponent = m_pTextureCom = dynamic_cast<Engine::CTexture*>(Engine::Clone(RESOURCE_STAGE, L"Texture_Item_Mini"));
		NULL_CHECK(pComponent, E_FAIL);
		m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Texture", pComponent);
		m_tFrame.fFrameCnt = 0;
		m_tFrame.fMaxFrameCnt = 7.f;
		m_tFrame.fTimeScale = 1.f;
		m_pTransformCom->m_vInfo[Engine::INFO_POS].y = 0.5f;
		break;
	case Engine::ITEM_PORTAL:
		pComponent = m_pTextureCom = dynamic_cast<Engine::CTexture*>(Engine::Clone(RESOURCE_STAGE, L"Texture_Item_Portal"));
		NULL_CHECK(pComponent, E_FAIL);
		m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Texture", pComponent);
		m_tFrame.fFrameCnt = 0;
		m_tFrame.fMaxFrameCnt = 4.f;
		m_tFrame.fTimeScale = 1.f;
		break;
	case Engine::ITEM_UPGRADE:
		pComponent = m_pTextureCom = dynamic_cast<Engine::CTexture*>(Engine::Clone(RESOURCE_STAGE, L"Texture_Item_Upgrade_Point"));
		NULL_CHECK(pComponent, E_FAIL);
		m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Texture", pComponent);
		m_tFrame.fFrameCnt = 0;
		m_tFrame.fMaxFrameCnt = 8.f;
		m_tFrame.fTimeScale = 1.f;
		break;

	case Engine::ITEM_AMMO:
		pComponent = m_pTextureCom = dynamic_cast<Engine::CTexture*>(Engine::Clone(RESOURCE_STAGE, L"Texture_Item_Pistol_Ammo_Box"));
		NULL_CHECK(pComponent, E_FAIL);
		m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Texture", pComponent);
		m_tFrame.fFrameCnt = 0;
		m_tFrame.fMaxFrameCnt = 0.f;
		m_tFrame.fTimeScale = 1.f;
		break;
	case Engine::ITEM_AMMO_MINIGUN:
		pComponent = m_pTextureCom = dynamic_cast<Engine::CTexture*>(Engine::Clone(RESOURCE_STAGE, L"Texture_Item_MiniGun_Ammo_Box"));
		NULL_CHECK(pComponent, E_FAIL);
		m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Texture", pComponent);
		m_tFrame.fFrameCnt = 0;
		m_tFrame.fMaxFrameCnt = 0.f;
		m_tFrame.fTimeScale = 1.f;
		break;
	case Engine::ITEM_AMMO_ARROW:
		pComponent = m_pTextureCom = dynamic_cast<Engine::CTexture*>(Engine::Clone(RESOURCE_STAGE, L"Texture_Item_Arrow_Ammo_Box"));
		NULL_CHECK(pComponent, E_FAIL);
		m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Texture", pComponent);
		m_tFrame.fFrameCnt = 0;
		m_tFrame.fMaxFrameCnt = 0.f;
		m_tFrame.fTimeScale = 1.f;
		break;
	case Engine::ITEM_AMMO_ROCKET:
		pComponent = m_pTextureCom = dynamic_cast<Engine::CTexture*>(Engine::Clone(RESOURCE_STAGE, L"Texture_Item_Rocket_Ammo_Box"));
		NULL_CHECK(pComponent, E_FAIL);
		m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Texture", pComponent);
		m_tFrame.fFrameCnt = 0;
		m_tFrame.fMaxFrameCnt = 0.f;
		m_tFrame.fTimeScale = 1.f;
		break;
	case Engine::ITEM_END:
		break;
	default:
		break;
	}
}

void CItem::Set_Pos(_vec3 pos)
{
	m_pTransformCom->m_vInfo[Engine::INFO_POS] = pos;
}

void CItem::Set_Type(Engine::ITEMTYPE type)
{
	m_ItemType = type;
}

HRESULT CItem::Add_Component(void)
{
	Engine::CComponent*		pComponent = nullptr;

	pComponent = m_pTransformCom = Engine::CTransform::Create();
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_DYNAMIC].emplace(L"Com_Transform", pComponent);

	/* For Debug */
	pComponent = m_pBufferCom = dynamic_cast<Engine::CLightTex*>(Engine::Clone(RESOURCE_STATIC, L"Buffer_LightTex"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Buffer", pComponent);

	//pComponent = m_pTextureCom = dynamic_cast<Engine::CTexture*>(Engine::Clone(RESOURCE_STAGE, L"Texture_Player"));
	//NULL_CHECK_RETURN(pComponent, E_FAIL);
	//m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Texture", pComponent);

	pComponent = m_pRendererCom = Engine::Get_Renderer();
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	pComponent->AddRef();
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Renderer", pComponent);

	return S_OK;
}

void CItem::Animation(const _float & fTimeDelta)
{
	m_tFrame.fFrameCnt += m_tFrame.fMaxFrameCnt * fTimeDelta * m_tFrame.fTimeScale;

	if (m_tFrame.fMaxFrameCnt <= m_tFrame.fFrameCnt)
	{
		m_tFrame.fFrameCnt = 0.f;
	}
	//switch (m_ItemType)
	//{
	//case Engine::NO_ITEM:
	//	break;
	//case Engine::ITEM_HP:
	//	if (m_tFrame.fMaxFrameCnt <= m_tFrame.fFrameCnt)
	//	{
	//		m_tFrame.fFrameCnt = 0.f;
	//	}
	//	break;
	//case Engine::ITEM_MANA:
	//	if (m_tFrame.fMaxFrameCnt <= m_tFrame.fFrameCnt)
	//	{
	//		m_tFrame.fFrameCnt = 0.f;
	//	}
	//	break;
	//case Engine::ITEM_MINIGAME:
	//	if (m_tFrame.fMaxFrameCnt <= m_tFrame.fFrameCnt)
	//	{
	//		m_tFrame.fFrameCnt = 0.f;
	//	}
	//	break;
	//case Engine::ITEM_PORTAL:
	//	if (m_tFrame.fMaxFrameCnt <= m_tFrame.fFrameCnt)
	//	{
	//		m_tFrame.fFrameCnt = 0.f;
	//	}
	//	break;
	//case Engine::ITEM_END:
	//	break;
	//default:
	//	break;
	//}
}

void CItem::Use_Item()
{
	bool temp = false;
	CPlayer* pPlayer = dynamic_cast<CPlayer*>(Engine::Get_GameObject(L"02.GameLogic", L"Obj_Player"));

	switch (m_ItemType)
	{
	case Engine::NO_ITEM:
		break;
	case Engine::ITEM_HP:
	
		break;
	case Engine::ITEM_MANA:
	

		break;
	case Engine::ITEM_MINIGAME:
		
		break;
	case Engine::ITEM_AMMO:
	
		break;
	case Engine::ITEM_UPGRADE:
		pPlayer = dynamic_cast<CPlayer*>(Engine::Get_GameObject(L"02.GameLogic", L"Obj_Player"));
		break;
	case Engine::ITEM_PORTAL:
		m_pNowScene->Set_SceneChange(true);
		break;
	case Engine::ITEM_AMMO_MINIGUN:
		
		break;
	case Engine::ITEM_AMMO_ARROW:
		
		break;
	case Engine::ITEM_AMMO_ROCKET:
	
		break;


	case Engine::ITEM_END:
		break;
	default:
		break;
	}
	Set_Enable(false);

}


CItem* CItem::Create(LPDIRECT3DDEVICE9 pGraphicDev, Engine::CScene* pNowScene)
{
	CItem*	pInstance = new CItem(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject()))
		Engine::Safe_Release(pInstance);

	pInstance->m_pNowScene = pNowScene;
	return pInstance;
}

void CItem::Free(void)
{
	Engine::CGameObject::Free();
}