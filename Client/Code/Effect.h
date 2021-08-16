#ifndef Effect_h__
#define Effect_h__

#include "Defines.h"
#include "GameObject.h"

BEGIN(Engine)

class CRcTex;
class CTexture;
class CTransform;
class CRenderer;
class CShader;

END
class CPlayer;
class CJanitor;
class CLighter;
class CInteractionObject;
class CEffect : public Engine::CGameObject
{
public:
	enum TYPE {FIRE,BLOOD1,BLOOD2};
private:
	explicit CEffect(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CEffect(void);

public:
	virtual HRESULT Ready_GameObject(void) override;
	virtual HRESULT LateReady_GameObject(void) override;
	virtual _int Update_GameObject(const _float& fTimeDelta) override;
	virtual void Render_GameObject(void) override;
	
private:
	HRESULT		Add_Component(void);
	HRESULT		SetUp_ConstantTable(LPD3DXEFFECT& pEffect);

private:
	Engine::CRcTex*			m_pBufferCom = nullptr;
	Engine::CTexture*		m_pTextureCom = nullptr;


	_float					m_fFrameCnt = 0;
	_float					m_fFrameMax = 0.f;
	TYPE                    m_EffectType;
	_bool                   m_bDraw = false;
	CPlayer*                m_pPlayer = nullptr;
	CJanitor*               m_pJanitor = nullptr;
	CLighter*               m_pLighter = nullptr;
	CInteractionObject*     m_pCage = nullptr;
	_vec4                   m_vRGB = { 1.f,1.f,1.f,1.f };
	_bool                   m_bEffectEnd = false;
public:
	static CEffect*		Create(LPDIRECT3DDEVICE9 pGraphicDev, TYPE type);

private:
	virtual void Free(void) override;
};

#endif // Effect_h__
