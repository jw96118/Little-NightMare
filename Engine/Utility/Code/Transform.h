#ifndef Transform_h__
#define Transform_h__

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CTransform : public CComponent
{
private:
	explicit CTransform(void);
	explicit	CTransform(const CTransform& tempTrans);
	virtual ~CTransform(void);

public:
	void				Get_Info(INFO eType, _vec3* pInfo);
	const _vec3*		Get_Info(INFO eType);
	const _matrix*		Get_WorldMatrixPointer() const;
	void  Get_WorldMatrix(_matrix* pWorld) const;

	void				Move_Pos(const _vec3* const pDir);
	void				Rotation(ROTATION eType, const _float& fAngle);

	const	_matrix*	Compute_LookAtTarget(const _vec3* pTargetPos);
	
	void				Chase_Target(const _vec3* pTargetPos,
										const _float& fSpeed);

	void				Set_Scale(const _float& fX, const _float& fY, const _float& fZ);
	void				Set_Pos(const _float& fX, const _float& fY, const _float& fZ);
	void				Set_Pos(const _vec3& vPos);
	void				Set_Pos(const _vec3* pPos);
	void				Set_Angle(const _vec3& vAngle);
	void                Save_Transform(DWORD& dwBytes, const HANDLE& hFile);
	void                Load_Transform(DWORD& dwBytes, const HANDLE& hFile);
public:
	HRESULT				Ready_Component(void);
	virtual _int		Update_Component(const _float& fTimeDelta);

public:
	_vec3				m_vInfo[INFO_END];
	_vec3				m_vScale;
	_vec3				m_vAngle;
	_matrix				m_matWorld;

public:
	static CTransform*		Create(void);
	static CTransform*		Clone(CTransform* Transform);

private:
	virtual void	Free(void);

};
END
#endif // Transform_h__
