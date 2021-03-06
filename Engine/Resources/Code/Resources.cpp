#include "Resources.h"

USING(Engine)

Engine::CResources::CResources(LPDIRECT3DDEVICE9 pGraphicDev)
	: m_pGraphicDev(pGraphicDev)
	, m_bClone(false)
{
	m_pGraphicDev->AddRef();
}

Engine::CResources::CResources(const CResources& rhs)
	: m_pGraphicDev(rhs.m_pGraphicDev)
	, m_bClone(true)
{
	m_pGraphicDev->AddRef();
}

Engine::CResources::~CResources(void)
{

}

CComponent* Engine::CResources::Clone(void)
{
	return nullptr;
}

void Engine::CResources::Free(void)
{
	Safe_Release(m_pGraphicDev);
}

void CResources::SetSaveType(const _ushort & wContainerIdx, const _tchar * pResourceTag)
{
	m_wContainerIdx = wContainerIdx;

	ZeroMemory(m_pResourceTag, 40 * sizeof(_tchar));
	lstrcpy(m_pResourceTag, pResourceTag);

}

