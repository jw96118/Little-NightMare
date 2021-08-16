#include "pch.h"
#include "Terrain.h"
#include "MainFrm.h"
#include "TerrainCol.h"
#include "ToolView.h"
#include "Export_Function.h"

CTerrain::CTerrain(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CGameObject(pGraphicDev)
{
	for(int i=0 ;i<3; i++)
	m_vNaviPos.push_back(_vec3(0.f, 0.f, 0.f));
}
CTerrain::~CTerrain(void)
{
}

HRESULT CTerrain::Ready_GameObject(void)
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);
	/*for (_uint i = 0; i < 2; ++i)
	{
		m_pGraphicDev->CreateTexture(1, 1, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &m_pTexture[i], NULL);

		D3DLOCKED_RECT	tLockRect;
		ZeroMemory(&tLockRect, sizeof(D3DLOCKED_RECT));

		m_pTexture[i]->LockRect(0, &tLockRect, NULL, NULL);

		*((_ulong*)tLockRect.pBits) = D3DXCOLOR(1.f, 1.f * (1.f - i), 1.f, 1.f);

		m_pTexture[i]->UnlockRect(0);
	}*/

	return S_OK;
}

_int CTerrain::Update_GameObject(const _float& fTimeDelta)
{
	Engine::CGameObject::Update_GameObject(fTimeDelta);

	m_pRendererCom->Add_RenderGroup(Engine::RENDER_NONALPHA, this);
	CToolView* pToolView = (CToolView*)((CMainFrame*)(AfxGetApp()->m_pMainWnd))->GetActiveView();
	//POINT
	BOOL chk = false;
	if (pToolView->m_DObjEnable)
	{
		chk = pToolView->m_DObj.m_NaviButton.GetCheck();
	}
	if (chk)
	{		
		POINT ptMousePos;
		GetCursorPos(&ptMousePos);
		ScreenToClient(g_hWnd, &ptMousePos);
		POINT Screan = pToolView->Get_WinSize();

		//cout << ptMousePos.x << " , " << ptMousePos.y << endl;
		if (ptMousePos.x >= 0 && ptMousePos.x <= Screan.x &&
			ptMousePos.y >= 0 && ptMousePos.y <= Screan.y)
		{
			if (Engine::Get_KeyMgr()->KeyPressing(Engine::KEY_LBUTTON))
			{

				m_pCalculatorCom->Picking_OnNaviTerrain(g_hWnd, m_pTerrain, m_pTransformCom, pToolView->Get_WinSize(), true, m_vNaviPos, m_iVertexNum);

				CMainFrame* pMain = (CMainFrame*)AfxGetMainWnd();

				pMain->m_wndProperties.SetTerrainInfo(this);
				if (m_vNaviPos.size() != 0)
				{
					m_vNaviPos;
				}
			}
			if (Engine::Get_KeyMgr()->KeyPressing(Engine::KEY_RBUTTON))
				m_pCalculatorCom->Picking_OnNaviTerrain(g_hWnd, m_pTerrain, m_pTransformCom, pToolView->Get_WinSize(), false, m_vNaviPos, m_iVertexNum);
		}
	}
	return 0;
}

void CTerrain::Render_GameObject(void)
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, &m_pTransformCom->m_matWorld);
	m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, FALSE);
	m_pGraphicDev->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
	m_pGraphicDev->SetTexture(0, nullptr);
	m_pTerrain->Render_Buffer();

	m_pGraphicDev->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);


}

vector<_vec3> CTerrain::GetNaviPos()
{
	return m_vNaviPos;
}

void CTerrain::Set_Vertex(int i, int j,float val)
{
	m_pTerrain->Set_Vertex(m_iVertexNum,i, j, val);
}


HRESULT CTerrain::Add_Component(void)
{

	Engine::CComponent*		pComponent = nullptr;

	pComponent = m_pTerrain = dynamic_cast<Engine::CTerrainCol*>(Engine::Clone(RESOURCE_STATIC, L"Buffer_TerrainCol"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Terrain", pComponent);

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

	return S_OK;
}


CTerrain* CTerrain::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CTerrain* pInstance = new CTerrain(pGraphicDev);
	if (FAILED(pInstance->Ready_GameObject()))
		Engine::Safe_Release(pInstance);

	
	return pInstance;
}



const CString CTerrain::GetString() const
{
	// TODO: 여기에 반환 구문을 삽입합니다.
	return m_TexName;
}



void CTerrain::SetString(CString str)
{
	lstrcpy(m_TexName, str);
}

void CTerrain::Save_NaviTerrain(DWORD& dwBytes, const HANDLE& hFile)
{
	m_pTerrain->Save_Navi(dwBytes, hFile);
}

void CTerrain::Load_NaviTerrain(DWORD& dwBytes, const HANDLE& hFile)
{
	m_pTerrain->Load_Navi(dwBytes, hFile);
}


void CTerrain::Free(void)
{

	Engine::CGameObject::Free();

}

