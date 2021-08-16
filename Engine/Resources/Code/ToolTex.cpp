#include "ToolTex.h"

USING(Engine)

Engine::CToolTex::CToolTex(LPDIRECT3DDEVICE9 pGraphicDev)
	: CVIBuffer(pGraphicDev)
{

}

Engine::CToolTex::CToolTex(const CToolTex& rhs)
	: CVIBuffer(rhs)
{

}

Engine::CToolTex::~CToolTex(void)
{

}

HRESULT Engine::CToolTex::Ready_Buffer(void)
{
	m_dwVtxSize = sizeof(VTXTEX);
	m_dwVtxCnt = 4;
	m_dwVtxFVF = FVF_TEX;
	m_dwTriCnt = 2;

	m_dwIdxSize = sizeof(INDEX16);
	m_IdxFmt = D3DFMT_INDEX16;

	FAILED_CHECK_RETURN(CVIBuffer::Ready_Buffer(), E_FAIL);

	VTXTEX*		pVertex = nullptr;

	m_pVB->Lock(0, 0, (void**)&pVertex, 0);		// 1. 메모리 잠그는 역할, 2. 3인자를 통해서 저장된 버텍스 접근 권한을 얻음
													// 오른쪽 위 
	pVertex[0].vPos = _vec3(-1.f,1.f, 0.f);
	pVertex[0].vTexUV = _vec2(0.f, 0.f);

	pVertex[1].vPos = _vec3(1.f, 1.f, 0.f);
	pVertex[1].vTexUV = _vec2(1.f, 0.f);

	pVertex[2].vPos = _vec3(1.f, -1.f, 0.f);
	pVertex[2].vTexUV = _vec2(1.f, 1.f);

	pVertex[3].vPos = _vec3(-1.f, -1.f, 0.f);
	pVertex[3].vTexUV = _vec2(0.f, 1.f);

	m_pVB->Unlock();

	INDEX16*		pIndex = nullptr;

	m_pIB->Lock(0, 0, (void**)&pIndex, 0);

	// 0
	pIndex[0]._0 = 0;
	pIndex[0]._1 = 1;
	pIndex[0]._2 = 2;

	// 1
	pIndex[1]._0 = 0;
	pIndex[1]._1 = 2;
	pIndex[1]._2 = 3;

	m_pIB->Unlock();

	return S_OK;
}

Engine::CResources* Engine::CToolTex::Clone(void)
{
	return new CToolTex(*this);
}

Engine::CToolTex* Engine::CToolTex::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CToolTex*	pInstance = new CToolTex(pGraphicDev);

	if (FAILED(pInstance->Ready_Buffer()))
		Safe_Release(pInstance);

	return pInstance;
}

void Engine::CToolTex::Free(void)
{
	CVIBuffer::Free();
}

