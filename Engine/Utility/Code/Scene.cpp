#include "Scene.h"

USING(Engine)

Engine::CScene::CScene(LPDIRECT3DDEVICE9 pGraphicDev)
	: m_pGraphicDev(pGraphicDev)
{
	m_pGraphicDev->AddRef();
}

Engine::CScene::~CScene(void)
{

}

HRESULT Engine::CScene::Ready_Scene(void)
{
	return S_OK;
}

HRESULT CScene::LateReady_Scene(void)
{
	for (auto& iter : m_mapLayer)
		FAILED_CHECK_RETURN(iter.second->LateReady_Layer(), E_FAIL);

	return S_OK;
}

_int Engine::CScene::Update_Scene(const _float& fTimeDelta)
{
	_int iExit = 0;

	for (auto& iter : m_mapLayer)
	{
		iExit = iter.second->Update_Layer(fTimeDelta);

		if (iExit & 0x00000001)
			return 1;

		if (iExit & 0x80000000)
			return -1;
	}

	return iExit;
}

_int CScene::LateUpdate_Scene(const _float & fTimeDelta)
{
	_int iExit = 0;

	for (auto& iter : m_mapLayer)
	{
		iExit = iter.second->LateUpdate_Layer(fTimeDelta);

		if (iExit & 0x00000001)
			return 1;

		if (iExit & 0x80000000)
			return -1;
	}

	return iExit;
}

void Engine::CScene::Render_Scene(void)
{
	
}

void CScene::Set_SceneChange(const bool & flag)
{
	m_bSceneChange = flag;
}

void Engine::CScene::Free(void)
{
	for_each(m_mapLayer.begin(), m_mapLayer.end(), CDeleteMap());
	m_mapLayer.clear();

	Safe_Release(m_pGraphicDev);
}

Engine::CComponent* Engine::CScene::Get_Component(const _tchar* pLayerTag, const _tchar* pObjTag, const _tchar* pComponentTag, COMPONENTID eID)
{
	// auto	iter = find_if(m_mapLayer.begin(), m_mapLayer.end(), CTag_Finder(pLayerTag));

	auto iter = m_mapLayer.find(pLayerTag);

	if (iter == m_mapLayer.end())
		return nullptr;

	return iter->second->Get_Component(pObjTag, pComponentTag, eID);

}

CGameObject * CScene::Get_GameObject(const _tchar * pLayerTag, const _tchar * pObjTag)
{
	// auto	iter = find_if(m_mapLayer.begin(), m_mapLayer.end(), CTag_Finder(pLayerTag));

	auto iter = m_mapLayer.find(pLayerTag);

	if (iter == m_mapLayer.end())
		return nullptr;

	return iter->second->Get_GameObject(pObjTag);
}

CLayer * CScene::Get_Layer(const _tchar * pLayerTag)
{
	auto iter = m_mapLayer.find(pLayerTag);

	if (iter == m_mapLayer.end())
		return nullptr;

	return iter->second;

}

