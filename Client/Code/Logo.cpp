#include "stdafx.h"
#include "Logo.h"
#include "Stage.h"
#include "Loadingbar.h"
#include "LoadingScene.h"
#include "Export_Function.h"

CLogo::CLogo(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CScene(pGraphicDev)
{

}

CLogo::~CLogo(void)
{

}

HRESULT CLogo::Ready_Scene(void)
{
	FAILED_CHECK_RETURN(Engine::CScene::Ready_Scene(), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Resource(m_pGraphicDev, RESOURCE_END), E_FAIL);

	FAILED_CHECK_RETURN(Ready_GameLogic_Layer(L"01.GameLogic"), E_FAIL);
	Engine::Play_BGM(L"SW_Music_Int_Hunger_Rhodes_LP.ogg");
	return S_OK;
}

_int CLogo::Update_Scene(const _float& fTimeDelta)
{
	_int iExit = Engine::CScene::Update_Scene(fTimeDelta);
	if (Engine::Get_KeyMgr()->KeyDown(Engine::KEY_Q))
	{
		Engine::CGameObject* pGameObject = Engine::Get_GameObject(L"01.GameLogic", L"BackGround");
		testf -= fTimeDelta;
		dynamic_cast<CTitle*>(pGameObject)->Set_TexX(testf);
	}
	if (Engine::Get_KeyMgr()->KeyDown(Engine::KEY_RETURN))
	{
		Engine::Stop_All();
		Engine::Play_Sound(L"SW_ui_button_start_3.ogg", Engine::CHANNELID::EFFECT, temp, false);
		m_SceneChange = true;
	}
	if (m_SceneChange)
	{
		Engine::CGameObject* pGameObject = Engine::Get_GameObject(L"01.GameLogic", L"BackGround_Black");
		m_fLoadingCount +=  fTimeDelta;
		dynamic_cast<CTitle*>(pGameObject)->Set_TexAlpha(m_fLoadingCount);
	
		if (m_fLoadingCount >= 1.f)
		{
			Engine::CScene*		pScene = CLoadIngScene::Create(m_pGraphicDev);
			NULL_CHECK_RETURN(pScene, E_FAIL);
			FAILED_CHECK_RETURN(Engine::SetUp_Scene(pScene), E_FAIL);
			return CHANGE_SCENE;
		}
	}

	return iExit;
}

void CLogo::Render_Scene(void)
{
	// DEBUG 용
	//Engine::Render_Font(L"Font_Default", m_pLoading->Get_LoadString(), &_vec2(10.f, 10.f), D3DXCOLOR(1.f, 0.f, 0.f, 1.f));
}

CLogo* CLogo::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CLogo*	pInstance = new CLogo(pGraphicDev);

	if (FAILED(pInstance->Ready_Scene()))
		Engine::Safe_Release(pInstance);

	return pInstance;
}

void CLogo::Free(void)
{
	//	Engine::Safe_Release(m_pLoading);
	Engine::CScene::Free();
}

HRESULT CLogo::Ready_GameLogic_Layer(const _tchar* pLayerTag)
{
	Engine::CLayer*		pLayer = Engine::CLayer::Create();
	NULL_CHECK_RETURN(pLayer, E_FAIL);

	Engine::CGameObject*		pGameObject = nullptr;

	//// BackGround
	//pGameObject = CBackGround::Create(m_pGraphicDev);
	//NULL_CHECK_RETURN(pGameObject, E_FAIL);
	//FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"BackGround", pGameObject), E_FAIL);

	//m_mapLayer.emplace(pLayerTag, pLayer);
	pGameObject = CTitle::Create(m_pGraphicDev, L"Texture_BackGround",_vec2(WINCX,WINCY),_vec3(WINCX * 0.5, WINCY * 0.5,0.3f));
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"BackGround", pGameObject), E_FAIL);

	//pGameObject = CTitle::Create(m_pGraphicDev, L"Texture_Enter",0.2f, _vec3(0.f, -0.5f, 0.21f));
	//NULL_CHECK_RETURN(pGameObject, E_FAIL);
	//FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"BackGround_Enter", pGameObject), E_FAIL);

	pGameObject = CTitle::Create(m_pGraphicDev, L"Texture_BackGround_Black", _vec2(WINCX, WINCY), _vec3(WINCX * 0.5, WINCY * 0.5, 0.2f));
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	dynamic_cast<CTitle*>(pGameObject)->Set_TexAlpha(0.f);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"BackGround_Black", pGameObject), E_FAIL);





	m_mapLayer.emplace(pLayerTag, pLayer);


	return S_OK;
}

HRESULT	CLogo::Ready_UI_Layer(const _tchar* pLayerTag)
{
	return S_OK;
}

HRESULT CLogo::Ready_Resource(LPDIRECT3DDEVICE9& pGraphicDev, RESOURCEID eMax)
{
	// 메모리 예약
	Engine::Reserve_ContainerSize(eMax);

	FAILED_CHECK_RETURN(Engine::Ready_Buffer(pGraphicDev,
		RESOURCE_STATIC,
		L"Buffer_TriCol",
		Engine::BUFFER_TRICOL),
		E_FAIL);

	FAILED_CHECK_RETURN(Engine::Ready_Buffer(pGraphicDev,
		RESOURCE_STATIC,
		L"Buffer_RcCol",
		Engine::BUFFER_RCCOL),
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

	// 텍스쳐
	FAILED_CHECK_RETURN(Engine::Ready_Texture(pGraphicDev,
		RESOURCE_LOGO,
		L"Texture_BackGround",
		Engine::TEX_NORMAL,
		L"../Bin/Resource/LittleNightMare/Resources/Textures/Logo/title.tga"),
		E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Texture(pGraphicDev,
		RESOURCE_LOGO,
		L"Texture_BackGround_Black",
		Engine::TEX_NORMAL,
		L"../Bin/Resource/LittleNightMare/Resources/Textures/Logo/BackGround_Black.tga"),
		E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Texture(pGraphicDev,
		RESOURCE_LOGO,
		L"Texture_Enter",
		Engine::TEX_NORMAL,
		L"../Bin/Resource/LittleNightMare/Resources/Textures/Logo/Logo.tga"),
		E_FAIL);

	
	return S_OK;
}
