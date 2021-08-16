#ifndef Cell_h__
#define Cell_h__

#include "Engine_Defines.h"
#include "Base.h"

BEGIN(Engine)

class CLine;
struct CheckCell
{
	CheckCell() {};
	CheckCell(float x1 ,float y1 ,float x2 ,float y2 )
	{
		this->x1 = x1;
		this->y1 = y1;
		this->x2 = x2;
		this->y2 = y2;
	}
	float x1 = 0.f;
	float y1 = 0.f;
	float x2 = 0.f;
	float y2 = 0.f;
};
class ENGINE_DLL CCell : public CBase
{

public:
	enum POINT		{ POINT_A, POINT_B, POINT_C, POINT_END };
	enum NEIGHBOR	{ NEIGHBOR_AB, NEIGHBOR_BC, NEIGHBOR_CA, NEIGHBOR_END };
	enum LINE		{ LINE_AB, LINE_BC, LINE_CA, LINE_END };
	enum COMPARE	{ COMPARE_MOVE, COMPARE_STOP };

private:
	explicit CCell(LPDIRECT3DDEVICE9 pGraphicDev);

	virtual ~CCell(void);

public:
	const _ulong*		Get_Index(void) { return &m_dwIndex; }
	const _vec3*		Get_Point(POINT eType) const        { return &m_vPoint[eType]; }
	_vec3		        Get_PointP(POINT eType)		        { return m_vPoint[eType]; }
	CCell*				Get_Neighbor(NEIGHBOR eType) const	{ return m_pNeighbor[eType]; }
	void				Set_Neighbor(NEIGHBOR eType, CCell* pNeighbor) { m_pNeighbor[eType] = pNeighbor; }
	_bool               Check_Pos(const _vec3 vPos);
public:
	HRESULT		Ready_Cell(	const _ulong& dwIndex,
							const _vec3* pPointA,
							const _vec3* pPointB,
							const _vec3* pPointC);

	_bool		Compare_Point(const _vec3* pPointF, 
							  const _vec3* pPointS, 
							  CCell* pCell);

	void		Render_Cell(void);

	COMPARE		Compare(const _vec3* pEndPos, _ulong* pIndex);


private:
	_vec3		m_vPoint[POINT_END];
	CCell*		m_pNeighbor[NEIGHBOR_END];
	CLine*		m_pLine[LINE_END];
	LPD3DXLINE	m_pD3DXLine;
	_ulong		m_dwIndex;
	LPDIRECT3DDEVICE9 m_pGraphicDev;

public:
	static CCell*		Create(LPDIRECT3DDEVICE9 pGraphicDev, const _ulong& dwIndex, const _vec3* pPointA, const _vec3* pPointB, const _vec3* pPointC);

	virtual void		Free(void);		
};

END
#endif // Cell_h__
