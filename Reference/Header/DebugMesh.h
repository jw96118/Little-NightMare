#ifndef DebugMesh_h__
#define DebugMesh_h__

#include "Base.h"
#include "Engine_Defines.h"

BEGIN(Engine)

class ENGINE_DLL CDebugMesh : public CBase
{
	DECLARE_SINGLETON(CDebugMesh)

private:
	explicit CDebugMesh(void);
	virtual	~CDebugMesh(void);

public:
	LPD3DXMESH				Get_BoxMesh(void);
	LPD3DXMESH				Get_SphereMesh(void);

public:
	HRESULT					Ready_DebugMesh(LPDIRECT3DDEVICE9 pGraphicDev);
	void					Render_BoxMesh(void);
	void					Render_SphereMesh(void);

public:
	LPDIRECT3DDEVICE9		m_pGraphicDev;
	LPD3DXMESH				m_pBoxMesh;
	LPD3DXMESH				m_pSphereMesh;

public:
	inline	virtual void	Free(void);
};

END

#endif // DebugMesh_h__
