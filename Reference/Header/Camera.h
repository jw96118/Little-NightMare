#ifndef Camera_h__
#define Camera_h__

#include "GameObject.h"

BEGIN(Engine)

class ENGINE_DLL CCamera : public CGameObject
{
protected:
	explicit CCamera(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CCamera(void);

public:
	const _matrix*		Get_ViewSpace(void);
	const _matrix*		Get_Projection(void);
	const _bool&		Get_IsMainCamera(void) const;

public:
	virtual HRESULT		Ready_GameObject(void) override;
	virtual _int		Update_GameObject(const _float& fTimeDelta) override;

protected:
	_vec3				m_vEye, m_vAt, m_vUp;
	_float				m_fFovY, m_fAspect, m_fNear, m_fFar;
	_matrix				m_matView, m_matProj;
	_float				m_fSpeed;
	_bool				m_bMainCamera;

protected:
	virtual void		Free(void) override;
};

END
#endif // Camera_h__
