#pragma once

#include "Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CScrollTex;
class CRcTex;
class CTexture;
class CTransform;
class CRenderer;
class CCalculator;

END
class CFirstPersonCamera;

class CTitle : public Engine::CGameObject
{
public:
	//static enum SCROLLDIR{LEFT,RIGHT};
	explicit CTitle(LPDIRECT3DDEVICE9 pGraphicDev,  wstring wstrTexint,_vec2 fScale ,_vec3 vPos);
	virtual ~CTitle(void);
public:
	void		Set_Camera(Engine::CGameObject* pCamera){m_pCamera = pCamera;}
	void        Set_TexAlpha(_float fAlpha);
	void        Set_TexX(_float fTextureX);
	void        Set_Pos(_vec2 vPos);
public:
	virtual HRESULT Ready_GameObject(void) override;
	virtual HRESULT LateReady_GameObject(void) override;
	virtual _int Update_GameObject(const _float& fTimeDelta) override;
	virtual void Render_GameObject(void) override;

private:
	HRESULT		Add_Component(void);
	HRESULT     SetUp_ConstantTable(LPD3DXEFFECT& pEffect);
private:
	Engine::CRcTex*		m_pBufferCom = nullptr;
	Engine::CTexture*		m_pTextureCom = nullptr;
	Engine::CRenderer*		m_pRendererCom = nullptr;
	Engine::CCalculator*	m_pCalculatorCom = nullptr;
	Engine::CGameObject*	m_pCamera = nullptr;


	CFirstPersonCamera*      m_pCam = nullptr;
	wstring					m_wstrTexture=L"";
	_matrix					m_MatWorld;
	_matrix					m_MatView;
	_vec3					m_vPos;
	_vec2					m_vScale;
	_float					m_fScroll;
	_float					m_fSpd;
	_float                  m_fAlpha;
	_float                  m_fTextureX;

	_float					m_fX, m_fY;
	_float					m_fSizeX, m_fSizeY;
	_matrix					m_matProj;


	//SCROLLDIR				m_eSCroll;
	//_tchar					m_szScrollVal[128];//Degug

public:
	static CTitle*		Create(LPDIRECT3DDEVICE9 pGraphicDev , wstring wstrTex, _vec2 vScale, _vec3 vPos);

private:
	virtual void Free(void) override;
};

