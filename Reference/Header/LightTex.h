#ifndef LightTex_h__
#define LightTex_h__

#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CLightTex : public CVIBuffer
{
private:
	explicit CLightTex(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CLightTex(const CLightTex& rhs);
	virtual ~CLightTex(void);

public:
	virtual HRESULT		Ready_Buffer(void) override;

public:
	static CLightTex*		Create(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual CResources* Clone(void) override;

private:
	virtual void		Free(void);
};

END
#endif // RcTex_h__
