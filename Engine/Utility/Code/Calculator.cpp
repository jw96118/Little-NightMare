#include "Calculator.h"
#include "TerrainTex.h"
#include "TerrainCol.h"
#include "Transform.h"
#include "Collider.h"
#include "iostream"
using namespace std;
USING(Engine)

Engine::CCalculator::CCalculator(LPDIRECT3DDEVICE9 pGraphicDev)
	: m_pGraphicDev(pGraphicDev)
{
	m_pGraphicDev->AddRef();
}

Engine::CCalculator::~CCalculator(void)
{

}

HRESULT Engine::CCalculator::Ready_Calculator(void)
{
	return S_OK;
}

_float Engine::CCalculator::Compute_HeightOnTerrain(const _vec3* pPos,
	const _vec3* pTerrainVtxPos,
	const _ulong& dwCntX,
	const _ulong& dwCntZ)
{
	// Ÿ�� ITV = 1.f
	_ulong	dwIndex = _ulong(pPos->z / 1.f) * dwCntX + _ulong(pPos->x / 1.f);

	_float	fRatioX = (pPos->x - pTerrainVtxPos[dwIndex + dwCntX].x) / 1.f;
	_float	fRatioZ = (pTerrainVtxPos[dwIndex + dwCntX].z - pPos->z) / 1.f;

	_float	fHeight[4] = {
		pTerrainVtxPos[dwIndex + dwCntX].y,
		pTerrainVtxPos[dwIndex + dwCntX + 1].y,
		pTerrainVtxPos[dwIndex + 1].y,
		pTerrainVtxPos[dwIndex].y
	};

	// ������ �� ���
	if (fRatioX > fRatioZ)
	{
		return fHeight[0] + (fHeight[1] - fHeight[0]) * fRatioX + (fHeight[2] - fHeight[1]) * fRatioZ;
	}
	// ���� �Ʒ� ���
	else
	{
		return fHeight[0] + (fHeight[2] - fHeight[3]) * fRatioX + (fHeight[3] - fHeight[0]) * fRatioZ;
	}

}

_float Engine::CCalculator::Compute_HeightOnTerrain1(_vec3* pPos,
	const _vec3* pTerrainVtxPos,
	const _ulong& dwCntX,
	const _ulong& dwCntZ)
{
	// Ÿ�� ITV = 1.f
	_ulong	dwIndex = _ulong(pPos->z / 1.f) * dwCntX + _ulong(pPos->x / 1.f);

	_float	fRatioX = (pPos->x - pTerrainVtxPos[dwIndex + dwCntX].x) / 1.f;
	_float	fRatioZ = (pTerrainVtxPos[dwIndex + dwCntX].z - pPos->z) / 1.f;

	D3DXPLANE		Plane;

	// ������ �� ���
	if (fRatioX > fRatioZ)
	{
		// ������ ��ġ �� �� ���� ���� ����� �������ִ� �Լ�
		D3DXPlaneFromPoints(&Plane,
			&pTerrainVtxPos[dwIndex + dwCntX],
			&pTerrainVtxPos[dwIndex + dwCntX + 1],
			&pTerrainVtxPos[dwIndex + 1]);
	}
	// ���� �Ʒ� ���
	else
	{
		D3DXPlaneFromPoints(&Plane,
			&pTerrainVtxPos[dwIndex + dwCntX],
			&pTerrainVtxPos[dwIndex + 1],
			&pTerrainVtxPos[dwIndex]);
	}

	// ax + by + cz + d =>  y = (-ax - cz -d ) / b

	pPos->y = (-Plane.a * pPos->x - Plane.c * pPos->z - Plane.d) / Plane.b;
	pPos->y += 1.f;

	return pPos->y;
}

Engine::CCalculator* Engine::CCalculator::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CCalculator*	pInstance = new CCalculator(pGraphicDev);

	if (FAILED(pInstance->Ready_Calculator()))
		Safe_Release(pInstance);

	return pInstance;
}

void Engine::CCalculator::Free(void)
{
	Safe_Release(m_pGraphicDev);
}

_vec3 Engine::CCalculator::Picking_OnTerrain(HWND hWnd,
	const CTerrainTex* pTerrainBufferCom,
	const CTransform* pTerrainTransformCom)
{
	POINT			ptMouse{};

	// ������ ���� == ����Ʈ ����
	GetCursorPos(&ptMouse);
	ScreenToClient(hWnd, &ptMouse);

	// �� ��Ʈ���� ��������

	D3DVIEWPORT9			ViewPort;
	ZeroMemory(&ViewPort, sizeof(D3DVIEWPORT9));
	m_pGraphicDev->GetViewport(&ViewPort);

	_vec3	vMousePos;

	vMousePos.x = ptMouse.x / (ViewPort.Width * 0.5f) - 1.f;
	vMousePos.y = -ptMouse.y / (ViewPort.Height * 0.5f) + 1.f;
	vMousePos.z = 0.f;

	// �������� �� �����̽���
	_matrix		matProj;

	m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &matProj);
	D3DXMatrixInverse(&matProj, NULL, &matProj);
	D3DXVec3TransformCoord(&vMousePos, &vMousePos, &matProj);

	_vec3	vRayPos, vRayDir;

	vRayPos = _vec3(0.f, 0.f, 0.f);
	vRayDir = vMousePos - vRayPos;

	// �� �����̽����� ���� �����̽���
	_matrix		matView;

	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
	D3DXMatrixInverse(&matView, NULL, &matView);
	D3DXVec3TransformCoord(&vRayPos, &vRayPos, &matView);
	D3DXVec3TransformNormal(&vRayDir, &vRayDir, &matView);

	// ���忡�� ���÷�
	_matrix		matWorld;
	pTerrainTransformCom->Get_WorldMatrix(&matWorld);
	D3DXMatrixInverse(&matWorld, NULL, &matWorld);
	D3DXVec3TransformCoord(&vRayPos, &vRayPos, &matWorld);
	D3DXVec3TransformNormal(&vRayDir, &vRayDir, &matWorld);

	_ulong	dwVtxCntX = pTerrainBufferCom->Get_VtxCntX();
	_ulong	dwVtxCntZ = pTerrainBufferCom->Get_VtxCntZ();

	const _vec3*	pTerrainVtx = pTerrainBufferCom->Get_VtxPos();

	_ulong			dwVtxIdx[3];
	_float			fU, fV, fDist;

	for (_ulong i = 0; i < dwVtxCntZ - 1; ++i)
	{
		for (_ulong j = 0; j < dwVtxCntX - 1; ++j)
		{
			_ulong	dwIndex = i * dwVtxCntX + j;

			// ������ ��
			dwVtxIdx[0] = dwIndex + dwVtxCntX;
			dwVtxIdx[1] = dwIndex + dwVtxCntX + 1;
			dwVtxIdx[2] = dwIndex + 1;

			if (D3DXIntersectTri(&pTerrainVtx[dwVtxIdx[1]],
				&pTerrainVtx[dwVtxIdx[2]],
				&pTerrainVtx[dwVtxIdx[0]],
				&vRayPos,
				&vRayDir, &fU, &fV, &fDist))
			{
				return _vec3(pTerrainVtx[dwVtxIdx[1]].x + (pTerrainVtx[dwVtxIdx[0]].x - pTerrainVtx[dwVtxIdx[1]].x) * fU,
					0.f,
					pTerrainVtx[dwVtxIdx[1]].z + (pTerrainVtx[dwVtxIdx[2]].z - pTerrainVtx[dwVtxIdx[1]].z) * fV);
			}


			// ���� �Ʒ�
			dwVtxIdx[0] = dwIndex + dwVtxCntX;
			dwVtxIdx[1] = dwIndex + 1;
			dwVtxIdx[2] = dwIndex;

			if (D3DXIntersectTri(&pTerrainVtx[dwVtxIdx[2]],
				&pTerrainVtx[dwVtxIdx[0]],
				&pTerrainVtx[dwVtxIdx[1]],
				&vRayPos,
				&vRayDir, &fU, &fV, &fDist))
			{
				return _vec3(pTerrainVtx[dwVtxIdx[2]].x + (pTerrainVtx[dwVtxIdx[1]].x - pTerrainVtx[dwVtxIdx[2]].x) * fU,
					0.f,
					pTerrainVtx[dwVtxIdx[2]].z + (pTerrainVtx[dwVtxIdx[0]].z - pTerrainVtx[dwVtxIdx[2]].z) * fV);
			}
		}
	}

	return _vec3(0.f, 0.f, 0.f);
}

void CCalculator::Picking_OnNaviTerrain(HWND hWnd, CTerrainCol * pTerrainBufferCom, const CTransform * pTerrainTransformCom, POINT WinPt,_bool flag, vector<_vec3> & vPos, int& num)
{
	POINT			ptMouse{};

	// ������ ���� == ����Ʈ ����
	GetCursorPos(&ptMouse);
	ScreenToClient(hWnd, &ptMouse);

	_vec3	vMousePos;

	vMousePos.x = ptMouse.x / (WinPt.x * 0.5f) - 1.f;
	vMousePos.y = -ptMouse.y / (WinPt.y * 0.5f) + 1.f;
	vMousePos.z = 0.f;

	// �������� �� �����̽���
	_matrix		matProj;

	m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &matProj);
	D3DXMatrixInverse(&matProj, NULL, &matProj);
	D3DXVec3TransformCoord(&vMousePos, &vMousePos, &matProj);

	_vec3	vRayPos, vRayDir;

	vRayPos = _vec3(0.f, 0.f, 0.f);
	vRayDir = vMousePos - vRayPos;

	// �� �����̽����� ���� �����̽���
	_matrix		matView;

	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
	D3DXMatrixInverse(&matView, NULL, &matView);
	D3DXVec3TransformCoord(&vRayPos, &vRayPos, &matView);
	D3DXVec3TransformNormal(&vRayDir, &vRayDir, &matView);

	// ���忡�� ���÷�
	_matrix		matWorld;
	pTerrainTransformCom->Get_WorldMatrix(&matWorld);
	D3DXMatrixInverse(&matWorld, NULL, &matWorld);
	D3DXVec3TransformCoord(&vRayPos, &vRayPos, &matWorld);
	D3DXVec3TransformNormal(&vRayDir, &vRayDir, &matWorld);

	_ulong	dwVtxCntX = pTerrainBufferCom->Get_VtxCntX();
	_ulong	dwVtxCntZ = pTerrainBufferCom->Get_VtxCntZ();

	const _vec3*	pTerrainVtx = pTerrainBufferCom->Get_VtxPos();

	_ulong			dwVtxIdx[3];
	_float			fU, fV, fDist;
	_ulong	        dwTriCnt = 0;
	for (_ulong i = 0; i < dwVtxCntZ - 1; ++i)
	{
		for (_ulong j = 0; j < dwVtxCntX - 1; ++j)
		{
			_ulong	dwIndex = (6 * j) + (i * 6 * dwVtxCntX);
			// ������ ��
			dwVtxIdx[0] = dwIndex ;
			dwVtxIdx[1] = dwIndex + 1;
			dwVtxIdx[2] = dwIndex + 2;

			if (D3DXIntersectTri(&pTerrainVtx[dwVtxIdx[0]],
				&pTerrainVtx[dwVtxIdx[1]],
				&pTerrainVtx[dwVtxIdx[2]],
				&vRayPos,
				&vRayDir, &fU, &fV, &fDist))
			{
				pTerrainBufferCom->Set_bNavi(dwTriCnt, flag);
				pTerrainBufferCom->Set_Select(dwTriCnt, vPos);
				num = dwTriCnt;
			}
			//pTerrainBufferCom->Set_bNavi(dwTriCnt, false);
			dwTriCnt++;

			// ���� �Ʒ�
			dwVtxIdx[0] = dwIndex + 3;
			dwVtxIdx[1] = dwIndex + 4;
			dwVtxIdx[2] = dwIndex + 5;

			if (D3DXIntersectTri(&pTerrainVtx[dwVtxIdx[0]],
				&pTerrainVtx[dwVtxIdx[1]],
				&pTerrainVtx[dwVtxIdx[2]],
				&vRayPos,
				&vRayDir, &fU, &fV, &fDist))
			{
				pTerrainBufferCom->Set_bNavi(dwTriCnt, flag);
				pTerrainBufferCom->Set_Select(dwTriCnt, vPos);
				num = dwTriCnt;
			}
			//pTerrainBufferCom->Set_bNavi(dwTriCnt, false);
			dwTriCnt++;
		}
	}


}

_bool CCalculator::Picking_StaticMesh(HWND hWnd, CCollider * pMeshBufferCom, const CTransform * pMeshTransformCom, POINT WinPt , _float& fDist)
{
	POINT			ptMouse{};

	// ������ ���� == ����Ʈ ����
	GetCursorPos(&ptMouse);
	ScreenToClient(hWnd, &ptMouse);


	_vec3	vMousePos;
	//cout << ViewPort.Width << " , " << ViewPort.Height << endl;
	vMousePos.x = ptMouse.x / (WinPt.x * 0.5f) - 1.f;
	vMousePos.y = -ptMouse.y / (WinPt.y * 0.5f) + 1.f;
	vMousePos.z = 0.f;

	// �������� �� �����̽���
	_matrix		matProj;

	m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &matProj);
	D3DXMatrixInverse(&matProj, NULL, &matProj);
	D3DXVec3TransformCoord(&vMousePos, &vMousePos, &matProj);

	_vec3	vRayPos, vRayDir;

	vRayPos = _vec3(0.f, 0.f, 0.f);
	vRayDir = vMousePos - vRayPos;

	// �� �����̽����� ���� �����̽���
	_matrix		matView;

	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
	D3DXMatrixInverse(&matView, NULL, &matView);
	D3DXVec3TransformCoord(&vRayPos, &vRayPos, &matView);
	D3DXVec3TransformNormal(&vRayDir, &vRayDir, &matView);

	// ���忡�� ���÷�
	_matrix		matWorld;
	pMeshTransformCom->Get_WorldMatrix(&matWorld);
	D3DXMatrixInverse(&matWorld, NULL, &matWorld);
	D3DXVec3TransformCoord(&vRayPos, &vRayPos, &matWorld);
	D3DXVec3TransformNormal(&vRayDir, &vRayDir, &matWorld);

	_float			fU = 0.f, fV = 0.f;

	if (Check_Cube(pMeshBufferCom->Get_Cube(0), pMeshBufferCom->Get_Cube(1), pMeshBufferCom->Get_Cube(2), vRayPos, vRayDir, fU, fV, fDist))
		return true;
	if (Check_Cube(pMeshBufferCom->Get_Cube(0), pMeshBufferCom->Get_Cube(2), pMeshBufferCom->Get_Cube(3), vRayPos, vRayDir, fU, fV, fDist))
		return true;
	if (Check_Cube(pMeshBufferCom->Get_Cube(4), pMeshBufferCom->Get_Cube(5), pMeshBufferCom->Get_Cube(1), vRayPos, vRayDir, fU, fV, fDist))
		return true;
	if (Check_Cube(pMeshBufferCom->Get_Cube(4), pMeshBufferCom->Get_Cube(1), pMeshBufferCom->Get_Cube(0), vRayPos, vRayDir, fU, fV, fDist))
		return true;
	if (Check_Cube(pMeshBufferCom->Get_Cube(1), pMeshBufferCom->Get_Cube(5), pMeshBufferCom->Get_Cube(6), vRayPos, vRayDir, fU, fV, fDist))
		return true;
	if (Check_Cube(pMeshBufferCom->Get_Cube(1), pMeshBufferCom->Get_Cube(6), pMeshBufferCom->Get_Cube(2), vRayPos, vRayDir, fU, fV, fDist))
		return true;

	if (Check_Cube(pMeshBufferCom->Get_Cube(4), pMeshBufferCom->Get_Cube(0), pMeshBufferCom->Get_Cube(3), vRayPos, vRayDir, fU, fV, fDist))
		return true;
	if (Check_Cube(pMeshBufferCom->Get_Cube(4), pMeshBufferCom->Get_Cube(3), pMeshBufferCom->Get_Cube(7), vRayPos, vRayDir, fU, fV, fDist))
		return true;

	if (Check_Cube(pMeshBufferCom->Get_Cube(3), pMeshBufferCom->Get_Cube(2), pMeshBufferCom->Get_Cube(6), vRayPos, vRayDir, fU, fV, fDist))
		return true;
	if (Check_Cube(pMeshBufferCom->Get_Cube(3), pMeshBufferCom->Get_Cube(6), pMeshBufferCom->Get_Cube(7), vRayPos, vRayDir, fU, fV, fDist))
		return true;

	if (Check_Cube(pMeshBufferCom->Get_Cube(7), pMeshBufferCom->Get_Cube(6), pMeshBufferCom->Get_Cube(5), vRayPos, vRayDir, fU, fV, fDist))
		return true;
	if (Check_Cube(pMeshBufferCom->Get_Cube(7), pMeshBufferCom->Get_Cube(5), pMeshBufferCom->Get_Cube(4), vRayPos, vRayDir, fU, fV, fDist))
		return true;


	return false;
}

_bool CCalculator::Picking_NaviMesh(HWND hWnd, _vec3 * vBuffer, const CTransform * pMeshTransformCom, POINT WinPt, _float & fDist)
{
	POINT			ptMouse{};

	// ������ ���� == ����Ʈ ����
	GetCursorPos(&ptMouse);
	ScreenToClient(hWnd, &ptMouse);


	_vec3	vMousePos;
	//cout << ViewPort.Width << " , " << ViewPort.Height << endl;
	vMousePos.x = ptMouse.x / (WinPt.x * 0.5f) - 1.f;
	vMousePos.y = -ptMouse.y / (WinPt.y * 0.5f) + 1.f;
	vMousePos.z = 0.f;

	// �������� �� �����̽���
	_matrix		matProj;

	m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &matProj);
	D3DXMatrixInverse(&matProj, NULL, &matProj);
	D3DXVec3TransformCoord(&vMousePos, &vMousePos, &matProj);

	_vec3	vRayPos, vRayDir;

	vRayPos = _vec3(0.f, 0.f, 0.f);
	vRayDir = vMousePos - vRayPos;

	// �� �����̽����� ���� �����̽���
	_matrix		matView;

	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
	D3DXMatrixInverse(&matView, NULL, &matView);
	D3DXVec3TransformCoord(&vRayPos, &vRayPos, &matView);
	D3DXVec3TransformNormal(&vRayDir, &vRayDir, &matView);

	// ���忡�� ���÷�
	_matrix		matWorld;
	pMeshTransformCom->Get_WorldMatrix(&matWorld);
	D3DXMatrixInverse(&matWorld, NULL, &matWorld);
	D3DXVec3TransformCoord(&vRayPos, &vRayPos, &matWorld);
	D3DXVec3TransformNormal(&vRayDir, &vRayDir, &matWorld);

	_float			fU = 0.f, fV = 0.f;


	if (D3DXIntersectTri(&vBuffer[0],
		&vBuffer[1],
		&vBuffer[2],
		&vRayPos,
		&vRayDir, &fU, &fV, &fDist))
	{
		return true;
	}
	return false;

}

_bool Engine::CCalculator::Collision_AABB(const _vec3* pDestMin, const _vec3* pDestMax, const _matrix* pDestWorld, const _vec3* pSourMin, const _vec3* pSourMax, const _matrix* pSourWorld)
{
	_vec3		vDestMin, vDestMax, vSourMin, vSourMax;
	_float		fMin, fMax;


	D3DXVec3TransformCoord(&vDestMin, pDestMin, pDestWorld);
	D3DXVec3TransformCoord(&vDestMax, pDestMax, pDestWorld);

	D3DXVec3TransformCoord(&vSourMin, pSourMin, pSourWorld);
	D3DXVec3TransformCoord(&vSourMax, pSourMax, pSourWorld);

	// x�࿡�� �ٶ���� ��

	fMin = max(vDestMin.x, vSourMin.x);
	fMax = min(vDestMax.x, vSourMax.x);

	if (fMax < fMin)
		return false;

	// y�࿡�� �ٶ���� ��

	fMin = max(vDestMin.y, vSourMin.y);
	fMax = min(vDestMax.y, vSourMax.y);

	if (fMax < fMin)
		return false;

	// z�࿡�� �ٶ���� ��

	fMin = max(vDestMin.z, vSourMin.z);
	fMax = min(vDestMax.z, vSourMax.z);

	if (fMax < fMin)
		return false;


	return true;
}

_bool Engine::CCalculator::Collision_OBB(const _vec3* pDestMin, const _vec3* pDestMax, const _matrix* pDestWorld, const _vec3* pSourMin, const _vec3* pSourMax, const _matrix* pSourWorld)
{
	OBB		tObb[2];
	ZeroMemory(&tObb, sizeof(OBB) * 2);

	Set_Point(&tObb[0], pDestMin, pDestMax);
	Set_Point(&tObb[1], pSourMin, pSourMax);

	for (_uint i = 0; i < 8; ++i)
	{
		D3DXVec3TransformCoord(&tObb[0].vPoint[i], &tObb[0].vPoint[i], pDestWorld);
		D3DXVec3TransformCoord(&tObb[1].vPoint[i], &tObb[1].vPoint[i], pSourWorld);
	}
	D3DXVec3TransformCoord(&tObb[0].vCenter, &tObb[0].vCenter, pDestWorld);
	D3DXVec3TransformCoord(&tObb[1].vCenter, &tObb[1].vCenter, pSourWorld);

	for (_uint i = 0; i < 2; ++i)
		Set_Axis(&tObb[i]);


	// �� �浹�� ����� ������� �浹ó���� �Ѵ�.

	_float		fDistance[3]{};			// 0�� : ù ��° �ڽ��� �� ������ ����� ���⺤���� ������ ������ ������ ������ ��
										// 1�� : �� ��° �ڽ��� �� ������ ����� ���⺤���� ������ ������ ������ ������ ��
										// 2�� : �� �ڽ��� ���� ���� ���̸� ������ ������ ������ ����


	for (_uint i = 0; i < 2; ++i)
	{
		for (_uint j = 0; j < 3; ++j)
		{
			fDistance[0] = fabs(D3DXVec3Dot(&tObb[0].vProjAxis[0], &tObb[i].vAxis[j])) +
				fabs(D3DXVec3Dot(&tObb[0].vProjAxis[1], &tObb[i].vAxis[j])) +
				fabs(D3DXVec3Dot(&tObb[0].vProjAxis[2], &tObb[i].vAxis[j]));

			fDistance[1] = fabs(D3DXVec3Dot(&tObb[1].vProjAxis[0], &tObb[i].vAxis[j])) +
				fabs(D3DXVec3Dot(&tObb[1].vProjAxis[1], &tObb[i].vAxis[j])) +
				fabs(D3DXVec3Dot(&tObb[1].vProjAxis[2], &tObb[i].vAxis[j]));

			fDistance[2] = fabs(D3DXVec3Dot(&(tObb[1].vCenter - tObb[0].vCenter), &tObb[i].vAxis[j]));

			if (fDistance[0] + fDistance[1] < fDistance[2])
				return false;
		}
	}

	return true;
}

void Engine::CCalculator::Set_Point(OBB* pObb, const _vec3* pMin, const _vec3* pMax)
{
	pObb->vPoint[0] = _vec3(pMin->x, pMax->y, pMin->z);
	pObb->vPoint[1] = _vec3(pMax->x, pMax->y, pMin->z);
	pObb->vPoint[2] = _vec3(pMax->x, pMin->y, pMin->z);
	pObb->vPoint[3] = _vec3(pMin->x, pMin->y, pMin->z);

	pObb->vPoint[4] = _vec3(pMin->x, pMax->y, pMax->z);
	pObb->vPoint[5] = _vec3(pMax->x, pMax->y, pMax->z);
	pObb->vPoint[6] = _vec3(pMax->x, pMin->y, pMax->z);
	pObb->vPoint[7] = _vec3(pMin->x, pMin->y, pMax->z);

	pObb->vCenter = (*pMin + *pMax) * 0.5f; // ������ 2�ϴ°ſ���...
}

void Engine::CCalculator::Set_Axis(OBB* pObb)
{
	pObb->vProjAxis[0] = (pObb->vPoint[2] + pObb->vPoint[5]) * 0.5f - pObb->vCenter;
	pObb->vProjAxis[1] = (pObb->vPoint[0] + pObb->vPoint[5]) * 0.5f - pObb->vCenter;
	pObb->vProjAxis[2] = (pObb->vPoint[7] + pObb->vPoint[5]) * 0.5f - pObb->vCenter;

	pObb->vAxis[0] = pObb->vPoint[2] - pObb->vPoint[3];
	pObb->vAxis[1] = pObb->vPoint[0] - pObb->vPoint[3];
	pObb->vAxis[2] = pObb->vPoint[7] - pObb->vPoint[3];

	for (_uint i = 0; i < 3; ++i)
		D3DXVec3Normalize(&pObb->vAxis[i], &pObb->vAxis[i]);
}

_bool CCalculator::Check_Cube(_vec3 vec1, _vec3 vec2, _vec3 vec3, _vec3 vRayPos, _vec3 vRayDir, _float fU, _float fV, _float& fDist)
{
	if (D3DXIntersectTri(&vec1,
		&vec2 ,
		&vec3,
		&vRayPos,
		&vRayDir, &fU, &fV, &fDist))
	{
		return true;
	}
	return false;
}

