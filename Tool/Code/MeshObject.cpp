#include "pch.h"
#include "MeshObject.h"
#include "Collider.h"
#include "MainFrm.h"
#include "ToolView.h"
#include "Optimization.h"
#include "Export_Function.h"

CMeshObject::CMeshObject(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CGameObject(pGraphicDev)
{

}

CMeshObject::CMeshObject(LPDIRECT3DDEVICE9 pGraphicDev, Engine::CGameObject * tempObject) : Engine::CGameObject(pGraphicDev)
{
	for (int i = 0; i < Engine::ID_END; i++)
	{
		auto iter = tempObject->GetMapCom(i).begin();
		for (; iter != tempObject->GetMapCom(i).end(); iter++)
		{
			if (wcscmp(iter->first.c_str(), L"Com_Buffer") == 0)
			{
				m_pBufferCom = dynamic_cast<Engine::CLightTex*>(Engine::Clone(RESOURCE_STATIC, L"Buffer_LightTex"));
				m_mapComponent[i].emplace(iter->first, m_pBufferCom);
			}
			else if(wcscmp(iter->first.c_str(), L"Com_Transform") == 0)
			{
				m_pTransformCom = Engine::CTransform::Clone(tempObject->GetTransformCom());
				m_mapComponent[i].emplace(iter->first, m_pTransformCom);
			}
			else if (wcscmp(iter->first.c_str(), L"Com_Renderer") == 0)
			{
				m_pRendererCom = Engine::Get_Renderer();
				NULL(m_pRendererCom, E_FAIL);
				m_pRendererCom->AddRef();
				m_mapComponent[i].emplace(iter->first, m_pRendererCom);
			}
			else if (wcscmp(iter->first.c_str(), L"Com_Texture") == 0)
			{
				m_pTextureCom = dynamic_cast<Engine::CTexture*>(Engine::Clone(iter->second));
				m_mapComponent[i].emplace(iter->first, m_pTextureCom);
				m_iTexCount = tempObject->Get_TexCount();
				
				lstrcpy(m_TexName, tempObject->Get_TexName());
			}
		}
	}
	m_ObjName = tempObject->Get_ObjName();
	m_ObjType = tempObject->Get_ObjType();
	m_MonsterType = tempObject->Get_MonsterType();
	m_ItemType = tempObject->Get_ItemType();
	m_eMeshType = dynamic_cast<CMeshObject*>(tempObject)->m_eMeshType;
}

CMeshObject::~CMeshObject(void)
{
}

HRESULT CMeshObject::Ready_GameObject(void)
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);
	m_pTransformCom->Set_Scale(0.01f, 0.01f, 0.01f);
	D3DXCreateLine(m_pGraphicDev, &m_pLine);
	m_pLine->SetWidth(5.f);
	m_pLine->SetAntialias(true);

	return S_OK;
}

_int CMeshObject::Update_GameObject(const _float& fTimeDelta)
{
	Engine::CGameObject::Update_GameObject(fTimeDelta);
	_vec3	vPos;
	m_pTransformCom->Get_Info(Engine::INFO_POS, &vPos);
	if (m_eMeshType == Engine::TYPE_STATIC)
	{
		//m_bDraw = m_pOptimizationCom->Is_InFrustum_ForObject(&vPos, 0.f);
	}
	m_pRendererCom->Add_RenderGroup(Engine::RENDER_NONALPHA, this);
	
	return 0;
}

void CMeshObject::Render_GameObject(void)
{
	if (m_eMeshType == Engine::TYPE_STATIC)
	{
		//if (false == m_bDraw)
		//	return;
	}
	m_pGraphicDev->SetTransform(D3DTS_WORLD, &m_pTransformCom->m_matWorld);
	m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, FALSE);
	if (m_eMeshType == Engine::TYPE_DYNAMIC)
	{
		m_pDynamicMeshCom->Render_Meshes();
		m_pGraphicDev->SetTexture(0, nullptr);
	}
	else if (m_eMeshType == Engine::TYPE_STATIC)
	{
		m_pStaticMeshCom->Render_Meshes();
		_matrix		matWorld;
		m_pTransformCom->Get_WorldMatrix(&matWorld);
		if (m_vecColliderCom.size() > 0)
		{
			m_vecColliderCom[0]->Render_Collider(Engine::COLLTYPE(m_bPick), &matWorld);
		}
		m_pGraphicDev->SetTexture(0, nullptr);
	}

	DrawLine();
}

HRESULT CMeshObject::Ready_CopyGameObject(void)
{
	return S_OK;
}



HRESULT CMeshObject::Load_GameObject(void)
{
	/*m_pTransformCom->Set_Scale(0.01f, 0.01f, 0.01f);*/
	D3DXCreateLine(m_pGraphicDev, &m_pLine);
	m_pLine->SetWidth(5.f);
	m_pLine->SetAntialias(true);
	return S_OK;
}

void CMeshObject::Set_sMeshName(CString sMeshName)
{
	m_sMeshName = sMeshName;
}

void CMeshObject::Set_TestCol()
{
	Engine::CComponent*		pComponent = nullptr;
	if (m_pStaticMeshCom != nullptr)
	{
		if (m_sMeshName.find(L"Stage", 0) == wstring::npos)
		{
			pComponent = Engine::CCollider::Create(m_pGraphicDev,
				m_pStaticMeshCom->Get_VtxPos(),
				m_pStaticMeshCom->Get_NumVtx(),
				m_pStaticMeshCom->Get_Stride());
			m_vecColliderCom.push_back(dynamic_cast<Engine::CCollider*>(pComponent));
			m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Collider", pComponent);
		}
	}
	pComponent = m_pOptimizationCom = dynamic_cast<Engine::COptimization*>(Engine::Clone_Prototype(L"Proto_Optimization"));
	NULL_CHECK(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Optimization", pComponent);
}

_bool CMeshObject::Picking()
{
	CToolView* pToolView = (CToolView*)((CMainFrame*)(AfxGetApp()->m_pMainWnd))->GetActiveView();
	if (m_eMeshType == Engine::TYPE_STATIC)
	{
		if (m_vecColliderCom.size() > 0)
		{
			if (m_pCalculatorCom->Picking_StaticMesh(g_hWnd, m_vecColliderCom[0], m_pTransformCom, pToolView->Get_WinSize(), m_fDist))
			{
				return true;
			}
			else
			{
				return false;
			}
		}
	}
	return false;
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
		if (m_sMeshName.find(L"Stage", 0) == wstring::npos)
		{
			pComponent = Engine::CCollider::Create(m_pGraphicDev,
				m_pStaticMeshCom->Get_VtxPos(),
				m_pStaticMeshCom->Get_NumVtx(),
				m_pStaticMeshCom->Get_Stride());
			NULL_CHECK_RETURN(pComponent, E_FAIL);
			m_vecColliderCom.push_back(dynamic_cast<Engine::CCollider*>(pComponent));
			m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Collider", pComponent);
		}
	}
	pComponent = m_pOptimizationCom = dynamic_cast<Engine::COptimization*>(Engine::Clone_Prototype(L"Proto_Optimization"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Optimization", pComponent);
	return S_OK;
}

void CMeshObject::DrawLine()
{
	float fLineLength = 700.f;

	if (m_bSelect)
	{
		_matrix m_matView, m_matProj;
		m_pGraphicDev->GetTransform(D3DTS_VIEW, &m_matView);
		m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &m_matProj);

		D3DXVECTOR3 pX[2] = { {0,0,0 },
						  {fLineLength,0,0} };
		D3DXVECTOR3 pY[2] = { {0,0,0 },
						  {0,fLineLength,0} };
		D3DXVECTOR3 pZ[2] = { {0,0,0 },
						  {0,0,fLineLength} };

		m_pLine->Begin();
		m_pLine->DrawTransform(pX, 2, &(m_pTransformCom->m_matWorld *m_matView* m_matProj), D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f)); 
		m_pLine->DrawTransform(pY, 2, &(m_pTransformCom->m_matWorld *m_matView* m_matProj), D3DXCOLOR(0.0f, 1.0f, 0.0f, 1.0f)); 
		m_pLine->DrawTransform(pZ, 2, &(m_pTransformCom->m_matWorld *m_matView* m_matProj), D3DXCOLOR(0.0f, 0.0f, 1.0f, 1.0f)); 
		m_pLine->End();
	}
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

CMeshObject * CMeshObject::Create(LPDIRECT3DDEVICE9 pGraphicDev,  Engine::MESHTYPE eMeshType, CString sMeshName)
{
	CMeshObject* pInstance = new CMeshObject(pGraphicDev);
	pInstance->m_eMeshType = eMeshType;
	pInstance->m_sMeshName = sMeshName;
	if (FAILED(pInstance->Ready_GameObject()))
		Engine::Safe_Release(pInstance);

	return pInstance;
}



CMeshObject * CMeshObject::Create(LPDIRECT3DDEVICE9 pGraphicDev, Engine::CGameObject * object)
{
	CMeshObject* pInstance = new CMeshObject(pGraphicDev,object);
	if (FAILED(pInstance->Ready_CopyGameObject()))
		Engine::Safe_Release(pInstance);

	return pInstance;
}

const CString CMeshObject::GetString() const
{
	// TODO: 여기에 반환 구문을 삽입합니다.
	return m_TexName;
}

const Engine::MESHTYPE & CMeshObject::GetMeshType() const
{
	// TODO: 여기에 반환 구문을 삽입합니다.
	return m_eMeshType;
}



void CMeshObject::SetString(CString str)
{
	lstrcpy(m_TexName, str);
}


void CMeshObject::Free(void)
{
	if(m_pLine != nullptr)
		m_pLine->Release();
	Engine::CGameObject::Free();

}

