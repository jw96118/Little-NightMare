#include "DebugMesh.h"

USING(Engine)
IMPLEMENT_SINGLETON(CDebugMesh)

Engine::CDebugMesh::CDebugMesh(void)
{

}

Engine::CDebugMesh::~CDebugMesh(void)
{
	Free();
}

LPD3DXMESH CDebugMesh::Get_BoxMesh(void)
{
	return m_pBoxMesh;
}

LPD3DXMESH CDebugMesh::Get_SphereMesh(void)
{
	return m_pSphereMesh;
}

HRESULT CDebugMesh::Ready_DebugMesh(LPDIRECT3DDEVICE9 pGraphicDev)
{
	m_pGraphicDev = pGraphicDev;
	m_pGraphicDev->AddRef();

	LPDIRECT3DVERTEXBUFFER9 m_pBoxBuffer = nullptr;
	LPDIRECT3DVERTEXBUFFER9 m_pSphereBuffer = nullptr;

	LPD3DXMESH	pBoxMesh = nullptr;
	LPD3DXMESH	pSphereMesh = nullptr;

	FAILED_CHECK_RETURN(D3DXCreateBox(pGraphicDev, 1.f, 1.f, 1.f, &pBoxMesh, nullptr), E_FAIL);
	FAILED_CHECK_RETURN(pBoxMesh->CloneMeshFVF(0, FVF_COL, pGraphicDev, &m_pBoxMesh), E_FAIL);
	FAILED_CHECK_RETURN(m_pBoxMesh->GetVertexBuffer(&m_pBoxBuffer), E_FAIL);

	pBoxMesh->Release();

	_uint uiNumVtx = m_pBoxMesh->GetNumVertices();
	VTXCOL* pVertices = nullptr;

	m_pBoxBuffer->Lock(0, 0, (void**)&pVertices, 0);

	for (_uint i = 0; i < uiNumVtx; i++)
		pVertices[i].dwColor = D3DCOLOR_RGBA(0, 255, 0, 255);

	m_pBoxBuffer->Unlock();
	m_pBoxBuffer->Release();

	FAILED_CHECK_RETURN(D3DXCreateSphere(pGraphicDev, 1.f, 20, 10, &pSphereMesh, nullptr), E_FAIL);
	FAILED_CHECK_RETURN(pSphereMesh->CloneMeshFVF(0, FVF_COL, pGraphicDev, &m_pSphereMesh), E_FAIL);
	FAILED_CHECK_RETURN(m_pSphereMesh->GetVertexBuffer(&m_pSphereBuffer), E_FAIL);

	pSphereMesh->Release();

	uiNumVtx = m_pSphereMesh->GetNumVertices();

	m_pSphereBuffer->Lock(0, 0, (void**)&pVertices, 0);

	for (_uint i = 0; i < uiNumVtx; i++)
		pVertices[i].dwColor = D3DCOLOR_RGBA(0, 255, 0, 255);

	m_pSphereBuffer->Unlock();
	m_pSphereBuffer->Release();

	return S_OK;
}

void CDebugMesh::Render_BoxMesh(void)
{
	m_pGraphicDev->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);

	m_pBoxMesh->DrawSubset(0);

	m_pGraphicDev->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
}

void CDebugMesh::Render_SphereMesh(void)
{
	m_pGraphicDev->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);

	m_pSphereMesh->DrawSubset(0);

	m_pGraphicDev->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
}


void Engine::CDebugMesh::Free(void)
{
	Safe_Release(m_pBoxMesh);
	Safe_Release(m_pSphereMesh);
	Safe_Release(m_pGraphicDev);
}

