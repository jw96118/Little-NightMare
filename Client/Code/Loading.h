#ifndef Loading_h__
#define Loading_h__

#include "Defines.h"
#include "Base.h"

#include <iostream>
#include <fstream>
#include <string>
#include <list>
#include <string>
#include <algorithm>
#include <io.h>
#include <vector>
#include "Export_Function.h"
using namespace std;

typedef struct _finddata_t  FILE_SEARCH;


class CLoading : public CBase 
{
private:
	explicit CLoading(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CLoading(void);

public:
	LOADINGID			Get_LoadingID(void) const;
	CRITICAL_SECTION*	Get_Crt(void);
	_bool				Get_Finish(void) const;
	const _tchar*		Get_LoadString(void);

public:
	static _uint		CALLBACK Thread_Main(void* pArg);

public:
	HRESULT				Ready_Loading(LOADINGID eLoading);
	_uint				Loading_ForStage(void);
private:
	void GetfileList(string path, const char * option, vector<wstring>& filelist);
	HRESULT Load_Mesh(Engine::MESHTYPE meshType, string path, wstring filename);
	HRESULT Start_Load(Engine::MESHTYPE meshType, vector<wstring>& filelist, string path1, string path2);
private:
	HANDLE				m_hThread;

	CRITICAL_SECTION	m_Crt;
	LOADINGID			m_eLoading;
	LPDIRECT3DDEVICE9	m_pGraphicDev;
	_bool				m_bFinish;
	_tchar				m_szLoading[256];

	_float *             m_pLoadCount = nullptr;


	_int count = 0;

public:
	static CLoading*	Create(LPDIRECT3DDEVICE9 pGraphicDev, LOADINGID eLoading, float* m_pLoadCount);
private:
	virtual void		Free(void);

};


#endif // Loading_h__
