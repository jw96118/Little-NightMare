#include "stdafx.h"
#include "Stage.h"
#include "ColObject.h"
#include "Export_Function.h"
#include "CollisionMgr.h"
#include "MeshObject.h"
#include "Leech.h"
#include "Title.h"
#include "Janitor.h"
#include "Janitor_Arm.h"
#include "FirstPersonCamera.h"
#include "InteractionObject.h"
#include "Lighter.h"
#include "Effect.h"
_bool g_bLoadFirst = false;
_bool g_bEnd = false;
_bool g_bColCheat = false;
CStage::CStage(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CScene(pGraphicDev)
	, m_fTime(0.f)
	, m_dwRenderCnt(0)
{
	ZeroMemory(m_szFPS, sizeof(m_szFPS));
}

CStage::~CStage(void)
{

}

HRESULT CStage::Ready_Scene(void)
{
	FAILED_CHECK_RETURN(Engine::CScene::Ready_Scene(), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Environment_Layer(L"01.Environment"), E_FAIL);
	FAILED_CHECK_RETURN(Ready_GameLogic_Layer(L"02.GameLogic"), E_FAIL);
	FAILED_CHECK_RETURN(Ready_DeathScene_Layer(L"03.Death"), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Effect_Layer(L"04.Effect"), E_FAIL);


	Engine::Stop_All();
	Engine::Play_BGM(L"SW_Amb_Tutorial_VeryLargeRoom_4.ogg");

	m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, TRUE);

	m_pGraphicDev->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	m_pGraphicDev->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	m_pGraphicDev->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

	Engine::CLayer*      pLayer = Engine::CLayer::Create();
	NULL_CHECK_RETURN(pLayer, E_FAIL);

	Engine::Set_RenderFlag(m_bSetRender);

	Engine::CGameObject*      pGameObject = nullptr;
	pGameObject = CLighter::Create(m_pGraphicDev);

	pLayer->Add_GameObject(L"Mesh_Lighter", pGameObject);
	m_mapLayer.emplace(L"Lighter", pLayer);

	return S_OK;
}

HRESULT CStage::LateReady_Scene(void)
{
	//Engine::Play_BGM(L"016 Medieval -  The Chamber.wav");

	FAILED_CHECK_RETURN(Engine::CScene::LateReady_Scene(), E_FAIL);

	// MainCamera ¼¼ÆÃ
	/*Engine::CCamera* pCamera = dynamic_cast<Engine::CCamera*>(Engine::Get_GameObject(L"01.Environment", L"MainCamera"));
	NULL_CHECK_RETURN(pCamera, E_FAIL);*/

	//if (pCamera->Get_IsMainCamera())
		//Engine::Get_Renderer()->Set_Perspective(*pCamera->Get_Projection());
	m_pPlayer = dynamic_cast<CPlayer*>(Engine::Get_GameObject(L"02.GameLogic", L"Dyn_Player"));

	FAILED_CHECK_RETURN(Ready_LightInfo(), E_FAIL);



	return S_OK;
}

_int CStage::Update_Scene(const _float& fTimeDelta)
{
	m_fTime += fTimeDelta;


	if (Engine::Get_KeyMgr()->KeyDown(Engine::KEY_I))
	{
		m_bSetRender = !m_bSetRender;
		Engine::Set_RenderFlag(m_bSetRender);
	}

	_int iEvent = Engine::CScene::Update_Scene(fTimeDelta);
	if (g_bEnd)
	{
		Engine::CGameObject* pGameObject = Engine::Get_GameObject(L"03.Death", L"BackGround_Black");
		m_fEndCount += fTimeDelta * 0.5f;
		dynamic_cast<CTitle*>(pGameObject)->Set_TexAlpha(m_fEndCount);
		if (m_fEndCount >= 1.f)
		{
			PostQuitMessage(0);
		}
	}
	if (m_bStart)
	{
		Engine::CGameObject* pGameObject = Engine::Get_GameObject(L"03.Death", L"BackGround_Black");
		m_fStartCount -= fTimeDelta;
		dynamic_cast<CTitle*>(pGameObject)->Set_TexAlpha(m_fStartCount);
		if (m_fStartCount <= 0.f)
		{
			m_bStart = false;
		}
	}
	else
	{
		if (m_pPlayer->Get_LoadScene())
		{
			Engine::CGameObject* pGameObject = Engine::Get_GameObject(L"03.Death", L"BackGround_Black");
			m_fDeathCount += fTimeDelta;
			dynamic_cast<CTitle*>(pGameObject)->Set_TexAlpha(m_fDeathCount);
			if (m_fDeathCount >= 1.f)
			{
				g_bLoadFirst = true;
				iEvent = Engine::CScene::Update_Scene(fTimeDelta);
				Engine::Play_BGM(L"SW_Amb_Tutorial_VeryLargeRoom_4.ogg");
			}
		}
		else if (g_bLoadFirst)
		{
			Engine::CGameObject* pGameObject = Engine::Get_GameObject(L"03.Death", L"BackGround_Black");
			m_fDeathCount -= fTimeDelta;
			dynamic_cast<CTitle*>(pGameObject)->Set_TexAlpha(m_fDeathCount);
			if (m_fDeathCount <= 0.f)
			{
				g_bLoadFirst = false;
			}
		}
	}
	return iEvent;
}

_int CStage::LateUpdate_Scene(const _float & fTimeDelta)
{
	Engine::CLayer* pLayerWall = nullptr;
	if (m_pPlayer->GetTransformCom()->Get_Info(Engine::INFO_POS)->x <= 96.f)
		pLayerWall = Engine::Get_Layer(L"Collider1");
	else
		pLayerWall = Engine::Get_Layer(L"Collider2");
	Engine::CLayer* pLayerGameLogic = Engine::Get_Layer(L"02.GameLogic");
	//Engine::CLayer* pLayerDoor = Engine::Get_Layer(L"05.DoorCol");
	//Engine::CLayer* pMonsterWall = Engine::Get_Layer(L"06.Monster");
	//Engine::CGameObject* pObject = Engine::Get_GameObject(L"02.GameLogic", L"Dyn_Player");
	if (!g_bColCheat)
	{
		_bool flag = false;
		if (m_pPlayer->GetTransformCom()->Get_Info(Engine::INFO_POS)->x > 96.f && m_pPlayer->GetTransformCom()->Get_Info(Engine::INFO_POS)->x <= 130.f)
		{
			Engine::CGameObject* pInterObject = Engine::Get_GameObject(L"02.GameLogic", L"Sta_Obj_WoodBox_10");
			CollisionMgr::CollisionWallToObject(pLayerWall, pInterObject);
		}

		if (pLayerWall != nullptr)
			CollisionMgr::CollisionWallToPlayer(pLayerWall, m_pPlayer, flag);
		CollisionMgr::CollisionObjectToPlayerObb(pLayerGameLogic, m_pPlayer);
		CollisionMgr::CollisionObjectToPlayerAABB(pLayerGameLogic, m_pPlayer, flag);
		if (m_pPlayer->GetTransformCom()->Get_Info(Engine::INFO_POS)->x <= 80.f)
			CollisionMgr::CollisionLeechToPlayer(pLayerGameLogic, m_pPlayer);

		if ((m_pPlayer->GetTransformCom()->Get_Info(Engine::INFO_POS)->x > 100.f && m_pPlayer->GetTransformCom()->Get_Info(Engine::INFO_POS)->x < 140.f) ||
			(m_pPlayer->GetTransformCom()->Get_Info(Engine::INFO_POS)->x > 158.f && m_pPlayer->GetTransformCom()->Get_Info(Engine::INFO_POS)->x < 210.f))
		{
			CollisionMgr::CollisionJanitorToPlayerAABB(pLayerGameLogic, m_pPlayer);
			CollisionMgr::CollisionJanitorToPlayer(pLayerGameLogic, m_pPlayer);
		}
		if ((m_pPlayer->GetTransformCom()->Get_Info(Engine::INFO_POS)->x > 140.f &&m_pPlayer->GetTransformCom()->Get_Info(Engine::INFO_POS)->x < 158.f))
		{
			CollisionMgr::CollisionJanitorArmToPlayer(pLayerGameLogic, m_pPlayer);
		}
		//CollisionMgr::CollisionWallToMonster(pLayerWall, pMonsterWall);
	}
	_int iEvent = Engine::CScene::LateUpdate_Scene(fTimeDelta);


	_matrix tempMat = m_pPlayer->Get_LighterPos();
	_vec3 testPos = { tempMat._41,tempMat._42, tempMat._43};
	
	Engine::Set_LightInfo(1)->Position = testPos;


	return NO_EVENT;
}

void CStage::Render_Scene(void)
{
	POINT ptMousePos;
	GetCursorPos(&ptMousePos);
	ScreenToClient(g_hWnd, &ptMousePos);
	//swprintf_s(m_szScrollPos, L"ScrollPos X=: %d  , Y= %d ", ptMousePos.x, ptMousePos.y);
	//Engine::Render_Font(L"Font_Default", m_szScrollPos, &_vec2(10.f, 10.f), D3DXCOLOR(0.f, 1.f, 0.f, 1.f));

	++m_dwRenderCnt;

	if (m_fTime >= 1.f)
	{
		wsprintf(m_szFPS, L"FPS : %d", m_dwRenderCnt);
		m_fTime = 0.f;
		m_dwRenderCnt = 0;
	}
	if(m_bSetRender)
	Engine::Render_Font(L"Font_Default", m_szFPS, &_vec2(10.f, 40.f), D3DXCOLOR(1.f, 0.f, 0.f, 1.f));
}

CStage* CStage::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CStage*   pInstance = new CStage(pGraphicDev);

	if (FAILED(pInstance->Ready_Scene()))
		Engine::Safe_Release(pInstance);

	return pInstance;
}

void CStage::Free(void)
{
	Engine::CScene::Free();
}

HRESULT CStage::Ready_GameLogic_Layer(const _tchar* pLayerTag)
{
	Engine::CLayer*      pLayer = Engine::CLayer::Create();
	NULL_CHECK_RETURN(pLayer, E_FAIL);
	Engine::CLayer*      pNaviLayer = Engine::CLayer::Create();
	NULL_CHECK_RETURN(pNaviLayer, E_FAIL);
	Engine::CLayer*      pColLayer1 = Engine::CLayer::Create();
	NULL_CHECK_RETURN(pColLayer1, E_FAIL);
	Engine::CLayer*      pColLayer2 = Engine::CLayer::Create();
	NULL_CHECK_RETURN(pColLayer2, E_FAIL);

	Engine::CGameObject*      pGameObject = nullptr;
	pGameObject = CTerrain::Create(m_pGraphicDev);

	Load_Map(pLayer, pLayerTag, L"../../Save/test3.dat");
	m_mapLayer.emplace(pLayerTag, pLayer);
	Load_Navi(pGameObject, L"Com_Navi_1", L"../../Save/Navitest1.dat");
	Load_Navi(pGameObject, L"Com_Navi_2", L"../../Save/Navitest2.dat");
	Load_Navi(pGameObject, L"Com_Navi_3", L"../../Save/Navitest3.dat");
	pNaviLayer->Add_GameObject(L"Terrain", pGameObject);
	m_mapLayer.emplace(L"Navi_Mesh", pNaviLayer);

	Load_Col(pColLayer1, pLayerTag, L"../../Save/testCol.dat");
	m_mapLayer.emplace(L"Collider1", pColLayer1);
	Load_Col(pColLayer2, pLayerTag, L"../../Save/testCol2.dat");
	m_mapLayer.emplace(L"Collider2", pColLayer2);


	return S_OK;
}
HRESULT CStage::Ready_LightInfo(void)
{
	D3DLIGHT9			tLightInfo;
	ZeroMemory(&tLightInfo, sizeof(D3DLIGHT9));

	tLightInfo.Type = D3DLIGHT_DIRECTIONAL;

	tLightInfo.Diffuse = D3DXCOLOR(0.1f, 0.1f, 0.1f, 1.f);
	tLightInfo.Specular = D3DXCOLOR(0.1f, 0.1f, 0.1f, 1.f);
	tLightInfo.Ambient = D3DXCOLOR(0.1f, 0.1f, 0.1f, 1.f);
	tLightInfo.Direction = _vec3(-1.f, -1.f, 1.f);

	FAILED_CHECK_RETURN(Engine::Ready_Light(m_pGraphicDev, &tLightInfo, 0), E_FAIL);

	if (m_pPlayer != nullptr)
	{
		tLightInfo.Type = D3DLIGHT_POINT;

		tLightInfo.Diffuse  = D3DXCOLOR(1.f, 0.6f, 0.4f, 1.f);
		tLightInfo.Specular = D3DXCOLOR(1.f, 0.6f, 0.4f, 1.f);
		tLightInfo.Ambient  = D3DXCOLOR(1.f, 0.6f, 0.4f, 1.f);
		_vec3 vPos = *m_pPlayer->GetTransformCom()->Get_Info(Engine::INFO_POS);
		vPos.y += 2.f;
		tLightInfo.Position = vPos;
		tLightInfo.Range = 3.f;

		FAILED_CHECK_RETURN(Engine::Ready_Light(m_pGraphicDev, &tLightInfo, 1), E_FAIL);
	}
	tLightInfo.Type = D3DLIGHT_POINT;

	tLightInfo.Diffuse = D3DXCOLOR(0.7f, 0.7f, 0.7f, 1.f);
	tLightInfo.Specular = D3DXCOLOR(0.7f, 0.7f, 0.7f, 1.f);
	tLightInfo.Ambient = D3DXCOLOR(0.7f, 0.7f, 0.7f, 1.f);

	tLightInfo.Position = _vec3(3.5f,5.f,6.f);
	tLightInfo.Range = 10.f;

	FAILED_CHECK_RETURN(Engine::Ready_Light(m_pGraphicDev, &tLightInfo, 2), E_FAIL);


	tLightInfo.Type = D3DLIGHT_POINT;
	tLightInfo.Diffuse = D3DXCOLOR(0.7f, 0.7f, 0.7f, 1.f);
	tLightInfo.Specular = D3DXCOLOR(0.7f, 0.7f, 0.7f, 1.f);
	tLightInfo.Ambient = D3DXCOLOR(0.7f, 0.7f, 0.7f, 1.f);

	tLightInfo.Position = _vec3(41.5f, 5.f, -3.f);
	tLightInfo.Range = 6.f;

	FAILED_CHECK_RETURN(Engine::Ready_Light(m_pGraphicDev, &tLightInfo, 3), E_FAIL);


	tLightInfo.Type = D3DLIGHT_POINT;
	tLightInfo.Diffuse = D3DXCOLOR(0.7f, 0.7f, 0.7f, 1.f);
	tLightInfo.Specular = D3DXCOLOR(0.7f, 0.7f, 0.7f, 1.f);
	tLightInfo.Ambient = D3DXCOLOR(0.7f, 0.7f, 0.7f, 1.f);
	tLightInfo.Position = _vec3(49.7f, 5.f, -3.f);
	tLightInfo.Range = 6.f;

	FAILED_CHECK_RETURN(Engine::Ready_Light(m_pGraphicDev, &tLightInfo, 4), E_FAIL);

	tLightInfo.Type = D3DLIGHT_POINT;
	tLightInfo.Diffuse = D3DXCOLOR(0.7f, 0.7f, 0.7f, 1.f);
	tLightInfo.Specular = D3DXCOLOR(0.7f, 0.7f, 0.7f, 1.f);
	tLightInfo.Ambient = D3DXCOLOR(0.7f, 0.7f, 0.7f, 1.f);
	tLightInfo.Position = _vec3(105.f, -3.f, 7.f);
	tLightInfo.Range = 6.f;

	FAILED_CHECK_RETURN(Engine::Ready_Light(m_pGraphicDev, &tLightInfo, 5), E_FAIL);

	tLightInfo.Type = D3DLIGHT_POINT;
	tLightInfo.Diffuse = D3DXCOLOR(0.7f, 0.7f, 0.7f, 1.f);
	tLightInfo.Specular = D3DXCOLOR(0.7f, 0.7f, 0.7f, 1.f);
	tLightInfo.Ambient = D3DXCOLOR(0.7f, 0.7f, 0.7f, 1.f);
	tLightInfo.Position = _vec3(192.5f, -3.f, 9.f);
	tLightInfo.Range = 7.f;

	FAILED_CHECK_RETURN(Engine::Ready_Light(m_pGraphicDev, &tLightInfo, 6), E_FAIL);


	return S_OK;
}
HRESULT   CStage::Ready_Environment_Layer(const _tchar* pLayerTag)
{
	Engine::CLayer*      pLayer = Engine::CLayer::Create();
	NULL_CHECK_RETURN(pLayer, E_FAIL);

	Engine::CGameObject*      pGameObject = nullptr;

	// SkyBox
	//pGameObject = CSkyBox::Create(m_pGraphicDev , 3);
	//NULL_CHECK_RETURN(pGameObject, E_FAIL);
	//FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"SkyBox", pGameObject), E_FAIL);

	// FirstPersonCamera (Player Camera)
	pGameObject = CFirstPersonCamera::Create(m_pGraphicDev, D3DXToRadian(60.f), _float(WINCX) / WINCY,
		1.f,
		1000.f);
	/*pGameObject = CDynamicCamera::Create(m_pGraphicDev,
		&_vec3(0.f, 5.f, -5.f),
		&_vec3(0.f, 0.f, 0.f),
		&_vec3(0.f, 1.f, 0.f),
		D3DXToRadian(60.f),
		_float(WINCX) / WINCY,
		1.f,
		1000.f,
		true);*/
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"MainCamera", pGameObject), E_FAIL);

	m_mapLayer.emplace(pLayerTag, pLayer);

	return S_OK;
}

HRESULT CStage::Ready_Effect_Layer(const _tchar * pLayerTag)
{
	Engine::CLayer*      pLayer = Engine::CLayer::Create();
	NULL_CHECK_RETURN(pLayer, E_FAIL);

	Engine::CGameObject*      pGameObject = nullptr;


	pGameObject = CEffect::Create(m_pGraphicDev, CEffect::FIRE);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Flame", pGameObject), E_FAIL);

	pGameObject = CEffect::Create(m_pGraphicDev, CEffect::BLOOD1);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Blood_1", pGameObject), E_FAIL);

	pGameObject = CEffect::Create(m_pGraphicDev, CEffect::BLOOD2);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Blood_2", pGameObject), E_FAIL);





	m_mapLayer.emplace(pLayerTag, pLayer);

	return S_OK;
}

HRESULT CStage::Ready_DeathScene_Layer(const _tchar * pLayerTag)
{
	Engine::CLayer*		pLayer = Engine::CLayer::Create();
	NULL_CHECK_RETURN(pLayer, E_FAIL);

	Engine::CGameObject*		pGameObject = nullptr;

	pGameObject = CTitle::Create(m_pGraphicDev, L"Texture_BackGround_Black", _vec2(WINCX, WINCY), _vec3(WINCX* 0.5f, WINCY * 0.5f, 0.f));
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	dynamic_cast<CTitle*>(pGameObject)->Set_TexAlpha(1.f);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"BackGround_Black", pGameObject), E_FAIL);

	m_mapLayer.emplace(pLayerTag, pLayer);

	return S_OK;
}




void CStage::Load_Map(Engine::CLayer* pLayer, const _tchar* pLayerTag, const TCHAR * pFilePath)
{
	int count = 0;

	HANDLE hFile = ::CreateFile(pFilePath, GENERIC_READ, 0, nullptr,
		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);

	if (INVALID_HANDLE_VALUE == hFile)
		return;
	DWORD dwBytes = 0;
	int tempInt[Engine::OBJ_END] = {};
	int tempInt2[Engine::OBJ_END] = {};
	_bool bArmFlag = false;
	for (int i = 0; i < Engine::OBJ_END; i++)
	{
		ReadFile(hFile, &tempInt[i], sizeof(int), &dwBytes, nullptr);
		ReadFile(hFile, &tempInt2[i], sizeof(int), &dwBytes, nullptr);
	}

	Engine::CGameObject*      pGameObject = nullptr;

	wstring tempRoofName = L"";
	_int    RoofCount = 0;
	pLayer->Load_Object(hFile, dwBytes, pFilePath, count);

	auto iter_Layer = pLayer->GetMapObj().begin();
	for (int i = 0; i < count; i++)
	{
		wstring temp = iter_Layer->first;
		if (temp.find(L"Stage", 0) != wstring::npos)
		{
			pGameObject = CMeshObject::Create(m_pGraphicDev, Engine::TYPE_STATIC);
			NULL_CHECK(pGameObject, E_FAIL);
			pGameObject->Set_ObjName(temp.c_str());
			iter_Layer->second = pGameObject->Load_Component(dwBytes, hFile, true);
			dynamic_cast<CMeshObject*>(pGameObject)->SetTestCol();
			iter_Layer++;
		}
		else if (temp.find(L"Player", 0) != wstring::npos)
		{
			pGameObject = CPlayer::Create(m_pGraphicDev);
			NULL_CHECK(pGameObject, E_FAIL);
			pGameObject->Set_ObjName(temp.c_str());
			iter_Layer->second = pGameObject->Load_Component(dwBytes, hFile, true);
			dynamic_cast<CPlayer*>(pGameObject)->Set_Shader();
			iter_Layer++;
		}
		else if (temp.find(L"Leech", 0) != wstring::npos)
		{
			pGameObject = CLeech::Create(m_pGraphicDev);
			NULL_CHECK(pGameObject, E_FAIL);
			pGameObject->Set_ObjName(temp.c_str());
			iter_Layer->second = pGameObject->Load_Component(dwBytes, hFile, true);
			dynamic_cast<CLeech*>(pGameObject)->Set_Shader();
			iter_Layer++;
		}
		else if (temp.find(L"Hatch", 0) != wstring::npos)
		{
			pGameObject = CInteractionObject::Create(m_pGraphicDev, Engine::TYPE_STATIC, CInteractionObject::HATCH);
			NULL_CHECK(pGameObject, E_FAIL);
			pGameObject->Set_ObjName(temp.c_str());
			iter_Layer->second = pGameObject->Load_Component(dwBytes, hFile, true);
			dynamic_cast<CInteractionObject*>(pGameObject)->SetTestCol();
			iter_Layer++;
		}
		else if (temp.find(L"WoodFloor", 0) != wstring::npos)
		{
			pGameObject = CInteractionObject::Create(m_pGraphicDev, Engine::TYPE_STATIC, CInteractionObject::WOODFLOOR);
			NULL_CHECK(pGameObject, E_FAIL);
			pGameObject->Set_ObjName(temp.c_str());
			iter_Layer->second = pGameObject->Load_Component(dwBytes, hFile, true);
			dynamic_cast<CInteractionObject*>(pGameObject)->SetTestCol();
			iter_Layer++;
		}
		else if (temp.find(L"WoodPanelDoor", 0) != wstring::npos)
		{
			pGameObject = CInteractionObject::Create(m_pGraphicDev, Engine::TYPE_STATIC, CInteractionObject::PANELDOOR);
			NULL_CHECK(pGameObject, E_FAIL);
			pGameObject->Set_ObjName(temp.c_str());
			iter_Layer->second = pGameObject->Load_Component(dwBytes, hFile, true);
			dynamic_cast<CInteractionObject*>(pGameObject)->SetTestCol();
			iter_Layer++;
		}
		else if (temp.find(L"ShipDoor_7", 0) != wstring::npos)
		{
			pGameObject = CInteractionObject::Create(m_pGraphicDev, Engine::TYPE_STATIC, CInteractionObject::SHIPDOOR);
			NULL_CHECK(pGameObject, E_FAIL);
			pGameObject->Set_ObjName(temp.c_str());
			iter_Layer->second = pGameObject->Load_Component(dwBytes, hFile, true);
			dynamic_cast<CInteractionObject*>(pGameObject)->SetTestCol();
			iter_Layer++;
		}
		else if (temp.find(L"ShipDoor", 0) != wstring::npos)
		{
			pGameObject = CInteractionObject::Create(m_pGraphicDev, Engine::TYPE_STATIC, CInteractionObject::LASTDOOR);
			NULL_CHECK(pGameObject, E_FAIL);
			pGameObject->Set_ObjName(temp.c_str());
			iter_Layer->second = pGameObject->Load_Component(dwBytes, hFile, true);
			dynamic_cast<CInteractionObject*>(pGameObject)->SetTestCol();
			iter_Layer++;
		}
		else if (temp.find(L"LeverBase", 0) != wstring::npos)
		{
			pGameObject = CInteractionObject::Create(m_pGraphicDev, Engine::TYPE_STATIC, CInteractionObject::LEVERFRAME);
			NULL_CHECK(pGameObject, E_FAIL);
			pGameObject->Set_ObjName(temp.c_str());
			iter_Layer->second = pGameObject->Load_Component(dwBytes, hFile, true);
			dynamic_cast<CInteractionObject*>(pGameObject)->SetTestCol();
			iter_Layer++;
		}
		else if (temp.find(L"Lever", 0) != wstring::npos)
		{
			pGameObject = CInteractionObject::Create(m_pGraphicDev, Engine::TYPE_STATIC, CInteractionObject::LEVER);
			NULL_CHECK(pGameObject, E_FAIL);
			pGameObject->Set_ObjName(temp.c_str());
			iter_Layer->second = pGameObject->Load_Component(dwBytes, hFile, true);
			dynamic_cast<CInteractionObject*>(pGameObject)->SetTestCol();
			iter_Layer++;
		}
		else if (temp.find(L"Box", 0) != wstring::npos)
		{
			pGameObject = CInteractionObject::Create(m_pGraphicDev, Engine::TYPE_STATIC, CInteractionObject::BOX);
			NULL_CHECK(pGameObject, E_FAIL);
			pGameObject->Set_ObjName(temp.c_str());
			iter_Layer->second = pGameObject->Load_Component(dwBytes, hFile, true);
			dynamic_cast<CInteractionObject*>(pGameObject)->SetTestCol();
			iter_Layer++;
		}
		else if (temp.find(L"Cage", 0) != wstring::npos)
		{
			pGameObject = CInteractionObject::Create(m_pGraphicDev, Engine::TYPE_STATIC, CInteractionObject::CAGE);
			NULL_CHECK(pGameObject, E_FAIL);
			pGameObject->Set_ObjName(temp.c_str());
			iter_Layer->second = pGameObject->Load_Component(dwBytes, hFile, true);
			dynamic_cast<CInteractionObject*>(pGameObject)->SetTestCol();
			iter_Layer++;
		}
		else if (temp.find(L"SpawnDoor", 0) != wstring::npos)
		{
			pGameObject = CInteractionObject::Create(m_pGraphicDev, Engine::TYPE_DYNAMIC, CInteractionObject::SPAWNDOOR);
			NULL_CHECK(pGameObject, E_FAIL);
			pGameObject->Set_ObjName(temp.c_str());
			iter_Layer->second = pGameObject->Load_Component(dwBytes, hFile, true);
			dynamic_cast<CInteractionObject*>(pGameObject)->SetTestCol();
			iter_Layer++;
		}
		else if (temp.find(L"AnimatedBar_19", 0) != wstring::npos)
		{
			pGameObject = CInteractionObject::Create(m_pGraphicDev, Engine::TYPE_DYNAMIC, CInteractionObject::CAGE_LEFT_OUT);
			NULL_CHECK(pGameObject, E_FAIL);
			pGameObject->Set_ObjName(temp.c_str());
			iter_Layer->second = pGameObject->Load_Component(dwBytes, hFile, true);
			dynamic_cast<CInteractionObject*>(pGameObject)->SetTestCol();
			iter_Layer++;
		}
		else if (temp.find(L"AnimatedBar", 0) != wstring::npos)
		{
			pGameObject = CInteractionObject::Create(m_pGraphicDev, Engine::TYPE_DYNAMIC, CInteractionObject::CAGE_RIGHT_OUT);
			NULL_CHECK(pGameObject, E_FAIL);
			pGameObject->Set_ObjName(temp.c_str());
			iter_Layer->second = pGameObject->Load_Component(dwBytes, hFile, true);
			dynamic_cast<CInteractionObject*>(pGameObject)->SetTestCol();
			iter_Layer++;
		}
		else if (temp.find(L"doorCrush_Bar_17", 0) != wstring::npos)
		{
			pGameObject = CInteractionObject::Create(m_pGraphicDev, Engine::TYPE_STATIC, CInteractionObject::CAGE_LEFT);
			NULL_CHECK(pGameObject, E_FAIL);
			pGameObject->Set_ObjName(temp.c_str());
			iter_Layer->second = pGameObject->Load_Component(dwBytes, hFile, true);
			dynamic_cast<CInteractionObject*>(pGameObject)->SetTestCol();
			iter_Layer++;
		}
		else if (temp.find(L"doorCrush_Bar", 0) != wstring::npos)
		{
			pGameObject = CInteractionObject::Create(m_pGraphicDev, Engine::TYPE_STATIC, CInteractionObject::CAGE_RIGHT);
			NULL_CHECK(pGameObject, E_FAIL);
			pGameObject->Set_ObjName(temp.c_str());
			iter_Layer->second = pGameObject->Load_Component(dwBytes, hFile, true);
			dynamic_cast<CInteractionObject*>(pGameObject)->SetTestCol();
			iter_Layer++;
		}

		else if (temp.find(L"Janitor_Arm", 0) != wstring::npos)
		{
			pGameObject = CJanitorArm::Create(m_pGraphicDev, bArmFlag);
			NULL_CHECK(pGameObject, E_FAIL);
			pGameObject->Set_ObjName(temp.c_str());
			iter_Layer->second = pGameObject->Load_Component(dwBytes, hFile, true);
			dynamic_cast<CJanitorArm*>(pGameObject)->Set_Shader();
			iter_Layer++;
			bArmFlag = true;
		}
		else if (temp.find(L"Janitor", 0) != wstring::npos)
		{
			pGameObject = CJanitor::Create(m_pGraphicDev);
			NULL_CHECK(pGameObject, E_FAIL);
			pGameObject->Set_ObjName(temp.c_str());
			iter_Layer->second = pGameObject->Load_Component(dwBytes, hFile, true);
			dynamic_cast<CJanitor*>(pGameObject)->Set_Shader();
			iter_Layer++;
		}
	}

	//pGameObject = CInteractionObject::Create(m_pGraphicDev, Engine::TYPE_STATIC, CInteractionObject::CAGE_LEFT);
	//pLayer->Add_GameObject(L"Cage_Left", pGameObject);

	//pGameObject = CInteractionObject::Create(m_pGraphicDev, Engine::TYPE_STATIC, CInteractionObject::CAGE_RIGHT);
	//pLayer->Add_GameObject(L"Cage_Right", pGameObject);


	CloseHandle(hFile);
}

void CStage::Load_Navi(Engine::CGameObject * pGameObject, const _tchar * pComName, const TCHAR * pFilePath)
{

	HANDLE hFile = ::CreateFile(pFilePath, GENERIC_READ, 0, nullptr,
		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);

	if (INVALID_HANDLE_VALUE == hFile)
		return;
	DWORD dwBytes = 0;

	dynamic_cast<CTerrain*>(pGameObject)->LoadNaviMesh(dwBytes, hFile, pComName);

	CloseHandle(hFile);
}

void CStage::Load_Col(Engine::CLayer * pLayer, const _tchar * pLayerTag, const TCHAR * pFilePath)
{
	int count = 0;

	HANDLE hFile = ::CreateFile(pFilePath, GENERIC_READ, 0, nullptr,
		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);

	if (INVALID_HANDLE_VALUE == hFile)
		return;
	DWORD dwBytes = 0;

	Engine::CGameObject*      pGameObject = nullptr;

	pLayer->Load_Object(hFile, dwBytes, pFilePath, count);

	auto iter_Layer = pLayer->GetMapObj().begin();
	for (int i = 0; i < count; i++)
	{

		pGameObject = CColObject::Create(m_pGraphicDev);

		NULL_CHECK(pGameObject, E_FAIL);
		iter_Layer->second = pGameObject->Load_Component(dwBytes, hFile, false);
		dynamic_cast<CColObject*>(iter_Layer->second)->Set_TestCol();
		pGameObject->Set_ObjName(iter_Layer->first.c_str());
		if (iter_Layer->first.find(L"Trigger", 0) != wstring::npos)
		{
			dynamic_cast<CColObject*>(iter_Layer->second)->SetColType(Engine::COL_TRIGGER);
		}
		iter_Layer++;
	}
	CloseHandle(hFile);

}

