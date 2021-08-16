
#include "SoundMgr.h"
USING(Engine)

IMPLEMENT_SINGLETON(CSoundMgr)

CSoundMgr::CSoundMgr()
{
}


CSoundMgr::~CSoundMgr()
{
	Release();
}

void CSoundMgr::Ready_SoundMgr(const char* path)
{
	//FMOD::System_Create(&m_pSystem);
	//m_pSystem->init(100, FMOD_INIT_NORMAL, nullptr);
	FMOD_System_Create(&m_pSystem);
	FMOD_System_Init(m_pSystem, 100, FMOD_INIT_NORMAL, nullptr);

	LoadSoundFile(path);
}

void CSoundMgr::Update_Sound()
{
	//m_pSystem->update();
	FMOD_System_Update(m_pSystem);
}

void CSoundMgr::Play_Sound(const wstring& wstrSoundKey, Engine::CHANNELID eID, _bool flag)
{
	auto iter_find = m_MapSound.find(wstrSoundKey);

	if (m_MapSound.end() == iter_find)
		return;
	//m_pSystem->playSound(iter_find->second,nullptr,FALSE, &m_pChannelArr[eID]);
	FMOD_System_PlaySound(m_pSystem,  iter_find->second , nullptr,FALSE, &m_pChannelArr[eID]);
	if (flag)
	 	 FMOD_Channel_SetMode(m_pChannelArr[eID], FMOD_LOOP_NORMAL);
	Update_Sound();
}

void CSoundMgr::Play_BGM(const wstring& wstrSoundKey)
{
	auto iter_find = m_MapSound.find(wstrSoundKey);

	if (m_MapSound.end() == iter_find)
		return;
	//m_pSystem->playSound(iter_find->second, nullptr,FALSE, &m_pChannelArr[Engine::CHANNELID::BGM]);
	
	FMOD_System_PlaySound(m_pSystem, iter_find->second, nullptr, FALSE, &m_pChannelArr[Engine::CHANNELID::BGM]);
		
    FMOD_Channel_SetMode(m_pChannelArr[Engine::CHANNELID::BGM], FMOD_LOOP_NORMAL);

	Update_Sound();
}

void CSoundMgr::Stop_Sound(Engine::CHANNELID eID)
{
	FMOD_Channel_Stop(m_pChannelArr[eID]);
	//m_pChannelArr[eID]->stop();
	Update_Sound();
}

void CSoundMgr::Stop_All()
{
	for (int i = 0; i < Engine::CHANNELID::MAX_CHANNEL; ++i)
	{
		FMOD_Channel_Stop(m_pChannelArr[i]);

		Update_Sound();
	}
}

void CSoundMgr::Set_Volume(Engine::CHANNELID eID, float fVol)
{
	FMOD_Channel_SetVolume(m_pChannelArr[eID], fVol);
	//m_pChannelArr[eID]->setVolume(fVol);
	Update_Sound();
}

void CSoundMgr::Release()
{
	for_each(m_MapSound.begin(), m_MapSound.end(),
		[](auto& MyPair)
	{
		FMOD_Sound_Release(MyPair.second);
		//MyPair.second->release();
	});

	m_MapSound.clear();
	//m_pSystem->release();

	FMOD_System_Release(m_pSystem);
	FMOD_System_Close(m_pSystem);
}

void CSoundMgr::LoadSoundFile(const char* path)
{
	_finddata_t fd;
	string temp = path;
	temp += "*.*";
	intptr_t handle = _findfirst(temp.c_str(), &fd);

	if (0 == handle)
	{
		FAILED(L"_findfirst failed!", L"Failed!!");
		return;
	}

	char szFullPath[128] = "";
	char szRelativePath[128] = "";
	strcpy_s(szRelativePath, path);
	int iResult = 0;

	while (-1 != iResult)
	{
		// szFullPath: "../Sound/"
		strcpy_s(szFullPath, szRelativePath);

		// fd.name: "Adam Levine - Lost Stars Lyrics.mp3"
		// szFullPath: "../Sound/Adam Levine - Lost Stars Lyrics.mp3"
		strcat_s(szFullPath, fd.name);

		// fd.name: "Adam Levine - Lost Stars Lyrics.mp3"
		TCHAR* pSoundKey = new TCHAR[strlen(fd.name) + 1];

		// 멀티 -> 와이드로 변환.
		MultiByteToWideChar(CP_ACP, 0, fd.name, strlen(fd.name) + 1,
			pSoundKey, strlen(fd.name) + 1);

		FMOD_SOUND* pSound = nullptr;
		//FMOD_RESULT eResult = m_pSystem->createSound(szFullPath, FMOD_CREATESAMPLE, nullptr, &pSound);
		FMOD_RESULT eResult = FMOD_System_CreateSound(m_pSystem, szFullPath, FMOD_CREATESAMPLE, nullptr, &pSound);

		if (FMOD_OK == eResult)
		{
			auto& iter_find = m_MapSound.find(pSoundKey);

			if (m_MapSound.end() == iter_find)
			{
				m_MapSound.insert({ pSoundKey, pSound });
				delete[] pSoundKey;
			}
			else
			{
				delete[] pSoundKey;
				//pSound->release();
				FMOD_Sound_Release(pSound);
			}
		}
		else
			delete[] pSoundKey;

		iResult = _findnext(handle, &fd);
	}
	//m_pSystem->update();
	FMOD_System_Update(m_pSystem);
}

void CSoundMgr::Free(void)
{
}
