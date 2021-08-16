#include "pch.h"
#include "NaviObject.h"
#include "ToolView.h"
#include "MainFrm.h"
#include "Export_Function.h"

CNaviObject::CNaviObject(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CGameObject(pGraphicDev)
{

}

CNaviObject::CNaviObject(LPDIRECT3DDEVICE9 pGraphicDev, Engine::CGameObject * pGameObject) : Engine::CGameObject(pGraphicDev)
{
	for (int i = 0; i < 3; i++)
	{
		m_NaviVertex[i] = dynamic_cast<CNaviObject*>(pGameObject)->m_NaviVertex[i];
	}
}

CNaviObject::~CNaviObject(void)
{

}

HRESULT CNaviObject::Ready_GameObject(void)
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	D3DXCreateLine(m_pGraphicDev, &m_pLine);
	m_pLine->SetWidth(2.f);
	m_pLine->SetAntialias(true);

	D3DXCreateLine(m_pGraphicDev, &m_pNaviLine);
	m_pNaviLine->SetWidth(2.f);
	m_pNaviLine->SetAntialias(true);
	m_NaviVertex[0] = { 0.f, 0.1f , 1.f };
	m_NaviVertex[1] = { 1.f, 0.1f , 0.f };
	m_NaviVertex[2] = { 0.f, 0.1f , 0.f };
	return S_OK;
}

_int CNaviObject::Update_GameObject(const _float& fTimeDelta)
{
	Engine::CGameObject::Update_GameObject(fTimeDelta);

	m_pRendererCom->Add_RenderGroup(Engine::RENDER_NONALPHA, this);

	return 0;
}

void CNaviObject::Render_GameObject(void)
{

	m_pGraphicDev->SetTransform(D3DTS_WORLD, &m_pTransformCom->m_matWorld);
	//Engine::Render_BoxMesh();
	DrawNaviLine();
	DrawLine();	
}

HRESULT CNaviObject::Add_Component(void)
{
	Engine::CComponent*		pComponent = nullptr;

	pComponent = m_pTransformCom = Engine::CTransform::Create();
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_DYNAMIC].emplace(L"Com_Transform", pComponent);
	m_pTransformCom->m_vInfo[Engine::INFO_POS] = { 0.f,0.f,0.f };
	pComponent = m_pRendererCom = Engine::Get_Renderer();
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	pComponent->AddRef();
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Renderer", pComponent);
	pComponent = m_pCalculatorCom = Engine::CCalculator::Create(m_pGraphicDev);
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Calculator", pComponent);


	return S_OK;
}

void CNaviObject::DrawLine()
{
	if (m_bSelect)
	{	
		_matrix m_matView, m_matProj;
		m_pGraphicDev->GetTransform(D3DTS_VIEW, &m_matView);
		m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &m_matProj);

		D3DXVECTOR3 pX[2] = { m_NaviVertex[0],
						  {m_NaviVertex[0].x,m_NaviVertex[0].y + 1.f,m_NaviVertex[0].z} };
		D3DXVECTOR3 pY[2] = { m_NaviVertex[1],
						  {m_NaviVertex[1].x,m_NaviVertex[1].y + 1.f,m_NaviVertex[1].z} };
		D3DXVECTOR3 pZ[2] = { m_NaviVertex[2],
						  {m_NaviVertex[2].x,m_NaviVertex[2].y + 1.f,m_NaviVertex[2].z} };

		m_pNaviLine->Begin();
		m_pNaviLine->DrawTransform(pX, 2, &(m_matView* m_matProj), D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f));    //이때의 행렬은 전체월드행렬*뷰*프로젝션 행렬을 넘겨줘야함
		m_pNaviLine->DrawTransform(pY, 2, &(m_matView* m_matProj), D3DXCOLOR(0.0f, 1.0f, 0.0f, 1.0f));    //이때의 행렬은 전체월드행렬*뷰*프로젝션 행렬을 넘겨줘야함
		m_pNaviLine->DrawTransform(pZ, 2, &(m_matView* m_matProj), D3DXCOLOR(0.0f, 0.0f, 1.0f, 1.0f));    //이때의 행렬은 전체월드행렬*뷰*프로젝션 행렬을 넘겨줘야함
		m_pNaviLine->End();
	}
}

void CNaviObject::DrawNaviLine()
{

	_matrix m_matView, m_matProj;
	m_pGraphicDev->GetTransform(D3DTS_VIEW, &m_matView);
	m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &m_matProj);

	D3DXVECTOR3 pNaviLine[4] = { m_NaviVertex[0],
						 m_NaviVertex[1] ,m_NaviVertex[2] ,m_NaviVertex[0] };

	for (_ulong i = 0; i < 4; ++i)
	{
		D3DXVec3TransformCoord(&pNaviLine[i], &pNaviLine[i], &m_matView);

		if (pNaviLine[i].z <= 0.1f)
			pNaviLine[i].z = 0.1f;

		D3DXVec3TransformCoord(&pNaviLine[i], &pNaviLine[i], &m_matProj);
	}
	m_pGraphicDev->EndScene();
	m_pGraphicDev->BeginScene();

	m_pLine->Begin();
	_matrix matTemp;
	m_pLine->DrawTransform(pNaviLine, 4, D3DXMatrixIdentity(&matTemp), D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f));    //이때의 행렬은 전체월드행렬*뷰*프로젝션 행렬을 넘겨줘야함

	m_pLine->End();
	
}

_vec3 * CNaviObject::Get_Navi()
{
	return m_NaviVertex;
}

HRESULT CNaviObject::Ready_CopyObject(void)
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	D3DXCreateLine(m_pGraphicDev, &m_pLine);
	m_pLine->SetWidth(2.f);
	m_pLine->SetAntialias(true);

	D3DXCreateLine(m_pGraphicDev, &m_pNaviLine);
	m_pNaviLine->SetWidth(2.f);
	m_pNaviLine->SetAntialias(true);
	return S_OK;
}

void CNaviObject::Set_Vertex(int i, int j, float Val)
{
	if (j == 0)
	{
		m_NaviVertex[i].x = Val;
	}
	if (j == 1)
	{
		m_NaviVertex[i].y = Val;
	}
	if (j == 2)
	{
		m_NaviVertex[i].z = Val;
	}
}


CNaviObject* CNaviObject::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CNaviObject* pInstance = new CNaviObject(pGraphicDev);
	if (FAILED(pInstance->Ready_GameObject()))
		Engine::Safe_Release(pInstance);
	return pInstance;
}

CNaviObject * CNaviObject::Create(LPDIRECT3DDEVICE9 pGraphicDev, Engine::CGameObject * pGameObject)
{
	CNaviObject* pInstance = new CNaviObject(pGraphicDev, pGameObject);
	if (FAILED(pInstance->Ready_CopyObject()))
		Engine::Safe_Release(pInstance);
	return pInstance;
}

void CNaviObject::Free(void)
{
	if(m_pLine != nullptr)
		m_pLine->Release();

	if (m_pNaviLine != nullptr)
		m_pNaviLine->Release();
	Engine::CGameObject::Free();
}

_bool CNaviObject::Picking()
{
	CToolView* pToolView = (CToolView*)((CMainFrame*)(AfxGetApp()->m_pMainWnd))->GetActiveView();

	if (m_pCalculatorCom->Picking_NaviMesh(g_hWnd, m_NaviVertex, m_pTransformCom, pToolView->Get_WinSize(), m_fDist))
	{
		return true;
	}
	else
	{
		return false;
	}

}

void CNaviObject::Save(DWORD& dwBytes, const HANDLE& hFile)
{
	WriteFile(hFile, &m_NaviVertex[0], sizeof(_vec3), &dwBytes, nullptr);
	WriteFile(hFile, &m_NaviVertex[1], sizeof(_vec3), &dwBytes, nullptr);
	WriteFile(hFile, &m_NaviVertex[2], sizeof(_vec3), &dwBytes, nullptr);
}

void CNaviObject::Load(DWORD& dwBytes, const HANDLE& hFile)
{
	ReadFile(hFile, &m_NaviVertex[0], sizeof(_vec3), &dwBytes, nullptr);
	ReadFile(hFile, &m_NaviVertex[1], sizeof(_vec3), &dwBytes, nullptr);
	ReadFile(hFile, &m_NaviVertex[2], sizeof(_vec3), &dwBytes, nullptr);
}

const float & CNaviObject::Get_fDist() const
{
	// TODO: 여기에 반환 구문을 삽입합니다.
	return m_fDist;
}