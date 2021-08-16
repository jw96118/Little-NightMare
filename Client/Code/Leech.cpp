#include "stdafx.h"
#include "Leech.h"
#include "Player.h"
#include "Export_Function.h"

CLeech::CLeech(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CGameObject(pGraphicDev)
{

}

CLeech::~CLeech(void)
{

}



HRESULT CLeech::Ready_GameObject(void)
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);
	D3DXCreateLine(m_pGraphicDev, &m_pLine);
	m_pLine->SetWidth(3.f);
	m_pLine->SetAntialias(true);

	return S_OK;
}

_int CLeech::Update_GameObject(const _float& fTimeDelta)
{
	if (m_pPlayerObj->GetTransformCom()->Get_Info(Engine::INFO_POS)->x < 96.f)
	{
		if (g_bLoadFirst)
		{
			Set_First();
			return NO_EVENT;
		}
		Engine::CGameObject::Update_GameObject(fTimeDelta);
		m_bChase_Player = m_pPlayerObj->Get_ChaseBool();
		Leech_Chase_Player();
		Leech_Animation();
		if (!m_bPlayerDeath)
			m_pDynamicMeshCom->Play_Animation(fTimeDelta);
		Leech_AniEvent();
		m_pRendererCom->Add_RenderGroup(Engine::RENDER_NONALPHA, this);

	}
	return NO_EVENT;
}

_int CLeech::LateUpdate_GameObject(const _float & fTimeDelta)
{
	m_pDynamicMeshCom->Play_Animation(0);
	return NO_EVENT;
}

void CLeech::Render_GameObject(void)
{
	if (m_pPlayerObj->GetTransformCom()->Get_Info(Engine::INFO_POS)->x < 96.f)
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
	}
	Draw_Sphere();
	/*if (m_vecColliderCom.size() > 0)
	{
		m_vecColliderCom[0]->Render_Collider(Engine::COL_FALSE, &m_ColWorld);
	}*/
	//_matrix matWorld,m_matView, m_matProj , mScale,mPos;
	//m_pGraphicDev->GetTransform(D3DTS_VIEW, &m_matView);
	//m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &m_matProj);

	//D3DXVECTOR3 pLookDir[2] = { {0.f,0.3f,0.f},  {tempdir.x * 1.f,0.3f,tempdir.z * 1.f} };
	//D3DXVECTOR3 pBoneDir[2] = { {0.f,0.3f,0.f}, {tempBonedir.x * 1.f,0.3f,tempBonedir.z * 1.f} };

	//for (_ulong i = 0; i < 2; ++i)
	//{
	//	//D3DXVec3TransformCoord(&pLookDir[i], &pLookDir[i], &m_matView);

	//	//if (pLookDir[i].z <= 0.1f)
	//	//	pLookDir[i].z = 0.1f;

	//	//D3DXVec3TransformCoord(&pLookDir[i], &pLookDir[i], &m_matProj);

	//	//D3DXVec3TransformCoord(&pBoneDir[i], &pBoneDir[i], &m_matView);

	//	//if (pBoneDir[i].z <= 0.1f)
	//	//	pBoneDir[i].z = 0.1f;

	//	//D3DXVec3TransformCoord(&pBoneDir[i], &pBoneDir[i], &m_matProj);

	//}
	//m_pGraphicDev->EndScene();
	//m_pGraphicDev->BeginScene();
	//D3DXMatrixScaling(&mScale, 1.f, 1.f, 1.f);
	//_vec3 vPos = *m_pTransformCom->Get_Info(Engine::INFO_POS);
	//D3DXMatrixTranslation(&mPos, vPos.x,vPos.y,vPos.z);

	//matWorld = mScale * mPos;
	//
	//m_pLine->Begin();
	//m_pLine->DrawTransform(pLookDir, 2, &(matWorld * m_matView* m_matProj), D3DXCOLOR(0.0f, 0.0f, 1.0f, 1.0f));    //이때의 행렬은 전체월드행렬*뷰*프로젝션 행렬을 넘겨줘야함
	//m_pLine->DrawTransform(pBoneDir, 2, &(matWorld * m_matView* m_matProj), D3DXCOLOR(0.0f, 1.0f, 0.0f, 1.0f));    //이때의 행렬은 전체월드행렬*뷰*프로젝션 행렬을 넘겨줘야함

	//m_pLine->End();
}

HRESULT CLeech::LateReady_GameObject(void)
{
	m_pNaviCom = dynamic_cast<Engine::CNaviMesh*>(Engine::Get_Component(L"Navi_Mesh", L"Terrain", L"Com_Navi_2", Engine::ID_STATIC));
	//m_pNaviCom->Set_Index_UsePos(*m_pTransformCom->Get_Info(Engine::INFO_POS));
	m_pDynamicMeshCom->Set_Matrix(90.f);
	m_pPlayerObj = dynamic_cast<CPlayer*>(Engine::Get_GameObject(L"02.GameLogic", L"Dyn_Player"));
	m_fSphereCol = 0.4f;
	D3DXCreateSphere(m_pGraphicDev, m_fSphereCol, 20, 20, &m_mSphere, NULL);
	m_vFirstPos = *m_pTransformCom->Get_Info(Engine::INFO_POS);
	m_vFirstAngle = m_pTransformCom->m_vAngle;
	return S_OK;
}

HRESULT CLeech::Add_Component(void)
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

const _vec3 & CLeech::Get_ColPos() const
{
	// TODO: 여기에 반환 구문을 삽입합니다.
	const Engine::D3DXFRAME_DERIVED* pBone = m_pDynamicMeshCom->Get_FrameByName("c_neck4_bn");
	//NULL_CHECK(pBone, 0);
	_matrix boneMat = pBone->CombinedTransformationMatrix * *m_pTransformCom->Get_WorldMatrixPointer();
    return _vec3( boneMat._41, boneMat._42, boneMat._43 );
}

const _float & CLeech::Get_ShpereRadius() const
{
	// TODO: 여기에 반환 구문을 삽입합니다.
	return m_fSphereCol;
}

void CLeech::Set_CatchPlayer(const _bool & flag)
{
	m_bCatch_Player = flag;
}

void CLeech::Leech_Animation()
{

	if (m_bChase_Player && m_pPlayerObj->GetTransformCom()->Get_Info(Engine::INFO_POS)->x <= 78.5f)
	{
		Engine::Play_Sound(L"SW_leech_shriek_clicky_1a.ogg", Engine::CHANNELID::MONSTER, m_bFindSound, false);
		if (!m_bCatch_Player)
		{
			switch (m_eDIr)
			{
			case DIR_FRONT:
				Engine::Play_Sound(L"SW_leech_slither_3_a.ogg", Engine::CHANNELID::MONSTER, m_bMoveSound, false);
				m_ianimationKey = 9;
				break;
			case DIR_LEFT:
				Engine::Play_Sound(L"SW_leech_slither_3_a.ogg", Engine::CHANNELID::MONSTER, m_bMoveSound, false);
				m_ianimationKey = 13;
				break;
			case DIR_RIGHT:
				Engine::Play_Sound(L"SW_leech_slither_3_b.ogg", Engine::CHANNELID::MONSTER, m_bMoveSound, false);
				m_ianimationKey = 11;
				break;
			}
		}
		else
		{
			if (!m_bCatchMove)
			{
				m_bCatchMove = true;
				m_pTransformCom->Set_Angle(_vec3(0.f, 0.f, 0.f));
				m_pTransformCom->Update_Component(0.f);
				_vec3 vlook = *m_pTransformCom->Get_Info(Engine::INFO_LOOK);
				_vec3 vRight = *m_pTransformCom->Get_Info(Engine::INFO_RIGHT);
				_vec3 vPDir = *m_pPlayerObj->GetTransformCom()->Get_Info(Engine::INFO_POS) - *m_pTransformCom->Get_Info(Engine::INFO_POS);
				D3DXVec3Normalize(&vlook, &vlook);
				D3DXVec3Normalize(&vPDir, &vPDir);
				_float test = 1.f;
				if (D3DXToDegree(acosf(D3DXVec3Dot(&vRight, &vPDir))) > 90)
					test = -1.f;
				m_pTransformCom->Rotation(Engine::ROT_Y, test*acosf(D3DXVec3Dot(&vlook, &vPDir)));
				m_pTransformCom->Set_Pos(*m_pPlayerObj->GetTransformCom()->Get_Info(Engine::INFO_POS) - vPDir*1.f);
			}
			Engine::Play_Sound(L"SW_leech_attach_3a.ogg", Engine::CHANNELID::MONSTER, m_bCatchSound, false);
			m_ianimationKey = 5;
		}
		if (m_bTestEnd)
			m_ianimationKey = 25;
	}
	else
	{
		m_ianimationKey = 15;
	}
	m_pDynamicMeshCom->Set_AnimationSet(m_ianimationKey, 0.01f);
}

void CLeech::Leech_Chase_Player()
{
	if (m_bChase_Player)
	{
		if (m_AniChange)
		{
			Engine::CTransform* pPlayerTrasnform = m_pPlayerObj->GetTransformCom();
			_vec3 PlayerPos = *pPlayerTrasnform->Get_Info(Engine::INFO_POS);
			_vec3 vPos = *m_pTransformCom->Get_Info(Engine::INFO_POS);



			_vec3 vLook = *m_pTransformCom->Get_Info(Engine::INFO_LOOK);

			_vec3 vRight = *m_pTransformCom->Get_Info(Engine::INFO_RIGHT);

			_vec3 vDir = PlayerPos - vPos;
			D3DXVec3Normalize(&vRight, &vRight);
			D3DXVec3Normalize(&vLook, &vLook);
			D3DXVec3Normalize(&vDir, &vDir);
			tempdir = vLook;
			_float lookAngle = D3DXToDegree(acosf(D3DXVec3Dot(&vDir, &vLook)));
			_float rightAngle = D3DXToDegree(acosf(D3DXVec3Dot(&vDir, &vRight)));

			if (lookAngle <= 30.f)
			{
				m_eDIr = DIR_FRONT;
			}
			else if (rightAngle <= 90.f)
			{
				m_eDIr = DIR_RIGHT;
			}
			else
			{
				m_eDIr = DIR_LEFT;

			}
		}
	}



}

void CLeech::Leech_AniEvent()
{
	if (m_bChase_Player)
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

		tempdir = vLook;
		tempBonedir = vBoneLook;

		//_vec3 tempPos;
		//D3DXVec3TransformCoord(&tempPos,&tempPos, &boneMat);
		_vec3 vPos = *m_pTransformCom->Get_Info(Engine::INFO_POS);

		_vec3 vDir2 = {};
		if (aniCount == 0)
		{
			m_vPrePos = vPos;
			vDir2 = tempPos - m_vPrePos;
		}
		else
			vDir2 = tempPos - m_vPrePos;

		_vec3 vtempPos = _vec3(0.f, 0.f, 0.f);

		if (vDir2 != _vec3(0.f, 0.f, 0.f))
		{
			//vtempPos = m_pNaviCom->Move_OnNaviMesh(&m_vPrePos, &vDir2);
			//cout << "vDir : " << vDir2.x << endl;
			//cout << "vtempPos : " << vtempPos.x << endl;
			//cout << "vPos : " << vPos.x - vDir.x << endl;

		}
		if (vtempPos == m_vPrePos && vDir2 != _vec3(0.f, 0.f, 0.f))
		{
			//tempPos = tempPos - vDir2;
			//m_pTransformCom->Set_Pos(vPos - vDir2);

			//cout << "변경 전 : " << tempPos.x << endl;
			//tempPos = vPos - vDir;
			//cout << "변경 후 : " << tempPos.x << endl;
		}
		m_vPrePos = tempPos;
		aniCount++;
		_float tempPercent = m_pDynamicMeshCom->Get_AniPercent();
		if (true == m_pDynamicMeshCom->Is_AnimationSetEnd() && tempPercent < 1.f)
		{
			if (m_bTestEnd)
			{
				m_bPlayerDeath = true;
			}
			else
			{
				m_ianimationKey = 15;
				m_pDynamicMeshCom->Set_AnimationSet(m_ianimationKey, 0.01f);
				m_pTransformCom->Set_Pos(tempPos);
				_float test = D3DXToDegree(acosf(D3DXVec3Dot(&vBoneLook, &vLook))) - 90.f;

				m_pTransformCom->Rotation(Engine::ROT_Y, D3DXToRadian(test));
				if (m_bCatch_Player)
				{
					m_bTestEnd = true;
					//tempPos = { m_pPlayerObj->GetTransformCom()->Get_Info(Engine::INFO_POS)->x,tempPos.y,
					//m_pPlayerObj->GetTransformCom()->Get_Info(Engine::INFO_POS)->z };
					m_pTransformCom->Rotation(Engine::ROT_Y, m_pTransformCom->m_vAngle.y - 180);
					m_pTransformCom->Set_Pos(tempPos);
				}
				else
				{
					m_bMoveSound = false;
				}
			}
			//m_pTransformCom->Set_Angle(_vec3(vRot));
			//Set_ColWorld(m_vColScale, tempPos);
			aniCount = 0;
			m_AniChange = true;
			
		}
	}
}


void CLeech::Free(void)
{
	m_mSphere->Release();
	m_pLine->Release();
	Engine::CGameObject::Free();
	
}





void CLeech::Set_Shader()
{
	Engine::CComponent*		pComponent = nullptr;
	pComponent = m_pShaderCom = dynamic_cast<Engine::CShader*>(Engine::Clone_Prototype(L"Shader_Mesh"));
	NULL_CHECK(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Shader", pComponent);

}

CLeech * CLeech::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CLeech* pInstance = new CLeech(pGraphicDev);
	if (FAILED(pInstance->Ready_GameObject()))
		Engine::Safe_Release(pInstance);

	return pInstance;
}


HRESULT CLeech::SetUp_ConstantTable(LPD3DXEFFECT& pEffect)
{
	_matrix			matWorld, matView, matProj;

	m_pTransformCom->Get_WorldMatrix(&matWorld);
	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
	m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &matProj);

	pEffect->SetMatrix("g_matWorld", &matWorld);
	pEffect->SetMatrix("g_matView", &matView);
	pEffect->SetMatrix("g_matProj", &matProj);

	const D3DLIGHT9*		pLight = Engine::Get_LightInfo(1);
	NULL_CHECK_RETURN(pLight, S_OK);


	pEffect->SetVector("g_vLightDir", &_vec4(pLight->Direction, 0.f));
	pEffect->SetVector("g_vLightDiffuse", (_vec4*)&pLight->Diffuse);
	pEffect->SetVector("g_vLightSpecular", (_vec4*)&pLight->Specular);
	pEffect->SetVector("g_vLightAmbient", (_vec4*)&pLight->Ambient);


	D3DMATERIAL9		tMtrlInfo;
	ZeroMemory(&tMtrlInfo, sizeof(D3DMATERIAL9));

	tMtrlInfo.Diffuse = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
	tMtrlInfo.Specular = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
	tMtrlInfo.Ambient = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
	tMtrlInfo.Emissive = D3DXCOLOR(0.f, 0.f, 0.f, 1.f);
	tMtrlInfo.Power = 100.f;

	pEffect->SetVector("g_vMtrlDiffuse", (_vec4*)&tMtrlInfo.Diffuse);
	pEffect->SetVector("g_vMtrlSpecular", (_vec4*)&tMtrlInfo.Specular);
	pEffect->SetVector("g_vMtrlAmbient", (_vec4*)&tMtrlInfo.Ambient);

	_vec4		vCamPos;
	D3DXMatrixInverse(&matView, NULL, &matView);
	memcpy(&vCamPos, &matView.m[3][0], sizeof(_vec4));

	pEffect->SetVector("g_vCamPos", &vCamPos);
	pEffect->SetFloat("g_fPower", tMtrlInfo.Power);

	return S_OK;
}

void CLeech::Draw_Sphere()
{
	_matrix matWorld;
	_matrix mPos;
	


	const Engine::D3DXFRAME_DERIVED* pBone = m_pDynamicMeshCom->Get_FrameByName("c_neck4_bn");
	//NULL_CHECK(pBone, 0);
	_matrix boneMat = pBone->CombinedTransformationMatrix * *m_pTransformCom->Get_WorldMatrixPointer();
	D3DXMatrixTranslation(&mPos,
		boneMat._41,
		boneMat._42,
		boneMat._43);
	matWorld = mPos;
	m_pGraphicDev->SetTransform(D3DTS_WORLD, &matWorld);
	m_pGraphicDev->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
	m_mSphere->DrawSubset(0);
	m_pGraphicDev->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
}

void CLeech::Set_First()
{
	m_pTransformCom->Set_Pos(m_vFirstPos);
	m_pTransformCom->Set_Angle(m_vFirstAngle);
	m_bChase_Player = false;
	m_bCatch_Player = false;
	m_AniChange = true;
	m_bTestEnd = false;
	m_bPlayerDeath = false;
	m_bCatchMove = false;

	m_bMoveSound = false;
	m_bFindSound = false;
	m_bCatchSound = false;
}
