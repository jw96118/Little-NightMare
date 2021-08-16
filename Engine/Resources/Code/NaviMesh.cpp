#include "NaviMesh.h"

USING(Engine)

Engine::CNaviMesh::CNaviMesh(LPDIRECT3DDEVICE9 pGraphicDev)
	:CMesh(pGraphicDev)
	, m_dwIndex(0)
{

}

Engine::CNaviMesh::CNaviMesh(const CNaviMesh& rhs)
	: CMesh(rhs)
	, m_vecCell(rhs.m_vecCell)
	, m_dwIndex(rhs.m_dwIndex)	
{
	for (auto& iter : m_vecCell)
		iter->AddRef();
}

Engine::CNaviMesh::~CNaviMesh(void)
{

}

void CNaviMesh::PushBack_Cell(CCell * pCell)
{
	m_vecCell.push_back(pCell);
}

void CNaviMesh::Set_Index_UsePos(const _vec3 vPos)
{
	for (int i = 0; i < m_vecCell.size(); i++)
	{
		if (m_vecCell[i]->Check_Pos(vPos))
		{
			m_dwIndex = i;
			return;
		}
	}
	m_dwIndex = 0;
}
HRESULT Engine::CNaviMesh::Ready_NaviMesh(void)
{
	//m_vecCell.reserve(4);

	//CCell*		pCell = nullptr;

	//// 0
	//pCell = CCell::Create(m_pGraphicDev,
	//					 m_vecCell.size(),
	//					 &_vec3(0.f, 0.f, 2.f), 
	//					 &_vec3(2.f, 0.f, 0.f), 
	//					 &_vec3(0.f, 0.f, 0.f));
	//NULL_CHECK_RETURN(pCell, E_FAIL);
	//m_vecCell.push_back(pCell);

	//// 1
	//pCell = CCell::Create(m_pGraphicDev,
	//					  m_vecCell.size(),
	//					  &_vec3(0.f, 0.f, 2.f),
	//					  &_vec3(2.f, 0.f, 2.f),
	//					  &_vec3(2.f, 0.f, 0.f));
	//NULL_CHECK_RETURN(pCell, E_FAIL);
	//m_vecCell.push_back(pCell);

	//// 2
	//pCell = CCell::Create(m_pGraphicDev,
	//	m_vecCell.size(),
	//	&_vec3(0.f, 0.f, 4.f),
	//					  &_vec3(2.f, 0.f, 2.f),
	//					  &_vec3(0.f, 0.f, 2.f));
	//NULL_CHECK_RETURN(pCell, E_FAIL);
	//m_vecCell.push_back(pCell);

	//// 3
	//pCell = CCell::Create(m_pGraphicDev,
	//	m_vecCell.size(),
	//	&_vec3(2.f, 0.f, 2.f),
	//					  &_vec3(4.f, 0.f, 0.f),
	//					  &_vec3(2.f, 0.f, 0.f));
	//NULL_CHECK_RETURN(pCell, E_FAIL);
	//m_vecCell.push_back(pCell);

	//FAILED_CHECK_RETURN(Link_Cell(), E_FAIL);

	return S_OK;
}

void CNaviMesh::Render_NaviMesh(void)
{
	for (auto& iter : m_vecCell)
		iter->Render_Cell();

}

_vec3 CNaviMesh::Move_OnNaviMesh(const _vec3 * pTargetPos, const _vec3 * pTargetDir, _bool& flag)
{
	_vec3	vEndPos = *pTargetPos + *pTargetDir;

	if (CCell::COMPARE_MOVE == m_vecCell[m_dwIndex]->Compare(&vEndPos, &m_dwIndex))
	{
		flag = true;
		return vEndPos;
	}
	else if (CCell::COMPARE_STOP == m_vecCell[m_dwIndex]->Compare(&vEndPos, &m_dwIndex))
	{
		flag = false;
		return *pTargetPos;
	}
}

_float CNaviMesh::Navi(int index,const _vec3 pPos)
{
	_vec3	vEndPos = pPos;
	D3DXPLANE Plane;
	_vec3  Point[CCell::POINT_END];
	for (int i = 0; i < CCell::POINT_END; i++)
	{
		Point[i] = m_vecCell[m_dwIndex]->Get_PointP((CCell::POINT)i);
	}
	D3DXPlaneFromPoints(&Plane, &Point[CCell::POINT_A], &Point[CCell::POINT_B], &Point[CCell::POINT_C]);
	_float test = ((vEndPos.x *Plane.a) + (vEndPos.y * Plane.b) + (vEndPos.z *Plane.c) + Plane.d) ;
	if (index == 0)
		return -((vEndPos.y * Plane.b) + (vEndPos.z *Plane.c) + Plane.d) / Plane.a;
	else if (index == 1)
		return  -((vEndPos.x *Plane.a) + (vEndPos.z *Plane.c) + Plane.d) / Plane.b;
	else if (index == 2)
		return -((vEndPos.x *Plane.a) + (vEndPos.y *Plane.b) + Plane.d) / Plane.c;
	
	
	return 0.f;
}

HRESULT Engine::CNaviMesh::Link_Cell(void)
{
	_ulong	dwSize = m_vecCell.size();

	for (_ulong i = 0; i < dwSize; ++i)
	{
		for (_ulong j = 0; j < dwSize; ++j)
		{
			if(i == j)
				continue;

			if(nullptr == m_vecCell[i]->Get_Neighbor(CCell::NEIGHBOR_AB) &&
				true == m_vecCell[j]->Compare_Point(m_vecCell[i]->Get_Point(CCell::POINT_A), 
													m_vecCell[i]->Get_Point(CCell::POINT_B), 
													m_vecCell[i]))
			{
				m_vecCell[i]->Set_Neighbor(CCell::NEIGHBOR_AB, m_vecCell[j]);
				continue;
			}

			if (nullptr == m_vecCell[i]->Get_Neighbor(CCell::NEIGHBOR_BC) &&
				true == m_vecCell[j]->Compare_Point(m_vecCell[i]->Get_Point(CCell::POINT_B),
													m_vecCell[i]->Get_Point(CCell::POINT_C),
													m_vecCell[i]))
			{
				m_vecCell[i]->Set_Neighbor(CCell::NEIGHBOR_BC, m_vecCell[j]);
				continue;
			}


			if (nullptr == m_vecCell[i]->Get_Neighbor(CCell::NEIGHBOR_CA) &&
				true == m_vecCell[j]->Compare_Point(m_vecCell[i]->Get_Point(CCell::POINT_C),
													m_vecCell[i]->Get_Point(CCell::POINT_A),
													m_vecCell[i]))
			{
				m_vecCell[i]->Set_Neighbor(CCell::NEIGHBOR_CA, m_vecCell[j]);
				continue;
			}
		}
	}

	return S_OK;
}

_bool CNaviMesh::Check_Sliding(_vec3 & pPos, _float fY,_bool& flag)
{
	D3DXPLANE Plane;
	_vec3  Point[CCell::POINT_END];
	_vec2  PointDot[CCell::POINT_END];
	for (int i = 0; i < CCell::POINT_END; i++)
	{
		Point[i] = m_vecCell[m_dwIndex]->Get_PointP((CCell::POINT)i);
		PointDot[i].x = m_vecCell[m_dwIndex]->Get_PointP((CCell::POINT)i).x - m_vecCell[m_dwIndex]->Get_PointP((CCell::POINT)0).x;
		PointDot[i].y = m_vecCell[m_dwIndex]->Get_PointP((CCell::POINT)i).y - m_vecCell[m_dwIndex]->Get_PointP((CCell::POINT)0).y;
		D3DXVec2Normalize(&PointDot[i], &PointDot[i]);
	}
	_vec2 test = { 0.f,1.f };
	_float fDot = D3DXToDegree(acosf(D3DXVec2Dot(&test, &PointDot[CCell::POINT_B])));
	if (abs(Point[CCell::POINT_A].x - Point[CCell::POINT_B].x) < 1.f)
	{
		if (fDot <= 25.f)
		{
			_vec3 tempVec3 = pPos;
			D3DXPlaneFromPoints(&Plane, &Point[CCell::POINT_A], &Point[CCell::POINT_B], &Point[CCell::POINT_C]);
			pPos.x = -((tempVec3.y *Plane.b) + (tempVec3.z *Plane.c) + Plane.d) / Plane.a;
			if (pPos.x < Point[CCell::POINT_A].x)
			{
				pPos.x = Point[CCell::POINT_A].x;
				pPos.y = Point[CCell::POINT_A].y;
				flag = true;
			}
			return true;
		}
		else if (fDot >= 160.f)
		{
			_vec3 tempVec3 = pPos;
			D3DXPlaneFromPoints(&Plane, &Point[CCell::POINT_A], &Point[CCell::POINT_B], &Point[CCell::POINT_C]);
			pPos.x = -((tempVec3.y *Plane.b) + (tempVec3.z *Plane.c) + Plane.d) / Plane.a;
			if (pPos.x > Point[CCell::POINT_B].x)
			{
				pPos.x = Point[CCell::POINT_B].x;
				pPos.y = Point[CCell::POINT_B].y;
				flag = true;
			}
			return true;
		}
	}
	return false;
}

Engine::CNaviMesh* Engine::CNaviMesh::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CNaviMesh*	pInstance = new CNaviMesh(pGraphicDev);

	if (FAILED(pInstance->Ready_NaviMesh()))
		Safe_Release(pInstance);

	return pInstance;
}

Engine::CResources* Engine::CNaviMesh::Clone(void)
{
	return new CNaviMesh(*this);
}

void Engine::CNaviMesh::Free(void)
{
	for_each(m_vecCell.begin(), m_vecCell.end(), CDeleteObj());
	m_vecCell.clear();

	CMesh::Free();
}

