#include "stdafx.h"
#include "Janitor_Arm.h"
#include "Player.h"
#include "Collider.h"

#include "Export_Function.h"

CJanitorArm::CJanitorArm(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CGameObject(pGraphicDev)
{

}

CJanitorArm::~CJanitorArm(void)
{

}



HRESULT CJanitorArm::Ready_GameObject(void)
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);
	D3DXCreateLine(m_pGraphicDev, &m_pLine);
	m_pLine->SetWidth(3.f);
	m_pLine->SetAntialias(true);
	m_iTurnMax = 1;
	return S_OK;
}

_int CJanitorArm::Update_GameObject(const _float& fTimeDelta)
{
	if (g_bLoadFirst)
		Set_First_BedRoom();
	if (m_pPlayerObj->GetTransformCom()->Get_Info(Engine::INFO_POS)->x < 180.5f)
	{

		Engine::CGameObject::Update_GameObject(fTimeDelta);
		m_bChase_Player = m_pPlayerObj->Get_ChaseBool();

		//Leech_Chase_Player();
		Janitor_Animation(fTimeDelta);
		if (!m_bPlayerDeath && m_bStartAni)
			m_pDynamicMeshCom->Play_Animation(fTimeDelta);
		AniEvent();
		m_pRendererCom->Add_RenderGroup(Engine::RENDER_NONALPHA, this);

	}
	return NO_EVENT;
}

_int CJanitorArm::LateUpdate_GameObject(const _float & fTimeDelta)
{
	m_pDynamicMeshCom->Play_Animation(0);

	return NO_EVENT;
}

void CJanitorArm::Render_GameObject(void)
{
	if (m_pPlayerObj->GetTransformCom()->Get_Info(Engine::INFO_POS)->x > 130.f)
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
		for (int i = 0; i < 2; i++)
			Draw_Sphere(i);

	}
}

HRESULT CJanitorArm::LateReady_GameObject(void)
{

	m_pDynamicMeshCom->Set_Matrix(90.f);
	m_pPlayerObj = dynamic_cast<CPlayer*>(Engine::Get_GameObject(L"02.GameLogic", L"Dyn_Player"));
	m_fSphereCol[0] = 0.4f;
	m_fSphereCol[1] = 0.4f;
	D3DXCreateSphere(m_pGraphicDev, m_fSphereCol[0], 20, 20, &m_mSphere1, NULL);
	D3DXCreateSphere(m_pGraphicDev, m_fSphereCol[1], 20, 20, &m_mSphere2, NULL);
	m_vFirstPos = *m_pTransformCom->Get_Info(Engine::INFO_POS);
	m_vFirstAngle = m_pTransformCom->m_vAngle;


	return S_OK;
}

HRESULT CJanitorArm::Add_Component(void)
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



const _vec3 & CJanitorArm::Get_ColPos(int i) const
{
	_matrix boneMat;
	switch (i)
	{
	case 0:
	{
		return m_vLHandPos;
	}
	break;
	case 1:
	{
		return m_vRHandPos;
	}
	break;
	}

}

const _matrix & CJanitorArm::Get_BoneMat() const
{
	// TODO: 여기에 반환 구문을 삽입합니다.
	const Engine::D3DXFRAME_DERIVED*	pBone = m_pDynamicMeshCom->Get_FrameByName("c_root_bn");
	return pBone->CombinedTransformationMatrix * *m_pTransformCom->Get_WorldMatrixPointer();
}

const _vec3 & CJanitorArm::Get_GrabPos(int i) const
{
	// TODO: 여기에 반환 구문을 삽입합니다.
	m_pDynamicMeshCom->Play_Animation(0);
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

const _float & CJanitorArm::Get_ShpereRadius(int i) const
{
	// TODO: 여기에 반환 구문을 삽입합니다.
	return m_fSphereCol[i];
}

const _float & CJanitorArm::Get_AniTime() const
{
	// TODO: 여기에 반환 구문을 삽입합니다.
	return  m_pDynamicMeshCom->Get_AniPercent();
}

const _bool & CJanitorArm::Get_CatchFlag() const
{
	// TODO: 여기에 반환 구문을 삽입합니다.
	return m_bCatchAni;
}

const _bool & CJanitorArm::Get_CatchRIght() const
{
	return m_bCatchRight;
}

const _bool & CJanitorArm::Get_CatchLeft() const
{
	return m_bCatchLeft;
}

void CJanitorArm::Set_CatchPlayer(const _bool & flag)
{
	m_bCatch_Player = flag;
}

void CJanitorArm::Set_TurnChase(const _bool & flag)
{
	m_bTurnChase = flag;
}

void CJanitorArm::Set_State(JANITOR_STATE eState)
{
	m_eState = eState;
}

const CJanitorArm::JANITOR_STATE & CJanitorArm::Get_State() const
{
	// TODO: 여기에 반환 구문을 삽입합니다.
	return m_eState;
}

void CJanitorArm::Reset_Ani()
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

void CJanitorArm::Janitor_Animation(const _float& fTimeDelta)
{
	if (m_eState == IDLE)
	{
		m_ianimationKey = 4;
		m_bStartAni = true;
	}
	if (m_bLRFlag)
	{
		switch (m_eState)
		{
		case START:
			m_ianimationKey = 7;

			break;
		case LOOP:
		{
			m_ianimationKey = 5;
		}
		break;


		case CATCH_PLAYER:
			if (!m_bCatchAni)
			{
				m_ianimationKey = 2;
				//Reset_Ani();
				//Check_Player();
				m_bCatchAni = true;
			}
			break;

		}
	}
	else
	{
		switch (m_eState)
		{
		case START:

			m_ianimationKey = 8;

			break;
		case LOOP:
		{
			m_ianimationKey = 6;
		}
		break;


		case CATCH_PLAYER:
			if (!m_bCatchAni)
			{
				m_ianimationKey = 3;
				//Reset_Ani();
				//Check_Player();
				m_bCatchAni = true;
			}
			break;

		}
	}
	m_pDynamicMeshCom->Set_AnimationSet(m_ianimationKey, 0.01);
}

void CJanitorArm::Check_Player()
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
			m_ianimationKey = 3;
			m_bCatchRight = true;
			m_bCatchLeft = true;
		}
		else
		{
			if (rightAngle <= 90.f)
			{
				m_ianimationKey = 2;
				m_bCatchRight = true;
			}
			else
			{
				m_ianimationKey = 4;
				m_bCatchLeft = true;
			}
		}
	}

}

void CJanitorArm::AniEvent()
{

	_float tempPercent = m_pDynamicMeshCom->Get_AniPercent();
	if (true == m_pDynamicMeshCom->Is_AnimationSetEnd() && tempPercent < 1.f)
	{
		_float test = 0.f;
		switch (m_eState)
		{
		case START:
			m_eState = LOOP;
			//if (m_bLRFlag)
			//	m_ianimationKey = 5;
			//else
			//	m_ianimationKey = 6;
			//m_pDynamicMeshCom->Set_AnimationSet(m_ianimationKey, 0.01);
			break;
		case LOOP:
			break;
		case CATCH_PLAYER:
			if (m_pPlayerObj->Get_JionterCatch())
			{
				m_bStartAni = false;
				m_bTestEnd = true;
			}
			break;
		}
		if (m_bTestEnd)
		{
			m_bPlayerDeath = true;
		}
		else
		{
		}
		aniCount = 0;

	}
}


void CJanitorArm::Free(void)
{
	m_mSphere1->Release();
	m_mSphere2->Release();
	m_pLine->Release();
	Engine::CGameObject::Free();

}





void CJanitorArm::Set_Shader()
{
	Engine::CComponent*		pComponent = nullptr;
	pComponent = m_pShaderCom = dynamic_cast<Engine::CShader*>(Engine::Clone_Prototype(L"Shader_Mesh"));
	NULL_CHECK(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Shader", pComponent);

}

CJanitorArm * CJanitorArm::Create(LPDIRECT3DDEVICE9 pGraphicDev, const _bool& flag)
{
	CJanitorArm* pInstance = new CJanitorArm(pGraphicDev);
	if (FAILED(pInstance->Ready_GameObject()))
		Engine::Safe_Release(pInstance);
	pInstance->m_bLRFlag = flag;
	return pInstance;
}


HRESULT CJanitorArm::SetUp_ConstantTable(LPD3DXEFFECT& pEffect)
{

	_matrix			matWorld, matView, matProj;

	m_pTransformCom->Get_WorldMatrix(&matWorld);
	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
	m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &matProj);

	pEffect->SetMatrix("g_matWorld", &matWorld);
	pEffect->SetMatrix("g_matView", &matView);
	pEffect->SetMatrix("g_matProj", &matProj);

	const D3DLIGHT9*		pLight = Engine::Get_LightInfo(0);
	NULL_CHECK_RETURN(pLight, S_OK);


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

Engine::CCollider * CJanitorArm::Get_BoxCollider()
{
	return m_vecColliderCom[0];
}

_matrix CJanitorArm::Get_ColWorld()
{
	return m_ColWorld;
}

void CJanitorArm::Draw_Sphere(int i)
{
	_matrix matWorld;
	_matrix mPos;

	_matrix boneMat;
	switch (i)
	{
	case 0:
	{
		const Engine::D3DXFRAME_DERIVED*pBone = m_pDynamicMeshCom->Get_FrameByName("l_middleFinger1_bn");
		boneMat = pBone->CombinedTransformationMatrix * *m_pTransformCom->Get_WorldMatrixPointer();

		m_vLHandPos = { boneMat._41,boneMat._42,boneMat._43 };
	}
	break;
	case 1:
	{
		const Engine::D3DXFRAME_DERIVED*pBone = m_pDynamicMeshCom->Get_FrameByName("r_middleFinger1_bn");
		boneMat = pBone->CombinedTransformationMatrix * *m_pTransformCom->Get_WorldMatrixPointer();
		m_vRHandPos = { boneMat._41,boneMat._42,boneMat._43 };
	}
	break;
	}

	D3DXMatrixTranslation(&mPos,
		boneMat._41,
		boneMat._42,
		boneMat._43);
	matWorld = mPos;
	m_pGraphicDev->SetTransform(D3DTS_WORLD, &matWorld);
	m_pGraphicDev->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);


	switch (i)
	{
	case 0:
		//m_mSphere1->DrawSubset(0);
		break;
	case 1:
		//m_mSphere2->DrawSubset(0);
		break;
	}


	m_pGraphicDev->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
}

void CJanitorArm::Set_First_BedRoom()
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
	m_eState = IDLE;
	m_iTurnMax = 1;
	m_iTurnCount = 0;
	aniCount = 0;
}

void CJanitorArm::Set_ColWorld(_vec3 vScale, _vec3 vPos)
{
	_matrix mScale, mPos;
	D3DXMatrixScaling(&mScale, vScale.x, vScale.y, vScale.z);
	D3DXMatrixTranslation(&mPos, vPos.x - 1.f,
		vPos.y,
		vPos.z - 1.f);
	m_ColWorld = mScale * mPos;
}
