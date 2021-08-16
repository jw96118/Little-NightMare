#ifndef DynamicCamera_h__
#define DynamicCamera_h__

#include "Defines.h"
#include "Camera.h"

BEGIN(Engine)

END

class CDynamicCamera : public Engine::CCamera
{
private:
	explicit CDynamicCamera(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CDynamicCamera(void);

public:
	HRESULT						Ready_GameObject(const _vec3* pEye,
												 const _vec3* pAt,
												 const _vec3* pUp,
												 const _float& fFovY,
												 const _float& fAspect,
												 const _float& fNear,
												 const _float& fFar,
												 const _bool& bMainCamera);
	virtual                     HRESULT LateReady_GameObject();
	virtual						_int Update_GameObject(const _float& fTimeDelta) override;

private:
	void						Key_Input(const _float& fTimeDelta);
	void						Mouse_Move(const _float& fTimeDelta);
	void						Mouse_Fix(void);

	//My
	void Key_Inputs(const _float& fTimeDelta);

private :
	_bool						m_bClick;
	_bool						m_bMouseFix;

public:
	static CDynamicCamera*		Create(LPDIRECT3DDEVICE9 pGraphicDev,
									   const _vec3* pEye,
									   const _vec3* pAt,
									   const _vec3* pUp,
									   const _float& fFovY,
									   const _float& fAspect,
									   const _float& fNear,
									   const _float& fFar,
									   const _bool& bMainCamera);

private:
	virtual void				Free(void) override;

};

#endif // DynamicCamera_h__
