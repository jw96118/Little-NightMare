#ifndef LoadIngScene_h__
#define LoadIngScene_h__

#include "Defines.h"
#include "Scene.h"

#include "BackGround.h"
#include "Title.h"

#include "Loading.h"

class CLoadIngScene : public Engine::CScene
{
private:
	explicit CLoadIngScene(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CLoadIngScene(void);

public:
	virtual HRESULT Ready_Scene(void) override;
	virtual _int	Update_Scene(const _float& fTimeDelta) override;
	virtual void	Render_Scene(void) override;

private:
	HRESULT			Ready_Environment_Layer(const _tchar* pLayerTag);
	HRESULT			Ready_GameLogic_Layer(const _tchar* pLayerTag);
	HRESULT			Ready_Resource(LPDIRECT3DDEVICE9& pGraphicDev, RESOURCEID eMax);

private:
	_matrix			m_matView, m_matProj;
	CLoading*		m_pLoading;
	_float          m_fLoadingCount = 0.f;
public:
	static CLoadIngScene*	Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	virtual void	Free(void) override;
};


#endif // Logo_h__
