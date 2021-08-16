#include "PrototypeMgr.h"


USING(Engine)
IMPLEMENT_SINGLETON(CPrototypeMgr)

Engine::CPrototypeMgr::CPrototypeMgr(void)
{

}

Engine::CPrototypeMgr::~CPrototypeMgr(void)
{
	Free();
}

HRESULT Engine::CPrototypeMgr::Ready_Prototype(const _tchar* pProtoTag, CComponent* pInstance)
{
	CComponent*	pPrototype = Find_Prototype(pProtoTag);

	if (nullptr != pPrototype)
		return E_FAIL;

	m_mapPrototype.emplace(pProtoTag, pInstance);

	return S_OK;
}

Engine::CComponent* Engine::CPrototypeMgr::Clone_Prototype(const _tchar* pProtoTag)
{
	CComponent*		pPrototype = Find_Prototype(pProtoTag);

	if (nullptr == pPrototype)
		return nullptr;

	return pPrototype->Clone();
}

Engine::CComponent* Engine::CPrototypeMgr::Find_Prototype(const _tchar* pProtoTag)
{
	auto	iter = m_mapPrototype.begin();

	for (; iter != m_mapPrototype.end(); iter++)
	{
		if(iter->first == pProtoTag)
			return iter->second;
	}


	return nullptr;

}

void Engine::CPrototypeMgr::Free(void)
{
	for_each(m_mapPrototype.begin(), m_mapPrototype.end(), CDeleteMap());
	m_mapPrototype.clear();
}

