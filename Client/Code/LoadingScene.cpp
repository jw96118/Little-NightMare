#include "stdafx.h"
#include "LoadIngScene.h"
#include "Stage.h"
#include "Loadingbar.h"
#include "Export_Function.h"

CLoadIngScene::CLoadIngScene(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CScene(pGraphicDev)
{

}

CLoadIngScene::~CLoadIngScene(void)
{

}

HRESULT CLoadIngScene::Ready_Scene(void)
{
	FAILED_CHECK_RETURN(Engine::CScene::Ready_Scene(), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Resource(m_pGraphicDev, RESOURCE_END), E_FAIL);

	FAILED_CHECK_RETURN(Ready_GameLogic_Layer(L"01.GameLogic"), E_FAIL);

	m_pLoading = CLoading::Create(m_pGraphicDev, LOADING_STAGE, &m_fLoadingCount);
	NULL_CHECK_RETURN(m_pLoading, E_FAIL);
	return S_OK;
}

_int CLoadIngScene::Update_Scene(const _float& fTimeDelta)
{
	_int iExit = Engine::CScene::Update_Scene(fTimeDelta);
	Engine::CGameObject* pGameObject = Engine::Get_GameObject(L"01.GameLogic", L"Loading_Bar");
	dynamic_cast<CLoadingBar*>(pGameObject)->Set_Load(m_fLoadingCount * 0.01f);
	//dynamic_cast<CLoadingBar*>(pGameObject)->Set_Pos(_vec2 (1140.f + 2.4f* m_fLoadingCount, WINCY * 0.1f));
	if (true == m_pLoading->Get_Finish())
	{
		if (Engine::Get_DIKeyState(DIK_RETURN) & 0x80)
		{
			Engine::CScene*		pScene = CStage::Create(m_pGraphicDev);
			NULL_CHECK_RETURN(pScene, E_FAIL);
			FAILED_CHECK_RETURN(Engine::SetUp_Scene(pScene), E_FAIL);

			return CHANGE_SCENE;
		}		
	}

	return iExit;
}

void CLoadIngScene::Render_Scene(void)
{
	// DEBUG 용
	Engine::Render_Font(L"Font_Default", m_pLoading->Get_LoadString(), &_vec2(10.f, 10.f), D3DXCOLOR(1.f, 0.f, 0.f, 1.f));
}

CLoadIngScene* CLoadIngScene::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CLoadIngScene*	pInstance = new CLoadIngScene(pGraphicDev);

	if (FAILED(pInstance->Ready_Scene()))
		Engine::Safe_Release(pInstance);

	return pInstance;
}

void CLoadIngScene::Free(void)
{
	Engine::Safe_Release(m_pLoading);
	Engine::CScene::Free();
}


HRESULT	CLoadIngScene::Ready_Environment_Layer(const _tchar* pLayerTag)
{
	return S_OK;
}

HRESULT CLoadIngScene::Ready_GameLogic_Layer(const _tchar* pLayerTag)
{
	Engine::CLayer*		pLayer = Engine::CLayer::Create();
	NULL_CHECK_RETURN(pLayer, E_FAIL);

	Engine::CGameObject*		pGameObject = nullptr;


	pGameObject = CLoadingBar::Create(m_pGraphicDev, L"Texture_Loading_Frame", _vec3(WINCX * 0.15f, WINCY*0.43f, 0.01f), 1.f, 0,1.f);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Loading_Frame", pGameObject), E_FAIL);



	pGameObject = CLoadingBar::Create(m_pGraphicDev, L"Texture_Loading_Bar", _vec3(WINCX * 0.15f, WINCY*0.43f, 0.02f), 1.f, 0, 0.f);
    NULL_CHECK_RETURN(pGameObject, E_FAIL);
	//dynamic_cast<CTitle*>(pGameObject)->Set_TexX(0.f);
    FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Loading_Bar", pGameObject), E_FAIL);







	m_mapLayer.emplace(pLayerTag, pLayer);



	return S_OK;
}


HRESULT CLoadIngScene::Ready_Resource(LPDIRECT3DDEVICE9& pGraphicDev, RESOURCEID eMax)
{
	// 메모리 예약
	Engine::Reserve_ContainerSize(eMax);
	FAILED_CHECK_RETURN(Engine::Ready_Texture(pGraphicDev,
		RESOURCE_LOGO,
		L"Texture_Loading_Bar",
		Engine::TEX_NORMAL,
		L"../Bin/Resource/LittleNightMare/Resources/Textures/LodingBar/Loading_Bar.png"),
		E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Texture(pGraphicDev,
		RESOURCE_LOGO,
		L"Texture_Loading_Frame",
		Engine::TEX_NORMAL,
		L"../Bin/Resource/LittleNightMare/Resources/Textures/LodingBar/Loading_Frame.png"),
		E_FAIL);
	//FAILED_CHECK_RETURN(Engine::Ready_Texture(pGraphicDev,
	//	RESOURCE_LOGO,
	//	L"Texture_Loading_Back",
	//	Engine::TEX_NORMAL,
	//	L"../Bin/Resource/Texture/bg_entering.png"),
	//	E_FAIL);

	return S_OK;
}
