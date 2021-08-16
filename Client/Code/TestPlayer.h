#ifndef TestPlayer_h__
#define TestPlayer_h__

#include "Defines.h"
#include "GameObject.h"

BEGIN(Engine)

class CRcTex;
class CTexture;
class CTransform;
class CRenderer;
class CCalculator;

END

class CTestPlayer : public Engine::CGameObject
{
private:
	explicit CTestPlayer(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CTestPlayer(void);

public:
	void					Set_Camera(Engine::CGameObject* pCamera);
		
public:
	virtual HRESULT			Ready_GameObject(void) override;
	virtual _int			Update_GameObject(const _float& fTimeDelta) override;
	virtual void			Render_GameObject(void) override;

private:
	HRESULT					Add_Component(void);
	void					Key_Input(const _float& fTimeDelta);
	void					SetUp_OnTerrain(void);

private:
	Engine::CRcTex*			m_pBufferCom = nullptr;
	Engine::CTexture*		m_pTextureCom = nullptr;
	Engine::CTransform*		m_pTransformCom = nullptr;
	Engine::CRenderer*		m_pRendererCom = nullptr;
	Engine::CCalculator*	m_pCalculatorCom = nullptr;

	Engine::CGameObject*	m_pCamera = nullptr;
	
	_vec3					m_vDir;
	_float					m_fSpeed;

	// temp
	_vec3 m_vPos;
	_vec3 m_vTarget;

public:
	static CTestPlayer*		Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	virtual void			Free(void) override;
};

#endif // TestPlayer_h__
