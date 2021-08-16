#include "TerrainCol.h"

USING(Engine)

Engine::CTerrainCol::CTerrainCol(LPDIRECT3DDEVICE9 pGraphicDev)
	: CVIBuffer(pGraphicDev)
	, m_pPos(nullptr)
	, m_bClone(false)
{
	ZeroMemory(&m_fH, sizeof(BITMAPFILEHEADER));
	ZeroMemory(&m_iH, sizeof(BITMAPFILEHEADER));
}

Engine::CTerrainCol::CTerrainCol(const CTerrainCol& rhs)
	: CVIBuffer(rhs)
	, m_pPos(rhs.m_pPos)
	, m_bClone(true)
	, m_fH(rhs.m_fH)
	, m_iH(rhs.m_iH), m_dwCntX(rhs.m_dwCntX), m_dwCntZ(rhs.m_dwCntZ),m_vStartPos(rhs.m_vStartPos)
{
	for (int i = 0; i < rhs.m_bNavi.size(); i++)
	{
		m_bNavi.push_back(rhs.m_bNavi[i]);
	}
}

Engine::CTerrainCol::~CTerrainCol(void)
{
}

const _vec3* Engine::CTerrainCol::Get_VtxPos(void) const
{
	return m_pPos;
}
_ulong Engine::CTerrainCol::Get_VtxCntX(void) const
{
	return m_dwCntX;
}

_ulong Engine::CTerrainCol::Get_VtxCntZ(void) const
{
	return m_dwCntZ;
}

void CTerrainCol::Save_Navi(DWORD& dwBytes, const HANDLE& hFile)
{
	_bool tempbool = false;
	_int  tempInt = m_bNavi.size();

	WriteFile(hFile, &tempInt, sizeof(_int), &dwBytes, nullptr);
	for (_int i = 0; i < m_bNavi.size(); i++)
	{
		tempbool = m_bNavi[i];	
		WriteFile(hFile, &tempbool, sizeof(_bool), &dwBytes, nullptr);
		if (m_bNavi[i])
		{
			VTXCOL*			pVertex = nullptr;
			INDEX32*		pIndex = nullptr;
			m_pIB->Lock(0, 0, (void**)&pIndex, 0);
			m_pVB->Lock(0, 0, (void**)&pVertex, 0);
			WriteFile(hFile, &pVertex[pIndex[i]._0], sizeof(VTXCOL), &dwBytes, nullptr);
			WriteFile(hFile, &pVertex[pIndex[i]._1], sizeof(VTXCOL), &dwBytes, nullptr);
			WriteFile(hFile, &pVertex[pIndex[i]._2], sizeof(VTXCOL), &dwBytes, nullptr);

			m_pVB->Unlock();
			m_pIB->Unlock();
		}
	}
}

void CTerrainCol::Load_Navi(DWORD& dwBytes, const HANDLE& hFile)
{
	_bool tempbool = false;
	_int  testnum = 0;
	ReadFile(hFile, &testnum, sizeof(_int), &dwBytes, nullptr);
	for (int i = 0; i < m_bNavi.size(); i++)
	{
		tempbool = false;		
		ReadFile(hFile, &tempbool, sizeof(_bool), &dwBytes, nullptr);
		if (tempbool)
		{
			m_bNavi[i] = tempbool;
			VTXCOL*			pVertex = nullptr;
			INDEX32*		pIndex = nullptr;
			m_pIB->Lock(0, 0, (void**)&pIndex, 0);
			m_pVB->Lock(0, 0, (void**)&pVertex, 0);
			ReadFile(hFile, &pVertex[pIndex[i]._0], sizeof(VTXCOL), &dwBytes, nullptr);
			ReadFile(hFile, &pVertex[pIndex[i]._1], sizeof(VTXCOL), &dwBytes, nullptr);
			ReadFile(hFile, &pVertex[pIndex[i]._2], sizeof(VTXCOL), &dwBytes, nullptr);

			m_pVB->Unlock();
			m_pIB->Unlock();
		}
	}

}

HRESULT Engine::CTerrainCol::Ready_Buffer(const _ulong& dwCntX,
	const _ulong& dwCntZ,
	const _ulong dwVtxItv, _vec3 vPos)
{
	m_dwVtxSize = sizeof(VTXCOL);
	m_dwVtxCnt = dwCntX * dwCntZ * 6;
	m_dwVtxFVF = FVF_COL;
	m_dwTriCnt = (dwCntX ) * (dwCntZ) * 2;
	m_dwCntZ = dwCntZ;
	m_dwCntX = dwCntX;
	m_vStartPos = vPos;
	m_pPos = new _vec3[m_dwVtxCnt];

	m_dwIdxSize = sizeof(INDEX32);
	m_IdxFmt = D3DFMT_INDEX32;

	FAILED_CHECK_RETURN(CVIBuffer::Ready_Buffer(), E_FAIL);



	_ulong			dwIndex = 0;
	VTXCOL*			pVertex = nullptr;
	_ulong          iVbIndex = 0;
	m_pVB->Lock(0, 0, (void**)&pVertex, 0);		// 1. 메모리 잠그는 역할, 2. 3인자를 통해서 저장된 버텍스 접근 권한을 얻음
													// 오른쪽 위 
	for (_ulong z = 0; z < dwCntZ; ++z)
	{
		for (_ulong x = 0; x < dwCntX; ++x)
		{
			//dwIndex = z * dwCntX + x;
			
			//아래 삼각형
			//4
			pVertex[iVbIndex].vPos = _vec3(_float(x * dwVtxItv + m_vStartPos.x), m_vStartPos.y, _float((z + 1) * dwVtxItv + m_vStartPos.z));

			m_pPos[iVbIndex] = pVertex[iVbIndex].vPos;

			pVertex[iVbIndex].dwColor = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
			iVbIndex++;
			//5
			pVertex[iVbIndex].vPos = _vec3(_float((x + 1) * dwVtxItv + m_vStartPos.x),  m_vStartPos.y, _float(z * dwVtxItv) + m_vStartPos.z);

			m_pPos[iVbIndex] = pVertex[iVbIndex].vPos;

			pVertex[iVbIndex].dwColor = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
			iVbIndex++;
			//6
			pVertex[iVbIndex].vPos = _vec3(_float(x * dwVtxItv + m_vStartPos.x), m_vStartPos.y, _float(z * dwVtxItv) + m_vStartPos.z);

			m_pPos[iVbIndex] = pVertex[iVbIndex].vPos;

			pVertex[iVbIndex].dwColor = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
			iVbIndex++;


			//위 삼각형
			//1
			pVertex[iVbIndex].vPos = _vec3(_float(x * dwVtxItv + m_vStartPos.x), m_vStartPos.y, _float((z + 1) * dwVtxItv + m_vStartPos.z));

			m_pPos[iVbIndex] = pVertex[iVbIndex].vPos;

			pVertex[iVbIndex].dwColor = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
			iVbIndex++;
			//2
			pVertex[iVbIndex].vPos = _vec3(_float((x + 1) * dwVtxItv + m_vStartPos.x), m_vStartPos.y, _float((z + 1) * dwVtxItv + m_vStartPos.z));

			m_pPos[iVbIndex] = pVertex[iVbIndex].vPos;

			pVertex[iVbIndex].dwColor = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
			iVbIndex++;
			//3
			pVertex[iVbIndex].vPos = _vec3(_float((x + 1) * dwVtxItv + m_vStartPos.x), m_vStartPos.y, _float(z * dwVtxItv + m_vStartPos.z));

			m_pPos[iVbIndex] = pVertex[iVbIndex].vPos;

			pVertex[iVbIndex].dwColor = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
			iVbIndex++;
		}
	}

	m_pVB->Unlock();
	//Safe_Delete_Array(pPixel);

	_ulong	dwTriCnt = 0;

	INDEX32*		pIndex = nullptr;

	m_pIB->Lock(0, 0, (void**)&pIndex, 0);

	for (_ulong z = 0; z < dwCntZ - 1; ++z)
	{
		for (_ulong x = 0; x < dwCntX - 1; ++x)
		{
			dwIndex = (6 * x) + (z * 6 * dwCntX);

			// 왼쪽 아래
	
			pIndex[dwTriCnt]._0 = dwIndex;
			pIndex[dwTriCnt]._1 = dwIndex + 1;
			pIndex[dwTriCnt]._2 = dwIndex + 2;
			m_bNavi.push_back(false);
			dwTriCnt++;

			// 오른쪽 위
			pIndex[dwTriCnt]._0 = dwIndex + 3;
			pIndex[dwTriCnt]._1 = dwIndex + 4;
			pIndex[dwTriCnt]._2 = dwIndex + 5;
			m_bNavi.push_back(false);
			dwTriCnt++;
		}
	}

	m_pIB->Unlock();

	return S_OK;
}

void CTerrainCol::Set_Select(int iIndex, vector<_vec3>& vPos)
{
	VTXCOL*			pVertex = nullptr;
	INDEX32*		pIndex = nullptr;

	m_pIB->Lock(0, 0, (void**)&pIndex, 0);
	m_pVB->Lock(0, 0, (void**)&pVertex, 0);
	if (m_bNavi[iIndex])
	{
		pVertex[pIndex[iIndex]._0].dwColor = D3DXCOLOR(1.f, 0.f, 0.f, 1.f);
		vPos[0] = pVertex[pIndex[iIndex]._0].vPos;
		pVertex[pIndex[iIndex]._1].dwColor = D3DXCOLOR(1.f, 0.f, 0.f, 1.f);
		vPos[1] = pVertex[pIndex[iIndex]._1].vPos;
		pVertex[pIndex[iIndex]._2].dwColor = D3DXCOLOR(1.f, 0.f, 0.f, 1.f);
		vPos[2] = pVertex[pIndex[iIndex]._2].vPos;
	}
	else
	{
		pVertex[pIndex[iIndex]._0].dwColor =  D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
		pVertex[pIndex[iIndex]._1].dwColor =  D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
		pVertex[pIndex[iIndex]._2].dwColor =  D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
	}

	m_pVB->Unlock();
	m_pIB->Unlock();
}

void CTerrainCol::Set_bNavi(int iIndex, bool flag)
{
	m_bNavi[iIndex] = flag;
}

void CTerrainCol::Set_Vertex(int iIndex, int i, int j,float Val)
{
	VTXCOL*			pVertex = nullptr;
	INDEX32*		pIndex = nullptr;

	m_pIB->Lock(0, 0, (void**)&pIndex, 0);
	m_pVB->Lock(0, 0, (void**)&pVertex, 0);
	if (i == 0)
	{
		if (j == 0)
		{
			pVertex[pIndex[iIndex]._0].vPos.x = Val;
		}
		else if (j == 1)
		{
			pVertex[pIndex[iIndex]._0].vPos.y = Val;
		}
		else if (j == 2)
		{
			pVertex[pIndex[iIndex]._0].vPos.z = Val;
		}
	}
	else if (i == 1)
	{
		if (j == 0)
		{
			pVertex[pIndex[iIndex]._1].vPos.x = Val;
		}
		else if (j == 1)
		{
			pVertex[pIndex[iIndex]._1].vPos.y = Val;
		}
		else if (j == 2)
		{
			pVertex[pIndex[iIndex]._1].vPos.z = Val;
		}
	}
	else if (i == 2)
	{
		if (j == 0)
		{
			pVertex[pIndex[iIndex]._2].vPos.x = Val;
		}
		else if (j == 1)
		{
			pVertex[pIndex[iIndex]._2].vPos.y = Val;
		}
		else if (j == 2)
		{
			pVertex[pIndex[iIndex]._2].vPos.z = Val;
		}
	}
	m_pVB->Unlock();
	m_pIB->Unlock();

}

Engine::CResources* Engine::CTerrainCol::Clone(void)
{
	return new CTerrainCol(*this);
}

Engine::CTerrainCol* Engine::CTerrainCol::Create(LPDIRECT3DDEVICE9 pGraphicDev,
	const _ulong& dwCntX,
	const _ulong& dwCntZ,
	const _ulong& dwVtxItv,
	_vec3 vPos)
{
	CTerrainCol*	pInstance = new CTerrainCol(pGraphicDev);

	if (FAILED(pInstance->Ready_Buffer(dwCntX, dwCntZ, dwVtxItv, vPos)))
		Safe_Release(pInstance);

	return pInstance;
}

void Engine::CTerrainCol::Free(void)
{
	CVIBuffer::Free();

	if (false == m_bClone)
		Safe_Delete_Array(m_pPos);
}

