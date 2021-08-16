#ifndef MainApp_h__
#define MainApp_h__

#include "Defines.h"
#include "Base.h"
#include "Engine_Defines.h"
#include "Export_Function.h"

BEGIN(Engine)

class CGraphicDev;
class CManagement;

END
class CMainApp : public CBase
{
private:	// 생성자, 소멸자
	explicit CMainApp(void);
	virtual ~CMainApp(void);

	// public 함수, 변수
public:
	HRESULT				Ready_MainApp(void);
	_int				Update_MainApp(const _float& fTimeDelta);
	_int				LateUpdate_MainApp(const _float& fTimeDelta);
	void				Render_MainApp(void);

	// protected 함수, 변수
protected:

	// private 함수, 변수
private:
	HRESULT				SetUp_DefaultSetting(LPDIRECT3DDEVICE9* ppGraphicDev);

	HRESULT				Ready_Scene(LPDIRECT3DDEVICE9& pGraphicDev, 
									Engine::CManagement** ppManagement);

private:
	Engine::CGraphicDev*		m_pDeviceClass;
	Engine::CManagement*		m_pManagement;
	LPDIRECT3DDEVICE9			m_pGraphicDev;

	_int						m_iEvent;

	_bool						m_bCursor;
public:		// 생성 함수
	static CMainApp*	Create(void);

private: 	// 소멸 함수
	virtual void		Free(void);


};

#endif // MainApp_h__

