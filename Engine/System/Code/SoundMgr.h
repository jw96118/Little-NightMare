#pragma once
#ifndef _SoundMgr_h_

#define _SoundMgr_h_

// FMOD: 유니티, 언리얼과 같은 상용 엔진에도 두루 쓰이는 사운드 관련 엔진.
// 사운드에 관련된 인터페이스를 제공.

#include "fmod_studio.hpp"
#include "fmod.hpp"

#include "Engine_Defines.h"
#include "Base.h"


#pragma comment(lib, "fmod_vc.lib")
#pragma comment(lib, "fmodL_vc.lib")


#include <io.h>

BEGIN(Engine)
class ENGINE_DLL CSoundMgr : public CBase
{
	DECLARE_SINGLETON(CSoundMgr)

public:

private:
	CSoundMgr();
	~CSoundMgr();

public:
	void Ready_SoundMgr(const char* path);
	void Update_Sound();
	void Play_Sound(const wstring& wstrSoundKey, Engine::CHANNELID eID, _bool flag = false);
	void Play_BGM(const wstring& wstrSoundKey);
	void Stop_Sound(Engine::CHANNELID eID);
	void Stop_All();
	void Set_Volume(Engine::CHANNELID eID, float fVol); // 볼륨의 범위는 최소 0.f ~ 최대 1.f까지.
	void Release();

private:
	void LoadSoundFile(const char* path);

private:
	// FMOD_SOUND: 사운드 파일에 대한 정보를 갖는 구조체.
	map<wstring, FMOD_SOUND*>	m_MapSound;

	// FMOD_CHANNEL: 사운드를 재생하고 관리하는 역할. ex) 볼륨 조절 등.
	FMOD_CHANNEL*	m_pChannelArr[Engine::CHANNELID::MAX_CHANNEL];
	
	// FMOD_SYSTEM: FMOD_SOUND과 FMOD_CHANNEL을 총괄 관리하는 객체.
	FMOD_SYSTEM*	m_pSystem;

public:
	virtual void	Free(void);
};
END
#endif // !_SoundMgr_h_