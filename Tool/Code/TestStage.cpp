#include "pch.h"
#include "TestStage.h"
#include "TempObject.h"
#include "ColObject.h"
#include "MainFrm.h"
#include "MeshObject.h"
#include "Terrain.h"
#include "Optimization.h"
#include "NaviObject.h"
#include "Export_Function.h"

CTestStage::CTestStage(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CScene(pGraphicDev)
{

}

CTestStage::~CTestStage(void)
{

}

HRESULT CTestStage::Ready_Scene(void)
{
	FAILED_CHECK_RETURN(Ready_Resource(m_pGraphicDev, RESOURCE_END), E_FAIL);
	FAILED_CHECK_RETURN(Engine::CScene::Ready_Scene(), E_FAIL);
	FAILED_CHECK_RETURN(Ready_GameLogic_Layer(L"GameLogic"), E_FAIL);
	m_pKeyMgr = Engine::Get_KeyMgr();

	return S_OK;
}

HRESULT CTestStage::LateReady_Scene(void)
{
	FAILED_CHECK_RETURN(Engine::CScene::LateReady_Scene(), E_FAIL);

	Engine::CCamera* pCamera = dynamic_cast<Engine::CCamera*>(Engine::Get_GameObject(L"GameLogic", L"DynamicCamera"));
	NULL_CHECK_RETURN(pCamera, E_FAIL);

	if (pCamera->Get_IsMainCamera())
		//Engine::Get_Renderer()->Set_Perspective(*pCamera->Get_Projection());

	return S_OK;
}

_int CTestStage::Update_Scene(const _float& fTimeDelta)
{
	m_pKeyMgr->Update();
	CToolView* pToolView = (CToolView*)((CMainFrame*)(AfxGetApp()->m_pMainWnd))->GetActiveView();
	BOOL chk = false;
	Engine::CScene::Update_Scene(fTimeDelta);
	if (pToolView->m_DObjEnable)
	{
		chk = pToolView->m_DObj.m_NaviButton.GetCheck();
	}
	if (!chk)
	{
		StaticMesh_Picking();
	}
	else
	{
		POINT ptMousePos;
		GetCursorPos(&ptMousePos);
		ScreenToClient(g_hWnd, &ptMousePos);
		POINT Screan = pToolView->Get_WinSize();

		//cout << ptMousePos.x << " , " << ptMousePos.y << endl;
		if (ptMousePos.x >= 0 && ptMousePos.x <= Screan.x &&
			ptMousePos.y >= 0 && ptMousePos.y <= Screan.y)
		{
			if (m_pKeyMgr->KeyDown(Engine::KEY_LBUTTON))
			{
				NaviMesh_Picking();
			}
		}
	}
	return S_OK;
}

void CTestStage::Render_Scene(void)
{

}

CTestStage* CTestStage::Create(LPDIRECT3DDEVICE9 pGraphicDev, CToolView* m_pView)
{
	CTestStage*	pInstance = new CTestStage(pGraphicDev);

	if (FAILED(pInstance->Ready_Scene()))
		Engine::Safe_Release(pInstance);
	pInstance->m_pView = m_pView;
	return pInstance;
}

void CTestStage::Free(void)
{
	Engine::CScene::Free();
}

HRESULT CTestStage::Ready_GameLogic_Layer(const _tchar* pLayerTag)
{
	Engine::CLayer* pLayer = Engine::CLayer::Create();
	NULL_CHECK_RETURN(pLayer, E_FAIL);
	m_pLayer = Engine::CLayer::Create();
	NULL_CHECK_RETURN(m_pLayer, E_FAIL);

	m_pColLayer = Engine::CLayer::Create();
	NULL_CHECK_RETURN(m_pColLayer, E_FAIL);
	m_pNaviLayer = Engine::CLayer::Create();
	NULL_CHECK_RETURN(m_pNaviLayer, E_FAIL);
	Engine::CGameObject*		pGameObject = nullptr;

	// Terrain
	//pGameObject = CTerrain::Create(m_pGraphicDev);
	//NULL_CHECK_RETURN(pGameObject, E_FAIL);
	//FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Terrain", pGameObject), E_FAIL);

	// Player
	//pGameObject = CPlayer::Create(m_pGraphicDev);
	//NULL_CHECK_RETURN(pGameObject, E_FAIL);
	//FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Player", pGameObject), E_FAIL);
	//
	//pGameObject = CToolCamera::Create(m_pGraphicDev);
	//NULL_CHECK_RETURN(pGameObject, E_FAIL);
	//FAILED_CHECK_RETURN(m_pLayer->Add_GameObject(L"ToolCamera", pGameObject), E_FAIL);


	pGameObject = CDynamicCamera::Create(m_pGraphicDev,
		&_vec3(0.f, 5.f, -5.f),
		&_vec3(0.f, 0.f, 0.f),
		&_vec3(0.f, 1.f, 0.f),
		D3DXToRadian(60.f),
		_float(WINCX) / WINCY,
		0.5f,
		1000.f,
		true);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"DynamicCamera", pGameObject), E_FAIL);

	/*pGameObject = m_pTerrain = CTerrain::Create(m_pGraphicDev);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Terrain", pGameObject), E_FAIL);*/

	m_mapLayer.emplace(pLayerTag, pLayer);

	return S_OK;
}

HRESULT CTestStage::Ready_Resource(LPDIRECT3DDEVICE9& pGraphicDev, RESOURCEID eMax)
{
	// 메모리 예약
	Engine::Reserve_ContainerSize(eMax);

	FAILED_CHECK_RETURN(Engine::Ready_DebugMesh(m_pGraphicDev), E_FAIL);

	FAILED_CHECK_RETURN(Engine::Ready_Buffer(pGraphicDev,
		RESOURCE_STATIC,
		L"Buffer_Terrain",
		Engine::BUFFER_TERRAINTEX,
		VTXCNTX,
		VTXCNTZ,
		VTXITV),

		E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Buffer(pGraphicDev,
		RESOURCE_STATIC,
		L"Buffer_TerrainCol",
		Engine::BUFFER_TERRAINCOL,
		200,
		30,
		VTXITV ,_vec3(0.f,0.1f,-10.f)),
		E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Buffer(pGraphicDev,
		RESOURCE_STATIC,
		L"Buffer_RcTex",
		Engine::BUFFER_RCTEX),
		E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Buffer(pGraphicDev,
		RESOURCE_STATIC,
		L"Buffer_LightTex",
		Engine::BUFFER_LIGHTTEX),
		E_FAIL);

	FAILED_CHECK_RETURN(Engine::Ready_Buffer(pGraphicDev,
		RESOURCE_STATIC,
		L"Tool_Buffer",
		Engine::BUFFER_TOOLTEX),
		E_FAIL);

	// buffer
	FAILED_CHECK_RETURN(Engine::Ready_Buffer(m_pGraphicDev,
		RESOURCE_STATIC,
		L"Buffer_TerrainTex",
		Engine::BUFFER_TERRAINTEX,
		VTXCNTX,
		VTXCNTZ,
		VTXITV),
		E_FAIL);

	FAILED_CHECK_RETURN(Engine::Ready_Buffer(m_pGraphicDev,
		RESOURCE_STATIC,
		L"Buffer_CubeTex",
		Engine::BUFFER_CUBETEX),
		E_FAIL);
	// 텍스쳐




	FAILED_CHECK_RETURN(Engine::Ready_Meshes(m_pGraphicDev,
		RESOURCE_STATIC,
		L"Mesh_Player",
		Engine::TYPE_DYNAMIC,
		L"../../Client/Bin/Resource/Mesh/DynamicMesh/Player/Six/",
		L"Six.X"),
		E_FAIL);

	FAILED_CHECK_RETURN(Engine::Ready_Meshes(m_pGraphicDev,
		RESOURCE_STATIC,
		L"Mesh_Leech",
		Engine::TYPE_DYNAMIC,
		L"../../Client/Bin/Resource/Mesh/DynamicMesh/Leech/",
		L"Leech.X"),
		E_FAIL);

	FAILED_CHECK_RETURN(Engine::Ready_Meshes(m_pGraphicDev,
		RESOURCE_STATIC,
		L"Mesh_Janitor",
		Engine::TYPE_DYNAMIC,
		L"../../Client/Bin/Resource/Mesh/DynamicMesh/Janitor/",
		L"Janitor.X"),
		E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Meshes(m_pGraphicDev,
		RESOURCE_STATIC,
		L"Mesh_Janitor_Arm",
		Engine::TYPE_DYNAMIC,
		L"../../Client/Bin/Resource/Mesh/DynamicMesh/Janitor_Arm/",
		L"Janitor_Arm.X"),
		E_FAIL);


	Engine::CComponent*		pComponent = nullptr;

	// Optimization
	pComponent = Engine::COptimization::Create(m_pGraphicDev, Engine::COptimization::Enable, 129, 129);
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	Engine::Ready_Prototype(L"Proto_Optimization", pComponent);
	return S_OK;
}

void CTestStage::StaticMesh_Picking()
{
	CMainFrame* pMain = (CMainFrame*)AfxGetMainWnd();
	Engine::CGameObject* tempObject = nullptr;
	auto iter_Layer = m_mapLayer.begin();
	for (; iter_Layer != m_mapLayer.end(); iter_Layer++)
	{
		if (iter_Layer->first == L"GameObject")
		{
			auto iter = iter_Layer->second->GetMapObj().begin();
			for (; iter != iter_Layer->second->GetMapObj().end(); iter++)
			{
			
					if (dynamic_cast<CMeshObject*>(iter->second)->Picking())
					{
						if (tempObject == nullptr)
						{
							tempObject = iter->second;
						}
						else
						{
							if (dynamic_cast<CMeshObject*>(tempObject)->Get_fDist() > dynamic_cast<CMeshObject*>(iter->second)->Get_fDist())
							{
								tempObject = iter->second;
							}
						}
					}
				
			}
			iter = iter_Layer->second->GetMapObj().begin();
			for (; iter != iter_Layer->second->GetMapObj().end(); iter++)
			{
				
					dynamic_cast<CMeshObject*>(iter->second)->SetbPick(false);
				
			}

		}
	}
	if (tempObject != nullptr)
	{
		dynamic_cast<CMeshObject*>(tempObject)->SetbPick(true);
		if (m_pKeyMgr->KeyDown(Engine::KEY_LBUTTON))
		{		
			pMain->m_wndProperties.SetObjectInfo(tempObject);
			CToolView* pToolView = (CToolView*)((CMainFrame*)(AfxGetApp()->m_pMainWnd))->GetActiveView();
			pToolView->m_DeleteBool = true;
			tempObject->SetSelect(true);
			if (pMain->m_wndClassView.preSelectObj != nullptr)
			{
				m_preSelectObject = pMain->m_wndClassView.preSelectObj;
			}
			if(m_preSelectObject != nullptr)
			{
				if (tempObject != m_preSelectObject)
				{
					m_preSelectObject->SetSelect(false);
				}
			}
			pMain->m_wndClassView.preSelectObj = m_preSelectObject = tempObject;
		}
	}

}

void CTestStage::NaviMesh_Picking()
{

	CMainFrame* pMain = (CMainFrame*)AfxGetMainWnd();
	Engine::CGameObject* tempObject = nullptr;
	auto iter_Layer = m_mapLayer.begin();
	for (; iter_Layer != m_mapLayer.end(); iter_Layer++)
	{
		if (iter_Layer->first == L"NaviObject")
		{
			auto iter = iter_Layer->second->GetMapObj().begin();
			for (; iter != iter_Layer->second->GetMapObj().end(); iter++)
			{

				if (dynamic_cast<CNaviObject*>(iter->second)->Picking())
				{
					if (tempObject == nullptr)
					{
						tempObject = iter->second;
					}
					else
					{
						if (dynamic_cast<CNaviObject*>(tempObject)->Get_fDist() > dynamic_cast<CNaviObject*>(iter->second)->Get_fDist())
						{
							tempObject = iter->second;
						}
					}
				}

			}
			iter = iter_Layer->second->GetMapObj().begin();
			for (; iter != iter_Layer->second->GetMapObj().end(); iter++)
			{

				//dynamic_cast<CNaviObject*>(iter->second)->SetbPick(false);

			}

		}
	}
	if (tempObject != nullptr)
	{
	
			pMain->m_wndProperties.SetTerrainInfo(tempObject);
			CToolView* pToolView = (CToolView*)((CMainFrame*)(AfxGetApp()->m_pMainWnd))->GetActiveView();
			pToolView->m_DeleteBool = true;
			tempObject->SetSelect(true);
			if (pMain->m_wndClassView.preSelectObj != nullptr)
			{
				m_preSelectObject = pMain->m_wndClassView.preSelectObj;
			}
			if (m_preSelectObject != nullptr)
			{
				if (tempObject != m_preSelectObject)
				{
					m_preSelectObject->SetSelect(false);
				}
			}
			pMain->m_wndClassView.preSelectObj = m_preSelectObject = tempObject;
		
	}

}

HRESULT CTestStage::Add_GameObject(Engine::OBJECTTYPE objType, int monsterType, const _tchar * pObjName, const _tchar* pObjTypeName)
{
	Engine::CGameObject*		pGameObject = nullptr;

	pGameObject = CTempObject::Create(m_pGraphicDev);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	pGameObject->Set_ObjType(objType);
	pGameObject->Set_ObjTypeName(pObjTypeName);
	pGameObject->Set_ObjName(pObjName);
	if (objType == Engine::OBJ_MONSTER)
		pGameObject->Set_MonsterType((Engine::MONSTERTYPE)monsterType);

	dynamic_cast<CTempObject*>(pGameObject)->SetTestTexure(objType);
	FAILED_CHECK_RETURN(m_pLayer->Add_GameObject(pObjName, pGameObject), E_FAIL);
	m_mapLayer.emplace(L"GameObject", m_pLayer);
	return S_OK;
}

HRESULT CTestStage::Add_GameObject_Mesh(Engine::OBJECTTYPE objType, Engine::MESHTYPE eMeshType, CString sMeshName, const _tchar * pObjName, const _tchar * pObjTypeName)
{
	Engine::CGameObject*		pGameObject = nullptr;

	pGameObject = CMeshObject::Create(m_pGraphicDev, eMeshType, sMeshName);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	pGameObject->Set_ObjType(objType);
	pGameObject->Set_ObjTypeName(pObjTypeName);
	pGameObject->Set_ObjName(pObjName);

	FAILED_CHECK_RETURN(m_pLayer->Add_GameObject(pObjName, pGameObject), E_FAIL);
	m_mapLayer.emplace(L"GameObject", m_pLayer);
	return S_OK;
}



HRESULT CTestStage::Copy_GameObject(Engine::CGameObject * object, const _tchar * pObjName)
{
	Engine::CGameObject*		pGameObject = nullptr;

	pGameObject = CTempObject::Create(m_pGraphicDev, object);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	dynamic_cast<CTempObject*>(pGameObject)->SetTestTexure(pGameObject->Get_ObjType());
	pGameObject->Set_ObjName(pObjName);
	FAILED_CHECK_RETURN(m_pLayer->Add_GameObject(pObjName, pGameObject), E_FAIL);

	m_mapLayer.emplace(L"GameObject", m_pLayer);

	return S_OK;
}

HRESULT CTestStage::Copy_GameObject(Engine::CGameObject * object, const _tchar * pObjName, _vec3 tempPos)
{
	Engine::CGameObject*		pGameObject = nullptr;

	pGameObject = CTempObject::Create(m_pGraphicDev, object);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	dynamic_cast<CTempObject*>(pGameObject)->SetTestTexure(pGameObject->Get_ObjType());
	pGameObject->GetTransformCom()->m_vInfo[Engine::INFO_POS] += tempPos;
	pGameObject->Set_ObjName(pObjName);
	FAILED_CHECK_RETURN(m_pLayer->Add_GameObject(pObjName, pGameObject), E_FAIL);

	m_mapLayer.emplace(L"GameObject", m_pLayer);

	return S_OK;
}



HRESULT CTestStage::Add_ColObject(const _tchar * pObjName,Engine::COLLTYPE coltype)
{
	Engine::CGameObject*		pGameObject = nullptr;

	pGameObject = CColObject::Create(m_pGraphicDev, coltype);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	pGameObject->Set_ObjType(Engine::OBJ_COL);
	pGameObject->Set_ObjName(pObjName);
	FAILED_CHECK_RETURN(m_pColLayer->Add_GameObject(pObjName, pGameObject), E_FAIL);
	m_mapLayer.emplace(L"ColObject", m_pColLayer);
}

HRESULT CTestStage::Add_NaviObject(const _tchar * pObjName)
{
	Engine::CGameObject*		pGameObject = nullptr;

	pGameObject = CNaviObject::Create(m_pGraphicDev);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);

	FAILED_CHECK_RETURN(m_pNaviLayer->Add_GameObject(pObjName, pGameObject), E_FAIL);
	m_mapLayer.emplace(L"NaviObject", m_pNaviLayer);
}

HRESULT CTestStage::Copy_NaviObject(Engine::CGameObject* object,const _tchar * pObjName)
{
	Engine::CGameObject*		pGameObject = nullptr;

	pGameObject = CNaviObject::Create(m_pGraphicDev, object);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	pGameObject->Set_ObjName(pObjName);
	FAILED_CHECK_RETURN(m_pNaviLayer->Add_GameObject(pObjName, pGameObject), E_FAIL);

	m_mapLayer.emplace(L"NaviObject", m_pNaviLayer);

	return S_OK;
}

void CTestStage::Save(const TCHAR * pFilePath)
{
	CToolView* pToolView = (CToolView*)((CMainFrame*)(AfxGetApp()->m_pMainWnd))->GetActiveView();

	HANDLE hFile = ::CreateFile(pFilePath, GENERIC_WRITE, 0, nullptr,
		CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);

	if (INVALID_HANDLE_VALUE == hFile)
		return;
	DWORD dwBytes = 0;
	for (int i = 0; i < Engine::OBJ_END; i++)
	{
		WriteFile(hFile, &pToolView->m_DObj.m_iObjCount[i], sizeof(int), &dwBytes, nullptr);
		WriteFile(hFile, &pToolView->m_DObj.m_iObjNameCount[i], sizeof(int), &dwBytes, nullptr);
	}

	auto iter = m_mapLayer.begin();
	for (; iter != m_mapLayer.end(); iter++)
	{
		if (wcscmp(iter->first.c_str(), L"GameObject") == 0)
		{
			iter->second->Save_Object(hFile, dwBytes, pFilePath, true);
			break;
		}
	}
	//m_pTerrain->Save_NaviTerrain(dwBytes, hFile);
	CloseHandle(hFile);
}

void CTestStage::Load(const TCHAR * pFilePath, int &count)
{
	bool flag = false;
	auto iter = m_mapLayer.begin();
	CMainFrame* pMain = (CMainFrame*)AfxGetMainWnd();
	pMain->m_wndClassView.ClearGameObject();
	for (; iter != m_mapLayer.end();)
	{

		if (wcscmp(iter->first.c_str(), L"GameObject") == 0)
		{
			Engine::Safe_Release(iter->second);
			Engine::Safe_Release(m_pLayer);
			iter = m_mapLayer.erase(iter);

			m_pLayer = Engine::CLayer::Create();
			flag = true;
			break;
		}
		else
		{
			iter++;
		}
	}
	if (!flag)
	{
		if (m_pLayer == nullptr)
		{
			m_pLayer = Engine::CLayer::Create();
		}
	}
	CToolView* pToolView = (CToolView*)((CMainFrame*)(AfxGetApp()->m_pMainWnd))->GetActiveView();

	HANDLE hFile = ::CreateFile(pFilePath, GENERIC_READ, 0, nullptr,
		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);

	if (INVALID_HANDLE_VALUE == hFile)
		return;
	DWORD dwBytes = 0;
	int tempCount = 0;
	for (int i = 0; i < Engine::OBJ_END; i++)
	{
		ReadFile(hFile, &pToolView->m_DObj.m_iObjCount[i], sizeof(int), &dwBytes, nullptr);
		ReadFile(hFile, &pToolView->m_DObj.m_iObjNameCount[i], sizeof(int), &dwBytes, nullptr);
		if(i != Engine::OBJ_COL)
			tempCount += pToolView->m_DObj.m_iObjCount[i];
	}

	m_pLayer->Load_Object(hFile, dwBytes, pFilePath, count);

	auto iter_Layer = m_pLayer->GetMapObj().begin();
	for (int i = 0; i < tempCount; i++)
	{
		Engine::CGameObject*		pGameObject = nullptr;
		wstring tempStr = iter_Layer->first.substr(0, 3);
		if (tempStr == L"Obj")
		{
			pGameObject = CTempObject::Create(m_pGraphicDev);
			NULL(pGameObject, E_FAIL);
			iter_Layer->second = pGameObject->Load_Component(dwBytes, hFile, true);
			pMain->m_wndClassView.UpdateList(iter_Layer->second->Get_ObjType(), iter_Layer->second->Get_MonsterType(), iter_Layer->first.c_str());
			dynamic_cast<CTempObject*>(pGameObject)->SetTestTexure(iter_Layer->second->Get_ObjType());
		}
		else if (tempStr == L"Sta")
		{
			pGameObject = CMeshObject::Create(m_pGraphicDev,Engine::TYPE_STATIC);
			NULL(pGameObject, E_FAIL);
			iter_Layer->second = pGameObject->Load_Component(dwBytes, hFile, true);
			dynamic_cast<CMeshObject*>(pGameObject)->Set_sMeshName(pGameObject->Get_MeshName().c_str());
			dynamic_cast<CMeshObject*>(pGameObject)->Set_TestCol();
			pGameObject->Set_ObjName(iter_Layer->first.c_str());
			pMain->m_wndClassView.UpdateMeshList(iter_Layer->second->Get_ObjType(), iter_Layer->first.c_str());
		}
		else if (tempStr == L"Dyn")
		{
			pGameObject = CMeshObject::Create(m_pGraphicDev, Engine::TYPE_DYNAMIC);
			NULL(pGameObject, E_FAIL);
			iter_Layer->second = pGameObject->Load_Component(dwBytes, hFile, true);
			dynamic_cast<CMeshObject*>(pGameObject)->Set_sMeshName(pGameObject->Get_MeshName().c_str());
			pGameObject->Set_ObjName(iter_Layer->first.c_str());
			pMain->m_wndClassView.UpdateMeshList(iter_Layer->second->Get_ObjType(), iter_Layer->first.c_str());
		}
		iter_Layer++;
	}
	//m_pTerrain->Load_NaviTerrain(dwBytes, hFile);

	CloseHandle(hFile);
	m_mapLayer.emplace(L"GameObject", m_pLayer);

}

void CTestStage::Navi_Save(const TCHAR * pFilePath)
{
	CToolView* pToolView = (CToolView*)((CMainFrame*)(AfxGetApp()->m_pMainWnd))->GetActiveView();

	HANDLE hFile = ::CreateFile(pFilePath, GENERIC_WRITE, 0, nullptr,
		CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);

	if (INVALID_HANDLE_VALUE == hFile)
		return;
	DWORD dwBytes = 0;

	auto iter = m_mapLayer.begin();
	for (; iter != m_mapLayer.end(); iter++)
	{
		if (wcscmp(iter->first.c_str(), L"NaviObject") == 0)
		{
			CToolView* pToolView = (CToolView*)((CMainFrame*)(AfxGetApp()->m_pMainWnd))->GetActiveView();
			auto iter_layer = iter->second->GetMapObj().begin();
			WriteFile(hFile, &pToolView->NaviAllCount, sizeof(int), &dwBytes, nullptr);
			for (; iter_layer != iter->second->GetMapObj().end(); iter_layer++)
			{
				dynamic_cast<CNaviObject*>(iter_layer->second)->Save(dwBytes, hFile);
			}
			//iter->second->Save_Object(hFile, dwBytes, pFilePath, false);
			break;
		}
	}
	CloseHandle(hFile);
}

void CTestStage::Navi_Load(const TCHAR * pFilePath, int & count)
{
	bool flag = false;
	auto iter = m_mapLayer.begin();
	CMainFrame* pMain = (CMainFrame*)AfxGetMainWnd();
	pMain->m_wndClassView.ClearColObject();
	for (; iter != m_mapLayer.end();)
	{
		if (wcscmp(iter->first.c_str(), L"NaviObject") == 0)
		{
			Engine::Safe_Release(iter->second);
			Engine::Safe_Release(m_pNaviLayer);
			iter = m_mapLayer.erase(iter);

			m_pNaviLayer = Engine::CLayer::Create();
			flag = true;
			break;
		}
		else
		{
			iter++;
		}
	}
	if (!flag)
	{
		if (m_pNaviLayer == nullptr)
		{
			m_pNaviLayer = Engine::CLayer::Create();
		}
	}
	CToolView* pToolView = (CToolView*)((CMainFrame*)(AfxGetApp()->m_pMainWnd))->GetActiveView();

	HANDLE hFile = ::CreateFile(pFilePath, GENERIC_READ, 0, nullptr,
		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);

	if (INVALID_HANDLE_VALUE == hFile)
		return;
	DWORD dwBytes = 0;



	ReadFile(hFile, &pToolView->NaviAllCount, sizeof(int), &dwBytes, nullptr);
	for (int i= 0;i< pToolView->NaviAllCount; i++)
	{
		Engine::CGameObject*		pGameObject = nullptr;
		pGameObject = CNaviObject::Create(m_pGraphicDev);
		NULL(pGameObject, E_FAIL);
		dynamic_cast<CNaviObject*>(pGameObject)->Load(dwBytes, hFile);
		CString str = L"Navi_Object";
		CString strCount;
		strCount.Format(_T("%d"), pToolView->NaviNowCount++);
		str += strCount;
		pGameObject->Set_ObjName(str);
		FAILED_CHECK_RETURN(m_pNaviLayer->Add_GameObject(str, pGameObject));
		pMain->m_wndClassView.UpdateNaviList(str);
	}
	CloseHandle(hFile);
	m_mapLayer.emplace(L"NaviObject", m_pNaviLayer);


}

void CTestStage::Col_Save(const TCHAR * pFilePath)
{
	CToolView* pToolView = (CToolView*)((CMainFrame*)(AfxGetApp()->m_pMainWnd))->GetActiveView();

	HANDLE hFile = ::CreateFile(pFilePath, GENERIC_WRITE, 0, nullptr,
		CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);

	if (INVALID_HANDLE_VALUE == hFile)
		return;
	DWORD dwBytes = 0;

	auto iter = m_mapLayer.begin();
	for (; iter != m_mapLayer.end(); iter++)
	{
		if (wcscmp(iter->first.c_str(), L"ColObject") == 0)
		{
			iter->second->Save_Object(hFile, dwBytes, pFilePath, false);
			break;
		}
	}
	CloseHandle(hFile);
}

void CTestStage::Col_Load(const TCHAR * pFilePath, int & count)
{
	bool flag = false;
	auto iter = m_mapLayer.begin();
	CMainFrame* pMain = (CMainFrame*)AfxGetMainWnd();
	pMain->m_wndClassView.ClearColObject();
	for (; iter != m_mapLayer.end();)
	{
		if (wcscmp(iter->first.c_str(), L"ColObject") == 0)
		{
			Engine::Safe_Release(iter->second);
			Engine::Safe_Release(m_pColLayer);
			iter = m_mapLayer.erase(iter);

			m_pColLayer = Engine::CLayer::Create();
			flag = true;
			break;
		}
		else
		{
			iter++;
		}
	}
	if (!flag)
	{
		if (m_pColLayer == nullptr)
		{
			m_pColLayer = Engine::CLayer::Create();
		}
	}
	CToolView* pToolView = (CToolView*)((CMainFrame*)(AfxGetApp()->m_pMainWnd))->GetActiveView();

	HANDLE hFile = ::CreateFile(pFilePath, GENERIC_READ, 0, nullptr,
		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);

	if (INVALID_HANDLE_VALUE == hFile)
		return;
	DWORD dwBytes = 0;

	m_pColLayer->Load_Object(hFile, dwBytes, pFilePath, count);

	auto iter_Layer = m_pColLayer->GetMapObj().begin();
	for (int i = 0; i < m_pColLayer->GetMapObj().size(); i++)
	{
		Engine::CGameObject*		pGameObject = nullptr;
		pGameObject = CColObject::Create(m_pGraphicDev, Engine::COLLTYPE(0));
		NULL(pGameObject, E_FAIL);
		iter_Layer->second = pGameObject->Load_Component(dwBytes, hFile, false);
		pMain->m_wndClassView.UpdateColList(iter_Layer->first.c_str());
		dynamic_cast<CColObject*>(iter_Layer->second)->Set_TestCol();
		pGameObject->Set_ObjName(iter_Layer->first.c_str());
		if (iter_Layer->first.find(L"Trigger", 0) != wstring::npos)
		{
			dynamic_cast<CColObject*>(iter_Layer->second)->SetColType(Engine::COL_TRIGGER);
		}
		iter_Layer++;

	}
	CloseHandle(hFile);
	m_mapLayer.emplace(L"ColObject", m_pColLayer);
}

HRESULT CTestStage::Delete_Object(const _tchar * pObjectName, int &count)
{
	Engine::CGameObject* tempObject = Get_GameObject(L"GameObject", pObjectName);

	auto iter = m_mapLayer.begin();
	for (; iter != m_mapLayer.end(); iter++)
	{
		if (wcscmp(iter->first.c_str(), L"GameObject") == 0)
		{
			auto iter_Layer = iter->second->GetMapObj().begin();
			for (; iter_Layer != iter->second->GetMapObj().end(); iter_Layer++)
			{
				if (iter_Layer->second == tempObject)
				{
					CMainFrame * pMain = (CMainFrame*)AfxGetMainWnd();
					pMain->m_wndClassView.DeleteList(iter_Layer->first.c_str());
					Engine::Safe_Release(iter_Layer->second);
					iter->second->GetMapObj().erase(iter_Layer->first);

					count--;
					return S_OK;
				}
			}
		}
	}


	return E_FAIL;
}

HRESULT CTestStage::Delete_Col_Object(const _tchar * pObjectName, int & count)
{
	Engine::CGameObject* tempObject = Get_GameObject(L"ColObject", pObjectName);

	auto iter = m_mapLayer.begin();
	for (; iter != m_mapLayer.end(); iter++)
	{
		if (wcscmp(iter->first.c_str(), L"ColObject") == 0)
		{
			auto iter_Layer = iter->second->GetMapObj().begin();
			for (; iter_Layer != iter->second->GetMapObj().end(); iter_Layer++)
			{
				if (iter_Layer->second == tempObject)
				{
					CMainFrame * pMain = (CMainFrame*)AfxGetMainWnd();
					pMain->m_wndClassView.DeleteList(iter_Layer->first.c_str());
					Engine::Safe_Release(iter_Layer->second);
					iter->second->GetMapObj().erase(iter_Layer->first);

					count--;
					return S_OK;
				}
			}
		}
	}

}

HRESULT CTestStage::Delete_Navi_Object(const _tchar * pObjectName, int & count)
{
	Engine::CGameObject* tempObject = Get_GameObject(L"NaviObject", pObjectName);

	auto iter = m_mapLayer.begin();
	for (; iter != m_mapLayer.end(); iter++)
	{
		if (wcscmp(iter->first.c_str(), L"NaviObject") == 0)
		{
			auto iter_Layer = iter->second->GetMapObj().begin();
			for (; iter_Layer != iter->second->GetMapObj().end(); iter_Layer++)
			{
				if (iter_Layer->second == tempObject)
				{
					CMainFrame * pMain = (CMainFrame*)AfxGetMainWnd();
					pMain->m_wndClassView.DeleteList(iter_Layer->first.c_str());
					Engine::Safe_Release(iter_Layer->second);
					iter->second->GetMapObj().erase(iter_Layer->first);

					count--;
					return S_OK;
				}
			}
		}
	}

}
