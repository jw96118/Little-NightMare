#ifndef NaviMesh_h__
#define NaviMesh_h__

#include "Mesh.h"
#include "Cell.h"

BEGIN(Engine)

class ENGINE_DLL CNaviMesh : public CMesh
{
private:
	explicit CNaviMesh(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CNaviMesh(const CNaviMesh& rhs);
	virtual ~CNaviMesh(void);

public:
	void		Set_Index(const _ulong& dwIndex) { m_dwIndex = dwIndex; }
	void		Set_Index_UsePos(const _vec3 vPos);
	void        PushBack_Cell(CCell* pCell);
public:
	HRESULT		Ready_NaviMesh(void);
	void		Render_NaviMesh(void);
	_vec3		Move_OnNaviMesh(const _vec3* pTargetPos, const _vec3* pTargetDir,_bool& flag);
	_float      Navi(int index,const _vec3 pPos);
	HRESULT		Link_Cell(void);
	_bool       Check_Sliding(_vec3& pPos,_float fY, _bool& flag);
private:
	vector<CCell*>			m_vecCell;
	_ulong					m_dwIndex;

public:
	static	CNaviMesh*		Create(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual CResources*		Clone(void);
	virtual void			Free(void);
};

END
#endif // NaviMesh_h__
