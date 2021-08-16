#include "LightTex.h"

USING(Engine)

Engine::CLightTex::CLightTex(LPDIRECT3DDEVICE9 pGraphicDev)
	: CVIBuffer(pGraphicDev)
{

}

Engine::CLightTex::CLightTex(const CLightTex& rhs)
	: CVIBuffer(rhs)
{

}

Engine::CLightTex::~CLightTex(void)
{

}

HRESULT Engine::CLightTex::Ready_Buffer(void)
{
	m_dwVtxSize = sizeof(VTXTEX);
	m_dwVtxCnt = 4;
	m_dwVtxFVF = FVF_TEX;
	m_dwTriCnt = 2;

	m_dwIdxSize = sizeof(INDEX16);
	m_IdxFmt = D3DFMT_INDEX16;

	FAILED_CHECK_RETURN(CVIBuffer::Ready_Buffer(), E_FAIL);

	VTXTEX*			pVertex = nullptr;

	m_pVB->Lock(0, 0, (void**)&pVertex, 0);		// 1. 메모리 잠그는 역할, 2. 3인자를 통해서 저장된 버텍스 접근 권한을 얻음
													// 오른쪽 위 
	pVertex[0].vPos = _vec3(-0.5f, 0.5f, 0.f);
	pVertex[0].vTexUV = _vec2(0.f, 0.f);
	pVertex[0].vNormal = _vec3(0.f, 0.f, 0.f);

	pVertex[1].vPos = _vec3(0.5f, 0.5f, 0.f);
	pVertex[1].vTexUV = _vec2(1.f, 0.f);
	pVertex[1].vNormal = _vec3(0.f, 0.f, 0.f);

	pVertex[2].vPos = _vec3(0.5f, -0.5f, 0.f);
	pVertex[2].vTexUV = _vec2(1.f, 1.f);
	pVertex[2].vNormal = _vec3(0.f, 0.f, 0.f);

	pVertex[3].vPos = _vec3(-0.5f, -0.5f, 0.f);
	pVertex[3].vTexUV = _vec2(0.f, 1.f);
	pVertex[3].vNormal = _vec3(0.f, 0.f, 0.f);



	INDEX16*		pIndex = nullptr;
	_vec3		vDest, vSour, vNormal;

	m_pIB->Lock(0, 0, (void**)&pIndex, 0);

	// 0
	pIndex[0]._0 = 0;
	pIndex[0]._1 = 1;
	pIndex[0]._2 = 2;

	vDest = pVertex[pIndex[0]._1].vPos - pVertex[pIndex[0]._0].vPos;
	vSour = pVertex[pIndex[0]._2].vPos - pVertex[pIndex[0]._1].vPos;

	D3DXVec3Cross(&vNormal, &vDest, &vSour);

	//D3DXVec3Normalize(&vNormal, &vNormal);
	//D3DXVec3Normalize(&vDest, &vDest);
	//D3DXVec3Normalize(&vSour, &vSour);
	//vNormal += vDest;
	//vNormal += vSour;

	pVertex[pIndex[0]._0].vNormal += vNormal;
	pVertex[pIndex[0]._1].vNormal += vNormal;
	pVertex[pIndex[0]._2].vNormal += vNormal;


	// 1
	pIndex[1]._0 = 0;
	pIndex[1]._1 = 2;
	pIndex[1]._2 = 3;

	vDest = pVertex[pIndex[1]._1].vPos - pVertex[pIndex[1]._0].vPos;
	vSour = pVertex[pIndex[1]._2].vPos - pVertex[pIndex[1]._1].vPos;
	D3DXVec3Cross(&vNormal, &vDest, &vSour);

	//D3DXVec3Normalize(&vNormal, &vNormal);
	//D3DXVec3Normalize(&vDest, &vDest);
	//D3DXVec3Normalize(&vSour, &vSour);
	//vNormal += vDest;
	//vNormal += vSour;

	pVertex[pIndex[1]._0].vNormal += vNormal;
	pVertex[pIndex[1]._1].vNormal += vNormal;
	pVertex[pIndex[1]._2].vNormal += vNormal;
	for (_ulong i = 0; i < m_dwVtxCnt; ++i)
		D3DXVec3Normalize(&pVertex[i].vNormal, &pVertex[i].vNormal);

	m_pVB->Unlock();
	m_pIB->Unlock();

	return S_OK;
}

Engine::CResources* Engine::CLightTex::Clone(void)
{
	return new CLightTex(*this);
}

Engine::CLightTex* Engine::CLightTex::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CLightTex*	pInstance = new CLightTex(pGraphicDev);

	if (FAILED(pInstance->Ready_Buffer()))
		Safe_Release(pInstance);

	return pInstance;
}

void Engine::CLightTex::Free(void)
{
	CVIBuffer::Free();
}

