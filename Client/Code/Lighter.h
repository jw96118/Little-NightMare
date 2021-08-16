#ifndef Lighter_h__
#define Lighter_h__

#include "Defines.h"
#include "GameObject.h"

BEGIN(Engine)

class CLightTex;
class CTexture;
class CTransform;
class CRenderer;

END
class CPlayer;
class CLighter : public Engine::CGameObject
{
private:
	explicit CLighter(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CLighter(void);

public:
	virtual HRESULT Ready_GameObject(void) override;
	virtual HRESULT LateReady_GameObject(void) override;
	virtual _int Update_GameObject(const _float& fTimeDelta) override;
	virtual void Render_GameObject(void) override;
	_vec3  Get_EffectPos();
private:
	HRESULT		Add_Component(void);
	HRESULT     SetUp_ConstantTable(LPD3DXEFFECT& pEffect);
public:

private:
	_vec3					m_vDir;
	_float					m_fSpeed = 20.f;
	ID3DXLine*              m_pLine;

	_bool                   m_bPick = false;
	_float                  m_fDist = 0.f;
	_bool                   m_bDraw = false;
	CPlayer*                m_pPlayer = nullptr;
public:
	static CLighter*		Create(LPDIRECT3DDEVICE9 pGraphicDev);

	const float& Get_fDist() const;

private:
	virtual void Free(void) override;
};

#endif // Terrain_h__
