#ifndef CToolTex_h__
#define CToolTex_h__

#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CToolTex : public CVIBuffer
{
private:
	explicit CToolTex(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CToolTex(const CToolTex& rhs);
	virtual ~CToolTex(void);

public:
	virtual HRESULT Ready_Buffer(void) override;


public:
	static CToolTex*		Create(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual CResources* Clone(void) override;
private:
	virtual void Free(void);
};

END
#endif // CToolTex_h__
