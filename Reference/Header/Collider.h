#ifndef Collider_h__
#define Collider_h__

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CCollider : public CComponent
{
private:
	explicit CCollider(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CCollider(void);

public:
	const _vec3*		Get_Min(void)  { return &m_vMin; }
	const _vec3*		Get_Max(void) { return &m_vMax; }
	const _matrix*		Get_ColliderMatrix(void) { return &m_matWorld; }
	_vec3         Get_Cube(int iIndex);
public:
	HRESULT		Ready_Collider(const _vec3* pPos, const _ulong& dwNumVtx, const _ulong& dwStride,const _bool& flag);
	void		Render_Collider(COLLTYPE eType, const _matrix* pColliderMatrix);

private:
	_vec3		m_vMin, m_vMax;
	_matrix		m_matWorld;

#ifdef _DEBUG
	LPDIRECT3DDEVICE9			m_pGraphicDev;
	LPDIRECT3DVERTEXBUFFER9		m_pVB;
	LPDIRECT3DINDEXBUFFER9		m_pIB;
	LPDIRECT3DTEXTURE9			m_pTexture[COL_END];
	_vec3 		                m_pVtxCube[8];
#endif

public:
	static CCollider*		Create(LPDIRECT3DDEVICE9 pGraphicDev, 
									const _vec3* pPos, 
									const _ulong& dwNumVtx,
									const _ulong& dwStride, const _bool& flag = false);

private:
	virtual void		Free(void);
};

END
#endif // Collider_h__
