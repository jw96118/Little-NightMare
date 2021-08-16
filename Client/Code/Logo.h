#ifndef Logo_h__
#define Logo_h__

#include "Defines.h"
#include "Scene.h"

#include "BackGround.h"
#include "Title.h"

#include "Loading.h"

class CLogo : public Engine::CScene
{
private:
	explicit CLogo(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CLogo(void);

public:
	virtual HRESULT Ready_Scene(void) override;
	virtual _int	Update_Scene(const _float& fTimeDelta) override;
	virtual void	Render_Scene(void) override;

private:
	HRESULT			Ready_GameLogic_Layer(const _tchar* pLayerTag);
	HRESULT			Ready_UI_Layer(const _tchar* pLayerTag);
	HRESULT			Ready_Resource(LPDIRECT3DDEVICE9& pGraphicDev, RESOURCEID eMax);

private:
	_matrix			m_matView, m_matProj;
	//CLoading*		m_pLoading;
	_float          m_fLoadingCount = 0.f;
	_bool           m_SceneChange = false;
	_bool           temp = false;
	LOGO            m_eLogo = START_GAME;
	_float          testf = 1.f;
public:
	static CLogo*	Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	virtual void	Free(void) override;
};


#endif // Logo_h__
