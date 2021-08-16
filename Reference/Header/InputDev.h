#ifndef InputDev_h__
#define InputDev_h__

#include "Engine_Defines.h"
#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CInputDev : public CBase
{
	DECLARE_SINGLETON(CInputDev)

private:
	explicit CInputDev(void);
	virtual ~CInputDev(void);

public:
	_byte		Get_DIKeyState(_ubyte byKeyID);
	_byte		Get_DIMouseState(MOUSEKEYSTATE eMouse);
	_long		Get_DIMouseMove(MOUSEMOVESTATE eMouseState);
	_byte	    Get_DIMouseWheel();

public:
	HRESULT		Ready_InputDev(HINSTANCE hInst, HWND hWnd);
	void		Set_InputDev(void);

private:
	LPDIRECTINPUT8			m_pInputSDK;

private:
	LPDIRECTINPUTDEVICE8	m_pKeyBoard;
	LPDIRECTINPUTDEVICE8	m_pMouse;

private:
	_byte					m_byKeyState[256];
	DIMOUSESTATE			m_tMouseState;

public:
	virtual void	Free(void);

};
END
#endif // InputDev_h__
