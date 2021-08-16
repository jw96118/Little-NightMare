#include "ScrollTex.h"

USING(Engine)

Engine::CScrollTex::CScrollTex(LPDIRECT3DDEVICE9 pGraphicDev)
	: CVIBuffer(pGraphicDev)
{

}

Engine::CScrollTex::CScrollTex(const CScrollTex& rhs)
	: CVIBuffer(rhs)
{

}

Engine::CScrollTex::~CScrollTex(void)
{

}

HRESULT Engine::CScrollTex::Ready_Buffer(void)
{
	m_dwVtxSize = sizeof(VTXTEX);
	m_dwVtxCnt = 8;
	m_dwVtxFVF = FVF_TEX;
	m_dwTriCnt = 4;

	m_dwIdxSize = sizeof(INDEX16);
	m_IdxFmt = D3DFMT_INDEX16;

	FAILED_CHECK_RETURN(CVIBuffer::Ready_Buffer(), E_FAIL);

	VTXTEX*			pVertex = nullptr;

	m_pVB->Lock(0, 0, (void**)&pVertex, 0);		// 1. 메모리 잠그는 역할, 2. 3인자를 통해서 저장된 버텍스 접근 권한을 얻음
													// 오른쪽 위 
	pVertex[0].vPos = _vec3(-0.5f, 0.5f, 0.f);
	pVertex[0].vTexUV = _vec2(0.f, 0.f);

	pVertex[1].vPos = _vec3( 0.f, 0.5f, 0.f);
	pVertex[1].vTexUV = _vec2(0.5f, 0.f);

	pVertex[2].vPos = _vec3( 0.f, -0.5f, 0.f);
	pVertex[2].vTexUV = _vec2(0.5f, 1.f);

	pVertex[3].vPos = _vec3(-0.5f, -0.5f, 0.f);
	pVertex[3].vTexUV = _vec2(0.f, 1.f);

	pVertex[4].vPos = _vec3(0.0f, 0.5f, 0.f);
	pVertex[4].vTexUV = _vec2(0.5f, 0.f);

	pVertex[5].vPos = _vec3(0.5f, 0.5f, 0.f);
	pVertex[5].vTexUV = _vec2(1.f, 0.f);

	pVertex[6].vPos = _vec3(0.5f, -0.5f, 0.f);
	pVertex[6].vTexUV = _vec2(1.f, 1.f);

	pVertex[7].vPos = _vec3(0.f, -0.5f, 0.f);
	pVertex[7].vTexUV = _vec2(0.5f, 1.f);
	
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

	pIndex[2]._0 = 4;
	pIndex[2]._1 = 5;
	pIndex[2]._2 = 6;

	pIndex[3]._0 = 4;
	pIndex[3]._1 = 6;
	pIndex[3]._2 = 7;

	m_pIB->Unlock();

	return S_OK;
}

Engine::CResources* Engine::CScrollTex::Clone(void)
{
	return new CScrollTex(*this);
}

Engine::CScrollTex* Engine::CScrollTex::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CScrollTex*	pInstance = new CScrollTex(pGraphicDev);

	if (FAILED(pInstance->Ready_Buffer()))
		Safe_Release(pInstance);

	return pInstance;
}

void Engine::CScrollTex::Free(void)
{
	CVIBuffer::Free();
}

