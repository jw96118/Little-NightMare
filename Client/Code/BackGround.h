#ifndef BackGround_h__
#define BackGround_h__

#include "Defines.h"
#include "GameObject.h"

BEGIN(Engine)

class CRcTex;
class CLightTex;
class CTexture;
class CTransform;
class CRenderer;

END

class CBackGround : public Engine::CGameObject
{
private:
	explicit CBackGround(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CBackGround(void);

public:
	virtual HRESULT			Ready_GameObject(void) override;
	virtual _int			Update_GameObject(const _float& fTimeDelta) override;
	virtual void			Render_GameObject(void) override;

private:
	HRESULT					Add_Component(void);

private:
	Engine::CRcTex*			m_pBufferCom;
	Engine::CLightTex*		m_pBufferLightCom;
	Engine::CTexture*		m_pTextureCom;
	Engine::CTransform*		m_pTransformCom;
	Engine::CRenderer*		m_pRendererCom;

	_float					m_fFrameCnt;
	_float					m_fFrameMax;

	_float					m_fWidth;
	_float					m_fHeight;
	
public:
	static CBackGround*		Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	virtual void			Free(void) override;
};

#endif // BackGround_h__
