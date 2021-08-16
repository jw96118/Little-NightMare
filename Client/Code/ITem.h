#ifndef Item_h__
#define Item_h__

#include "Defines.h"
#include "GameObject.h"

BEGIN(Engine)

class CTransform;
class CRenderer;
class CTexture;
class CLightTex;
class CCamera;
class CScene;
END

class CWeaponMgr;
class CStageMini;
class CItem : public Engine::CGameObject
{
private:
	explicit CItem(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CItem(void);

public:
	virtual HRESULT			Ready_GameObject(void) override;
	virtual HRESULT			LateReady_GameObject(void) override;
	virtual _int			Update_GameObject(const _float& fTimeDelta) override;
	virtual void			Render_GameObject(void) override;


	void					Render_Collider();
	void					Update_Collider(void);
	void                    Set_Texture();
	void                    Set_Pos(_vec3 pos);
	void                    Set_Type(Engine::ITEMTYPE type);

private:
	HRESULT					Add_Component(void);
	void                    Animation(const _float& fTimeDelta);
	void					Use_Item();
private:
	Engine::CTransform*		m_pTargetTransformCom = nullptr;
	SPHERECOLLIDER			m_tCollider;
	_float                  m_fAniCount;
	FRAME					m_tFrame;
	Engine::CScene*         m_pNowScene = nullptr;
public:
	static CItem*			Create(LPDIRECT3DDEVICE9 pGraphicDev, Engine::CScene* pNowScene );

private:
	virtual void			Free(void) override;
};

#endif // Player_h__
