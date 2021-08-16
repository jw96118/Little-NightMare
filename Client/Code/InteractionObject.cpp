#include "stdafx.h"
#include "InteractionObject.h"
#include "Collider.h"
#include "Export_Function.h"
#include "Player.h"
#include "Janitor.h"
#include <iostream>
using namespace std;
CInteractionObject::CInteractionObject(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CGameObject(pGraphicDev)
{

}

CInteractionObject::~CInteractionObject(void)
{
}

HRESULT CInteractionObject::Ready_GameObject(void)
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);
	return S_OK;
}

HRESULT CInteractionObject::LateReady_GameObject(void)
{
	m_pPlayerObj = dynamic_cast<CPlayer*>(Engine::Get_GameObject(L"02.GameLogic", L"Dyn_Player"));
	switch (m_eObjType)
	{
	case HATCH:
		break;
	case WOODFLOOR:
		m_fAngle = m_pTransformCom->m_vAngle.y;
		break;
	case PANELDOOR:
		m_fAngle = m_pTransformCom->m_vAngle.z;
		break;
	case LEVER:
		m_pTransformCom->Set_Angle(_vec3(D3DXToRadian(-45.f), D3DXToRadian(0.f), D3DXToRadian(0.f)));
		m_fAngle = D3DXToRadian(-45.f);
		m_bLeverReady = true;
		break;
	case SHIPDOOR:
		m_fShipDoorY = m_pTransformCom->Get_Info(Engine::INFO_POS)->y;
		break;
	case SPAWNDOOR:
		//m_bSpawnDoorAniFlag = true;
		break;
	case LASTDOOR:
		break;
	case CAGE:
		m_iAnimation = 5;
		break;
	case CAGE_LEFT_OUT:
	case CAGE_RIGHT_OUT:
		m_bEndAni = false;
		m_bNotDraw = true;
		m_pTransformCom->Set_Angle(_vec3(D3DXToRadian(0.f), D3DXToRadian(180.f), D3DXToRadian(0.f)));
		break;

	}
	m_vFirstPos = *m_pTransformCom->Get_Info(Engine::INFO_POS);
	m_vFirstAngle = m_pTransformCom->m_vAngle;
	return S_OK;
}

_int CInteractionObject::Update_GameObject(const _float& fTimeDelta)
{
	if (g_bLoadFirst)
		Set_SavePoint();
	Engine::CGameObject::Update_GameObject(fTimeDelta);

	Mesh_Animation(fTimeDelta);


	m_pRendererCom->Add_RenderGroup(Engine::RENDER_NONALPHA, this);

	return 0;
}

void CInteractionObject::Render_GameObject(void)
{


	/*_matrix		matWorld;
	m_pTransformCom->Get_WorldMatrix(&matWorld);

	m_pColliderCom->Render_Collider(Engine::COLLTYPE(m_bColl), &matWorld);*/

	LPD3DXEFFECT	pEffect = nullptr;
	pEffect = m_pShaderCom->Get_EffectHandle();
	NULL_CHECK(pEffect);
	pEffect->AddRef();


	FAILED_CHECK_RETURN(SetUp_ConstantTable(pEffect), );

	_uint	iPassMax;
	iPassMax = 0;

	pEffect->Begin(&iPassMax, 0);

	pEffect->BeginPass(0);
	if (m_pStaticMeshCom != nullptr)
	{
		if (!m_bNotDraw)
			m_pStaticMeshCom->Render_Meshes(pEffect);
	}
	else if (m_pDynamicMeshCom != nullptr)
	{
		m_pDynamicMeshCom->Play_Animation(0.f);
		if (!m_bNotDraw)
			m_pDynamicMeshCom->Render_Meshes(pEffect);
	}
	pEffect->EndPass();

	pEffect->End();

	Safe_Release(pEffect);

	//Render_Mesh();
}


HRESULT CInteractionObject::Load_GameObject(void)
{
	Engine::CComponent*		pComponent = nullptr;

	return S_OK;
}

Engine::CCollider * CInteractionObject::Get_BoxCollider()
{
	return m_vecColliderCom[0];
}

const _bool & CInteractionObject::Get_ReadyLever() const
{
	// TODO: 여기에 반환 구문을 삽입합니다.
	return m_bLeverReady;
}

const _bool & CInteractionObject::Get_CollWall() const
{
	// TODO: 여기에 반환 구문을 삽입합니다.
	return m_bCollWall;
}

const _bool & CInteractionObject::Get_NotRender() const
{
	// TODO: 여기에 반환 구문을 삽입합니다.
	return m_bNotDraw;
}

void CInteractionObject::Set_CollWall(const _bool & flag)
{
	m_bCollWall = flag;
}

void CInteractionObject::Set_NotRender(const _bool & flag)
{
	m_bNotDraw = flag;
}

void CInteractionObject::Set_CageState(CAGESTATE eState)
{
	m_eCageState = eState;
}

CAGESTATE CInteractionObject::Get_CageState()
{
	return m_eCageState;
}



const float & CInteractionObject::Get_fDist() const
{
	// TODO: 여기에 반환 구문을 삽입합니다.
	return m_fDist;
}

HRESULT CInteractionObject::Add_Component(void)
{

	return S_OK;
}

void CInteractionObject::SetTestCol()
{
	Engine::CComponent*		pComponent = nullptr;
	if (m_pStaticMeshCom != nullptr)
	{
		pComponent = Engine::CCollider::Create(m_pGraphicDev,
			m_pStaticMeshCom->Get_VtxPos(),
			m_pStaticMeshCom->Get_NumVtx(),
			m_pStaticMeshCom->Get_Stride());
		m_vecColliderCom.push_back(dynamic_cast<Engine::CCollider*>(pComponent));
		m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Collider", pComponent);
	}
	else
	{
		pComponent = Engine::CCollider::Create(m_pGraphicDev,
			m_pTransformCom->Get_Info(Engine::INFO_POS),
			0.f,
			0.f, true);
		m_vecColliderCom.push_back(dynamic_cast<Engine::CCollider*>(pComponent));
		m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Collider", pComponent);
	}
	pComponent = m_pShaderCom = dynamic_cast<Engine::CShader*>(Engine::Clone_Prototype(L"Shader_Mesh"));
	NULL_CHECK(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Shader", pComponent);
}

void CInteractionObject::SetTexCount(int count)
{
	m_iTexCount = count;
}

void CInteractionObject::SetbPick(_bool flag)
{
	m_bPick = flag;
}



CInteractionObject * CInteractionObject::Create(LPDIRECT3DDEVICE9 pGraphicDev, Engine::MESHTYPE eMeshType, OBJTYPE  eObjType)
{
	CInteractionObject* pInstance = new CInteractionObject(pGraphicDev);
	pInstance->m_eMeshType = eMeshType;
	pInstance->m_eObjType = eObjType;
	if (FAILED(pInstance->Load_GameObject()))
		Engine::Safe_Release(pInstance);

	return pInstance;
}


const Engine::MESHTYPE & CInteractionObject::GetMeshType() const
{
	// TODO: 여기에 반환 구문을 삽입합니다.
	return m_eMeshType;
}

HRESULT CInteractionObject::SetUp_ConstantTable(LPD3DXEFFECT& pEffect)
{
	_matrix			matWorld, matView, matProj;

	m_pTransformCom->Get_WorldMatrix(&matWorld);
	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
	m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &matProj);

	pEffect->SetMatrix("g_matWorld", &matWorld);
	pEffect->SetMatrix("g_matView", &matView);
	pEffect->SetMatrix("g_matProj", &matProj);

	return S_OK;
}

void CInteractionObject::Mesh_Animation(const _float& fdeltaTime)
{
	switch (m_eObjType)
	{
	case HATCH:
		if (!m_pPlayerObj->Get_HatchOpen())
		{
			if (!m_pPlayerObj->Get_PullHatch())
			{
				m_fAngle += D3DXToRadian(2.f);
				if (m_fAngle > D3DXToRadian(0.f))
					m_fAngle = D3DXToRadian(0.f);
			}
			if (!m_pPlayerObj->Get_HatchOpenAni())
			{
				m_fAngle = D3DXToRadian(-35.f * m_pPlayerObj->Get_HatchCount());
				if (m_fAngle < D3DXToRadian(-20.f))
					m_fAngle = D3DXToRadian(-20.f);
			}
			else
			{
				m_fAngle -= D3DXToRadian(5.f);
				if (m_fAngle < D3DXToRadian(-150.f))
					m_fAngle = D3DXToRadian(-150.f);
			}
			m_pTransformCom->Set_Angle(_vec3(0.f, D3DXToRadian(180.f), m_fAngle));
		}
		break;
	case WOODFLOOR:
		if (m_pPlayerObj->Get_WoodFloor())
		{

			if (D3DXToDegree(m_pTransformCom->m_vAngle.x) > 0)
			{
				m_fAngle -= D3DXToRadian(200.f * fdeltaTime);
				if (m_fAngle < D3DXToRadian(-180.f))
					m_fAngle = D3DXToRadian(-180.f);
			}
			else
			{
				m_fAngle += D3DXToRadian(200.f * fdeltaTime);
				if (m_fAngle > D3DXToRadian(180.f))
					m_fAngle = D3DXToRadian(180.f);
			}
			m_pTransformCom->m_vAngle.y = m_fAngle;
		}
		break;
	case PANELDOOR:
		if (m_pPlayerObj->Get_DoorPush())
		{
			m_fAngle -= D3DXToRadian(100.f * fdeltaTime);
			if (m_fAngle < D3DXToRadian(-15.f))
			{
				Engine::Play_Sound(L"SW_PullPlank_Land_1a.ogg", Engine::CHANNELID::EFFECT, m_bPanelDoorSound, false);
				m_fAngle = D3DXToRadian(-15.f);
			}
			m_pTransformCom->m_vAngle.z = m_fAngle;
		}
		break;
	case LEVER:
	{
		_vec3 vPPos = *m_pPlayerObj->GetTransformCom()->Get_Info(Engine::INFO_POS);
		_float fVolume = 1.f - abs(D3DXVec3Length(&(vPPos - *m_pTransformCom->Get_Info(Engine::INFO_POS)))) *0.08f;
		if (fVolume < 0.f)
			fVolume = 0.f;

		if (m_pPlayerObj->Get_LeverPush())
		{
			m_bLeverSoundEnd = false;
			Engine::Play_Sound(L"SW_big_lever_pull_loop_2.ogg", Engine::CHANNELID::LEVER, m_bLeverSound, true);
			m_bLeverReady = false;
			Engine::Set_Volume(Engine::CHANNELID::LEVER, fVolume);

			_vec3 PlayerHand = m_pPlayerObj->Get_Player_Hand();

			if (m_pPlayerObj->Get_AniTime() > 0.1f)
				m_iLeverFlag = true;
			_vec3 vPos = *m_pTransformCom->Get_Info(Engine::INFO_POS);
			PlayerHand.x = vPos.x = 0.f;
			_vec3 vPlayerHandDir = vPos - PlayerHand;
			D3DXVec3Normalize(&vPlayerHandDir, &vPlayerHandDir);

			if (!m_iLeverFlag || m_iLeverCount == 0)
				m_vPreDir = vPlayerHandDir;
			else
			{
				m_fAngle += acosf(D3DXVec3Dot(&vPlayerHandDir, &m_vPreDir));
				m_vPreDir = vPlayerHandDir;
				m_pTransformCom->Set_Angle(_vec3(m_fAngle, 0.f, 0.f));
			}
			m_iLeverCount++;
		}
		else
		{
			m_iLeverFlag = false;
			m_fAngle -= D3DXToRadian(10.f * fdeltaTime);
			m_iLeverCount = 0;
			if (D3DXToDegree(m_fAngle) <= -45.f)
			{
				if (m_bLeverSound)
				{
					Engine::Stop_Sound(Engine::CHANNELID::LEVER);
					Engine::Play_Sound(L"SW_big_lever_end_latch.ogg", Engine::CHANNELID::LEVER, m_bLeverSoundEnd, false);
				}
				m_bLeverSound = false;
				m_fAngle = D3DXToRadian(-45.f);
				m_bLeverReady = true;
			}

			m_pTransformCom->Set_Angle(_vec3(m_fAngle, 0.f, 0.f));

		}
	}
	break;
	case SHIPDOOR:
	{
		CInteractionObject*	m_pLever = dynamic_cast<CInteractionObject*>(Engine::Get_GameObject(L"02.GameLogic", L"Sta_Obj_BigLever_8"));
		_float fY = m_fShipDoorY + (D3DXToDegree(m_pLever->Get_Angle()) + 45.f) * 0.05f;
		if ((D3DXToDegree(m_pLever->Get_Angle()) + 45.f) > 0)
		{
			Engine::Play_Sound(L"SW_submarine_door_loop_1.ogg", Engine::CHANNELID::SHIP_DOOR, m_bShipSound, true);
			m_bShipSoundEnd = false;
		}
		else if ((D3DXToDegree(m_pLever->Get_Angle()) + 45.f) == 0)
		{
			if (m_bShipSound)
			{
				Engine::Stop_Sound(Engine::CHANNELID::SHIP_DOOR);
				Engine::Play_Sound(L"SW_Door_ShipDoor_Close_Start.ogg", Engine::CHANNELID::SHIP_DOOR, m_bShipSoundEnd, false);
			}
			m_bShipSound = false;
		}

		_vec3 tempPos = *m_pTransformCom->Get_Info(Engine::INFO_POS);
		tempPos.y = fY;
		m_pTransformCom->Set_Pos(tempPos);
	}
	break;

	case BOX:
		if (m_pPlayerObj->Get_BoxMove() && m_pPlayerObj->Get_NaviFlag())
		{
			if (m_pPlayerObj->Get_PreDir() != _vec3(0.f, 0.f, 0.f))
				Engine::Play_Sound(L"SW_WoodenChair_Slide_LP.ogg", Engine::CHANNELID::BOX, m_bBoxSound, true);
			else
			{
				m_bBoxSound = false;
				Engine::Stop_Sound(Engine::CHANNELID::BOX);
			}
			m_pTransformCom->Set_Pos(*m_pTransformCom->Get_Info(Engine::INFO_POS) + m_pPlayerObj->Get_PreDir());
		}
		else
		{
			m_bBoxSound = false;
			Engine::Stop_Sound(Engine::CHANNELID::BOX);
		}
		break;
	case SPAWNDOOR:
		if (m_pPlayerObj->Get_LastStage())
		{
			if (!m_bSpawnDoorLastFlag)
			{
				m_bSpawnDoorLastFlag = true;
				m_bSpawnDoorAniFlag = true;
				m_pDynamicMeshCom->Set_AnimationSet(4, 0.25);
				m_bAniSpeed = 4.f;
			}
		}
		else if (m_pPlayerObj->Get_SpawnJionterBedRoom())
		{
			CJanitor* pBoss = dynamic_cast<CJanitor*>(Engine::Get_GameObject(L"02.GameLogic", L"Dyn_Monster_Mesh_Janitor_6"));
			if (pBoss->Get_AniTime() > 0.045f)
			{
				if (!m_bSpawnDoorFlag)
				{
					m_bSpawnDoorFlag = true;
					m_bSpawnDoorAniFlag = true;
					m_pDynamicMeshCom->Set_AnimationSet(3, 0.25);
				}
			}
		}
		if (m_bSpawnDoorAniFlag)
		{
			_float tempPercent = m_pDynamicMeshCom->Get_AniPercent();
			if (tempPercent > 0.01f)
			{
				Engine::Play_Sound(L"SW_wooden_door_handle_down_1a.ogg", Engine::CHANNELID::SPAWN_DOOR, m_bSpawnDoorhandleSound, false);
			}
			if (tempPercent > 0.1f)
			{
				Engine::Play_Sound(L"SW_wooden_door_open_creak_1a.ogg", Engine::CHANNELID::SPAWN_DOOR, m_bSpawnDoorOpenSound, false);
			}
			if (tempPercent > 0.7f)
			{
				Engine::Play_Sound(L"SW_wooden_door_close_creak_1a.ogg", Engine::CHANNELID::SPAWN_DOOR, m_bSpawnDoorCloseSound, false);
			}
			if (true == m_pDynamicMeshCom->Is_AnimationSetEnd() && tempPercent < 1.f)
			{
				
				Engine::Play_Sound(L"SW_wooden_door_close_1a.ogg", Engine::CHANNELID::SPAWN_DOOR, m_bSpawnDoorCloseEndSound, false);
				m_bSpawnDoorAniFlag = false;
				m_bSpawnDoorhandleSound = false;
				m_bSpawnDoorOpenSound = false;
				m_bSpawnDoorCloseSound = false;
				m_bSpawnDoorCloseEndSound = false;

			}
			m_pDynamicMeshCom->Play_Animation(fdeltaTime * m_bAniSpeed);
		}
		break;
	case LASTDOOR:
	{
		CInteractionObject* pCage = dynamic_cast<CInteractionObject*>(Engine::Get_GameObject(L"02.GameLogic", L"Dyn_Obj_BossCage_13"));
		const Engine::D3DXFRAME_DERIVED* pBone = pCage->m_pDynamicMeshCom->Get_FrameByName("TopTop");
		_matrix boneMat = pBone->CombinedTransformationMatrix * *pCage->m_pTransformCom->Get_WorldMatrixPointer();
		_vec3 vPos = *m_pTransformCom->Get_Info(Engine::INFO_POS);
		vPos.y = boneMat._42;
		m_pTransformCom->Set_Pos(vPos);
	}
	break;
	case CAGE:
	{
		_vec3 vPPos = *m_pPlayerObj->GetTransformCom()->Get_Info(Engine::INFO_POS);
		_float fVolume = 1.f - abs(D3DXVec3Length(&(vPPos - *m_pTransformCom->Get_Info(Engine::INFO_POS)))) *0.1f;
		//cout << fVolume << endl;
		if (fVolume < 0.f)
			fVolume = 0.f;
		
		switch (m_eCageState)
		{
		case crush1:
			if (!m_bCageSound[m_eCageState])
				Engine::Stop_Sound(Engine::CHANNELID::CAGE);
			Engine::Play_Sound(L"SW_CageDoorCrush_1stCrush.ogg", Engine::CHANNELID::CAGE, m_bCageSound[m_eCageState], false);
			break;
		case crush2:
			if (!m_bCageSound[m_eCageState])
				Engine::Stop_Sound(Engine::CHANNELID::CAGE);
			Engine::Play_Sound(L"SW_CageDoorCrush_2ndCrush.ogg", Engine::CHANNELID::CAGE, m_bCageSound[m_eCageState], false);
			break;
		case crush3:
		
			if (!m_bCageSound[m_eCageState])
			{
				Engine::Stop_All();
				Engine::Stop_Sound(Engine::CHANNELID::CAGE);
			}
			Engine::Play_Sound(L"SW_CageDoorCrush_3rdCrush.ogg", Engine::CHANNELID::CAGE, m_bCageSound[m_eCageState], false);
			Engine::Play_Sound(L"SW_Janitor_scream_distant_2_a.ogg", Engine::CHANNELID::MONSTER, m_bMonsterScream, false);
			Engine::Play_Sound(L"SW_Janitor_ChopArm.ogg", Engine::CHANNELID::MONSTER_HEART, m_bMonsterChase, false);
			break;
		case wobble1:
			Engine::Play_Sound(L"SW_CageDoorCrush_PullStress_LP.ogg", Engine::CHANNELID::CAGE, m_bCageSound[m_eCageState], true);
			Engine::Set_Volume(Engine::CHANNELID::CAGE, fVolume);

			break;
		case wobble2:
			Engine::Play_Sound(L"SW_CageDoorCrush_PullStress_LP.ogg", Engine::CHANNELID::CAGE, m_bCageSound[m_eCageState], true);
			break;
		}
		Engine::Set_Volume(Engine::CHANNELID::CAGE, fVolume);
		switch (m_eCageState)
		{
		case crush1:
			m_iAnimation = 0;
			break;
		case crush2:
		{
			m_iAnimation = 1;
			_float tempPercent = m_pDynamicMeshCom->Get_AniPercent();
			if (true == m_pDynamicMeshCom->Is_AnimationSetEnd() && tempPercent < 1.f)
				m_eCageState = wobble2;
		}
		break;
		case crush3:
		{
			m_iAnimation = 2;
			_float tempPercent = m_pDynamicMeshCom->Get_AniPercent();
			if (true == m_pDynamicMeshCom->Is_AnimationSetEnd() && tempPercent < 1.f)
				m_bEndAni = true;
		}
		break;
		case wobble1:
			m_iAnimation = 5;
			break;
		case wobble2:
			m_iAnimation = 6;
			break;
		}
		m_pDynamicMeshCom->Set_AnimationSet(m_iAnimation, 0.25);
		if (!m_bEndAni)
			m_pDynamicMeshCom->Play_Animation(fdeltaTime);
	}
		break;
	case CAGE_LEFT_OUT:
	case CAGE_RIGHT_OUT:
		if (!m_bNotDraw)
		{
			if (!m_bEndAni)
			{
				_float tempPersent = m_pDynamicMeshCom->Get_AniPercent();
				if (tempPersent < 0.9f)
					Engine::Play_Sound(L"SW_CageDoorCrush_BarSnap_1.ogg", Engine::CHANNELID::CAGE, m_bCageBarSound[0], false);
				else
					Engine::Play_Sound(L"SW_CageDoorCrush_PipeLand_1.ogg", Engine::CHANNELID::CAGE, m_bCageBarSound[1], false);
				m_pDynamicMeshCom->Set_AnimationSet(0, 0.25);
				if (m_pDynamicMeshCom->Is_AnimationSetEnd() == true)
					m_bEndAni = true;
				m_pDynamicMeshCom->Play_Animation(fdeltaTime * 2.f);
			}
		}
		break;
	}

}

void CInteractionObject::Set_SavePoint()
{
	switch (m_eObjType)
	{
	case WOODFLOOR:
		if (D3DXToDegree(m_pTransformCom->m_vAngle.x) > 0)
			m_pTransformCom->m_vAngle.y = D3DXToRadian(-90.f);
		else
			m_pTransformCom->m_vAngle.y = D3DXToRadian(90.f);
		m_bWoodFloorSound = false;
		m_fAngle = m_pTransformCom->m_vAngle.y;
		break;
	case BOX:
		m_pTransformCom->Set_Pos(m_vFirstPos);
		m_pTransformCom->Set_Angle(m_vFirstAngle);
		break;
	case SPAWNDOOR:
		m_bSpawnDoorAniFlag = false;
		m_bSpawnDoorFlag = false;
		m_bSpawnDoorLastFlag = false;
		m_bSpawnDoorhandleSound = false;
		m_bSpawnDoorOpenSound = false;
		m_bSpawnDoorCloseSound = false;

		m_pDynamicMeshCom->Reset_Animation();
		break;
	case LASTDOOR:
		break;
	case CAGE:
		m_eCageState = wobble1;
		Engine::Stop_Sound(Engine::CHANNELID::CAGE);
		for (int i = 0; i < CAGE_END; i++)
			m_bCageSound[i] = false;
		break;
	case CAGE_LEFT:
	case CAGE_RIGHT:
		m_bNotDraw = false;
		break;
	case CAGE_LEFT_OUT:
	case CAGE_RIGHT_OUT:
		m_bNotDraw = true;
		m_bEndAni = false;
		m_bCageBarSound[0] = false;
		m_bCageBarSound[1] = false;
		m_pDynamicMeshCom->Set_AnimationSet(0, 0.25);
		m_pDynamicMeshCom->Reset_Animation();
		break;
	}
}

void CInteractionObject::Render_Mesh()
{
	_matrix		matWorld;
	m_pTransformCom->Get_WorldMatrix(&matWorld);
	if (m_eObjType == SPAWNDOOR)
	{
		_matrix mScale, mRot;
		D3DXMatrixScaling(&mScale, 20.f, 500.f, 500.f);
		D3DXMatrixRotationY(&mRot, D3DXToRadian(180.f));
		matWorld = mScale * mRot * matWorld;
		//matWorld._11 *= 5.f;
		//matWorld._22 *= 500.f;
		//matWorld._33 *= 500.f;
	}
	//matWorld._11 *= 0.5f;
	for (int i = 0; i < m_vecColliderCom.size(); i++)
	{
		m_vecColliderCom[i]->Render_Collider(m_eType[i], &matWorld);
	}
}

const _float& CInteractionObject::Get_Angle() const
{
	return m_fAngle;
}


void CInteractionObject::Free(void)
{
	//m_pLine->Release();
	Engine::CGameObject::Free();
}

