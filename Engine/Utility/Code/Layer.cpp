#include "Layer.h"

USING(Engine)

Engine::CLayer::CLayer(void)
{

}

Engine::CLayer::~CLayer(void)
{

}

HRESULT Engine::CLayer::Ready_Layer(void)
{
	return S_OK;
}

HRESULT CLayer::LateReady_Layer(void)
{
	for (auto& iter : m_mapObject)
		FAILED_CHECK_RETURN(iter.second->LateReady_GameObject(), E_FAIL);

	return S_OK;
}

_int Engine::CLayer::Update_Layer(const _float& fTimeDelta)
{
	_int iExit = 0;

	for (auto& iter : m_mapObject)
	{
		if (!iter.second->Get_Enable())
			continue;

		iExit = iter.second->Update_GameObject(fTimeDelta);

		if (iExit & 0x00000001)
			return 1;

		if (iExit & 0x80000000)
			return -1;
	}

	return iExit;
}

_int CLayer::LateUpdate_Layer(const _float & fTimeDelta)
{
	_int iExit = 0;

	for (auto& iter : m_mapObject)
	{
		if (!iter.second->Get_Enable())
			continue;

		iExit = iter.second->LateUpdate_GameObject(fTimeDelta);

		if (iExit & 0x00000001)
			return 1;

		if (iExit & 0x80000000)
			return -1;
	}

	return iExit;
}

//void Engine::CLayer::Render_Layer(void)
//{
//
//}

HRESULT Engine::CLayer::Add_GameObject(const _tchar* pObjTag, CGameObject* pGameObject)
{
	if (nullptr == pGameObject)
		return E_FAIL;

	m_mapObject.emplace(pObjTag, pGameObject);

	return S_OK;
}


map<wstring, CGameObject*>& CLayer::GetMapObj()
{
	// TODO: 여기에 반환 구문을 삽입합니다.
	return m_mapObject;
}

Engine::CLayer* Engine::CLayer::Create(void)
{
	CLayer*		pInstance = new CLayer;

	if (FAILED(pInstance->Ready_Layer()))
		Safe_Release(pInstance);

	return pInstance;
}



void Engine::CLayer::Free(void)
{
	for_each(m_mapObject.begin(), m_mapObject.end(), CDeleteMap());
		
	m_mapObject.clear();
}

Engine::CComponent* Engine::CLayer::Get_Component(const _tchar* pObjTag, const _tchar* pComponentTag, COMPONENTID eID)
{
	//auto	iter = find_if(m_mapObject.begin(), m_mapObject.end(), CTag_Finder(pObjTag));

	//if (iter == m_mapObject.end())
	//	return nullptr;

	//return iter->second->Get_Component(pComponentTag, eID);
	auto	iter = m_mapObject.begin();

	for (; iter != m_mapObject.end(); iter++)
	{
		if (iter->first == pObjTag)
			return iter->second->Get_Component(pComponentTag, eID);
	}
	return nullptr;
}

CGameObject * CLayer::Get_GameObject(const _tchar * pObjTag)
{
	//auto	iter = find_if(m_mapObject.begin(), m_mapObject.end(), CTag_Finder(pObjTag));

	//if (iter == m_mapObject.end())
	//	return nullptr;

	//return iter->second;

	auto	iter = m_mapObject.begin();

	for (; iter != m_mapObject.end(); iter++)
	{
		if (iter->first == pObjTag)
			return iter->second;
	}
	return nullptr;
}

void CLayer::Save_Object(HANDLE& hFile, DWORD& dwBytes, const TCHAR * pFilePath,bool flag)
{
	auto iter = m_mapObject.begin();

	size_t tempSize = m_mapObject.size();
	WriteFile(hFile, &tempSize, sizeof(size_t), &dwBytes, nullptr);
	for (; iter != m_mapObject.end(); iter++)
	{
		size_t tempLen = (wcslen(iter->first.c_str()) * sizeof(_tchar));
		WriteFile(hFile, &tempLen, sizeof(size_t), &dwBytes, nullptr);
		WriteFile(hFile, iter->first.c_str(), tempLen * sizeof(_tchar), &dwBytes, nullptr);
	}
	iter = m_mapObject.begin();

	for (; iter != m_mapObject.end(); iter++)
	{
		iter->second->Save_Component(dwBytes, hFile,flag);
	}

	//CloseHandle(hFile);
}

void CLayer::Load_Object(HANDLE& hFile, DWORD& dwBytes, const TCHAR * pFilePath, int &count)
{
	// hFile = ::CreateFile(pFilePath, GENERIC_READ, 0, nullptr,
	//	OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);

	//if (INVALID_HANDLE_VALUE == hFile)
	//	return;

	if (!m_mapObject.empty())
	{
		for_each(m_mapObject.begin(), m_mapObject.end(), CDeleteMap());
		m_mapObject.clear();
	}
	size_t     tempSize = 0;
	size_t     templen = 0;

	ReadFile(hFile, &tempSize, sizeof(size_t), &dwBytes, nullptr);
	for (int i = 0; i < tempSize; i++)
	{
		ReadFile(hFile, &templen, sizeof(size_t), &dwBytes, nullptr);

		ZeroMemory(tempKey, wcslen(tempKey) * sizeof(_tchar));

		ReadFile(hFile, tempKey, templen * sizeof(_tchar), &dwBytes, nullptr);
		wstring test;
		test = tempKey;
		m_mapObject.emplace(test, nullptr);
		count++;
	}
	ZeroMemory(tempKey, wcslen(tempKey) * sizeof(_tchar));
}

