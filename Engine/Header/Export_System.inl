// GraphicDev
// Get
// Set
// General
HRESULT		Ready_GraphicDev(HWND hWnd, WINMODE eMode, const _uint& iSizeX, const _uint& iSizeY, Engine::CGraphicDev** ppGraphicDev)
{
	return CGraphicDev::GetInstance()->Ready_GraphicDev(hWnd, eMode, iSizeX, iSizeY, ppGraphicDev);
}
void		Render_Begin(D3DXCOLOR Color)
{
	CGraphicDev::GetInstance()->Render_Begin(Color);
}
void		Render_End(void)
{
	CGraphicDev::GetInstance()->Render_End();
}
void			Render_End(HWND hWnd)
{
	CGraphicDev::GetInstance()->Render_End(hWnd);
}
//Key
CKeyMgr*	Get_KeyMgr(void)
{
	return CKeyMgr::GetInstance();
}
// TimerMgr
// Get
void		Set_TimeDelta(const _tchar* pTimerTag)
{
	CTimerMgr::GetInstance()->Set_TimeDelta(pTimerTag);
}
// Set
_float		Get_TimeDelta(const _tchar* pTimerTag)
{
	return CTimerMgr::GetInstance()->Get_TimeDelta(pTimerTag);
}
// General
HRESULT	Ready_Timer(const _tchar* pTimerTag)
{
	return CTimerMgr::GetInstance()->Ready_Timer(pTimerTag);
}

// FrameMgr
// Get
// Set
// General
_bool		IsPermit_Call(const _tchar* pFrameTag, const _float& fTimeDelta)
{
	return CFrameMgr::GetInstance()->IsPermit_Call(pFrameTag, fTimeDelta);
}
HRESULT		Ready_Frame(const _tchar* pFrameTag, const _float& fCallLimit)
{
	return CFrameMgr::GetInstance()->Ready_Frame(pFrameTag, fCallLimit);
}

// FontMgr
// Get
// Set
// General
HRESULT	Ready_Font(LPDIRECT3DDEVICE9 pGraphicDev,
	const _tchar* pFontTag,
	const _tchar* pFontType,
	const _uint& iWidth,
	const _uint& iHeight,
	const _uint& iWeight)
{
	return CFontMgr::GetInstance()->Ready_Font(pGraphicDev, pFontTag, pFontType, iWidth, iHeight, iWeight);
}

void	Render_Font(const _tchar* pFontTag, const _tchar* pString, const _vec2* pPos, D3DXCOLOR Color)
{
	CFontMgr::GetInstance()->Render_Font(pFontTag, pString, pPos, Color);
}

// InputDev
// Get
_byte	Get_DIKeyState(_ubyte byKeyID)
{
	return CInputDev::GetInstance()->Get_DIKeyState(byKeyID);
}
_byte	Get_DIMouseState(MOUSEKEYSTATE eMouse)
{
	return CInputDev::GetInstance()->Get_DIMouseState(eMouse);
}
_byte	Get_DIMouseWheel()
{
	return CInputDev::GetInstance()->Get_DIMouseWheel();
}
_long	Get_DIMouseMove(MOUSEMOVESTATE eMouseState)
{
	return CInputDev::GetInstance()->Get_DIMouseMove(eMouseState);
}
// Set
void	Set_InputDev(void)
{
	CInputDev::GetInstance()->Set_InputDev();
}
// General
HRESULT Ready_InputDev(HINSTANCE hInst, HWND hWnd)
{
	return CInputDev::GetInstance()->Ready_InputDev(hInst, hWnd);
}

// DebugMesh
// Get
LPD3DXMESH	Get_BoxMesh(void)
{
	return CDebugMesh::GetInstance()->Get_BoxMesh();
}
LPD3DXMESH	Get_SphereMesh(void)
{
	return CDebugMesh::GetInstance()->Get_SphereMesh();
}
// Set
// General
HRESULT		Ready_DebugMesh(LPDIRECT3DDEVICE9 pGraphicDev)
{
	return CDebugMesh::GetInstance()->Ready_DebugMesh(pGraphicDev);
}
void		Render_BoxMesh(void)
{
	CDebugMesh::GetInstance()->Render_BoxMesh();
}
void		Render_SphereMesh(void)
{
	CDebugMesh::GetInstance()->Render_SphereMesh();
}

//Sound
void Ready_SoundMgr(const char* path)
{
	CSoundMgr::GetInstance()->Ready_SoundMgr(path);
}
void Update_Sound()
{
	CSoundMgr::GetInstance()->Update_Sound();
}
void Play_Sound(const wstring& wstrSoundKey, Engine::CHANNELID eID, bool& flag, bool flagLoop)
{
	if (!flag)
	{
		CSoundMgr::GetInstance()->Play_Sound(wstrSoundKey, eID, flagLoop);
		flag = true;
	}
}
void Play_BGM(const wstring& wstrSoundKey)
{
	CSoundMgr::GetInstance()->Play_BGM(wstrSoundKey);
}
void Stop_Sound(Engine::CHANNELID eID)
{
	CSoundMgr::GetInstance()->Stop_Sound(eID);
}
void Stop_All()
{
	CSoundMgr::GetInstance()->Stop_All();
}
void Set_Volume(Engine::CHANNELID eID, float fVol)
{
	CSoundMgr::GetInstance()->Set_Volume(eID, fVol);
}


// Release System
void			Release_System(void)
{
	CSoundMgr::GetInstance()->DestroyInstance();
	CKeyMgr::GetInstance()->DestroyInstance();
	CDebugMesh::GetInstance()->DestroyInstance();
	CInputDev::GetInstance()->DestroyInstance();
	CFontMgr::GetInstance()->DestroyInstance();
	CFrameMgr::GetInstance()->DestroyInstance();
	CTimerMgr::GetInstance()->DestroyInstance();
	CGraphicDev::GetInstance()->DestroyInstance();
}