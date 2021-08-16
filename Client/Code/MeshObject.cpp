#include "stdafx.h"
#include "MeshObject.h"
#include "Collider.h"
#include "Player.h"
#include "Export_Function.h"

CMeshObject::CMeshObject(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CGameObject(pGraphicDev)
{

}

CMeshObject::~CMeshObject(void)
{
}

HRESULT CMeshObject::Ready_GameObject(void)
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);
	//m_pTransformCom->Set_Scale(0.01f, 0.01f, 0.01f);
	//D3DXCreateLine(m_pGraphicDev, &m_pLine);
	//m_pLine->SetWidth(5.f);
	//m_pLine->SetAntialias(true);
	
	return S_OK;
}

HRESULT CMeshObject::LateReady_GameObject(void)
{
	m_pPlayer = dynamic_cast<CPlayer*>(Engine::Get_GameObject(L"02.GameLogic", L"Dyn_Player"));
	return S_OK;
}

_int CMeshObject::Update_GameObject(const _float& fTimeDelta)
{
	Engine::CGameObject::Update_GameObject(fTimeDelta);
	//if (m_pPlayer->GetTransformCom()->Get_Info(Engine::INFO_POS)->x > 105.f)
	//{
	//	if (m_ObjName.find(L"Stage2", 0) != wstring::npos)
	//	{
	//		m_bDraw = true;
	//	}
	//	else
	//		m_bDraw = false;
	//}
	//else
		m_bDraw = true;



	m_pRendererCom->Add_RenderGroup(Engine::RENDER_NONALPHA, this);
	
	return 0;
}

void CMeshObject::Render_GameObject(void)
{
	/*_matrix		matWorld;
	m_pTransformCom->Get_WorldMatrix(&matWorld);

	m_pColliderCom->Render_Collider(Engine::COLLTYPE(m_bColl), &matWorld);*/
	if (false == m_bDraw)
		return;
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
		m_pStaticMeshCom->Render_Meshes(pEffect);
	}
	pEffect->EndPass();

	pEffect->End();

	Safe_Release(pEffect);
}


HRESULT CMeshObject::Load_GameObject(void)
{
	/*m_pTransformCom->Set_Scale(0.01f, 0.01f, 0.01f);*/
	Engine::CComponent*		pComponent = nullptr;

	return S_OK;
}



const float & CMeshObject::Get_fDist() const
{
	// TODO: 여기에 반환 구문을 삽입합니다.
	return m_fDist;
}

HRESULT CMeshObject::Add_Component(void)
{

	Engine::CComponent*		pComponent = nullptr;
	if (m_eMeshType == Engine::TYPE_DYNAMIC)
	{
		pComponent = m_pDynamicMeshCom = dynamic_cast<Engine::CDynamicMesh*>(Engine::Clone(RESOURCE_STATIC, m_sMeshName.c_str()));
		NULL_CHECK_RETURN(pComponent, E_FAIL);
		m_mapComponent[Engine::ID_DYNAMIC].emplace(L"Com_Mesh", pComponent);
	}
	else if (m_eMeshType == Engine::TYPE_STATIC)
	{
		pComponent = m_pStaticMeshCom = dynamic_cast<Engine::CStaticMesh*>(Engine::Clone(RESOURCE_STATIC, m_sMeshName.c_str()));
		NULL_CHECK_RETURN(pComponent, E_FAIL);
		m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Mesh", pComponent);
	}

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

	if (m_pStaticMeshCom != nullptr)
	{
		pComponent  = Engine::CCollider::Create(m_pGraphicDev,
			m_pStaticMeshCom->Get_VtxPos(),
			m_pStaticMeshCom->Get_NumVtx(),
			m_pStaticMeshCom->Get_Stride());
		NULL_CHECK_RETURN(pComponent, E_FAIL);
		m_vecColliderCom.push_back(dynamic_cast<Engine::CCollider*>(pComponent));
		m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Collider", pComponent);
	}


	return S_OK;
}

void CMeshObject::SetTestCol()
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

	pComponent = m_pShaderCom = dynamic_cast<Engine::CShader*>(Engine::Clone_Prototype(L"Shader_Mesh"));
	NULL_CHECK(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Shader", pComponent);

}

void CMeshObject::SetTexCount(int count)
{
	m_iTexCount = count;
}

void CMeshObject::SetbPick(_bool flag)
{
	m_bPick = flag;
}



CMeshObject * CMeshObject::Create(LPDIRECT3DDEVICE9 pGraphicDev, Engine::MESHTYPE eMeshType)
{
	CMeshObject* pInstance = new CMeshObject(pGraphicDev);
	pInstance->m_eMeshType = eMeshType;
	if (FAILED(pInstance->Load_GameObject()))
		Engine::Safe_Release(pInstance);

	return pInstance;
}


const Engine::MESHTYPE & CMeshObject::GetMeshType() const
{
	// TODO: 여기에 반환 구문을 삽입합니다.
	return m_eMeshType;
}

HRESULT CMeshObject::SetUp_ConstantTable(LPD3DXEFFECT& pEffect)
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

void CMeshObject::Free(void)
{
	//m_pLine->Release();
	Engine::CGameObject::Free();
}

