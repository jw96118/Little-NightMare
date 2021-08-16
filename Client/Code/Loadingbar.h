#pragma once

#include "Defines.h"
#include "GameObject.h"

BEGIN(Engine)

class CRcTex;
class CTexture;
class CTransform;
class CRenderer;
class CCalculator;

END


class CLoadingBar : public Engine::CGameObject

{
public:
public:
	explicit CLoadingBar(LPDIRECT3DDEVICE9 pGraphicDev,  wstring wstrTexint, _vec3 vPos,_float fScale,_int iPivot, _float fLoad );
	virtual ~CLoadingBar(void);
public:
	void     Set_Load(const float& fLoad);
public:
	virtual HRESULT Ready_GameObject(void) override;
	virtual _int Update_GameObject(const _float& fTimeDelta) override;
	virtual void Render_GameObject(void) override;

private:
	HRESULT		Add_Component(void);
	void		CalculateTransform();
	void		SetPivot();
	void		SetScale();
	void		SetPos();
	HRESULT        SetUp_ConstantTable(LPD3DXEFFECT& pEffect);
private:
	Engine::CRcTex*			m_pBufferCom = nullptr;

	wstring					m_wstrTexture=L"";
	_matrix					m_MatWorld;
	_matrix					m_MatView;
	_vec3					m_vPos;
	_int					m_iPivot;
	_float					m_fScale;
	_float                  m_fLoad ;

public:
	static CLoadingBar*		Create(LPDIRECT3DDEVICE9 pGraphicDev , wstring wstrTex, _vec3 vPos,_float fScale=1.0f,_int iPivot = 3, _float Load = 0.f);

private:
	virtual void Free(void) override;
};

