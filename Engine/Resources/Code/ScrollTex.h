#ifndef ScrollTex_h__
#define ScrollTex_h__

#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CScrollTex : public CVIBuffer
{
private:
	explicit CScrollTex(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CScrollTex(const CScrollTex& rhs);
	virtual ~CScrollTex(void);

public:
	virtual HRESULT Ready_Buffer(void) override;


public:
	static CScrollTex*		Create(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual CResources* Clone(void) override;

private:
	virtual void Free(void);
};

END
#endif // ScrollTex_h__
