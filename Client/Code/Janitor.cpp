#include "stdafx.h"
#include "Janitor.h"
#include "Player.h"
#include "Collider.h"
#include "InteractionObject.h"
#include "Export_Function.h"

CJanitor::CJanitor(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CGameObject(pGraphicDev)
{

}

CJanitor::~CJanitor(void)
{

}



HRESULT CJanitor::Ready_GameObject(void)
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);
	D3DXCreateLine(m_pGraphicDev, &m_pLine);
	m_pLine->SetWidth(3.f);
	m_pLine->SetAntialias(true);
	m_iTurnMax = 1;
	return S_OK;
}

_int CJanitor::Update_GameObject(const _float& fTimeDelta)
{
	CInteractionObject* pCage = dynamic_cast<CInteractionObject*>(Engine::Get_GameObject(L"02.GameLogic", L"Dyn_Obj_BossCage_13"));
	if (g_bLoadFirst)
	{
		Set_First_BedRoom();
		return NO_EVENT;
	}
	if (pCage->Get_CageState() == CAGESTATE::crush3)
	{
		m_bStartAni = false;
		m_eState = DEATH;
		m_fEndCount += fTimeDelta;
		if (m_fEndCount > 3.f)
		{
			g_bEnd = true;
		}
	}
	else if (m_pPlayerObj->GetTransformCom()->Get_Info(Engine::INFO_POS)->x > 135.5f && !m_pPlayerObj->Get_LastStage())
	{
		m_eState = STOP;	
	}
	else if(m_pPlayerObj->GetTransformCom()->Get_Info(Engine::INFO_POS)->x > 189.f && m_pPlayerObj->Get_LastStage())
	{
		if (m_eState != LAST_LOOP && m_eState != LAST_CATCH && m_eState != LAST_BREAK)
		{
			m_eState = LAST_START;
			m_pTransformCom->Set_Pos(_vec3(186.f, -7.45f, 10.1f));
			m_pTransformCom->Set_Angle(_vec3(0.f, D3DXToRadian(90.f), 0.f));
		}
	}
	Engine::CGameObject::Update_GameObject(fTimeDelta);

	Janitor_Animation(fTimeDelta * m_AniSpeed);
	if (!m_bPlayerDeath && m_bStartAni)
		m_pDynamicMeshCom->Play_Animation(fTimeDelta);
	AniEvent();
	m_pRendererCom->Add_RenderGroup(Engine::RENDER_NONALPHA, this);


	return NO_EVENT;
}

_int CJanitor::LateUpdate_GameObject(const _float & fTimeDelta)
{
	m_pDynamicMeshCom->Play_Animation(0);

	return NO_EVENT;
}

void CJanitor::Render_GameObject(void)
{
	if (m_pPlayerObj->GetTransformCom()->Get_Info(Engine::INFO_POS)->x > 100.f)
	{
		m_pDynamicMeshCom->Play_Animation(0);
		LPD3DXEFFECT	pEffect = m_pShaderCom->Get_EffectHandle();
		NULL_CHECK(pEffect);
		pEffect->AddRef();

		_uint	iPassMax = (_uint)0;

		FAILED_CHECK_RETURN(SetUp_ConstantTable(pEffect), );

		pEffect->Begin(&iPassMax, 0);

		pEffect->BeginPass(0);

		m_pDynamicMeshCom->Render_Meshes(pEffect);


		pEffect->EndPass();

		pEffect->End();

		Safe_Release(pEffect);
		//for (int i = 0; i < 3; i++)
			//Draw_Sphere(i);

		if (m_vecColliderCom.size() > 0)
		{
			//m_vecColliderCom[0]->Render_Collider(Engine::COL_FALSE, &m_ColWorld);
		}

	}
}

HRESULT CJanitor::LateReady_GameObject(void)
{
	m_pNaviCom = dynamic_cast<Engine::CNaviMesh*>(Engine::Get_Component(L"Navi_Mesh", L"Terrain", L"Com_Navi_2", Engine::ID_STATIC));
	//m_pNaviCom->Set_Index_UsePos(*m_pTransformCom->Get_Info(Engine::INFO_POS));
	m_pDynamicMeshCom->Set_Matrix(90.f);
	m_pPlayerObj = dynamic_cast<CPlayer*>(Engine::Get_GameObject(L"02.GameLogic", L"Dyn_Player"));
	m_fSphereCol[0] = 1.f;
	m_fSphereCol[1] = 0.4f;
	m_fSphereCol[2] = 0.4f;
	D3DXCreateSphere(m_pGraphicDev, m_fSphereCol[0], 20, 20, &m_mSphere1, NULL);
	D3DXCreateSphere(m_pGraphicDev, m_fSphereCol[1], 20, 20, &m_mSphere2, NULL);
	D3DXCreateSphere(m_pGraphicDev, m_fSphereCol[2], 20, 20, &m_mSphere3, NULL);
	m_vFirstPos = *m_pTransformCom->Get_Info(Engine::INFO_POS);
	m_vFirstAngle = m_pTransformCom->m_vAngle;


	return S_OK;
}

HRESULT CJanitor::Add_Component(void)
{
	//Engine::CComponent*		pComponent = nullptr;

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



const _vec3 & CJanitor::Get_ColPos(int i) const
{
	_matrix boneMat;
	switch (i)
	{
	case 0:
	{
		const Engine::D3DXFRAME_DERIVED*	pBone = m_pDynamicMeshCom->Get_FrameByName("c_root_bn");
		boneMat = pBone->CombinedTransformationMatrix * *m_pTransformCom->Get_WorldMatrixPointer();
	}
	break;
	case 1:
	{
		const Engine::D3DXFRAME_DERIVED*pBone = m_pDynamicMeshCom->Get_FrameByName("l_hand_bn");
		boneMat = pBone->CombinedTransformationMatrix * *m_pTransformCom->Get_WorldMatrixPointer();
	}
	break;
	case 2:
	{
		const Engine::D3DXFRAME_DERIVED*pBone = m_pDynamicMeshCom->Get_FrameByName("r_hand_bn");
		boneMat = pBone->CombinedTransformationMatrix * *m_pTransformCom->Get_WorldMatrixPointer();
	}
	break;
	}

	return _vec3(boneMat._41, boneMat._42, boneMat._43);
}

const _matrix & CJanitor::Get_BoneMat() const
{
	// TODO: 여기에 반환 구문을 삽입합니다.
	const Engine::D3DXFRAME_DERIVED*	pBone = m_pDynamicMeshCom->Get_FrameByName("c_root_bn");
	return pBone->CombinedTransformationMatrix * *m_pTransformCom->Get_WorldMatrixPointer();
}

const _vec3 & CJanitor::Get_GrabPos(int i) const
{
	// TODO: 여기에 반환 구문을 삽입합니다.

	_matrix boneMat;
	switch (i)
	{
	case 1:
	{
		const Engine::D3DXFRAME_DERIVED*pBone = m_pDynamicMeshCom->Get_FrameByName("l_grabPosition_bn");
		boneMat = pBone->CombinedTransformationMatrix * *m_pTransformCom->Get_WorldMatrixPointer();
	}
	break;
	case 2:
	{
		const Engine::D3DXFRAME_DERIVED*pBone = m_pDynamicMeshCom->Get_FrameByName("r_grabPosition_bn");
		boneMat = pBone->CombinedTransformationMatrix * *m_pTransformCom->Get_WorldMatrixPointer();
	}
	break;
	}

	return _vec3(boneMat._41, boneMat._42, boneMat._43);

}

const _float & CJanitor::Get_ShpereRadius(int i) const
{
	// TODO: 여기에 반환 구문을 삽입합니다.
	return m_fSphereCol[i];
}

const _float & CJanitor::Get_AniTime() const
{
	// TODO: 여기에 반환 구문을 삽입합니다.
	return  m_pDynamicMeshCom->Get_AniPercent();
}

const _bool & CJanitor::Get_CatchFlag() const
{
	// TODO: 여기에 반환 구문을 삽입합니다.
	return m_bCatchAni;
}

const _bool & CJanitor::Get_CatchRIght() const
{
	return m_bCatchRight;
}

const _bool & CJanitor::Get_CatchLeft() const
{
	return m_bCatchLeft;
}

void CJanitor::Set_CatchPlayer(const _bool & flag)
{
	m_bCatch_Player = flag;
}

void CJanitor::Set_TurnChase(const _bool & flag)
{
	m_bTurnChase = flag;
}

void CJanitor::Set_State(JANITOR_STATE eState)
{
	m_eState = eState;
}

void CJanitor::Set_AnimationKey(int iKey)
{
	m_ianimationKey = iKey;

	m_pDynamicMeshCom->Set_AnimationSet(m_ianimationKey, 0.25);
}

const CJanitor::JANITOR_STATE & CJanitor::Get_State() const
{
	// TODO: 여기에 반환 구문을 삽입합니다.
	return m_eState;
}

void CJanitor::Reset_Ani()
{
	const Engine::D3DXFRAME_DERIVED* pBone = m_pDynamicMeshCom->Get_FrameByName("c_root_bn");
	NULL_CHECK(pBone, 0);
	_matrix boneMat = pBone->CombinedTransformationMatrix * *m_pTransformCom->Get_WorldMatrixPointer();
	_vec3 tempPos = { boneMat._41, boneMat._42, boneMat._43 };

	_vec3 vLook = *m_pTransformCom->Get_Info(Engine::INFO_LOOK);
	D3DXVec3Normalize(&vLook, &vLook);

	_vec3 vBoneLook;
	memcpy(&vBoneLook, &boneMat.m[2][0], sizeof(_vec3));
	vBoneLook.y = 0.f;
	D3DXVec3Normalize(&vBoneLook, &vBoneLook);

	_vec3 vPos = *m_pTransformCom->Get_Info(Engine::INFO_POS);


	m_pTransformCom->Rotation(Engine::ROT_Y, D3DXToRadian(acosf(D3DXVec3Dot(&vBoneLook, &vLook))));

	m_pTransformCom->Set_Pos(tempPos);
	m_pDynamicMeshCom->Reset_Animation();
}

_vec3 CJanitor::Get_BloodEffectPos(int i)
{

	m_pDynamicMeshCom->Play_Animation(0.f);
	_matrix boneMat;


	switch (i)
	{
	case 1:
	{
		const Engine::D3DXFRAME_DERIVED*pBone = m_pDynamicMeshCom->Get_FrameByName("r_arm_sub3_bn");
		boneMat = pBone->CombinedTransformationMatrix * *m_pTransformCom->Get_WorldMatrixPointer();
		return _vec3(boneMat._41, boneMat._42, boneMat._43);
	}
	case 2:
	{
		const Engine::D3DXFRAME_DERIVED*pBone = m_pDynamicMeshCom->Get_FrameByName("l_arm_sub3_bn");
		boneMat = pBone->CombinedTransformationMatrix * *m_pTransformCom->Get_WorldMatrixPointer();
		return _vec3(boneMat._41, boneMat._42, boneMat._43);
	}
	}
}

void CJanitor::Janitor_Animation(const _float& fTimeDelta)
{
	_vec3 vPPos = *m_pPlayerObj->GetTransformCom()->Get_Info(Engine::INFO_POS);
	_float fVolume = 1.f - abs(D3DXVec3Length(&(vPPos - *m_pTransformCom->Get_Info(Engine::INFO_POS)))) *0.1f;
	if (fVolume < 0.f)
		fVolume = 0.f;
	_float tempPerSent = m_pDynamicMeshCom->Get_AniPercent();
	switch (m_eState)
	{
	case CJanitor::BED_SPAWN:
		if (m_pPlayerObj->Get_SpawnJionterBedRoom())
		{
			if(tempPerSent > 0.5f)
				Engine::Play_Sound(L"SW_Janitor_teeth_grind_3_c.ogg", Engine::CHANNELID::MONSTER, m_bTeethSound, false);

			Engine::Play_Sound(L"SW_Janitor_breathing_investigating_1.ogg", Engine::CHANNELID::MONSTER_BREATH, m_bBreathSound, true);

			Engine::Play_Sound(L"SW_music_stinger_Janitor_heartbeat.ogg",   Engine::CHANNELID::MONSTER_HEART, m_bHeartBeatSound, true);


			m_ianimationKey = 0;
			m_bStartAni = true;
		}
		break;
	case CJanitor::WALK:
	{
		Engine::Play_Sound(L"SW_roger_footstep_walk_3a.ogg", Engine::CHANNELID::MONSTER_WORKING, m_bWorkingSound, false);
		if (!m_bWalkSoundHalf && tempPerSent > 0.5f)
		{
			m_bWorkingSound = false;
			m_bWalkSoundHalf = true;
		}
		//else if (true == m_pDynamicMeshCom->Is_AnimationSetEnd())
		//{
		//	m_pDynamicMeshCom->Reset_Animation();
		//	m_bWorkingSound = false;
		//	m_bWalkSoundHalf = false;
		//}
		m_ianimationKey = 22;
		_vec3 vDir = *m_pTransformCom->Get_Info(Engine::INFO_LOOK);
		_vec3 vPos = *m_pTransformCom->Get_Info(Engine::INFO_POS);
		//vDir.z = 0.f;
		D3DXVec3Normalize(&vDir, &vDir);
		m_fSpeed = 1.5f;
		if(m_pTransformCom->Get_Info(Engine::INFO_POS)->x < 130.f)
		m_pTransformCom->Set_Pos(vPos + (vDir * m_fSpeed * fTimeDelta));
	}
	break;
	case CJanitor::TURN_180:
		Engine::Stop_Sound(Engine::CHANNELID::MONSTER_WORKING);
		m_bWorkingSound = false;
		m_ianimationKey = 26;
		break;
	case CHASE_PLAYER:
	{
		Engine::Play_Sound(L"Jointer_FindPlayer.mp3", Engine::CHANNELID::EFFECT, m_bFindSound, false);
		m_ianimationKey = 22;
		_vec3 vDir = *m_pPlayerObj->GetTransformCom()->Get_Info(Engine::INFO_POS) - *m_pTransformCom->Get_Info(Engine::INFO_POS);
		_vec3 vPos = *m_pTransformCom->Get_Info(Engine::INFO_POS);
		_vec3 vLook = *m_pTransformCom->Get_Info(Engine::INFO_LOOK);
		_vec3 vRight = *m_pTransformCom->Get_Info(Engine::INFO_RIGHT);
		_float tempY = 1.f;
		//vDir.z = 0.f;
		D3DXVec3Normalize(&vDir, &vDir);
		D3DXVec3Normalize(&vLook, &vLook);
		D3DXVec3Normalize(&vRight, &vRight);

		if (acosf(D3DXVec3Dot(&vDir, &vRight) > 0))
			tempY = -1.f;
		m_pTransformCom->Rotation(Engine::ROT_Y, tempY * D3DXToRadian(acosf(D3DXVec3Dot(&vDir, &vLook))));
		m_fSpeed = 4.5f;
		m_pTransformCom->Set_Pos(vPos + (vDir * m_fSpeed * fTimeDelta));
	}
	break;
	case CATCH_PLAYER:
		Engine::Play_Sound(L"Jointer_FindPlayer.mp3", Engine::CHANNELID::EFFECT, m_bFindSound, false);
		if (!m_bCatchAni)
		{
			Reset_Ani();
			Check_Player();
			m_bCatchAni = true;
		}
		break;
	case CATCH_FAIL:
		m_ianimationKey = 17;
		m_bFindSound = false;
		break;
	case STOP:
		m_fPipeSceneOut -= 0.05f;
		m_bFindSound = false;
		if (m_fPipeSceneOut > 0)
		{
			Engine::Set_Volume(Engine::CHANNELID::MONSTER, m_fPipeSceneOut);
			Engine::Set_Volume(Engine::CHANNELID::MONSTER_HEART, m_fPipeSceneOut);
			Engine::Set_Volume(Engine::CHANNELID::MONSTER_BREATH, m_fPipeSceneOut);
			Engine::Set_Volume(Engine::CHANNELID::BGM, m_fPipeSceneOut);
		}
		else if(m_fPipeSceneOut <=0 && !m_bStopSound)
		{
			m_bStopSound = true;
			m_fPipeSceneOut = 0.f;
			Engine::Stop_Sound(Engine::CHANNELID::MONSTER);
			Engine::Stop_Sound(Engine::CHANNELID::MONSTER_HEART);
			Engine::Stop_Sound(Engine::CHANNELID::MONSTER_BREATH);
			Engine::Stop_Sound(Engine::CHANNELID::BGM);
			Engine::Play_Sound(L"SW_music_GuestEntry_Corpus_Low.ogg", Engine::CHANNELID::BGM, m_bBgmSound, true);
			Engine::Set_Volume(Engine::CHANNELID::BGM, 1.f);
		}

		if (!m_pPlayerObj->Get_LastStage())
		{
			m_ianimationKey = 21;
			m_pTransformCom->Set_Angle(_vec3(0.f, D3DXToRadian(90.f), 0.f));
			m_pTransformCom->Set_Pos(_vec3(159.f, -7.45f, 10.f));
		}
		else
			m_eState = LAST_SPAWN;
		break;
	case LAST_SPAWN:
	{
		m_ianimationKey = 1;
	
		if (tempPerSent > 0.1f)
		{
			Engine::Play_Sound(L"SW_music_Janitor_suspense_loop_2.ogg", Engine::CHANNELID::BGM, m_bLastSpawnSound, true);
		}
		m_bStartAni = true;
		m_AniSpeed = 4.f;
	}
		break;
	case LAST_START:
		m_AniSpeed = 1.f;
		m_ianimationKey = 16;
		Engine::Play_Sound(L"Jointer_BossStageFinal.ogg", Engine::CHANNELID::MONSTER, m_bLastStartSound, true);
		m_bStartAni = true;
		break;
	case LAST_LOOP:
		m_AniSpeed = 1.f;
		m_ianimationKey = 14;
		m_bStartAni = true;
		break;
	case LAST_CATCH:
		break;
	case LAST_BREAK:
		m_AniSpeed = 1.f;
		m_ianimationKey = 15;
		Engine::Stop_Sound(Engine::CHANNELID::MONSTER);
		break;

	}

	m_pDynamicMeshCom->Set_AnimationSet(m_ianimationKey, 0.25);


	Engine::Set_Volume(Engine::CHANNELID::MONSTER_BREATH, fVolume);
	Engine::Set_Volume(Engine::CHANNELID::MONSTER_WORKING, fVolume);
	
}


void CJanitor::Check_Player()
{

	Engine::CTransform* pPlayerTrasnform = m_pPlayerObj->GetTransformCom();
	_vec3 PlayerPos = *pPlayerTrasnform->Get_Info(Engine::INFO_POS);
	_vec3 vPos = *m_pTransformCom->Get_Info(Engine::INFO_POS);



	_vec3 vLook = *m_pTransformCom->Get_Info(Engine::INFO_LOOK);

	_vec3 vRight = *m_pTransformCom->Get_Info(Engine::INFO_RIGHT);

	_vec3 vDir = PlayerPos - vPos;
	_float length = D3DXVec3Length(&vDir);
	D3DXVec3Normalize(&vRight, &vRight);
	D3DXVec3Normalize(&vLook, &vLook);
	D3DXVec3Normalize(&vDir, &vDir);
	_float lookAngle = D3DXToDegree(acosf(D3DXVec3Dot(&vDir, &vLook)));
	_float rightAngle = D3DXToDegree(acosf(D3DXVec3Dot(&vDir, &vRight)));

	if (lookAngle <= 90.f)
	{
		if (length < 2.f)
		{
			m_ianimationKey = 19;
			m_bCatchRight = true;
			m_bCatchLeft = true;
		}
		else
		{
			if (rightAngle <= 90.f)
			{
				m_ianimationKey = 18;
				m_bCatchRight = true;
			}
			else
			{
				m_ianimationKey = 20;
				m_bCatchLeft = true;
			}
		}
	}

}

void CJanitor::AniEvent()
{

	m_AniChange = false;
	const Engine::D3DXFRAME_DERIVED* pBone = m_pDynamicMeshCom->Get_FrameByName("c_root_bn");
	NULL_CHECK(pBone, 0);
	_matrix boneMat = pBone->CombinedTransformationMatrix * *m_pTransformCom->Get_WorldMatrixPointer();
	_vec3 tempPos = { boneMat._41, boneMat._42, boneMat._43 };

	_vec3 vLook = *m_pTransformCom->Get_Info(Engine::INFO_LOOK);
	D3DXVec3Normalize(&vLook, &vLook);

	_vec3 vBoneLook;
	memcpy(&vBoneLook, &boneMat.m[2][0], sizeof(_vec3));
	vBoneLook.y = 0.f;
	D3DXVec3Normalize(&vBoneLook, &vBoneLook);

	_vec3 vPos = *m_pTransformCom->Get_Info(Engine::INFO_POS);


	_float tempPercent = m_pDynamicMeshCom->Get_AniPercent();
	Set_ColWorld(_vec3(2.f, 2.f, 2.f), tempPos);
	if (tempPercent > 0.85f)
	{
		if (m_eState == LAST_START)
		{
			m_eState = LAST_LOOP;
			m_pDynamicMeshCom->Set_AnimationSet(14, 0.25);
		}
		else if (m_eState == LAST_BREAK)
		{
			m_eState = LAST_LOOP;
			m_pDynamicMeshCom->Set_AnimationSet(14, 0.25);
		}
	}
	if (true == m_pDynamicMeshCom->Is_AnimationSetEnd() && tempPercent < 1.f)
	{
		_float test = 0.f;
		switch (m_eState)
		{
		case CJanitor::BED_SPAWN:
			m_eState = WALK;
			//m_ianimationKey = 1;
		//	m_pDynamicMeshCom->Set_AnimationSet(m_ianimationKey, 0.001);

			test = D3DXToDegree(acosf(D3DXVec3Dot(&vBoneLook, &vLook)));
			m_pTransformCom->Rotation(Engine::ROT_Y, D3DXToRadian(test));
			break;
		case CJanitor::WALK:
			//test = D3DXToDegree(acosf(D3DXVec3Dot(&vBoneLook, &vLook))) -90.f;
			//m_ianimationKey = 1;
			//m_pDynamicMeshCom->Set_AnimationSet(m_ianimationKey, 0.001);
			m_pDynamicMeshCom->Reset_Animation();
			m_bWorkingSound = false;
			m_bWalkSoundHalf = false;
			if (m_iTurnCount >= m_iTurnMax)
			{
				m_eState = TURN_180;
				m_iTurnCount = 0;
				m_iTurnMax = 2;
			}
			m_iTurnCount++;
			break;
		case CJanitor::TURN_180:
			if (!m_bTurnChase)
				m_eState = WALK;
			else
				m_eState = CHASE_PLAYER;
			m_iTurnCount = 0;
			test = D3DXToDegree(acosf(D3DXVec3Dot(&vBoneLook, &vLook))) + 90.f;
			m_pTransformCom->Rotation(Engine::ROT_Y, D3DXToRadian(test));
			break;
		case  CJanitor::CATCH_PLAYER:
			if (m_pPlayerObj->Get_JionterCatch())
			{
				m_bStartAni = false;
				m_bTestEnd = true;
			}
			else
			{
				m_bCatchAni = false;
				m_eState = CATCH_FAIL;
				m_bCatchRight = false;
				m_bCatchLeft = false;
			}
			break;
		case CJanitor::CATCH_FAIL:
			m_eState = CHASE_PLAYER;
			break;
		case LAST_SPAWN:
			m_eState = CHASE_PLAYER;
			break;
		case LAST_START:
			//m_eState = LAST_LOOP;
			//m_pDynamicMeshCom->Set_AnimationSet(14, 0.25);
			//m_pTransformCom->Set_Angle(_vec3(0.f, D3DXToRadian(90.f), 0.f));
			break;
		case LAST_CATCH:
			if (m_pPlayerObj->Get_JionterCatch())
			{
				m_bStartAni = false;
				m_bTestEnd = true;
			}
			else
			{
				m_eState = LAST_LOOP;
			}
			break;
		}
		if (m_bTestEnd)
		{
			m_bPlayerDeath = true;
		}
		else
		{
			if (m_eState != LAST_LOOP && m_eState != LAST_START && m_eState != LAST_CATCH && m_eState != LAST_BREAK)
			{
				m_pTransformCom->Set_Pos(tempPos);
				m_ianimationKey = 23;
				m_pDynamicMeshCom->Set_AnimationSet(m_ianimationKey, 0.01);
			}
		}
		m_AniChange = true;

	}
}


void CJanitor::Free(void)
{
	m_mSphere1->Release();
	m_mSphere2->Release();
	m_mSphere3->Release();
	m_pLine->Release();
	Engine::CGameObject::Free();

}





void CJanitor::Set_Shader()
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

}

CJanitor * CJanitor::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CJanitor* pInstance = new CJanitor(pGraphicDev);
	if (FAILED(pInstance->Ready_GameObject()))
		Engine::Safe_Release(pInstance);

	return pInstance;
}


HRESULT CJanitor::SetUp_ConstantTable(LPD3DXEFFECT& pEffect)
{

	_matrix			matWorld, matView, matProj;

	m_pTransformCom->Get_WorldMatrix(&matWorld);
	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
	m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &matProj);

	pEffect->SetMatrix("g_matWorld", &matWorld);
	pEffect->SetMatrix("g_matView", &matView);
	pEffect->SetMatrix("g_matProj", &matProj);

	//const D3DLIGHT9*		pLight = Engine::Get_LightInfo(0);
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
	//tMtrlInfo.Power = 100.f;

	//pEffect->SetVector("g_vMtrlDiffuse", (_vec4*)&tMtrlInfo.Diffuse);
	//pEffect->SetVector("g_vMtrlSpecular", (_vec4*)&tMtrlInfo.Specular);
	//pEffect->SetVector("g_vMtrlAmbient", (_vec4*)&tMtrlInfo.Ambient);

	//_vec4		vCamPos;
	//D3DXMatrixInverse(&matView, NULL, &matView);
	//memcpy(&vCamPos, &matView.m[3][0], sizeof(_vec4));

	//pEffect->SetVector("g_vCamPos", &vCamPos);
	//pEffect->SetFloat("g_fPower", tMtrlInfo.Power);

	return S_OK;

	return S_OK;
}

Engine::CCollider * CJanitor::Get_BoxCollider()
{
	return m_vecColliderCom[0];
}

_matrix CJanitor::Get_ColWorld()
{
	return m_ColWorld;
}

void CJanitor::Draw_Sphere(int i)
{
	_matrix matWorld;
	_matrix mPos;

	_matrix boneMat;
	switch (i)
	{
	case 0:
	{
		const Engine::D3DXFRAME_DERIVED*	pBone = m_pDynamicMeshCom->Get_FrameByName("c_root_bn");
		boneMat = pBone->CombinedTransformationMatrix * *m_pTransformCom->Get_WorldMatrixPointer();
	}
	break;
	case 1:
	{
		const Engine::D3DXFRAME_DERIVED*pBone = m_pDynamicMeshCom->Get_FrameByName("l_middleFinger1_bn");
		boneMat = pBone->CombinedTransformationMatrix * *m_pTransformCom->Get_WorldMatrixPointer();
	}
	break;
	case 2:
	{
		const Engine::D3DXFRAME_DERIVED*pBone = m_pDynamicMeshCom->Get_FrameByName("r_middleFinger1_bn");
		boneMat = pBone->CombinedTransformationMatrix * *m_pTransformCom->Get_WorldMatrixPointer();
	}
	break;
	}
	if (i == 0)
	{
		_vec3 BoneLook = { boneMat._21 ,boneMat._22,boneMat._23};
		D3DXVec3Normalize(&BoneLook,&BoneLook);
		D3DXMatrixTranslation(&mPos,
			boneMat._41 + BoneLook.x * 2.f,
			boneMat._42 + BoneLook.y * 2.f,
			boneMat._43 + BoneLook.z * 2.f);
	}
	else
		D3DXMatrixTranslation(&mPos,
			boneMat._41 ,
			boneMat._42 ,
			boneMat._43 );
	matWorld = mPos;
	m_pGraphicDev->SetTransform(D3DTS_WORLD, &matWorld);
	m_pGraphicDev->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);


	switch (i)
	{
	case 0:
		m_mSphere1->DrawSubset(0);
		break;
	case 1:
		m_mSphere2->DrawSubset(0);
		break;
	case 2:
		m_mSphere3->DrawSubset(0);
		break;
	}


	m_pGraphicDev->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
}

void CJanitor::Set_First_BedRoom()
{
	m_pTransformCom->Set_Pos(m_vFirstPos);
	m_pTransformCom->Set_Angle(m_vFirstAngle);
	m_pDynamicMeshCom->Reset_Animation();
	m_bChase_Player = false;
	m_bCatch_Player = false;
	m_AniChange = true;
	m_bTestEnd = false;
	m_bPlayerDeath = false;
	m_bCatchMove = false;
	m_bStartAni = false;
	m_bCatchAni = false;
	m_eState = BED_SPAWN;
	m_iTurnMax = 1;
	m_iTurnCount = 0;
	aniCount = 0;

	Engine::Stop_Sound(Engine::MONSTER);
	Engine::Stop_Sound(Engine::MONSTER_BREATH);
	m_bLastSpawnSound = false;
	m_bLastStartSound = false;
	m_bTeethSound = false;
	m_bBreathSound = false;
	m_bHeartBeatSound = false;
	m_bFindSound = false;
	m_bWorkingSound = false;
	m_bWalkSoundHalf = false;
	m_AniSpeed = 1.f;
}

void CJanitor::Set_ColWorld(_vec3 vScale, _vec3 vPos)
{
	_matrix mScale, mPos;
	D3DXMatrixScaling(&mScale, vScale.x, vScale.y, vScale.z);
	D3DXMatrixTranslation(&mPos, vPos.x - 1.f,
		vPos.y,
		vPos.z - 1.f);
	m_ColWorld = mScale * mPos;
}
//void CJanitor::TestAstar(float fTimeDelta)
//{
//	_vec3 vDir = *m_pPlayerObj->GetTransformCom()->Get_Info(Engine::INFO_POS) - *m_pTransformCom->Get_Info(Engine::INFO_POS);
//	_vec3 vPos = *m_pTransformCom->Get_Info(Engine::INFO_POS);
//	_vec3 vLook = *m_pTransformCom->Get_Info(Engine::INFO_LOOK);
//	_vec3 vRight = *m_pTransformCom->Get_Info(Engine::INFO_RIGHT);
//	_float tempY = 1.f;
//	//vDir.z = 0.f;
//	D3DXVec3Normalize(&vDir, &vDir);
//	D3DXVec3Normalize(&vLook, &vLook);
//	D3DXVec3Normalize(&vRight, &vRight);
//
//	if (acosf(D3DXVec3Dot(&vDir, &vRight) > 0))
//		tempY = -1.f;
//	m_pTransformCom->Rotation(Engine::ROT_Y, tempY * D3DXToRadian(acosf(D3DXVec3Dot(&vDir, &vLook))));
//	m_fSpeed = 4.5f;
//	m_pTransformCom->Set_Pos(vPos + (vDir * m_fSpeed * fTimeDelta));
//}
//
//void CJanitor::PathFinding(AstarNode startNode, AstarNode endNode, list<AstarNode> & openList, list<AstarNode> & closeList)
//{
//	openList.clear();
//	closeList.clear();
//	AstarNode currentNode;
//	openList.push_back(startNode);
//	m_pNaviCom->Set_Index_UsePos(*m_pTransformCom->Get_Info(Engine::INFO_POS));
//	while (!openList.empty())
//	{
//		//갈 수 있는 길 중 가중치가 낮은 길을 찾음
//		auto tempIter = openList.begin();
//		for (auto iter = openList.begin(); iter != openList.end(); iter++)
//		{
//			if (iter == openList.begin())
//				currentNode = *iter;
//			else
//				if (iter->F <= currentNode.F && iter->H < currentNode.H)
//				{
//					currentNode = *iter;
//					tempIter = iter;
//				}
//		}
//		//찾은 길은 갈 수 있는 목록에서 제거
//		//한번 들른 곳은 목록에 추가
//		openList.erase(tempIter);
//		closeList.push_back(currentNode);
//
//		if (currentNode.x == endNode.x && currentNode.y == endNode.y)
//		{
//			return;
//		}
//		//OpenListAdd();
//
//	}
//}
//void CJanitor::OpenListAdd(int checkX, int checkY, Engine::CNaviMesh*	m_pNaviCom, list<AstarNode> & openList, list<AstarNode> & closeList, AstarNode currentNode)
//{
//	//if(closeList)
//	AstarNode NeighborNode;
//	int MoveCost = currentNode.G;//+ 이동거리
//	if (MoveCost < NeighborNode.G)
//	{
//		NeighborNode.G = MoveCost;
//		NeighborNode.H = MoveCost;
//		NeighborNode.ParentNode = currentNode;
//
//		openList.push_back(NeighborNode);
//	}
//}