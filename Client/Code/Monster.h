#ifndef Monster_h__
#define Monster_h__

#include "Defines.h"
#include "GameObject.h"

BEGIN(Engine)

class CRcTex;
class CTexture;
class CTransform;
class CRenderer;
class CCalculator;

END

class CMonster : public Engine::CGameObject
{
protected:
	explicit CMonster(LPDIRECT3DDEVICE9 pGraphicDev, const _tchar* pLayerTag, wstring wstrObjName);
	virtual ~CMonster(void);

public:

public:
	void					Set_Pos(_float fX, _float fY, _float fZ);
	void					SetScale(_float fScale);
	void					Get_Damage(_float fDamage);
public:
	virtual HRESULT			Ready_GameObject(void) override;
	virtual _int			Update_GameObject(const _float& fTimeDelta) override;
	virtual void			Render_GameObject(void) override;
	virtual HRESULT			LateReady_GameObject(void);

	virtual void			Update_HitCollider(void)PURE;
	virtual void			Update_BoxCollider(void)PURE;

	virtual	HRESULT			Add_Component(void);

protected:

protected:
	void					SetUp_OnTerrain(void);
	virtual void			Change_Animation(_uint uiAnimationID) PURE;
	virtual void			Animator_GameObject(const _float& fTimeDelta) PURE;
	virtual void			Change_State(Animation::MONSTER State)PURE;
	virtual _int			Update_Pattern(_float fTimeDelta)PURE;


protected:


protected:
	Engine::CTransform*		m_pTargetTransformCom = nullptr;
	const _tchar*			m_pLayerTag;
	wstring					m_wstrObjName;
	_matrix					m_matWorld;
	_matrix					m_matView;

	_vec3					m_vTargetPos;
	_vec3					m_vPos;
	_vec3					m_vDir;

	OBJ_DATA				m_tData;


protected:
	virtual void Free(void) override;

	// Test Code
public:
	void Damage(void)
	{
		m_tData.fCurHP -= 50.f;
	}
};

#endif // Monster_h__
