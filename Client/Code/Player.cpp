#include "stdafx.h"
#include "Player.h"
#include "PhysicMgr.h"
#include "Collider.h"
#include "InteractionObject.h"
#include "Janitor.h"
#include "Janitor_Arm.h"
#include "FirstPersonCamera.h"
#include "Export_Function.h"
#include "CollisionMgr.h"
#include <iostream>
using namespace std;
CPlayer::CPlayer(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CGameObject(pGraphicDev)
{

}

CPlayer::~CPlayer(void)
{

}

HRESULT CPlayer::Ready_GameObject(void)
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	//m_pTransformCom->Set_Scale(0.01f, 0.01f, 0.01f);
	//m_pTransformCom->Set_Pos(0.01f, 0.01f, 0.01f);
	//animatioKey = 120;

	return S_OK;
}

HRESULT CPlayer::LateReady_GameObject(void)
{
	//m_ianimationKey = 0;
	//m_pTransformCom->Set_Pos(140.f, -5.65f, 5.5f);
	//m_pTransformCom->Set_Pos(79.5f, -10.8f, 1.f);
	//m_pTransformCom->Set_Pos(57.f, -3.5f, -3.f);
	//m_pTransformCom->Set_Pos(98.f, -5.65f, 1.f);
	m_vCheckPoint = _vec3(57.f, -3.5f, -3.f);
	Set_NaviMesh(L"Com_Navi_1");
	//Set_NaviMesh(L"Com_Navi_2");
	//Set_NaviMesh(L"Com_Navi_3");
	//m_pDynamicMeshCom->Set_AnimationSet(m_ianimationKey);
	m_fSphereCol = 0.4f;
	D3DXCreateSphere(m_pGraphicDev, m_fSphereCol, 20, 20, &m_mSphere, NULL);
	m_pLever = dynamic_cast<CInteractionObject*>(Engine::Get_GameObject(L"02.GameLogic", L"Sta_Obj_BigLever_8"));
	m_pCam = dynamic_cast<CFirstPersonCamera*>(Engine::Get_GameObject(L"01.Environment", L"MainCamera"));
	m_pCam->Set_At(*m_pTransformCom->Get_Info(Engine::INFO_POS));
	m_pTransformCom->Set_Angle(_vec3(0.f, D3DXToRadian(-90.f), 0.f));
	return S_OK;
}

_int CPlayer::Update_GameObject(const _float& fTimeDelta)
{
	if (g_bLoadFirst)
	{
		Set_SavePoint();
		Engine::CGameObject::Update_GameObject(fTimeDelta);

		m_pRendererCom->Add_RenderGroup(Engine::RENDER_NONALPHA, this);

		return 0;
	}
	m_bResetBGM = false;
	Engine::CGameObject::Update_GameObject(fTimeDelta);


	//SetUp_OnTerrain();
	//Trigger_Event();
	if (!g_bLoadFirst || !m_bStartAni)
		Key_Input(fTimeDelta);


	Gravity(fTimeDelta);

	Player_Animation();
	//m_pDynmicRIght->Set_AnimationSet(340, 0.25);
	if (!m_bLeechDeathEnd && !m_bJanitorDeathEnd)
	{
		m_pDynamicMeshCom->Play_Animation(fTimeDelta);
		m_pDynmicRIght->Play_Animation(fTimeDelta);
	}
	Turn();

	if (m_pTransformCom->Get_Info(Engine::INFO_POS)->x > 160.f && m_pTransformCom->Get_Info(Engine::INFO_POS)->x < 165.f)
	{
		m_bPipeSceneOut -= 0.05f;
		if (m_bPipeSceneOut > 0)
		{
			Engine::Set_Volume(Engine::CHANNELID::MONSTER, m_bPipeSceneOut);
		}
		else
		{
			m_bPipeSceneOut = 0.f;
			Engine::Stop_Sound(Engine::CHANNELID::MONSTER);
		}
	}
	if (m_pTransformCom->Get_Info(Engine::INFO_POS)->x > 78.5f && m_pTransformCom->Get_Info(Engine::INFO_POS)->x < 96.f)
		m_vCheckPoint = _vec3(79.5f, -10.8f, 1.f);
	else if (m_pTransformCom->Get_Info(Engine::INFO_POS)->x > 96.f&& m_pTransformCom->Get_Info(Engine::INFO_POS)->x < 137.f)
		m_vCheckPoint = _vec3(98.f, -5.65f, 1.f);
	else if (m_pTransformCom->Get_Info(Engine::INFO_POS)->x > 137.f)
	{
		m_vCheckPoint = _vec3(140.f, -5.65f, 5.5f);
	}
	if (m_bLighter)
	{
		Engine::Set_LightInfo(1)->Diffuse = D3DXCOLOR(1.f, 0.8f, 0.4f, 1.f);
		Engine::Set_LightInfo(1)->Specular = D3DXCOLOR(1.f, 0.8f, 0.4f, 1.f);
		Engine::Set_LightInfo(1)->Ambient = D3DXCOLOR(1.f, 0.8f, 0.4f, 1.f);
	}
	else
	{
		Engine::Set_LightInfo(1)->Diffuse = D3DXCOLOR(0.f, 0.f, 0.f, 1.f);
		Engine::Set_LightInfo(1)->Specular = D3DXCOLOR(0.f, 0.f, 0.f, 1.f);
		Engine::Set_LightInfo(1)->Ambient = D3DXCOLOR(0.f, 0.f, 0.f, 1.f);
	}
	m_pRendererCom->Add_RenderGroup(Engine::RENDER_SHADOWDEPTH, this);
	m_pRendererCom->Add_RenderGroup(Engine::RENDER_NONALPHA, this);
	return 0;
}

_int CPlayer::LateUpdate_GameObject(const _float & fTimeDelta)
{
	Trigger_Event();
	if (m_bPushCtrlTrigger)
	{
		m_fSpeed = 1.5f;
		m_bPushCtrl = true;
	}
	else
	{
		m_fSpeed = 3.f;
		m_bPushCtrl = false;
	}

	return 0;
}

void CPlayer::Render_GameObject(void)
{
	LPD3DXEFFECT	pEffect = m_pShaderCom->Get_EffectHandle();
	NULL_CHECK(pEffect);
	pEffect->AddRef();

	_uint	iPassMax = (_uint)0;

	FAILED_CHECK_RETURN(SetUp_ConstantTable(pEffect), );

	pEffect->Begin(&iPassMax, 0);

	pEffect->BeginPass(0);
	m_pDynamicMeshCom->Set_Test(m_pDynmicRIght);
	m_pDynamicMeshCom->Render_Meshes(pEffect);


	pEffect->EndPass();

	pEffect->End();

	Safe_Release(pEffect);

	//Draw_Sphere();

	if (m_vecColliderCom.size() > 0)
	{
		//m_vecColliderCom[0]->Render_Collider(Engine::COL_FALSE, &m_ColWorld);
	}
}

void CPlayer::Render_ShadowDepth(void)
{
	LPD3DXEFFECT	pEffect = m_pShaderCom->Get_EffectHandle();
	NULL_CHECK(pEffect);
	pEffect->AddRef();

	pEffect->SetMatrix("g_matWorld", m_pTransformCom->Get_WorldMatrixPointer());




	Engine::CGameObject* pCam = Engine::Get_GameObject(L"01.Environment", L"MainCamera");
	//_vec3 tempPos = Get_LightPos();
	_vec3 tempPos = *this->GetTransformCom()->Get_Info(Engine::INFO_POS);
	_vec3 test = Get_LightPos();
	D3DXVec3Normalize(&test, &test);
	test.y = 0.3f;

	_vec3		vLightEye = Get_LightPos();

	_vec3		vLightAt = *this->GetTransformCom()->Get_Info(Engine::INFO_POS);

	_vec3		vLightUp = _vec3(0.f, 1.f, 0.f);
	_matrix		LightViewMatrix;

	D3DXMatrixLookAtLH(&LightViewMatrix, &vLightEye, &vLightAt, &vLightUp);

	pEffect->SetMatrix("g_matView", &LightViewMatrix);

	_matrix		LightProjMatrix;

	D3DXMatrixPerspectiveFovLH(&LightProjMatrix, D3DXToRadian(100.f), _float(WINCX) / WINCY, 0.2f, 10000.f);

	pEffect->SetMatrix("g_matProj", &LightProjMatrix);

	pEffect->Begin(nullptr, 0);
	pEffect->BeginPass(1);
	m_pTransformCom->Rotation(Engine::ROT_Y, D3DXToRadian(180.f));
	m_pDynamicMeshCom->Render_Meshes(pEffect);
	m_pTransformCom->Rotation(Engine::ROT_Y, D3DXToRadian(-180.f));

	pEffect->EndPass();
	pEffect->End();

	Safe_Release(pEffect);
}

_vec3 CPlayer::Get_LightPos()
{
	m_pDynamicMeshCom->Play_Animation(0.f);
	// TODO: 여기에 반환 구문을 삽입합니다.
	//const Engine::D3DXFRAME_DERIVED* pBone = m_pDynamicMeshCom->Get_FrameByName("skinned_c_lighterLightPos1_bn");

	//const _matrix boneMat = pBone->CombinedTransformationMatrix * *m_pTransformCom->Get_WorldMatrixPointer();

	const Engine::D3DXFRAME_DERIVED* pBone = m_pDynamicMeshCom->Get_FrameByName("skinned_root_bn");

	_matrix boneMat = pBone->CombinedTransformationMatrix * *m_pTransformCom->Get_WorldMatrixPointer();

	if (m_bLighter)
	{
		_vec3 tempPos = *this->GetTransformCom()->Get_Info(Engine::INFO_POS);
		_vec3 test = { boneMat._21, boneMat._22, boneMat._23 };
		D3DXVec3Normalize(&test, &test);
		test.y = 0.3f;
		return tempPos + test * 5.f;
	}
	else
	{
		_vec3 vPos = *m_pTransformCom->Get_Info(Engine::INFO_POS);
		for (int i = 2; i <= 6; i++)
		{
			_vec3 vLightPos = Engine::Get_LightInfo(i)->Position;
			if (D3DXVec3Length(&(vLightPos - vPos)) < 6.f)
			{
				return vLightPos;
			}
		}
		vPos.y += 5.f;
		vPos.z += 0.1f;
		return vPos;
	}


	return _vec3(boneMat._21, boneMat._22, boneMat._23);
}

void CPlayer::Set_NaviMesh(const _tchar* comName)
{
	m_pNaviCom = dynamic_cast<Engine::CNaviMesh*>(Engine::Get_Component(L"Navi_Mesh", L"Terrain", comName, Engine::ID_STATIC));
	m_pNaviCom->Set_Index_UsePos(*m_pTransformCom->Get_Info(Engine::INFO_POS));
}

void CPlayer::Set_Shader()
{
	Engine::CComponent*		pComponent = nullptr;
	pComponent = m_pShaderCom = dynamic_cast<Engine::CShader*>(Engine::Clone_Prototype(L"Shader_Mesh"));
	NULL_CHECK(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Shader", pComponent);

	pComponent = Engine::CCollider::Create(m_pGraphicDev,
		m_pTransformCom->Get_Info(Engine::INFO_POS),
		0.f,
		0.f, true);
	NULL_CHECK(pComponent, E_FAIL);
	m_vecColliderCom.push_back(dynamic_cast<Engine::CCollider*>(pComponent));
	m_mapComponent[Engine::ID_DYNAMIC].emplace(L"Com_Collider", pComponent);

	m_pDynmicRIght = dynamic_cast<Engine::CDynamicMesh*>(Engine::Clone(RESOURCE_STATIC, L"Mesh_Player"));
	NULL_CHECK(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_DYNAMIC].emplace(L"Com_RightMesh", pComponent);


}

Engine::CCollider * CPlayer::Get_BoxCollider()
{
	return m_vecColliderCom[0];
}

_matrix CPlayer::Get_ColWorld()
{
	return m_ColWorld;
}

void CPlayer::Set_Trigger(const _tchar * name)
{
	wstring tempstr = name;
	for (auto iter = m_wTriggerName.begin(); iter != m_wTriggerName.end(); iter++)
	{
		if (*iter == tempstr)
			return;
	}
	m_wTriggerName.push_back(name);
}

void CPlayer::Set_CatchLeech(const _bool & flag)
{
	m_bCatchLeech = flag;
}

void CPlayer::Set_CatchJanitor(const _bool & flag, const _int& index, const _bool& bHandFlag, Engine::CGameObject* pObject)
{
	_bool tempflag = false;
	Engine::Play_Sound(L"Monster_Grab.ogg", Engine::CHANNELID::EFFECT, tempflag, false);
	m_bJanitorDeath = flag;
	m_iJanitorHand = index;
	m_bJanitorHand = bHandFlag;
	m_pJanitor = pObject;

}

void CPlayer::Set_OnCol(const _bool & flag)
{
	m_bOnCol = flag;
}

void CPlayer::Delete_TriggerName(wstring str)
{

	for (auto iter = m_wTriggerName.begin(); iter != m_wTriggerName.end(); )
	{
		if (*iter == str)
		{
			iter = m_wTriggerName.erase(iter);
			return;
		}
		else
			iter++;
	}
}

void CPlayer::Land()
{
	if (m_bJump)
	{
		m_fJumpPower = 0.f;
		m_bLanding = true;
		m_bJump = false;
		m_bFalling = false;
	}

}

void CPlayer::Trigger_Event()
{
	if (m_wTriggerName.size() == 0)
	{
		m_bClimeReady = false;
		m_bClimeAir = false;
		m_bPushCtrlTrigger = false;
		m_bBoxCarryTrigger = false;
		m_bPushLeverTrigger = false;
		m_bBoxUpTrigger = false;
		m_bUpPipeTrigger = false;
		m_bLeftCageTrigger = false;
		m_bRightCageTrigger = false;
		return;
	}
	for (auto iter = m_wTriggerName.begin(); iter != m_wTriggerName.end(); iter++)
	{
		if (m_pTransformCom->Get_Info(Engine::INFO_POS)->x <= 96.f)
		{
			if (*iter == L"Col_Trigger_6")
			{
				if (m_eDir == DIR_RIGHT)
				{
					if (m_bJump)
					{
						if (!m_bClimeAir)
						{
							m_bCantMove = true;
							m_bClimeAir = true;
							m_fJumpPower = 0.f;
							m_vColScale = { 0.5f, 0.2f, 0.5f };
							m_pTransformCom->Set_Pos(_vec3(41.75f, 1.4f, m_pTransformCom->Get_Info(Engine::INFO_POS)->z));
						}
					}
					else
					{
						m_bClimeReady = true;
					}
				}
				else
				{
					m_bClimeReady = false;
					m_bClimeAir = false;
				}
			}
			if (*iter == L"Col_Trigger_3" || *iter == L"Col_Trigger_4" || *iter == L"Col_Trigger_5")
			{
				//m_bClimeReady = false;
				//m_bClime = false;
				//m_bClimeAir = false;
				m_bPushCtrlTrigger = true;
			}
			else
				m_bPushCtrlTrigger = false;
			if (*iter == L"Col_Trigger_7")
			{
				Engine::Stop_Sound(Engine::CHANNELID::BGM);
				Engine::Play_Sound(L"SW_Wood_Debris_Crash_04.ogg", Engine::CHANNELID::EFFECT, m_bWoodFloorSound, false);
				m_bCantMove = true;
				m_bFalling = true;
				m_bWoodFloor = true;
				m_fJumpPower = 0.f;
				m_fAcc = 0.17f;
				TurnOffLighter(true);
				Set_NaviMesh(L"Com_Navi_2");
			}
			if (*iter == L"Col_Trigger_8")
			{
				m_bLeechChase = true;
				m_bCantMove = false;
				Engine::Play_Sound(L"SW_amb_prison_distant_horn_2f.ogg", Engine::CHANNELID::BGM, m_bSceondBgm, true);
			}
			if (!m_bHatchOpen)
			{
				if (*iter == L"Col_Trigger_9")
				{
					if (m_eDir == DIR_RIGHT && m_ePreDir == DIR_RIGHT)
					{
						for (auto iter2 = m_wTriggerName.begin(); iter2 != m_wTriggerName.end(); iter2++)
						{
							if (*iter2 == L"Col_Trigger_10")
							{
								m_bTriggerGrip = true;
								break;
							}
						}
					}
				}
				else
					m_bTriggerGrip = false;
			}
			if (!m_bPushDoorOpen)
			{
				if (*iter == L"Col_Trigger_11")
				{
					if (m_eDir == DIR_RIGHT && m_ePreDir == DIR_RIGHT)
					{
						m_bPushDoorTrigger = true;
					}
				}
			}
			if (*iter == L"Col_Trigger_12" || *iter == L"Col_Trigger_13" || *iter == L"Col_Trigger_14" || *iter == L"Col_Trigger_15"
				|| *iter == L"Col_Trigger_16" || *iter == L"Col_Trigger_17" || *iter == L"Col_Trigger_18" || *iter == L"Col_Trigger_19")
			{
				m_bFallDown = true;
				m_bFalling = true;
			}
			if (*iter == L"Col_Trigger_27" || *iter == L"Col_Trigger_28" || *iter == L"Col_Trigger_29"
				|| *iter == L"Col_Trigger_30" || *iter == L"Col_Trigger_31")
			{

				m_bFalling = true;
				Set_NaviMesh(L"Com_Navi_2");
			}
			if (*iter == L"Col_Trigger_32")
			{
				Set_NaviMesh(L"Com_Navi_3");
			}
			if (*iter == L"Col_Trigger_34")
			{
				Set_NaviMesh(L"Com_Navi_2");
			}
			if (*iter == L"Col_Trigger_33")
			{
				if (m_eDir == DIR_LEFT && m_ePreDir == DIR_LEFT)
					m_bPushLeverTrigger = true;
				else
					m_bPushLeverTrigger = false;
			}
			else
				m_bPushLeverTrigger = false;
		}
		else
		{
			if (*iter == L"Box_Trigger_Z")
			{
				Engine::CGameObject* pBox = dynamic_cast<CInteractionObject*>(Engine::Get_GameObject(L"02.GameLogic", L"Sta_Obj_WoodBox_10"));
				if (m_pTransformCom->Get_Info(Engine::INFO_POS)->z > pBox->GetTransformCom()->Get_Info(Engine::INFO_POS)->z)
				{
					if (m_eDir == DIR_BOTTOM && m_ePreDir == DIR_BOTTOM)
					{
						m_bBoxCarryTrigger = true;
						m_eBoxCarryDir = DIR_BOTTOM;
					}
					else
						m_bBoxCarryTrigger = false;
				}
				else
				{
					if (m_eDir == DIR_TOP && m_ePreDir == DIR_TOP)
					{
						m_bBoxCarryTrigger = true;
						m_eBoxCarryDir = DIR_TOP;
					}
					else
						m_bBoxCarryTrigger = false;
				}
			}
			else if (*iter == L"Box_Trigger_X")
			{
				Engine::CGameObject* pBox = dynamic_cast<CInteractionObject*>(Engine::Get_GameObject(L"02.GameLogic", L"Sta_Obj_WoodBox_10"));
				if (m_pTransformCom->Get_Info(Engine::INFO_POS)->x > pBox->GetTransformCom()->Get_Info(Engine::INFO_POS)->x)
				{
					if (m_eDir == DIR_LEFT && m_ePreDir == DIR_LEFT)
					{
						m_bBoxCarryTrigger = true;
						m_eBoxCarryDir = DIR_LEFT;
					}
					else
						m_bBoxCarryTrigger = false;
				}
				else
				{
					if (m_eDir == DIR_RIGHT && m_ePreDir == DIR_RIGHT)
					{
						m_bBoxCarryTrigger = true;
						m_eBoxCarryDir = DIR_RIGHT;
					}
					else
						m_bBoxCarryTrigger = false;
				}
			}
			else
				m_bBoxCarryTrigger = false;
			if (*iter == L"Col_Trigger_1")
			{
				if (m_eDir == DIR_RIGHT && m_ePreDir == DIR_RIGHT)
				{
					m_bBoxUpTrigger = true;
				}
			}
			else
				m_bBoxUpTrigger = false;
			if (*iter == L"Col_Trigger_12")
			{
				if (!m_bSpawnJionterBedRoom)
				{
					Engine::Stop_Sound(Engine::CHANNELID::BGM);
					Engine::Play_BGM(L"SW_music_Janitor_suspense_loop_2.ogg");
					m_bSpawnJionterBedRoom = true;
				}
			}
			if (*iter == L"Col_Trigger_13")
				m_bPushCtrlTrigger = true;
			else
				m_bPushCtrlTrigger = false;

			if (*iter == L"Col_Trigger_18")
			{
				if (m_eDir == DIR_RIGHT && m_ePreDir == DIR_RIGHT)
				{
					m_bUpPipeTrigger = true;
				}
			}
			else
				m_bUpPipeTrigger = false;
			if (*iter == L"Col_Trigger_19")
			{
				if (m_eDir == DIR_RIGHT && m_ePreDir == DIR_RIGHT)
				{
					if (!m_bJanitorArmSound)
					{
						Engine::Stop_All();
						Engine::Play_Sound(L"SW_music_stinger_Janitor_3a.ogg", Engine::CHANNELID::MONSTER, m_bJanitorArmSound, false);
						Engine::Play_Sound(L"SW_Janitor_breathing_erratic_distant.ogg", Engine::CHANNELID::MONSTER, m_bJanitorAirEffect, true);

					}
					CJanitorArm* pJanitor = dynamic_cast<CJanitorArm*>(Engine::Get_GameObject(L"02.GameLogic", L"Dyn_Monster_Mesh_Janitor_Arm_8"));
					if (pJanitor->Get_State() == CJanitorArm::IDLE)
						pJanitor->Set_State(CJanitorArm::START);
				}
				else if (m_eDir == DIR_LEFT && m_ePreDir == DIR_LEFT)
				{
					CJanitorArm* pJanitor = dynamic_cast<CJanitorArm*>(Engine::Get_GameObject(L"02.GameLogic", L"Dyn_Monster_Mesh_Janitor_Arm_7"));
					if (pJanitor->Get_State() == CJanitorArm::IDLE)
						pJanitor->Set_State(CJanitorArm::START);
				}
			}
			if (*iter == L"Col_Trigger_20")
			{
				m_bLastStage = true;
			}

			if (*iter == L"Col_Trigger_22")
			{
				CInteractionObject* pObject = dynamic_cast<CInteractionObject*>(Engine::Get_GameObject(L"02.GameLogic", L"Sta_Obj_SKM_PrisonContainer04_doorCrush_Bar_17"));
				if (m_eDir == DIR_LEFT && m_ePreDir == DIR_LEFT)
				{
					if (!pObject->Get_NotRender())
						m_bLeftCageTrigger = true;
					else
						m_bLeftCageTrigger = false;
				}
				else
					m_bLeftCageTrigger = false;
			}
			if (*iter == L"Col_Trigger_23")
			{
				CInteractionObject* pObject = dynamic_cast<CInteractionObject*>(Engine::Get_GameObject(L"02.GameLogic", L"Sta_Obj_SKM_PrisonContainer04_doorCrush_Bar_18"));
				CInteractionObject* pObject2 = dynamic_cast<CInteractionObject*>(Engine::Get_GameObject(L"02.GameLogic", L"Sta_Obj_SKM_PrisonContainer04_doorCrush_Bar_17"));
				if (m_eDir == DIR_LEFT && m_ePreDir == DIR_LEFT)
				{
					if (!pObject->Get_NotRender() && pObject2->Get_NotRender())
						m_bRightCageTrigger = true;
					else
						m_bRightCageTrigger = false;
				}
				else
					m_bRightCageTrigger = false;
			}

		}
	}
	//Set_ColWorld(m_vColScale,*m_pTransformCom->Get_Info(Engine::INFO_POS));

}

const _bool & CPlayer::Get_ChaseBool() const
{
	// TODO: 여기에 반환 구문을 삽입합니다.
	return m_bLeechChase;
}

const _bool & CPlayer::Get_Falling() const
{
	// TODO: 여기에 반환 구문을 삽입합니다.
	return m_bFalling;
}

const _bool & CPlayer::Get_CatchLeech() const
{
	// TODO: 여기에 반환 구문을 삽입합니다.
	return m_bCatchLeech;
}

const _bool & CPlayer::Get_LoadScene() const
{
	// TODO: 여기에 반환 구문을 삽입합니다.
	return m_bLoadScene;
}

const _bool & CPlayer::Get_HatchOpenAni() const
{
	// TODO: 여기에 반환 구문을 삽입합니다.
	return m_bPullDownGrip;
}

const _bool & CPlayer::Get_PullHatch() const
{
	// TODO: 여기에 반환 구문을 삽입합니다.
	return m_bCatchGrip;
}

const _bool & CPlayer::Get_HatchOpen() const
{
	// TODO: 여기에 반환 구문을 삽입합니다.
	return m_bHatchOpen;
}

const _bool & CPlayer::Get_WoodFloor() const
{
	// TODO: 여기에 반환 구문을 삽입합니다.
	return m_bWoodFloor;
}

const _bool & CPlayer::Get_DoorPush() const
{
	// TODO: 여기에 반환 구문을 삽입합니다.
	return m_bPushDoor;
}

const _bool & CPlayer::Get_LeverPush() const
{
	// TODO: 여기에 반환 구문을 삽입합니다.
	return m_bPushLever;
}

const _bool & CPlayer::Get_BoxMove() const
{
	// TODO: 여기에 반환 구문을 삽입합니다.
	return m_bBoxCarryMove;
}

const _bool & CPlayer::Get_NaviFlag() const
{
	// TODO: 여기에 반환 구문을 삽입합니다.
	return m_bNaviFlag;
}

const _bool & CPlayer::Get_Turn() const
{
	// TODO: 여기에 반환 구문을 삽입합니다.
	return m_bTurn;
}

const _bool & CPlayer::Get_SpawnJionterBedRoom() const
{
	// TODO: 여기에 반환 구문을 삽입합니다.
	return m_bSpawnJionterBedRoom;
}

const _bool & CPlayer::Get_JionterCatch() const
{
	// TODO: 여기에 반환 구문을 삽입합니다.
	return m_bJanitorDeath;
}

const _bool & CPlayer::Get_LastStage() const
{
	// TODO: 여기에 반환 구문을 삽입합니다.
	return m_bLastStage;
}

const _bool & CPlayer::Get_LighterFlag() const
{
	// TODO: 여기에 반환 구문을 삽입합니다.
	return m_bLighter;
}

const _vec3 & CPlayer::Get_AniPos() const
{
	// TODO: 여기에 반환 구문을 삽입합니다.
	return m_aniPos;
}

const _vec3 & CPlayer::Get_PreDir() const
{
	// TODO: 여기에 반환 구문을 삽입합니다.
	return m_vPreDir;
}

const _vec3 & CPlayer::Get_Player_Hand() const
{
	// TODO: 여기에 반환 구문을 삽입합니다.
	const Engine::D3DXFRAME_DERIVED* pBone = m_pDynamicMeshCom->Get_FrameByName("skinned_r_middleHand_bn");
	//NULL_CHECK(pBone, 0);
	_matrix boneMat = pBone->CombinedTransformationMatrix * *m_pTransformCom->Get_WorldMatrixPointer();
	return _vec3(boneMat._41, boneMat._42, boneMat._43);
}

const _vec3 & CPlayer::Get_ColScale() const
{
	// TODO: 여기에 반환 구문을 삽입합니다.
	return m_vColScale;
}

const _matrix & CPlayer::Get_LighterPos() const
{
	m_pDynmicRIght->Play_Animation(0.f);
	// TODO: 여기에 반환 구문을 삽입합니다.
	const Engine::D3DXFRAME_DERIVED* pBone = m_pDynmicRIght->Get_FrameByName("skinned_r_handPlacement_bn");

	const _matrix boneMat = pBone->CombinedTransformationMatrix * *m_pTransformCom->Get_WorldMatrixPointer();

	return boneMat;
}

const _float & CPlayer::Get_NaviY() const
{
	// TODO: 여기에 반환 구문을 삽입합니다.
	return m_pNaviCom->Navi(1, *m_pTransformCom->Get_Info(Engine::INFO_POS));
}

const _float & CPlayer::Get_ShpereRadius() const
{
	// TODO: 여기에 반환 구문을 삽입합니다.
	return m_fSphereCol;
}

const _float & CPlayer::Get_HatchCount() const
{
	// TODO: 여기에 반환 구문을 삽입합니다.
	return m_fGrabCount;
}

const _float & CPlayer::Get_AniTime() const
{
	// TODO: 여기에 반환 구문을 삽입합니다.
	return m_pDynamicMeshCom->Get_AniPercent();
}

const _bool & CPlayer::GetAniflag() const
{
	// TODO: 여기에 반환 구문을 삽입합니다.
	if (m_bTurn || m_bClimeAir || m_bClime || m_bPullDownGrip || m_bLeftCagePullDown || m_bRightCagePullDown)
	{
		return true;
	}
	return false;
}

HRESULT CPlayer::Add_Component(void)
{
	Engine::CComponent*		pComponent = nullptr;

	//pComponent = m_pMeshCom = dynamic_cast<Engine::CDynamicMesh*>(Engine::Clone(RESOURCE_STAGE, L"Mesh_Player"));
	//NULL_CHECK_RETURN(pComponent, E_FAIL);
	//m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Mesh", pComponent);

	//pComponent = m_pTransformCom = Engine::CTransform::Create();
	//NULL_CHECK_RETURN(pComponent, E_FAIL);
	//m_mapComponent[Engine::ID_DYNAMIC].emplace(L"Com_Transform", pComponent);

	//pComponent = m_pRendererCom = Engine::Get_Renderer();
	//NULL_CHECK_RETURN(pComponent, E_FAIL);
	//pComponent->AddRef();
	//m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Renderer", pComponent);

	//pComponent = m_pCalculatorCom = Engine::CCalculator::Create(m_pGraphicDev);
	//NULL_CHECK_RETURN(pComponent, E_FAIL);
	//m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Calculator", pComponent);

	//// collider
	//pComponent = m_pColliderCom = Engine::CCollider::Create(m_pGraphicDev,
	//	m_pMeshCom->Get_VtxPos(),
	//	m_pMeshCom->Get_NumVtx(),
	//	m_pMeshCom->Get_Stride());
	//NULL_CHECK_RETURN(pComponent, E_FAIL);
	//m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Collider", pComponent);


	return S_OK;
}

void CPlayer::Key_Input(const _float& fTimeDelta)
{
	//m_pTransformCom->Get_Info(Engine::INFO_LOOK, &m_vDir);

	m_vColScale = { 0.f,0.f,0.f };
	if (GetAsyncKeyState(VK_CONTROL) & 0x8000)
	{
		if (!m_bJump && !m_bPushCtrlTrigger && !m_bFalling && !m_bBoxCarry)
		{
			m_fSpeed = 1.5f;
			m_vColScale = { 0.5f, 0.75f, 0.5f };
			m_bPushCtrl = true;
		}
		else if (m_bPushCtrlTrigger)
		{
			m_fSpeed = 1.5f;
			m_vColScale = { 0.5f, 0.75f, 0.5f };
			m_bPushCtrl = true;
		}
	}
	else
	{
		if (m_bPushCtrlTrigger)
		{
			m_fSpeed = 1.5f;
			m_vColScale = { 0.5f, 0.75f, 0.5f };
			m_bPushCtrl = true;
		}
		else if (m_bClime || m_bClimeAir)
		{
			m_fSpeed = 1.5f;
			m_vColScale = { 0.2f, 0.3f, 0.2f };
		}
		else
		{
			m_fSpeed = 3.f;
			m_vColScale = { 0.5f, 1.5f, 0.5f };
			//m_vColScale = { 0.5f, 0.75f, 0.5f };

			m_bPushCtrl = false;
		}
	}
	if (GetAsyncKeyState(VK_SHIFT) & 0x8000)
	{
		if (!m_bPushCtrlTrigger && !m_bClime && !m_bClimeAir && !m_bBoxCarry)
		{
			m_fSpeed = 5.f;
			m_vColScale = { 0.5f, 1.5f, 0.5f };
			//m_vColScale = { 0.5f, 0.75f, 0.5f };
			m_bRun = true;
		}
	}
	else
		m_bRun = false;

	if (Engine::Get_KeyMgr()->KeyDown(Engine::KEY_P))
	{
		m_pTransformCom->Set_Pos(98.f, -5.65f, 1.f);
		m_vCheckPoint = _vec3(57.f, -3.5f, -3.f);
		Set_NaviMesh(L"Com_Navi_3");
	}
	if (Engine::Get_KeyMgr()->KeyDown(Engine::KEY_DOWN))
	{
		g_bColCheat = !g_bColCheat;
		//if (m_ianimationKey > 0)
		//	m_ianimationKey--;
		//m_pDynamicMeshCom->Set_AnimationSet(m_ianimationKey);
		//cout << "Animation Key : " << m_ianimationKey << endl;
	}
	if (Engine::Get_KeyMgr()->KeyDown(Engine::KEY_RIGHT))
	{
		m_bLoadScene = true;

	}

	m_pDynamicMeshCom->Set_AnimationSet(m_ianimationKey);
	_vec3	vPos, m_vDir;
	m_pTransformCom->Get_Info(Engine::INFO_POS, &vPos);

	if (GetAsyncKeyState('W') & 0x8000)
	{
		m_vDir = _vec3(0.f, 0.f, 1.f);
		D3DXVec3Normalize(&m_vDir, &m_vDir);

		if (!m_bTurn)
			m_eDir = DIR_TOP;
		if (m_bBoxCarry)
		{
			m_bBoxCarryMove = true;
			m_fSpeed = 1.5f;
		}
		m_vPreDir = m_vDir * m_fSpeed * fTimeDelta;
		if (m_eDir != m_ePreDir && !m_bBoxCarry)
		{
			m_bTurn = true;
			m_bCantMove = true;
		}
		if (!m_bCantMove)
		{
			m_bMove = true;
			m_pTransformCom->Set_Pos(&m_pNaviCom->Move_OnNaviMesh(&vPos, &(m_vDir * m_fSpeed * fTimeDelta), m_bNaviFlag));
			Set_ColWorld(m_vColScale, *m_pTransformCom->Get_Info(Engine::INFO_POS));
		}

	}

	if (GetAsyncKeyState('S') & 0x8000)
	{
		m_vDir = _vec3(0.f, 0.f, -1.f);
		D3DXVec3Normalize(&m_vDir, &m_vDir);
		if (!m_bTurn)
			m_eDir = DIR_BOTTOM;
		if (m_bBoxCarry)
		{
			m_bBoxCarryMove = true;
			m_fSpeed = 1.5f;
		}
		m_vPreDir = m_vDir * m_fSpeed * fTimeDelta;
		if (m_eDir != m_ePreDir && !m_bBoxCarry)
		{
			m_bTurn = true;
			m_bCantMove = true;
		}
		if (!m_bCantMove)
		{
			m_bMove = true;
			m_pTransformCom->Set_Pos(&m_pNaviCom->Move_OnNaviMesh(&vPos, &(m_vDir * m_fSpeed * fTimeDelta), m_bNaviFlag));
			Set_ColWorld(m_vColScale, *m_pTransformCom->Get_Info(Engine::INFO_POS));
		}


	}


	if (GetAsyncKeyState('A') & 0x8000)
	{
		m_vDir = _vec3(-1.f, 0.f, 0.f);
		D3DXVec3Normalize(&m_vDir, &m_vDir);
		if (!m_bTurn)
			m_eDir = DIR_LEFT;
		if (m_bBoxCarry)
		{
			m_bBoxCarryMove = true;
			m_fSpeed = 1.5f;
		}
		m_vPreDir = m_vDir * m_fSpeed * fTimeDelta;
		if (m_eDir != m_ePreDir && !m_bBoxCarry)
		{
			m_bTurn = true;
			m_bCantMove = true;
		}
		if (!m_bCantMove)
		{
			m_bMove = true;
			m_pTransformCom->Set_Pos(&m_pNaviCom->Move_OnNaviMesh(&vPos, &(m_vDir * m_fSpeed * fTimeDelta), m_bNaviFlag));
			Set_ColWorld(m_vColScale, *m_pTransformCom->Get_Info(Engine::INFO_POS));
		}


	}

	if (GetAsyncKeyState('D') & 0x8000)
	{
		m_vDir = _vec3(1.f, 0.f, 0.f);
		D3DXVec3Normalize(&m_vDir, &m_vDir);
		if (!m_bTurn && !m_bRightCageCatch && !m_bLeftCageCatch)
			m_eDir = DIR_RIGHT;
		if (m_bBoxCarry)
		{
			m_bBoxCarryMove = true;
			m_fSpeed = 1.5f;
		}
		if (m_bRightCageCatch)
		{
			m_bRightCagePull = true;
			m_bCantMove = true;
		}
		else if (m_bLeftCageCatch)
		{
			m_bLeftCagePull = true;
			m_bCantMove = true;
		}
		m_vPreDir = m_vDir * m_fSpeed * fTimeDelta;

		if (m_bPushDoorTrigger && !m_bPushDoorOpen)
		{
			m_bCantMove = true;
			m_bPushDoor = true;
		}
		if (m_eDir != m_ePreDir && !m_bBoxCarry && !m_bRightCageCatch && !m_bLeftCageCatch)
		{
			m_bTurn = true;
			m_bCantMove = true;
		}
		if (!m_bCantMove)
		{
			m_bMove = true;
			m_pTransformCom->Set_Pos(&m_pNaviCom->Move_OnNaviMesh(&vPos, &(m_vDir * m_fSpeed * fTimeDelta), m_bNaviFlag));
			Set_ColWorld(m_vColScale, *m_pTransformCom->Get_Info(Engine::INFO_POS));
		}
	}
	if (!(GetAsyncKeyState('W') & 0x8000) && !(GetAsyncKeyState('S') & 0x8000) &&
		!(GetAsyncKeyState('D') & 0x8000) && !(GetAsyncKeyState('A') & 0x8000))
	{
		m_bMove = false;
		if (m_bBoxCarry)
			m_bBoxCarryMove = false;
	}
	if (GetAsyncKeyState(VK_LBUTTON) & 0x8000)
	{
		if (!m_bHatchOpen && !m_bJump)
		{
			if (m_bTriggerGrip)
			{
				if (!m_bCatchGrip)
				{
					m_vGrabfirstPos = *m_pTransformCom->Get_Info(Engine::INFO_POS);
					m_bCatchGrip = true;
					TurnOffLighter();
				}
				m_bCantMove = true;
			}
			if (m_bCatchGrip)
			{
				if (GetAsyncKeyState('A') & 0x8000)
				{
					m_bPullGrip = true;
					m_bTurn = false;
					m_vDir = _vec3(-1.f, 0.f, 0.f);
					D3DXVec3Normalize(&m_vDir, &m_vDir);
					if (m_fGrabCount < 0.1f)
						m_pTransformCom->Set_Pos(&m_pNaviCom->Move_OnNaviMesh(&vPos, &(m_vDir * m_fSpeed * 0.5f * fTimeDelta), m_bNaviFlag));
					Engine::Play_Sound(L"SW_hatch_vent_open_a.ogg", Engine::CHANNELID::HATCH, m_bHatchOpenSound, false);
					m_fGrabCount += fTimeDelta;
				}
			}
		}
		if (m_bPushLeverTrigger && m_pLever->Get_ReadyLever() && !m_bJump)
		{
			m_pTransformCom->Set_Pos(m_pLever->GetTransformCom()->Get_Info(Engine::INFO_POS));
			m_bPushLever = true;
			m_bCantMove = true;
			TurnOffLighter();
		}
		if (m_bBoxCarryTrigger && !m_bJump)
		{
			m_bBoxCarry = true;
			TurnOffLighter();
		}
		if (m_bLeftCageTrigger)
		{
			m_bLeftCageCatch = true;
			TurnOffLighter();
		}
		else if (m_bRightCageTrigger)
		{
			m_bRightCageCatch = true;
			TurnOffLighter();
		}
	}
	else
	{
		if (!m_bHatchOpen && !m_bPullDownGrip && !m_bClime && !m_bClimeAir)
		{
			m_bCatchGrip = false;
			m_bCantMove = false;
			m_bPullGrip = false;
			m_bHatchOpenSound = false;
			m_fGrabCount = 0.f;
		}
		m_bBoxCarry = false;
		m_bBoxCarryMove = false;
		m_bLeftCageCatch = false;
		m_bRightCageCatch = false;
	}
	if (Engine::Get_KeyMgr()->KeyDown(Engine::KEY_F))
	{
		if (!m_bMove && !m_bTurn && !m_bLeechDeath && !m_bJanitorDeath && !m_bClime && !m_bJump && !m_bFalling && !m_bLanding)
		{
			if (!m_bLighter)
				m_bLighterIgnight = true;
			else
			{
				TurnOffLighter();
			}
		}
	}

	if (Engine::Get_KeyMgr()->KeyDown(Engine::KEY_SPACE))
	{

		if (m_bClimeReady)
		{
			m_bClime = true;
			m_bCantMove = true;
			m_vColScale = { 0.5f, 0.75f, 0.5f };
			m_pTransformCom->Set_Pos(_vec3(41.78f, 0.6f, m_pTransformCom->Get_Info(Engine::INFO_POS)->z));
		}
		else if (m_bBoxCarryTrigger && !m_bBoxCarry)
		{
			m_bClime = true;
			m_bCantMove = true;

			Engine::CGameObject* pBox = dynamic_cast<CInteractionObject*>(Engine::Get_GameObject(L"02.GameLogic", L"Sta_Obj_WoodBox_10"));
			_vec3 vBoxPos = *pBox->GetTransformCom()->Get_Info(Engine::INFO_POS);
			switch (m_eBoxCarryDir)
			{
			case DIR_LEFT:
				m_pTransformCom->Set_Pos(_vec3(vBoxPos.x + 1.2f, vBoxPos.y + 0.4f, m_pTransformCom->Get_Info(Engine::INFO_POS)->z));
				break;
			case DIR_RIGHT:
				m_pTransformCom->Set_Pos(_vec3(vBoxPos.x - 1.2f, vBoxPos.y + 0.4f, m_pTransformCom->Get_Info(Engine::INFO_POS)->z));
				break;
			case DIR_TOP:
				m_pTransformCom->Set_Pos(_vec3(m_pTransformCom->Get_Info(Engine::INFO_POS)->x, vBoxPos.y + 0.4f, vBoxPos.z - 1.2f));
				break;
			case DIR_BOTTOM:
				m_pTransformCom->Set_Pos(_vec3(m_pTransformCom->Get_Info(Engine::INFO_POS)->x, vBoxPos.y + 0.4f, vBoxPos.z + 1.2f));
				break;
			}
		}
		else if (m_bBoxUpTrigger && !m_bJump)
		{
			m_bClime = true;
			m_bCantMove = true;
			_vec3 tempPos = *m_pTransformCom->Get_Info(Engine::INFO_POS);
			m_pTransformCom->Set_Pos(_vec3(112.f, tempPos.y + 0.4f, tempPos.z));
		}
		else if (m_bUpPipeTrigger && !m_bJump)
		{
			m_bClime = true;
			m_bCantMove = true;
			_vec3 tempPos = *m_pTransformCom->Get_Info(Engine::INFO_POS);
			m_pTransformCom->Set_Pos(_vec3(142.f, tempPos.y - 0.2f, tempPos.z));
		}
		else
		{
			if (!m_bJump && !m_bPushCtrlTrigger && !m_bFalling && !m_bBoxCarry && !m_bTurn)
			{
				m_bJump = true;
				m_fJumpPower = 0.15f;
			}
		}


	}

	if (Engine::Get_DIKeyState(DIK_RETURN) & 0x80)
	{
		//m_pDynamicMeshCom->Set_AnimationSet(30);
	}



}

CPlayer* CPlayer::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CPlayer*	pInstance = new CPlayer(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject()))
		Engine::Safe_Release(pInstance);

	return pInstance;
}

void CPlayer::Free(void)
{
	m_mSphere->Release();
	Engine::CGameObject::Free();
}

void CPlayer::SetUp_OnTerrain(void)
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

	m_pTransformCom->Set_Pos(vPos.x, fHeight, vPos.z);
}

HRESULT CPlayer::SetUp_ConstantTable(LPD3DXEFFECT& pEffect)
{
	_matrix			matWorld, matView, matProj;

	m_pTransformCom->Get_WorldMatrix(&matWorld);
	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
	m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &matProj);

	pEffect->SetMatrix("g_matWorld", &matWorld);
	pEffect->SetMatrix("g_matView", &matView);
	pEffect->SetMatrix("g_matProj", &matProj);

	//const D3DLIGHT9*		pLight = Engine::Get_LightInfo(1);
	//NULL_CHECK_RETURN(pLight, S_OK);


	//pEffect->SetVector("g_vLightDir", &_vec4(pLight->Direction, 0.f));
	//pEffect->SetVector("g_vLightDiffuse", (_vec4*)&pLight->Diffuse);
	//pEffect->SetVector("g_vLightSpecular", (_vec4*)&pLight->Specular);
	//pEffect->SetVector("g_vLightAmbient", (_vec4*)&pLight->Ambient);


	//D3DMATERIAL9		tMtrlInfo;
	//ZeroMemory(&tMtrlInfo, sizeof(D3DMATERIAL9));

	//tMtrlInfo.Diffuse = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
	//tMtrlInfo.Specular = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
	//tMtrlInfo.Ambient = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
	//tMtrlInfo.Emissive = D3DXCOLOR(0.f, 0.f, 0.f, 1.f);
	//tMtrlInfo.Power = 10.f;

	//pEffect->SetVector("g_vMtrlDiffuse", (_vec4*)&tMtrlInfo.Diffuse);
	//pEffect->SetVector("g_vMtrlSpecular", (_vec4*)&tMtrlInfo.Specular);
	//pEffect->SetVector("g_vMtrlAmbient", (_vec4*)&tMtrlInfo.Ambient);

	//_vec4		vCamPos;
	//D3DXMatrixInverse(&matView, NULL, &matView);
	//memcpy(&vCamPos, &matView.m[3][0], sizeof(_vec4));

	//pEffect->SetVector("g_vCamPos", &vCamPos);
	//pEffect->SetFloat("g_fPower", tMtrlInfo.Power);

	return S_OK;
}

void CPlayer::Player_Animation()
{
	_float tempPercent = m_pDynamicMeshCom->Get_AniPercent();
	if (m_bPushCtrl)
	{
		if (m_bMove)
		{
			if (!m_bJump)
			{
				Play_MoveSound(L"SW_Foley_Six_MetalSolid_Step_Sneak_01.ogg");

			}
			if (!m_bLighter)
			{
				m_ianimationKey = 0;
				m_iRightAnimationKey = 0;
			}
			else
			{
				m_ianimationKey = 82;
				m_iRightAnimationKey = 82;
			}
			if (m_bTurnEnd)
			{
				m_dAnimationCount = 0.01;
				m_dRightAnimationCount = 0.01;
				m_bTurnEnd = false;
			}
			else if (m_bClimeEnd || m_bPullDownGripEnd)
			{
				m_dRightAnimationCount = 0.01;
				m_dAnimationCount = 0.01;
				m_bClimeEnd = false;
				m_bPullDownGripEnd = false;
			}
			else
			{
				m_dRightAnimationCount = 0.25;
				m_dAnimationCount = 0.25;
			}
		}
		else
		{
			if (!m_bLighter)
			{
				m_ianimationKey = 27;
				m_iRightAnimationKey = 27;
			}
			else
			{
				m_ianimationKey = 83;
				m_iRightAnimationKey = 83;
			}
			if (m_bTurnEnd)
			{
				m_dAnimationCount = 0.01;
				m_dRightAnimationCount = 0.01;
				m_bTurnEnd = false;
			}
			else if (m_bClimeEnd || m_bPullDownGripEnd)
			{
				m_dAnimationCount = 0.01;
				m_dRightAnimationCount = 0.01;
				m_bClimeEnd = false;
				m_bPullDownGripEnd = false;
			}
			else
			{
				m_dAnimationCount = 0.25;
				m_dRightAnimationCount = 0.25;
			}
		}
		if (m_bTurn)
		{
			switch (m_ePreDir)
			{
			case DIR_LEFT:
				switch (m_eDir)
				{
				case DIR_RIGHT:
					m_ianimationKey = 6;
					m_dAnimationCount = 0.01;
					m_iRightAnimationKey = 6;
					m_dRightAnimationCount = 0.01;

					break;
				case DIR_TOP:
					m_ianimationKey = 8;
					m_dAnimationCount = 0.01;
					m_iRightAnimationKey = 8;
					m_dRightAnimationCount = 0.01;
					break;
				case DIR_BOTTOM:
					m_ianimationKey = 9;
					m_dAnimationCount = 0.01;
					m_iRightAnimationKey = 9;
					m_dRightAnimationCount = 0.01;
					break;
				}
				break;
			case DIR_RIGHT:
				switch (m_eDir)
				{
				case DIR_LEFT:
					m_ianimationKey = 6;
					m_dAnimationCount = 0.01;
					m_iRightAnimationKey = 6;
					m_dRightAnimationCount = 0.01;
					break;
				case DIR_TOP:
					m_ianimationKey = 9;
					m_dAnimationCount = 0.01;
					m_iRightAnimationKey = 9;
					m_dRightAnimationCount = 0.01;
					break;
				case DIR_BOTTOM:
					m_ianimationKey = 8;
					m_dAnimationCount = 0.01;
					m_iRightAnimationKey = 8;
					m_dRightAnimationCount = 0.01;
					break;
				}
				break;
			case DIR_TOP:
				switch (m_eDir)
				{
				case DIR_LEFT:
					m_ianimationKey = 9;
					m_dAnimationCount = 0.01;
					m_iRightAnimationKey = 9;
					m_dRightAnimationCount = 0.01;
					break;
				case DIR_RIGHT:
					m_ianimationKey = 8;
					m_dAnimationCount = 0.01;
					m_iRightAnimationKey = 8;
					m_dRightAnimationCount = 0.01;
					break;
				case DIR_BOTTOM:
					m_ianimationKey = 6;
					m_dAnimationCount = 0.01;
					m_iRightAnimationKey = 6;
					m_dRightAnimationCount = 0.01;
					break;
				}
				break;
			case DIR_BOTTOM:
				switch (m_eDir)
				{
				case DIR_LEFT:
					m_ianimationKey = 8;
					m_dAnimationCount = 0.01;
					m_iRightAnimationKey = 8;
					m_dRightAnimationCount = 0.01;
					break;
				case DIR_RIGHT:
					m_ianimationKey = 9;
					m_dAnimationCount = 0.01;
					m_iRightAnimationKey = 9;
					m_dRightAnimationCount = 0.01;
					break;
				case DIR_TOP:
					m_ianimationKey = 6;
					m_dAnimationCount = 0.01;
					m_iRightAnimationKey = 6;
					m_dRightAnimationCount = 0.01;
					break;
				}
				break;
			}
		}
	}
	else if (m_bRun)
	{
		if (m_bMove)
		{
			if (!m_bJump)
			{
				Play_MoveSound(L"SW_Foley_Six_MetalSolid_Step_Run_01.ogg");

			}
			if (!m_bLighter)
			{
				m_ianimationKey = 70;
				m_iRightAnimationKey = 70;
			}
			else
			{
				m_ianimationKey = 77;
				m_iRightAnimationKey = 77;
			}

			if (m_bTurnEnd)
			{
				m_dAnimationCount = 0.01;
				m_dRightAnimationCount = 0.01;
				m_bTurnEnd = false;
			}
			else if (m_bClimeEnd || m_bPullDownGripEnd)
			{
				m_dAnimationCount = 0.01;
				m_dRightAnimationCount = 0.01;
				m_bClimeEnd = false;
				m_bPullDownGripEnd = false;
			}
			else
			{
				m_dAnimationCount = 0.25;
				m_dRightAnimationCount = 0.25;
			}
		}
		else
		{
			if (!m_bLighter)
			{
				m_ianimationKey = 66;
				m_iRightAnimationKey = 66;
			}
			else
			{
				m_ianimationKey = 66;
				m_iRightAnimationKey = 66;
			}

			if (m_bTurnEnd)
			{
				m_dAnimationCount = 0.01;
				m_dRightAnimationCount = 0.01;
				m_bTurnEnd = false;
			}
			else if (m_bClimeEnd || m_bPullDownGripEnd)
			{
				m_dAnimationCount = 0.01;
				m_dRightAnimationCount = 0.01;
				m_bClimeEnd = false;
				m_bPullDownGripEnd = false;
			}
			else
			{
				m_dAnimationCount = 0.25;
				m_dRightAnimationCount = 0.25;
			}
		}
		if (m_bTurn)
		{
			switch (m_ePreDir)
			{
			case DIR_LEFT:
				switch (m_eDir)
				{
				case DIR_RIGHT:
					m_ianimationKey = 61;
					m_dAnimationCount = 0.01;
					m_iRightAnimationKey = 61;
					m_dRightAnimationCount = 0.01;
					break;
				case DIR_TOP:
					m_ianimationKey = 62;
					m_dAnimationCount = 0.01;
					m_iRightAnimationKey = 62;
					m_dRightAnimationCount = 0.01;

					break;
				case DIR_BOTTOM:
					m_ianimationKey = 125;
					m_dAnimationCount = 0.01;
					m_iRightAnimationKey = 125;
					m_dRightAnimationCount = 0.01;
					break;
				}
				break;
			case DIR_RIGHT:
				switch (m_eDir)
				{
				case DIR_LEFT:
					m_ianimationKey = 61;
					m_dAnimationCount = 0.01;
					m_iRightAnimationKey = 61;
					m_dRightAnimationCount = 0.01;
					break;
				case DIR_TOP:
					m_ianimationKey = 125;
					m_dAnimationCount = 0.01;
					m_iRightAnimationKey = 125;
					m_dRightAnimationCount = 0.01;
					break;
				case DIR_BOTTOM:
					m_ianimationKey = 62;
					m_dAnimationCount = 0.01;
					m_iRightAnimationKey = 62;
					m_dRightAnimationCount = 0.01;
					break;
				}
				break;
			case DIR_TOP:
				switch (m_eDir)
				{
				case DIR_LEFT:
					m_ianimationKey = 125;
					m_dAnimationCount = 0.01;
					m_iRightAnimationKey = 125;
					m_dRightAnimationCount = 0.01;
					break;
				case DIR_RIGHT:
					m_ianimationKey = 62;
					m_dAnimationCount = 0.01;
					m_iRightAnimationKey = 62;
					m_dRightAnimationCount = 0.01;
					break;
				case DIR_BOTTOM:
					m_ianimationKey = 61;
					m_dAnimationCount = 0.01;
					m_iRightAnimationKey = 61;
					m_dRightAnimationCount = 0.01;
					break;
				}
				break;
			case DIR_BOTTOM:
				switch (m_eDir)
				{
				case DIR_LEFT:
					m_ianimationKey = 62;
					m_dAnimationCount = 0.01;
					m_iRightAnimationKey = 62;
					m_dRightAnimationCount = 0.01;
					break;
				case DIR_RIGHT:
					m_ianimationKey = 125;
					m_dAnimationCount = 0.01;
					m_iRightAnimationKey = 125;
					m_dRightAnimationCount = 0.01;
					break;
				case DIR_TOP:
					m_ianimationKey = 61;
					m_dAnimationCount = 0.01;
					m_iRightAnimationKey = 61;
					m_dRightAnimationCount = 0.01;
					break;

					break;
				}
				break;
			}
		}
	}
	else
	{
		if (m_bMove)
		{
			if (!m_bJump)
			{
				Play_MoveSound(L"SW_Foley_Six_MetalSolid_Step_Walk_01.ogg");
			}
			if (!m_bLighter)
			{
				m_ianimationKey = 134;
				m_iRightAnimationKey = 134;
			}
			else
			{
				m_ianimationKey = 134;
				m_iRightAnimationKey = 340;
			}
			if (m_bTurnEnd)
			{
				m_dAnimationCount = 0.01;
				m_dRightAnimationCount = 0.01;
				m_bTurnEnd = false;
				m_bWalkSoundHalf = false;
			}
			else if (m_bClimeEnd || m_bPullDownGripEnd)
			{
				m_dAnimationCount = 0.01;
				m_dRightAnimationCount = 0.01;
				m_bClimeEnd = false;
				m_bPullDownGripEnd = false;
			}
			else
			{
				m_dAnimationCount = 0.25;
				m_dRightAnimationCount = 0.25;
			}
		}
		else
		{

			if (!m_bLighter)
			{
				m_ianimationKey = 30;
				m_iRightAnimationKey = 30;
			}
			else
			{
				m_ianimationKey = 30;
				m_iRightAnimationKey = 340;
			}
			if (m_bTurnEnd)
			{
				m_dAnimationCount = 0.01;
				m_dRightAnimationCount = 0.01;
				m_bTurnEnd = false;
			}
			else if (m_bClimeEnd || m_bPullDownGripEnd || m_bStartAniEnd)
			{
				m_dAnimationCount = 0.01;
				m_dRightAnimationCount = 0.01;
				m_bClimeEnd = false;
				m_bPullDownGripEnd = false;
				m_bStartAniEnd = false;
			}
			else if (m_bLeftCagePullDownReset)
			{
				m_bLeftCagePullDownReset = false;
				m_dAnimationCount = 0.01;
				m_dRightAnimationCount = 0.01;
			}
			else if (m_bRightCagePullDownReset)
			{
				m_bRightCagePullDownReset = false;
				m_dAnimationCount = 0.01;
				m_dRightAnimationCount = 0.01;
			}
			else
			{
				m_dAnimationCount = 0.25;
				m_dRightAnimationCount = 0.25;
				//	m_dAnimationCount = 0.01;
			}
		}
		if (m_bTurn)
		{
			switch (m_ePreDir)
			{
			case DIR_LEFT:
				switch (m_eDir)
				{
				case DIR_RIGHT:
					m_ianimationKey = 121;
					m_dAnimationCount = 0.01;
					m_iRightAnimationKey = 121;
					m_dRightAnimationCount = 0.01;

					break;
				case DIR_TOP:
					m_ianimationKey = 124;
					m_dAnimationCount = 0.01;
					m_iRightAnimationKey = 124;
					m_dRightAnimationCount = 0.01;

					break;
				case DIR_BOTTOM:
					m_ianimationKey = 125;
					m_dAnimationCount = 0.01;
					m_iRightAnimationKey = 125;
					m_dRightAnimationCount = 0.01;

					break;
				}
				break;
			case DIR_RIGHT:
				switch (m_eDir)
				{
				case DIR_LEFT:
					m_ianimationKey = 121;
					m_dAnimationCount = 0.01;
					m_iRightAnimationKey = 121;
					m_dRightAnimationCount = 0.01;

					break;
				case DIR_TOP:
					m_ianimationKey = 125;
					m_dAnimationCount = 0.01;
					m_iRightAnimationKey = 125;
					m_dRightAnimationCount = 0.01;

					break;
				case DIR_BOTTOM:
					m_ianimationKey = 124;
					m_dAnimationCount = 0.01;
					m_iRightAnimationKey = 124;
					m_dRightAnimationCount = 0.01;

					break;
				}
				break;
			case DIR_TOP:
				switch (m_eDir)
				{
				case DIR_LEFT:
					m_ianimationKey = 125;
					m_dAnimationCount = 0.01;
					m_iRightAnimationKey = 125;
					m_dRightAnimationCount = 0.01;

					break;
				case DIR_RIGHT:
					m_ianimationKey = 124;
					m_dAnimationCount = 0.01;
					m_iRightAnimationKey = 124;
					m_dRightAnimationCount = 0.01;

					break;
				case DIR_BOTTOM:
					m_ianimationKey = 121;
					m_dAnimationCount = 0.01;
					m_iRightAnimationKey = 121;
					m_dRightAnimationCount = 0.01;

					break;
				}
				break;
			case DIR_BOTTOM:
				switch (m_eDir)
				{
				case DIR_LEFT:
					m_ianimationKey = 124;
					m_dAnimationCount = 0.01;
					m_iRightAnimationKey = 124;
					m_dRightAnimationCount = 0.01;

					break;
				case DIR_RIGHT:
					m_ianimationKey = 125;
					m_dAnimationCount = 0.01;
					m_iRightAnimationKey = 125;
					m_dRightAnimationCount = 0.01;

					break;
				case DIR_TOP:
					m_ianimationKey = 121;
					m_dAnimationCount = 0.01;
					m_iRightAnimationKey = 121;
					m_dRightAnimationCount = 0.01;

					break;

					break;
				}
				break;
			}
		}


	}
	if (m_bJump)
	{
		Engine::Play_Sound(L"SW_six_raincoat_jump_2a.ogg", Engine::CHANNELID::PLAYER, m_bJumpSound, false);
		if (true == m_pDynamicMeshCom->Is_AnimationSetEnd() && m_bJumpFalling)
		{
			m_bFalling = true;
		}
		m_ianimationKey = 120;
		m_bJumpFalling = true;
		m_dAnimationCount = 0.01;
		m_iRightAnimationKey = 120;
		m_dRightAnimationCount = 0.01;
	}
	if (m_bLanding)
	{
		Engine::Play_Sound(L"SW_six_concrete_land_1a.ogg", Engine::CHANNELID::PLAYER, m_bLandSound, false);
		m_ianimationKey = 97;
		m_dAnimationCount = 0.01;
		m_iRightAnimationKey = 97;
		m_dRightAnimationCount = 0.01;
		if (m_pDynamicMeshCom->Is_AnimationSetEnd())
		{
			m_bLanding = false;
			m_bLandSound = false;
			m_bJumpSound = false;
		}
	}
	if (m_bClime)
	{
		Engine::Play_Sound(L"SW_six_raincoat_jump_2b.ogg", Engine::CHANNELID::PLAYER, m_bClimbSound, false);
		m_ianimationKey = 245;
		m_dAnimationCount = 0.01;
		m_iRightAnimationKey = 245;
		m_dRightAnimationCount = 0.01;
	}
	if (m_bClimeAir)
	{
		m_bJump = false;
		m_ianimationKey = 231;
		m_dAnimationCount = 0.01;
		m_iRightAnimationKey = 231;
		m_dRightAnimationCount = 0.01;
	}
	if (m_bFalling)
	{
		m_ianimationKey = 174;
		m_dAnimationCount = 0.25;
		m_iRightAnimationKey = 174;
		m_dRightAnimationCount = 0.25;
	}
	if (m_bCatchLeech)
	{
		m_bCantMove = true;
		m_ianimationKey = 94;
		m_dAnimationCount = 0.25;
		m_iRightAnimationKey = 94;
		m_dRightAnimationCount = 0.25;
	}
	if (m_bLeechDeath)
	{
		if (tempPercent > 0.5f)
			Engine::Play_Sound(L"SW_Music_Stinger_CorpusMembrane.ogg", Engine::CHANNELID::EFFECT, m_bLeechDeathSound, false);
		m_ianimationKey = 95;
		m_dAnimationCount = 0.25;
		m_iRightAnimationKey = 95;
		m_dRightAnimationCount = 0.25;
	}
	if (m_bJanitorDeath)
	{
		_vec3 vGrabPos;
		if (!m_bJanitorHand)
		{
			CJanitor* pJanitor = dynamic_cast<CJanitor*>(m_pJanitor);
			vGrabPos = pJanitor->Get_GrabPos(m_iJanitorHand);
			switch (m_iJanitorHand)
			{
			case 1:
				vGrabPos.y -= 0.8f;
				break;
			case 2:
				vGrabPos.y -= 0.5f;
				break;
			}
		}
		else
		{
			CJanitorArm* pJanitor = dynamic_cast<CJanitorArm*>(m_pJanitor);
			vGrabPos = pJanitor->Get_GrabPos(m_iJanitorHand);
			vGrabPos.y -= 0.5f;
		}

		m_pTransformCom->Set_Pos(vGrabPos);
		m_bCantMove = true;
		m_ianimationKey = 165;
		m_dAnimationCount = 0.25;
		m_iRightAnimationKey = 165;
		m_dRightAnimationCount = 0.25;
	}
	if (m_bPullDownGrip)
	{
		Engine::Play_Sound(L"SW_hatch_vent_slam_a.ogg", Engine::CHANNELID::HATCH, m_bHatchOpenSoundEnd, false);
		m_ianimationKey = 225;
		m_dAnimationCount = 0.01;
		m_iRightAnimationKey = 225;
		m_dRightAnimationCount = 0.01;
	}
	else if (m_bPullGrip)
	{
		if (m_fGrabCount > 1.f)
			m_bPullDownGrip = true;
		m_ianimationKey = 159;
		m_dAnimationCount = 0.25;
		m_iRightAnimationKey = 159;
		m_dRightAnimationCount = 0.25;
	}
	else if (m_bCatchGrip)
	{
		m_ianimationKey = 160;
		m_dAnimationCount = 0.25;
		m_iRightAnimationKey = 160;
		m_dRightAnimationCount = 0.25;
	}
	if (m_bPushDoor)
	{
		if (!m_bPushDoorOpen)
		{
			m_ianimationKey = 184;
			m_dAnimationCount = 0.25;
			m_iRightAnimationKey = 184;
			m_dRightAnimationCount = 0.25;
			if (true == m_pDynamicMeshCom->Is_AnimationSetEnd() && tempPercent < 1.f)
			{
				m_bPushDoorOpen = true;
				m_bCantMove = false;
				m_eDir = DIR_RIGHT;
				m_ePreDir = DIR_RIGHT;
			}
		}
	}
	if (m_bPushLever)
	{
		m_ianimationKey = 84;
		m_dAnimationCount = 0.25;
		m_iRightAnimationKey = 84;
		m_dRightAnimationCount = 0.25;
		if (true == m_pDynamicMeshCom->Is_AnimationSetEnd() && tempPercent < 1.f)
		{
			m_bPushLever = false;
			m_bCantMove = false;
		}
	}
	if (m_bLeftCagePullDown || m_bRightCagePullDown)
	{
		CInteractionObject* pObjectAni = nullptr;
		CInteractionObject* pObject = nullptr;
		CInteractionObject* pCage = dynamic_cast<CInteractionObject*>(Engine::Get_GameObject(L"02.GameLogic", L"Dyn_Obj_BossCage_13"));
		if (m_bLeftCagePull)
		{
			pObjectAni = dynamic_cast<CInteractionObject*>(Engine::Get_GameObject(L"02.GameLogic", L"Dyn_Obj_doorCrush_AnimatedBar_19"));
			pObject = dynamic_cast<CInteractionObject*>(Engine::Get_GameObject(L"02.GameLogic", L"Sta_Obj_SKM_PrisonContainer04_doorCrush_Bar_17"));
			pObjectAni->Set_NotRender(false);
			pObject->Set_NotRender(true);
			CJanitor* pJanitor = dynamic_cast<CJanitor*>(Engine::Get_GameObject(L"02.GameLogic", L"Dyn_Monster_Mesh_Janitor_6"));
			if (pJanitor->Get_State() != CJanitor::LAST_BREAK && !m_bMakeBreak)
			{
				pJanitor->Set_State(CJanitor::LAST_BREAK);
				m_bMakeBreak = true;
			}
			if (pCage->Get_CageState() == CAGESTATE::wobble1)
				pCage->Set_CageState(CAGESTATE::crush2);

		}
		else
		{
			pObjectAni = dynamic_cast<CInteractionObject*>(Engine::Get_GameObject(L"02.GameLogic", L"Dyn_Obj_doorCrush_AnimatedBar_20"));
			pObject = dynamic_cast<CInteractionObject*>(Engine::Get_GameObject(L"02.GameLogic", L"Sta_Obj_SKM_PrisonContainer04_doorCrush_Bar_18"));
			pObjectAni->Set_NotRender(false);
			pObject->Set_NotRender(true);
			if (pCage->Get_CageState() == CAGESTATE::wobble2)
				pCage->Set_CageState(CAGESTATE::crush3);
		}
		m_ianimationKey = 226;
		m_dAnimationCount = 0.25;
		m_iRightAnimationKey = 226;
		m_dRightAnimationCount = 0.25;
	}
	else if (m_bLeftCagePull || m_bRightCagePull)
	{

		m_ianimationKey = 192;
		m_dAnimationCount = 0.25;
		m_iRightAnimationKey = 192;
		m_dRightAnimationCount = 0.25;
	}
	else if (m_bLeftCageCatch || m_bRightCageCatch)
	{
		m_ianimationKey = 213;
		m_dAnimationCount = 0.25;
		m_iRightAnimationKey = 213;
		m_dRightAnimationCount = 0.25;
	}
	if (m_bBoxCarry)
	{
		if (!m_bBoxCarryMove)
		{
			m_ianimationKey = 213;
			m_dAnimationCount = 0.25;
			m_iRightAnimationKey = 213;
			m_dRightAnimationCount = 0.25;
		}
		else
		{
			switch (m_eBoxCarryDir)
			{
			case DIR_LEFT:
				switch (m_eDir)
				{
				case DIR_LEFT:
					m_ianimationKey = 199;
					m_dAnimationCount = 0.25;
					m_iRightAnimationKey = 199;
					m_dRightAnimationCount = 0.25;
					break;
				case DIR_RIGHT:
					m_ianimationKey = 202;
					m_dAnimationCount = 0.25;
					m_iRightAnimationKey = 202;
					m_dRightAnimationCount = 0.25;
					break;
				case DIR_TOP:
					m_ianimationKey = 194;
					m_dAnimationCount = 0.25;
					m_iRightAnimationKey = 194;
					m_dRightAnimationCount = 0.25;
					break;
				case DIR_BOTTOM:
					m_ianimationKey = 196;
					m_dAnimationCount = 0.25;
					m_iRightAnimationKey = 196;
					m_dRightAnimationCount = 0.25;
					break;
				}
				break;
			case DIR_RIGHT:
				switch (m_eDir)
				{
				case DIR_LEFT:
					m_ianimationKey = 202;
					m_dAnimationCount = 0.25;
					m_iRightAnimationKey = 202;
					m_dRightAnimationCount = 0.25;
					break;
				case DIR_RIGHT:
					m_ianimationKey = 199;
					m_dAnimationCount = 0.25;
					m_iRightAnimationKey = 199;
					m_dRightAnimationCount = 0.25;
					break;
				case DIR_TOP:
					m_ianimationKey = 196;
					m_dAnimationCount = 0.25;
					m_iRightAnimationKey = 196;
					m_dRightAnimationCount = 0.25;
					break;
				case DIR_BOTTOM:
					m_ianimationKey = 194;
					m_dAnimationCount = 0.25;
					m_iRightAnimationKey = 194;
					m_dRightAnimationCount = 0.25;
					break;
				}
				break;
			case DIR_TOP:
				switch (m_eDir)
				{
				case DIR_LEFT:
					m_ianimationKey = 196;
					m_dAnimationCount = 0.25;
					m_iRightAnimationKey = 196;
					m_dRightAnimationCount = 0.25;
					break;
				case DIR_RIGHT:
					m_ianimationKey = 194;
					m_dAnimationCount = 0.25;
					m_iRightAnimationKey = 194;
					m_dRightAnimationCount = 0.25;
					break;
				case DIR_TOP:
					m_ianimationKey = 199;
					m_dAnimationCount = 0.25;
					m_iRightAnimationKey = 199;
					m_dRightAnimationCount = 0.25;
					break;
				case DIR_BOTTOM:
					m_ianimationKey = 202;
					m_dAnimationCount = 0.25;
					m_iRightAnimationKey = 202;
					m_dRightAnimationCount = 0.25;
					break;
				}
				break;
			case DIR_BOTTOM:
				switch (m_eDir)
				{
				case DIR_LEFT:
					m_ianimationKey = 194;
					m_dAnimationCount = 0.25;
					m_iRightAnimationKey = 194;
					m_dRightAnimationCount = 0.25;
					break;
				case DIR_RIGHT:
					m_ianimationKey = 196;
					m_dAnimationCount = 0.25;
					m_iRightAnimationKey = 196;
					m_dRightAnimationCount = 0.25;
					break;
				case DIR_TOP:
					m_ianimationKey = 202;
					m_dAnimationCount = 0.25;
					m_iRightAnimationKey = 202;
					m_dRightAnimationCount = 0.25;
					break;
				case DIR_BOTTOM:
					m_ianimationKey = 199;
					m_dAnimationCount = 0.25;
					m_iRightAnimationKey = 199;
					m_dRightAnimationCount = 0.25;
					break;
				}
				break;
			}
		}
	}
	if (m_bLighterIgnight)
	{
		if (m_bPushCtrl)
		{
			if (m_bMove)
			{
			}
			else
			{

				m_ianimationKey = 259;
				m_dAnimationCount = 0.25;
				m_iRightAnimationKey = 259;
				m_dRightAnimationCount = 0.25;
				if (tempPercent > 0.2f)
				{
					Engine::Play_Sound(L"LighterOn.ogg", Engine::CHANNELID::LIGHTER, m_bLighterOnSound, false);
					m_bLighterOffSound = false;
					m_bLighter = true;
					m_bLighterIgnight = false;
				}
			}
		}
		else if (m_bRun)
		{
			if (m_bMove)
			{
			}
			else
			{
			}
		}
		else
		{
			if (m_bMove)
			{
			}
			else
			{

				m_ianimationKey = 81;
				m_dAnimationCount = 0.25;
				m_iRightAnimationKey = 81;
				m_dRightAnimationCount = 0.25;
				if (tempPercent > 0.5f)
				{
					Engine::Play_Sound(L"LighterOn.ogg", Engine::CHANNELID::LIGHTER, m_bLighterOnSound, false);
					m_bLighterOffSound = false;
					m_bLighter = true;
				}
			}
		}
		if (m_pDynamicMeshCom->Is_AnimationSetEnd() == true)
			m_bLighterIgnight = false;
	}

	if (m_bStartAni)
	{
		Engine::Play_Sound(L"WakeUp1.ogg", Engine::CHANNELID::PLAYER, m_bWakeUpSound, false);
		m_ianimationKey = 58;
		m_dAnimationCount = 0.01;
		m_iRightAnimationKey = 58;
		m_dRightAnimationCount = 0.01;
	}

	m_pDynamicMeshCom->Set_AnimationSet(m_ianimationKey, m_dAnimationCount);
	m_pDynmicRIght->Set_AnimationSet(m_iRightAnimationKey, m_dRightAnimationCount);

}

void CPlayer::Gravity(const _float& fTimeDelta)
{
	_vec3 vPos;
	_vec3 vtempPos;
	vPos = *m_pTransformCom->Get_Info(Engine::INFO_POS);
	vtempPos = *m_pTransformCom->Get_Info(Engine::INFO_POS);
	if (!m_bClime && !m_bClimeAir && !m_bJanitorDeath)
	{
		if (m_bJump)
		{
			m_fAcc += 0.05f * fTimeDelta;
		}
		else if (m_bFalling)
		{
		}
		else
			m_fAcc = 0.15f;

		vPos.y += CPhysicMgr::ConvertGravity(&vPos, m_fJumpPower, m_fAcc);

		float y = m_pNaviCom->Navi(1, vPos);
		if (!m_bFallDown)
		{
			if (y > vPos.y)
			{
				_bool bSlideflag = false;
				_int count = 0;
				if (m_pNaviCom->Check_Sliding(vPos, vPos.y, bSlideflag))
				{
					m_bFalling = true;
					m_bCantMove = true;
					vPos.y = m_pNaviCom->Navi(1, vPos);
					if (bSlideflag)
					{
						if (m_bJump)
							m_bLanding = true;
						if (m_iSlidingCount > 1)
						{
							m_bLanding = true;
						}
						m_iSlidingCount = 0;
						m_bJump = false;
						m_bFalling = false;
						m_bCantMove = false;
						m_bOnCol = false;
						m_bJumpFalling = false;
						m_fJumpPower = 0.f;
					}
					m_iSlidingCount++;
				}
				else
				{
					vPos.y = y;
					m_fJumpPower = 0.f;
					m_iSlidingCount = 0;
					if (m_bJump || m_bOnCol || m_bFalling)
						m_bLanding = true;
					m_bJump = false;
					m_bFalling = false;
					m_bJumpFalling = false;
					m_bOnCol = false;
				}
			}
			else
			{
				Engine::CGameObject* tempBox = (Engine::Get_GameObject(L"02.GameLogic", L"Sta_Obj_WoodBox_10"));
				Engine::CGameObject* tempCol = nullptr;
				if (m_pTransformCom->Get_Info(Engine::INFO_POS)->x > 90)
					tempCol = (Engine::Get_GameObject(L"Collider2", L"Col_Col_0"));
				else
					tempCol = (Engine::Get_GameObject(L"Collider1", L"Col_Col_35"));
				if (!m_bJump && !m_bClime)
				{
					if (!CollisionMgr::CollisionObjectPlayerAABB_Y(vPos, tempBox) && !CollisionMgr::CollisionColPlayerAABB_Y(vPos, tempCol))
					{
						m_bFalling = true;
					}
					else
					{
						if (m_bFalling)
						{
							m_bOnCol = true;
							m_bLanding = true;
							m_bJump = false;
							m_bFalling = false;
							m_bJumpFalling = false;
						}
					}
				}
				//m_bFalling = true;
			}
		}
		if (vPos.y < -15.f)
		{
			m_bLoadScene = true;
		}
		m_pTransformCom->Set_Pos(&vPos);
		Set_ColWorld(m_vColScale, vPos);
	}
}

void CPlayer::Turn()
{
	if (m_bTurn || m_bClimeAir || m_bClime || m_bCatchLeech || m_bLeechDeath || m_bLeechDeathEnd || m_bPullDownGrip ||
		m_bJanitorDeath || m_bJanitorDeathEnd || m_bLeftCagePullDown || m_bLeftCagePull || m_bRightCagePullDown || m_bRightCagePull || m_bStartAni)
	{
		_vec3	vPos, vDir;
		m_pTransformCom->Get_Info(Engine::INFO_POS, &vPos);
		const Engine::D3DXFRAME_DERIVED* pBone = m_pDynamicMeshCom->Get_FrameByName("skinned_root_bn");
		NULL_CHECK(pBone, 0);
		_matrix boneMat = pBone->CombinedTransformationMatrix * *m_pTransformCom->Get_WorldMatrixPointer();
		Engine::CGameObject* pBox = dynamic_cast<CInteractionObject*>(Engine::Get_GameObject(L"02.GameLogic", L"Sta_Obj_WoodBox_10"));

		_vec3 tempPos = { boneMat._41, boneMat._42, boneMat._43 };
		m_aniPos = tempPos;
		//Set_ColWorld(m_vColScale, tempPos);
		if (vPos.x > 95.f &&vPos.x <= 120.f)
		{
			CollisionMgr::CollisionObjectToPlayerAABB(this, pBox);
			//Set_ColWorld(m_vColScale, *m_pTransformCom->Get_Info(Engine::INFO_POS));
			Engine::CLayer*	pLayerWall = Engine::Get_Layer(L"Collider2");
			_bool tempbool;
			if (pLayerWall != nullptr)
				CollisionMgr::CollisionWallToPlayer(pLayerWall, this, tempbool);
		}
		Set_ColWorld(m_vColScale, tempPos);
		if (aniCount == 0)
		{
			m_vPrePos = vPos;
			vDir = tempPos - m_vPrePos;
		}
		else
			vDir = tempPos - m_vPrePos;
		_vec3 vtempPos = _vec3(0.f, 0.f, 0.f);
		if (vDir != _vec3(0.f, 0.f, 0.f))
		{
			if (!m_bJanitorDeath)
				vtempPos = m_pNaviCom->Move_OnNaviMesh(&m_vPrePos, &vDir, m_bNaviFlag);
		}
		if (vtempPos == m_vPrePos && vDir != _vec3(0.f, 0.f, 0.f))
		{
			tempPos = tempPos - vDir;

			m_pTransformCom->Set_Pos(vPos - vDir);
			Set_ColWorld(m_vColScale, tempPos);
		}
		m_vPrePos = tempPos;
		aniCount++;
		_float tempPercent = m_pDynamicMeshCom->Get_AniPercent();
		if (tempPercent > 0.5f)
		{
			if (m_bJanitorDeath)
			{
				m_bLoadScene = true;
				m_bJanitorDeathEnd = true;
			}
		}
		if (tempPercent > 0.85f)
		{
			if (m_bLeftCagePullDown)
			{
				m_bLeftCagePullDownReset = true;
				m_bLeftCagePullDown = false;
				m_bLeftCagePull = false;
				m_bCantMove = false;
				m_bMakeBreak = false;

				m_bCantMove = false;
				m_pTransformCom->Set_Pos(tempPos);
				Set_ColWorld(m_vColScale, tempPos);
				m_bEndAniPos = true;
				m_ePreDir = m_eDir;
				aniCount = 0;
				return;
			}
			else if (m_bRightCagePullDown)
			{
				m_bRightCagePullDownReset = true;
				m_bRightCagePullDown = false;
				m_bRightCagePull = false;
				m_bCantMove = false;
				m_bMakeBreak = false;

				m_bCantMove = false;
				m_pTransformCom->Set_Pos(tempPos);
				Set_ColWorld(m_vColScale, tempPos);
				m_bEndAniPos = true;
				m_ePreDir = m_eDir;
				aniCount = 0;
				return;
			}



		}
		if (true == m_pDynamicMeshCom->Is_AnimationSetEnd() && tempPercent < 1.f)
		{
			if (m_bLeechDeathEnd || m_bJanitorDeathEnd)
			{
				m_bLoadScene = true;
			}
			else
			{
				if (m_bLeechDeath)
				{
					m_bLeechDeathEnd = true;
				}
				else if (m_bJanitorDeath)
				{
					//m_bLoadScene = true;
					//m_bJanitorDeathEnd = true;
				}
				else
				{
					if (m_bTurn)
					{
						switch (m_eDir)
						{
						case DIR_LEFT:
							m_pTransformCom->Set_Angle(_vec3(D3DXToRadian(0.f), D3DXToRadian(0.f), D3DXToRadian(0.f)));
							break;
						case DIR_RIGHT:
							m_pTransformCom->Set_Angle(_vec3(D3DXToRadian(0.f), D3DXToRadian(180.f), D3DXToRadian(0.f)));
							break;
						case DIR_TOP:
							m_pTransformCom->Set_Angle(_vec3(D3DXToRadian(0.f), D3DXToRadian(90.f), D3DXToRadian(0.f)));
							break;
						case DIR_BOTTOM:
							m_pTransformCom->Set_Angle(_vec3(D3DXToRadian(0.f), D3DXToRadian(-90.f), D3DXToRadian(0.f)));
							break;
						}
						m_bTurn = false;
						m_bTurnEnd = true;
					}
					else if (m_bClimeAir)
					{
						m_bClimeAir = false;
					}
					else if (m_bClime)
					{
						m_bClimbSound = false;
						m_bClime = false;
						m_bClimeEnd = true;
						m_bClimeReady = false;
					}
					else if (m_bPullDownGrip)
					{
						m_eDir = DIR_RIGHT;
						m_ePreDir = DIR_RIGHT;
						m_bPullDownGrip = false;
						m_bCatchGrip = false;
						m_bCantMove = false;
						m_bPullGrip = false;
						m_fGrabCount = 0.f;
						m_bHatchOpen = true;
						m_bPullDownGripEnd = true;
					}

					else if (m_bLeftCagePull)
					{
						m_bLeftCagePullDown = true;
					}

					else if (m_bRightCagePull)
					{
						m_bRightCagePullDown = true;
					}
					else if (m_bStartAni)
					{
						m_bStartAni = false;
						m_bStartAniEnd = true;
					}

					m_bCantMove = false;
					m_pTransformCom->Set_Pos(tempPos);
					Set_ColWorld(m_vColScale, tempPos);
					//m_pDynamicMeshCom->Set_Matrix(m_fAngle);
					m_vTest = tempPos;
					m_bEndAniPos = true;
					m_ePreDir = m_eDir;
					aniCount = 0;
					if (m_bCatchLeech)
					{
						m_bLeechDeath = true;
					}
				}
			}
		}

	}

}

void CPlayer::Set_ColWorld(_vec3 vScale, _vec3 vPos)
{
	_matrix mScale, mPos;
	D3DXMatrixScaling(&mScale, vScale.x, vScale.y, vScale.z);
	D3DXMatrixTranslation(&mPos, vPos.x - 0.2f,
		vPos.y,
		vPos.z - 0.2f);
	m_ColWorld = mScale * mPos;
}


void CPlayer::Draw_Sphere()
{
	_matrix matWorld;
	_matrix mPos;
	//D3DXMatrixTranslation(&mPos,
	//	m_pTransformCom->Get_Info(Engine::INFO_POS)->x,
	//	m_pTransformCom->Get_Info(Engine::INFO_POS)->y,
	//	m_pTransformCom->Get_Info(Engine::INFO_POS)->z);
	D3DXMatrixTranslation(&mPos,
		Get_LightPos().x,
		Get_LightPos().y,
		Get_LightPos().z);
	matWorld = mPos;
	m_pGraphicDev->SetTransform(D3DTS_WORLD, &matWorld);
	m_pGraphicDev->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
	m_mSphere->DrawSubset(0);
	m_pGraphicDev->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
}

void CPlayer::Set_SavePoint()
{
	m_pTransformCom->Set_Pos(m_vCheckPoint);
	m_pCam->Set_At(m_vCheckPoint);
	m_pTransformCom->Set_Angle(_vec3(0.f, D3DXToRadian(180.f), 0.f));
	Engine::Stop_All();
	if (m_vCheckPoint == _vec3(57.f, -3.5f, -3.f))
	{
		Engine::Play_Sound(L"SW_Amb_Tutorial_VeryLargeRoom_4.ogg", Engine::CHANNELID::BGM, m_bResetBGM, true);
	}
	else if (m_vCheckPoint == _vec3(79.5f, -10.8f, 1.f))
	{
		Engine::Play_Sound(L"SW_amb_prison_distant_horn_2f.ogg", Engine::CHANNELID::BGM, m_bResetBGM, true);
	}
	else if (m_vCheckPoint == _vec3(98.f, -5.65f, 1.f))
	{
	}

	else if (m_vCheckPoint == _vec3(140.f, -5.65f, 5.5f))
	{
		Engine::Play_Sound(L"SW_music_GuestEntry_Corpus_Low.ogg", Engine::CHANNELID::BGM, m_bResetBGM, true);
	}


	if (m_bLeechDeathEnd)
	{
		Set_NaviMesh(L"Com_Navi_1");
	}
	else if (m_bJanitorDeathEnd)
	{
		Set_NaviMesh(L"Com_Navi_3");
	}

	m_pNaviCom->Set_Index_UsePos(m_vCheckPoint);
	m_fSpeed = 3.f;
	m_fJumpPower = 0.f;
	m_fAcc = 0.f;
	m_bPushCtrl = false;
	m_bPushCtrlTrigger = false;
	m_bTurn = false;
	m_bTurnEnd = false;
	m_bMove = false;
	m_bJump = false;
	m_bLanding = false;
	m_bRun = false;
	m_bClime = false;
	m_bClimeReady = false;
	m_bClimeAir = false;
	m_bClimeEnd = false;
	m_bFalling = false;
	m_bCantMove = false;
	m_bEndAniPos = false;
	m_bLeechChase = false;
	m_bCatchLeech = false;
	m_bLeechDeath = false;
	m_bLeechDeathEnd = false;
	m_vCheckPoint;
	m_eDir = DIR_RIGHT;
	m_ePreDir = DIR_RIGHT;
	m_ColWorld;
	m_bColl = false;
	m_bLoadScene = false;

	m_bCatchGrip = false;
	m_bPullGrip = false;
	m_bPullDownGrip = false;
	m_bTriggerGrip = false;
	m_bHatchOpen = false;
	m_bWoodFloor = false;

	m_bFallDown = false;

	m_bPushLeverTrigger = false;
	m_bPushLever = false;
	m_bBoxCarryTrigger = false;
	m_bBoxCarry = false;
	m_bBoxCarryMove = false;
	m_bNaviFlag = false;
	m_bBoxUpTrigger = false;
	m_bOnCol = false;
	m_bSpawnJionterBedRoom = false;
	m_bJumpFalling = false;
	m_bUpPipeTrigger = false;
	m_bJanitorDeath = false;
	m_bJanitorDeathEnd = false;

	m_bJanitorHand = false;
	m_bLastStage = false;
	m_bLeftCageTrigger = false;
	m_bRightCageTrigger = false;
	m_bLeftCageCatch = false;
	m_bRightCageCatch = false;
	m_bLeftCagePull = false;
	m_bRightCagePull = false;
	m_bLeftCagePullDown = false;
	m_bRightCagePullDown = false;
	m_bLighter = false;
	m_bLighterIgnight = false;
	m_bMakeBreak = false;


	m_bWalkSound = false;
	m_bWalkSoundHalf = false;
	m_bJanitorArmSound = false;
	m_bJanitorAirEffect = false;
	m_bJumpSound = false;
	m_bLandSound = false;
	m_bHatchOpenSound = false;
	m_bHatchOpenSoundEnd = false;
	m_bWoodFloorSound = false;
	m_bSceondBgm = false;
	m_bLeechDeathSound = false;
	m_bPipeBgmSound = false;
	m_bClimbSound = false;
	m_bLighterOnSound = false;
	m_bLighterOffSound = false;


	Engine::Set_LightInfo(1)->Diffuse = D3DXCOLOR(0.f, 0.f, 0.f, 1.f);
	Engine::Set_LightInfo(1)->Specular = D3DXCOLOR(0.f, 0.f, 0.f, 1.f);
	Engine::Set_LightInfo(1)->Ambient = D3DXCOLOR(0.f, 0.f, 0.f, 1.f);
	m_pDynamicMeshCom->Set_AnimationSet(30, 0.25);
	m_pDynmicRIght->Set_AnimationSet(30, 0.25);
	m_pDynamicMeshCom->Play_Animation(0.1f);
	m_pDynmicRIght->Play_Animation(0.1f);
}

void CPlayer::Play_MoveSound(const _tchar * name)
{
	Engine::Play_Sound(name, Engine::CHANNELID::PLAYER, m_bWalkSound, false);
	_float tempPercent = m_pDynamicMeshCom->Get_AniPercent();
	if (!m_bWalkSoundHalf && tempPercent > 0.5f)
	{
		m_bWalkSound = false;
		m_bWalkSoundHalf = true;
	}
	else if (true == m_pDynamicMeshCom->Is_AnimationSetEnd() && !m_bLanding)
	{
		m_pDynamicMeshCom->Reset_Animation();
		m_bWalkSound = false;
		m_bWalkSoundHalf = false;
	}
}

void CPlayer::TurnOffLighter(_bool flag)
{
	if (m_bLighter)
	{
		if (!flag)
		{
			Engine::Play_Sound(L"LighterOff.ogg", Engine::CHANNELID::LIGHTER, m_bLighterOffSound, false);
		}
		m_bLighterOnSound = false;

		m_bLighter = false;
		m_bLighterIgnight = false;
	}
}
