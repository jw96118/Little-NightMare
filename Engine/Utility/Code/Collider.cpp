#include "Collider.h"

USING(Engine)
Engine::CCollider::CCollider(LPDIRECT3DDEVICE9 pGraphicDev)
	: m_pGraphicDev(pGraphicDev)
{
	m_pGraphicDev->AddRef();
}

Engine::CCollider::~CCollider(void)
{

}

_vec3 CCollider::Get_Cube(int iIndex)
{
	return m_pVtxCube[iIndex];
}

HRESULT Engine::CCollider::Ready_Collider(const _vec3* pPos, const _ulong& dwNumVtx, const _ulong& dwStride, const _bool& flag)
{
	if(!flag)
	D3DXComputeBoundingBox(pPos, dwNumVtx, sizeof(_vec3), &m_vMin, &m_vMax);
	else
	{
		m_vMin = { 0.f,0.f,0.f };
		m_vMax = { 1.f,1.f,1.f };
	}
	//D3DXCreateBox

#ifdef _DEBUG

	FAILED_CHECK_RETURN(m_pGraphicDev->CreateVertexBuffer(sizeof(VTXCUBE) * 8,	// ������ ������ ũ��
		0,					// ������ ����(���ڰ� 0�� ��� ���� ����)
		FVF_CUBE,			// �����ϴ� ���ؽ��� �Ӽ���
		D3DPOOL_MANAGED,	// �޸� Ǯ�� ����
		&m_pVB,
		NULL),
		E_FAIL);


	FAILED_CHECK_RETURN(m_pGraphicDev->CreateIndexBuffer(sizeof(INDEX32) * 12,	// ������ �ε��� ������ ũ��
		0,					// ������ ����(���ڰ� 0�� ��� ���� ����)
		D3DFMT_INDEX32,			// �����ϴ� �ε����� �Ӽ���
		D3DPOOL_MANAGED,	// �޸� Ǯ�� ����
		&m_pIB,
		NULL),
		E_FAIL);

	VTXCUBE*		pVtxCube = nullptr;

	m_pVB->Lock(0, 0, (void**)&pVtxCube, 0);

	m_pVtxCube[0] = pVtxCube[0].vPos = _vec3(m_vMin.x, m_vMax.y, m_vMin.z);
	pVtxCube[0].vTex = pVtxCube[0].vPos;

	m_pVtxCube[1] = pVtxCube[1].vPos = _vec3(m_vMax.x, m_vMax.y, m_vMin.z);
	pVtxCube[1].vTex = pVtxCube[1].vPos;

	m_pVtxCube[2] = pVtxCube[2].vPos = _vec3(m_vMax.x, m_vMin.y, m_vMin.z);
	pVtxCube[2].vTex = pVtxCube[2].vPos;

	m_pVtxCube[3] = pVtxCube[3].vPos = _vec3(m_vMin.x, m_vMin.y, m_vMin.z);
	pVtxCube[3].vTex = pVtxCube[3].vPos;


	m_pVtxCube[4] = pVtxCube[4].vPos = _vec3(m_vMin.x, m_vMax.y, m_vMax.z);
	pVtxCube[4].vTex = pVtxCube[4].vPos;

	m_pVtxCube[5] = pVtxCube[5].vPos = _vec3(m_vMax.x, m_vMax.y, m_vMax.z);
	pVtxCube[5].vTex = pVtxCube[5].vPos;

	m_pVtxCube[6] = pVtxCube[6].vPos = _vec3(m_vMax.x, m_vMin.y, m_vMax.z);
	pVtxCube[6].vTex = pVtxCube[6].vPos;

	m_pVtxCube[7] = pVtxCube[7].vPos = _vec3(m_vMin.x, m_vMin.y, m_vMax.z);
	pVtxCube[7].vTex = pVtxCube[7].vPos;

	m_pVB->Unlock();

	INDEX32*	pIndex = NULL;

	m_pIB->Lock(0, 0, (void**)&pIndex, 0);

	// +x
	pIndex[0]._0 = 1;
	pIndex[0]._1 = 5;
	pIndex[0]._2 = 6;

	pIndex[1]._0 = 1;
	pIndex[1]._1 = 6;
	pIndex[1]._2 = 2;

	// -x
	pIndex[2]._0 = 4;
	pIndex[2]._1 = 0;
	pIndex[2]._2 = 3;

	pIndex[3]._0 = 4;
	pIndex[3]._1 = 3;
	pIndex[3]._2 = 7;


	// +y
	pIndex[4]._0 = 4;
	pIndex[4]._1 = 5;
	pIndex[4]._2 = 1;

	pIndex[5]._0 = 4;
	pIndex[5]._1 = 1;
	pIndex[5]._2 = 0;

	// -y	 
	pIndex[6]._0 = 3;
	pIndex[6]._1 = 2;
	pIndex[6]._2 = 6;

	pIndex[7]._0 = 3;
	pIndex[7]._1 = 6;
	pIndex[7]._2 = 7;

	// +z
	pIndex[8]._0 = 7;
	pIndex[8]._1 = 6;
	pIndex[8]._2 = 5;

	pIndex[9]._0 = 7;
	pIndex[9]._1 = 5;
	pIndex[9]._2 = 4;

	// -z
	pIndex[10]._0 = 0;
	pIndex[10]._1 = 1;
	pIndex[10]._2 = 2;

	pIndex[11]._0 = 0;
	pIndex[11]._1 = 2;
	pIndex[11]._2 = 3;

	m_pIB->Unlock();
	
#endif

	for (_uint i = 0; i < COL_END; ++i)
	{
		m_pGraphicDev->CreateTexture(1, 1, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &m_pTexture[i], NULL);

		D3DLOCKED_RECT	tLockRect;
		ZeroMemory(&tLockRect, sizeof(D3DLOCKED_RECT));

		m_pTexture[i]->LockRect(0, &tLockRect, NULL, NULL);
		if (i != COL_TRIGGER)
		{
			*((_ulong*)tLockRect.pBits) = D3DXCOLOR(1.f * i, 1.f * (1.f - i), 0.f, 1.f);
		}
		else
			*((_ulong*)tLockRect.pBits) = D3DXCOLOR(0.f, 0.f, 1.f, 1.f);
		m_pTexture[i]->UnlockRect(0);
	}

	return S_OK;
}

void Engine::CCollider::Render_Collider(COLLTYPE eType, const _matrix* pColliderMatrix)
{
	m_matWorld = *pColliderMatrix;

#ifdef _DEBUG
	m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, FALSE);
	m_pGraphicDev->SetTransform(D3DTS_WORLD, pColliderMatrix);

	m_pGraphicDev->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);

	m_pGraphicDev->SetTexture(0, m_pTexture[eType]);

	m_pGraphicDev->SetStreamSource(0, m_pVB, 0, sizeof(VTXCUBE));
	m_pGraphicDev->SetFVF(FVF_CUBE);
	m_pGraphicDev->SetIndices(m_pIB);

	m_pGraphicDev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 8, 0, 12);

	m_pGraphicDev->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID	);
	m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, FALSE);
#endif
}

Engine::CCollider* Engine::CCollider::Create(LPDIRECT3DDEVICE9 pGraphicDev, const _vec3* pPos, const _ulong& dwNumVtx, const _ulong& dwStride, const _bool& flag)
{
	CCollider*	pInstance = new CCollider(pGraphicDev);

	if (FAILED(pInstance->Ready_Collider(pPos, dwNumVtx, dwStride, flag)))
		Safe_Release(pInstance);

	return pInstance;
}

void Engine::CCollider::Free(void)
{
	#ifdef _DEBUG
	Safe_Release(m_pVB);
	Safe_Release(m_pIB);

	for (_uint i = 0; i < COL_END; ++i)
		Safe_Release(m_pTexture[i]);

#endif


	Safe_Release(m_pGraphicDev);
}

