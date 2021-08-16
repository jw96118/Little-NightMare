#include "stdafx.h"
#include "Lighter.h"
#include "Collider.h"
#include "Player.h"
#include "Export_Function.h"

CLighter::CLighter(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CGameObject(pGraphicDev)
{

}

CLighter::~CLighter(void)
{
}

HRESULT CLighter::Ready_GameObject(void)
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);
	//m_pTransformCom->Set_Scale(0.015f, 0.015f, 0.015f);
	m_pTransformCom->Set_Pos(_vec3(0.f,0.f,0.f));
	//D3DXCreateLine(m_pGraphicDev, &m_pLine);
	//m_pLine->SetWidth(5.f);
	//m_pLine->SetAntialias(true);
	
	return S_OK;
}

HRESULT CLighter::LateReady_GameObject(void)
{
	m_pPlayer = dynamic_cast<CPlayer*>(Engine::Get_GameObject(L"02.GameLogic", L"Dyn_Player"));
	return S_OK;
}

_int CLighter::Update_GameObject(const _float& fTimeDelta)
{
	Engine::CGameObject::Update_GameObject(fTimeDelta);


	//m_pTransformCom->Set_Pos(m_pPlayer->Get_LighterPos());

	if(m_pPlayer->Get_LighterFlag())
		m_pRendererCom->Add_RenderGroup(Engine::RENDER_NONALPHA, this);
	
	return 0;
}

void CLighter::Render_GameObject(void)
{

	LPD3DXEFFECT	pEffect = nullptr; 
	pEffect = m_pShaderCom->Get_EffectHandle();
	NULL_CHECK(pEffect);
	pEffect->AddRef();

	
	FAILED_CHECK_RETURN(SetUp_ConstantTable(pEffect), );

	_uint	iPassMax;
	iPassMax = 0;

	pEffect->Begin(&iPassMax, 0);

	pEffect->BeginPass(0);
	m_pDynamicMeshCom->Render_Meshes(pEffect);
	pEffect->EndPass();

	pEffect->End();

	Safe_Release(pEffect);
}

_vec3 CLighter::Get_EffectPos()
{
	_matrix			matWorld, tempScale, tempMat, tempRot;

	D3DXMatrixIdentity(&matWorld);
	D3DXMatrixScaling(&tempScale, 2.f, 2.f, 2.f);
	D3DXMatrixRotationX(&tempRot, D3DXToRadian(90.f));
	tempMat = m_pPlayer->Get_LighterPos();
	matWorld = tempScale * tempRot * tempMat;


	const Engine::D3DXFRAME_DERIVED* pBone = m_pDynamicMeshCom->Get_FrameByName("Flame");

	_matrix boneMat = pBone->CombinedTransformationMatrix * matWorld;

	return _vec3(boneMat._41, boneMat._42, boneMat._43);
}





const float & CLighter::Get_fDist() const
{
	// TODO: 여기에 반환 구문을 삽입합니다.
	return m_fDist;
}

HRESULT CLighter::Add_Component(void)
{

	Engine::CComponent*		pComponent = nullptr;

	pComponent = m_pDynamicMeshCom = dynamic_cast<Engine::CDynamicMesh*>(Engine::Clone(RESOURCE_STATIC,L"Mesh_Lighter"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_DYNAMIC].emplace(L"Com_Mesh", pComponent);


	pComponent = m_pTransformCom = Engine::CTransform::Create();
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_DYNAMIC].emplace(L"Com_Transform", pComponent);

	pComponent = m_pRendererCom = Engine::Get_Renderer();
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	pComponent->AddRef();
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Renderer", pComponent);

	pComponent = m_pCalculatorCom = Engine::CCalculator::Create(m_pGraphicDev);
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Calculator", pComponent);

	pComponent = m_pShaderCom = dynamic_cast<Engine::CShader*>(Engine::Clone_Prototype(L"Shader_Mesh"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Shader", pComponent);

	return S_OK;
}




CLighter * CLighter::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CLighter* pInstance = new CLighter(pGraphicDev);
	if (FAILED(pInstance->Ready_GameObject()))
		Engine::Safe_Release(pInstance);

	return pInstance;
}



HRESULT CLighter::SetUp_ConstantTable(LPD3DXEFFECT& pEffect)
{
	_matrix			matWorld, matView, matProj, tempScale , tempRot,tempMat;

	//m_pTransformCom->Get_WorldMatrix(&matWorld);
	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
	m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &matProj);
	D3DXMatrixIdentity(&matWorld);
	D3DXMatrixScaling(&tempScale, 2.f, 2.f, 2.f);
	D3DXMatrixRotationX(&tempRot, D3DXToRadian(90.f));
	tempMat = m_pPlayer->Get_LighterPos();
	matWorld = tempScale * tempRot * tempMat;
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
}

void CLighter::Free(void)
{
	//m_pLine->Release();
	Engine::CGameObject::Free();
}

