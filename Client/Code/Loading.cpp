#include "stdafx.h"
#include "Loading.h"

#define LOAD_MAX 28
CLoading::CLoading(LPDIRECT3DDEVICE9 pGraphicDev)
	: m_pGraphicDev(pGraphicDev)
	, m_bFinish(false)
{
	ZeroMemory(m_szLoading, sizeof(_tchar) * 256);
	m_pGraphicDev->AddRef();
}

CLoading::~CLoading(void)
{

}

LOADINGID CLoading::Get_LoadingID(void) const
{
	return m_eLoading;
}

CRITICAL_SECTION* CLoading::Get_Crt(void)
{
	return &m_Crt;
}

_bool CLoading::Get_Finish(void) const
{
	return m_bFinish;
}

const _tchar* CLoading::Get_LoadString(void)
{
	return m_szLoading;
}

_uint	CALLBACK CLoading::Thread_Main(void* pArg)
{
	CLoading*		pLoading = (CLoading*)pArg;

	_uint iFlag = 0;

	EnterCriticalSection(pLoading->Get_Crt());

	switch (pLoading->Get_LoadingID())
	{
	case LOADING_STAGE:
		iFlag = pLoading->Loading_ForStage();
		break;

	case LOADING_BOSS:
		break;
	}

	LeaveCriticalSection(pLoading->Get_Crt());
	_endthreadex(0);

	return iFlag;
}

HRESULT CLoading::Ready_Loading(LOADINGID eLoading)
{
	InitializeCriticalSection(&m_Crt);

	m_hThread = (HANDLE)_beginthreadex(NULL, 0, Thread_Main, this, 0, NULL);

	m_eLoading = eLoading;

	return S_OK;
}

_uint CLoading::Loading_ForStage(void)
{
	int i = 0;

	float temp = 100.f;
	//int i = 0;
	if (m_pLoadCount != nullptr)
	{
		// buffer
		FAILED_CHECK_RETURN(Engine::Ready_Buffer(m_pGraphicDev,
			RESOURCE_STATIC,
			L"Buffer_TerrainTex",
			Engine::BUFFER_TERRAINTEX,
			VTXCNTX,
			VTXCNTZ,
			VTXITV),
			E_FAIL);

		FAILED_CHECK_RETURN(Engine::Ready_Buffer(m_pGraphicDev,
			RESOURCE_STATIC,
			L"Buffer_TerrainCol",
			Engine::BUFFER_TERRAINCOL,
			VTXCNTX,
			VTXCNTZ,
			VTXITV),
			E_FAIL);

		FAILED_CHECK_RETURN(Engine::Ready_Buffer(m_pGraphicDev,
			RESOURCE_STATIC,
			L"Buffer_CubeTex",
			Engine::BUFFER_CUBETEX),
			E_FAIL);

		//lstrcpy(m_szLoading, L"Texture Loading.............................");

		// 텍스쳐
		FAILED_CHECK_RETURN(Engine::Ready_Texture(m_pGraphicDev,
			RESOURCE_STAGE,
			L"Texture_Effect_Blood",
			Engine::TEX_NORMAL,
			L"../Bin/Resource/LittleNightMare/Resources/Textures/Blood/Blood_0%d.png", 32),
			E_FAIL);
		*m_pLoadCount += temp / LOAD_MAX;

		FAILED_CHECK_RETURN(Engine::Ready_Texture(m_pGraphicDev,
			RESOURCE_STAGE,
			L"Texture_Effect_Fire",
			Engine::TEX_NORMAL,
			L"../Bin/Resource/LittleNightMare/Resources/Textures/Fire/TEX_LighterFlame.tga", 1),
			E_FAIL);
		*m_pLoadCount += temp / LOAD_MAX;



		FAILED_CHECK_RETURN(Engine::Ready_Meshes(m_pGraphicDev,
			RESOURCE_STATIC,
			L"Mesh_Player",
			Engine::TYPE_DYNAMIC,
			L"../Bin/Resource/Mesh/DynamicMesh/Player/Six/",
			L"Six.X"),
			E_FAIL);
		*m_pLoadCount += temp / LOAD_MAX;
		FAILED_CHECK_RETURN(Engine::Ready_Meshes(m_pGraphicDev,
			RESOURCE_STATIC,
			L"Mesh_Lighter",
			Engine::TYPE_DYNAMIC,
			L"../Bin/Resource/Mesh/DynamicMesh/Player_Lighter/",
			L"Lighter.X"),
			E_FAIL);
		*m_pLoadCount += temp / LOAD_MAX;
		FAILED_CHECK_RETURN(Engine::Ready_Meshes(m_pGraphicDev,
			RESOURCE_STATIC,
			L"Mesh_LighterFrame",
			Engine::TYPE_STATIC,
			L"../Bin/Resource/Mesh/DynamicMesh/Player_LighterFlame/",
			L"LighterFlame.X"),
			E_FAIL);
		*m_pLoadCount += temp / LOAD_MAX;

		FAILED_CHECK_RETURN(Engine::Ready_Meshes(m_pGraphicDev,
			RESOURCE_STATIC,
			L"Mesh_Leech",
			Engine::TYPE_DYNAMIC,
			L"../Bin/Resource/Mesh/DynamicMesh/Leech/",
			L"Leech.X"),
			E_FAIL);
		*m_pLoadCount += temp / LOAD_MAX;
		FAILED_CHECK_RETURN(Engine::Ready_Meshes(m_pGraphicDev,
			RESOURCE_STATIC,
			L"Mesh_Janitor",
			Engine::TYPE_DYNAMIC,
			L"../Bin/Resource/Mesh/DynamicMesh/Janitor/",
			L"Janitor.X"),
			E_FAIL);
		*m_pLoadCount += temp / LOAD_MAX;
		FAILED_CHECK_RETURN(Engine::Ready_Meshes(m_pGraphicDev,
			RESOURCE_STATIC,
			L"Mesh_Janitor_Arm",
			Engine::TYPE_DYNAMIC,
			L"../Bin/Resource/Mesh/DynamicMesh/Janitor_Arm/",
			L"Janitor_Arm.X"),
			E_FAIL);
		*m_pLoadCount += temp / LOAD_MAX;

		//lstrcpy(m_szLoading, L"Loading Complete!!!");
		//lstrcpy(m_szLoading, L"스테이지 로딩.............................");
		vector<wstring>   vecFolderList;
		Start_Load(Engine::TYPE_STATIC, vecFolderList, "../../Client/Bin/Resource/Mesh/StaticMesh/Stage",
			"../../Client/Bin/Resource/Mesh/StaticMesh/Stage/");
		Start_Load(Engine::TYPE_STATIC, vecFolderList, "../../Client/Bin/Resource/Mesh/Object",
			"../../Client/Bin/Resource/Mesh/Object/");

		FAILED_CHECK_RETURN(Engine::Ready_Meshes(m_pGraphicDev,
			RESOURCE_STAGE,
			L"Mesh_Navi",
			Engine::TYPE_NAVI,
			NULL,
			NULL),
			E_FAIL);


		//lstrcpy(m_szLoading, L"로딩 끝.............................");

		m_bFinish = true;
	}


	return 0;
}

CLoading* CLoading::Create(LPDIRECT3DDEVICE9 pGraphicDev, LOADINGID eLoading, float* pLoadCount)
{
	CLoading*	pInstance = new CLoading(pGraphicDev);
	pInstance->m_pLoadCount = pLoadCount;

	if (FAILED(pInstance->Ready_Loading(eLoading)))
		Safe_Release(pInstance);
	return pInstance;

}

void CLoading::Free(void)
{
	WaitForSingleObject(m_hThread, INFINITE);
	CloseHandle(m_hThread);
	DeleteCriticalSection(&m_Crt);

	Engine::Safe_Release(m_pGraphicDev);
}

void CLoading::GetfileList(string path, const char * option, vector<wstring>& filelist)
{

	intptr_t h_file;
	char search_Path[100];

	FILE_SEARCH file_search;


	sprintf_s(search_Path, option, path.c_str());

	if ((h_file = _findfirst(search_Path, &file_search)) == -1L)
	{
		printf("No files in current directory!\n");

	}
	else
	{
		while (true)
		{
			string tempstr = file_search.name;
			if (tempstr != "." && tempstr != "..")
			{

				wstring temp;
				temp.assign(tempstr.begin(), tempstr.end());
				filelist.push_back(temp);
			}

			if (_findnext(h_file, &file_search) != 0)
			{
				break;
			}
		}


		_findclose(h_file);
	}


}

HRESULT CLoading::Load_Mesh(Engine::MESHTYPE meshType, string path, wstring filename)
{

	wstring cFilePath;
	wstring cFileName;
	cFileName = filename.substr(0, filename.length() - 2);
	cFilePath.assign(path.begin(), path.end());
	if (FAILED(Engine::Ready_Meshes(m_pGraphicDev,
		RESOURCE_STATIC,
		cFileName.c_str(),
		meshType,
		cFilePath.c_str(),
		filename.c_str())))
	{
		return E_FAIL;
	}
	*m_pLoadCount += 100.f / LOAD_MAX;


	return S_OK;
}

HRESULT CLoading::Start_Load(Engine::MESHTYPE meshType, vector<wstring>& filelist, string path1, string path2)
{
	vector<wstring> tempString;
	if (filelist.size() <= 0)
	{
		GetfileList(path1, "%s/*", filelist);
	}


	if (filelist.size() > 0)
	{
		for (int i = 0; i < filelist.size(); i++)
		{
			string tempPath;
			tempPath.assign(filelist[i].begin(), filelist[i].end());
			string tempdynamic = tempPath;
			tempPath = path2 + tempPath + "/";
			GetfileList(tempPath, "%s*.x", tempString);

			for (int j = 0; j < tempString.size(); j++)
			{
				if (tempdynamic.find("Dynamic") != string::npos)
				{
					if(FAILED(Load_Mesh(Engine::TYPE_DYNAMIC, tempPath, tempString[j])))
					return E_FAIL;
				}
				else
				{
					if (FAILED(Load_Mesh(meshType, tempPath, tempString[j])))
					{
						return E_FAIL;
					}
				}
			}
			tempString.clear();
		}
		filelist.clear();
	}


}
