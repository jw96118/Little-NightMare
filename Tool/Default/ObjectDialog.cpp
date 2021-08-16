// ObjectDialog.cpp: 구현 파일
//

#include "pch.h"
#include "Tool.h"
#include "MainFrm.h"
#include "ObjectDialog.h"
#include "afxdialogex.h"
#include "ToolView.h"
#include "TestStage.h"
#include "Export_Function.h"
// CObjectDialog 대화 상자

IMPLEMENT_DYNAMIC(CObjectDialog, CDialogEx)

CObjectDialog::CObjectDialog(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_OBJ, pParent)
{

}

CObjectDialog::~CObjectDialog()
{
}

BOOL CObjectDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	CString wstring;
	m_ObjCom.AddString(L"Stage1");
	m_ObjCom.AddString(L"Object");
	m_ObjCom.AddString(L"Player");
	m_ObjCom.AddString(L"Monster");
	m_ObjCom.AddString(L"Col");
	//Start_Load(Engine::TYPE_STATIC, m_vecTerrainFolderList, "../../Client/Bin/Resource/Mesh/StaticMesh/Terrain",
	//	"../../Client/Bin/Resource/Mesh/StaticMesh/Terrain/");
	Start_Load(Engine::TYPE_STATIC, m_vecTerrainFolderList, "../../Client/Bin/Resource/Mesh/StaticMesh/Stage",
		"../../Client/Bin/Resource/Mesh/StaticMesh/Stage/");
	Start_Load(Engine::TYPE_STATIC, m_vecObjectFolderList, "../../Client/Bin/Resource/Mesh/Object",
		"../../Client/Bin/Resource/Mesh/Object/");
	//Start_Load(Engine::TYPE_STATIC, m_vecWallFolderList, "../../Client/Bin/Resource/Mesh/StaticMesh/Stage2",
		//"../../Client/Bin/Resource/Mesh/StaticMesh/Stage2/");

    /*vector<CString> tempString;
	if (m_vecTerrainFolderList.size() <= 0)
	{
		GetfileList("../../Client/Bin/Resource/Mesh/StaticMesh/Terrain", "%s/*", m_vecTerrainFolderList);
	}
	if (m_vecWallFolderList.size() <= 0)
	{
		GetfileList("../../Client/Bin/Resource/Mesh/StaticMesh/Wall", "%s/*", m_vecWallFolderList);
	}
	

	if (m_vecTerrainFolderList.size() > 0)
	{
		for (int i = 0; i < m_vecTerrainFolderList.size(); i++)
		{
			string tempPath = CT2CA(m_vecTerrainFolderList[i].operator LPCWSTR());
			tempPath = "../../Client/Bin/Resource/Mesh/StaticMesh/Terrain/" + tempPath + "/";
			GetfileList(tempPath, "%s*.x", tempString);

			for (int j = 0; j < tempString.size(); j++)
			{
				Load_Mesh(Engine::TYPE_STATIC, tempPath, tempString[j]);
			}
			tempString.clear();
		}
	}*/
	return TRUE;  // return TRUE unless you set the focus to a control
}

void CObjectDialog::Set_Scene(Engine::CScene * pScene)
{
	m_pScene = pScene;
}

void CObjectDialog::CreateObject(Engine::OBJECTTYPE objTag)
{
	CString str = L"";
	CString strMonster = L"";
	CString MeshName = L"";
	bool tempAttribute = false;
	bool tempItemFlag = false;
	bool tempColflag = false;
	Engine::MESHTYPE eMesh_Type;
	switch (objTag)
	{
	case Engine::OBJ_TERRAIN:
		str = L"Sta_Terrain";
		eMesh_Type = Engine::TYPE_STATIC;
		break;
	case Engine::OBJ_WALL:
		if (m_ComboFolder.GetCurSel() == 0)
		{
			str = L"Dyn_Obj";
			eMesh_Type = Engine::TYPE_DYNAMIC;
		}
		else
		{
			str = L"Sta_Obj";
			eMesh_Type = Engine::TYPE_STATIC;
		}
		tempAttribute = true;
		break;
	case Engine::OBJ_PLAYER:
		str = L"Dyn_Player";
		MeshName = L"Mesh_Player";
		eMesh_Type = Engine::TYPE_DYNAMIC;
		break;
	case Engine::OBJ_MONSTER:
		str = L"Dyn_Monster";
		eMesh_Type = Engine::TYPE_DYNAMIC;
		break;
	case Engine::OBJ_COL:
		str = L"Col";
		tempAttribute = true;
		tempColflag = true;
		break;
	case Engine::OBJ_END:
		break;

	}

	int coltype = 0;
	CString strCount;
	strCount.Format(_T("%d"), m_iObjNameCount[objTag]);
	CString strName;
	strName = str;
	switch (objTag)
	{
	case Engine::OBJ_TERRAIN:
		m_TypeCombo.GetLBText(m_TypeCombo.GetCurSel(), MeshName);
		MeshName = MeshName.Left(MeshName.GetLength() - 2);
		str = str + "_" + MeshName + "_" + strCount;
		break;
	case Engine::OBJ_WALL:
		m_TypeCombo.GetLBText(m_TypeCombo.GetCurSel(), MeshName);
		MeshName = MeshName.Left(MeshName.GetLength() - 2);
		str = str + "_" + MeshName + "_" + strCount;
		break;
	case Engine::OBJ_PLAYER:
		break;
	case Engine::OBJ_MONSTER:
		m_TypeCombo.GetLBText(m_TypeCombo.GetCurSel(), MeshName);
		str = str + "_" + MeshName + "_" + strCount;
		break;
	case Engine::OBJ_COL:
		m_TypeCombo.GetLBText(m_TypeCombo.GetCurSel(), MeshName);
	
		if (1 == m_TypeCombo.GetCurSel())
			coltype = 2;
		else
			coltype = m_TypeCombo.GetCurSel();
		str = str + "_" + MeshName + "_" + strCount;

		break;
	case Engine::OBJ_END:
		break;
	}
	CMainFrame* pMain = (CMainFrame*)AfxGetMainWnd();
	_bool CreateSuccess = false;
	if (tempAttribute)
	{
		if (m_TypeCombo.GetCurSel() < 0 )
			CreateSuccess = false;
		else
			CreateSuccess = true;
	}
	else
		CreateSuccess = true;
	if (CreateSuccess)
	{
		if (!tempColflag)
		{
			if (!FAILED(dynamic_cast<CTestStage*>(m_pScene)->Add_GameObject_Mesh(objTag, eMesh_Type, MeshName, str, strName)))
			{
				pMain->m_wndClassView.UpdateMeshList(objTag, str);

				m_iObjCount[objTag]++;
				m_iObjNameCount[objTag]++;
				cout << "staticMesh 생성" << endl;
			}
		}
		else
		{
			if (!FAILED(dynamic_cast<CTestStage*>(m_pScene)->Add_ColObject(str,Engine::COLLTYPE(coltype))))
			{
				pMain->m_wndClassView.UpdateMeshList(objTag, str);

				m_iObjCount[objTag]++;
				m_iObjNameCount[objTag]++;
				cout << "충돌 오브젝트 생성" << endl;
			}

		}
	}
	else
	{
		AfxMessageBox(L"속성을 확인해주세요");
	}
}

void CObjectDialog::GetfileList(string path , const char * option,vector<CString>& filelist)
{

	intptr_t  h_file;
	char search_Path[100];

	FILE_SEARCH file_search;

	int            testCount1 = 0;
	int            testCount2 = 0;
	sprintf_s(search_Path, option, path.c_str());
	//path = path + option;
//	strcpy_s(search_Path, path.c_str());
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
				//printf("%s\n", file_search.name);
				
				CString temp; 
				temp = tempstr.c_str();
				filelist.push_back(temp);
			}
			else
			{
				testCount1++;
			}
			if (_findnext(h_file, &file_search) != 0)
			{

				break;
			}
		}


		_findclose(h_file);
	}

}

HRESULT CObjectDialog::Load_Mesh(Engine::MESHTYPE meshType,string path,CString filename)
{
	CMainFrame* pMain = (CMainFrame*)AfxGetMainWnd();
	CToolView* pToolView = pToolView = (CToolView*)((CMainFrame*)(AfxGetApp()->m_pMainWnd))->GetActiveView();

	CString cFilePath;
	CString cFileName;
	cFileName = filename.Left(filename.GetLength()-2);
	cFilePath = path.c_str();
	if (FAILED(Engine::Ready_Meshes(pToolView->m_pDevice,
		RESOURCE_STATIC,
		cFileName,
		meshType,
		cFilePath,
		filename)))
	{
		return E_FAIL;
	}



	return S_OK;
}

void CObjectDialog::Start_Load(Engine::MESHTYPE meshType,vector<CString>& filelist, string path1, string path2)
{
	vector<CString> tempString;
	if (filelist.size() <= 0)
	{
		GetfileList(path1, "%s/*", filelist);
	}


	if (filelist.size() > 0)
	{
		for (int i = 0; i < filelist.size(); i++)
		{
			string tempPath = CT2CA(filelist[i].operator LPCWSTR());
			string tempdynamic = tempPath;
			tempPath = path2 + tempPath + "/";
			GetfileList(tempPath, "%s*.x", tempString);

			for (int j = 0; j < tempString.size(); j++)
			{
				if(tempdynamic.find("Dynamic") != string::npos)
					Load_Mesh(Engine::TYPE_DYNAMIC, tempPath, tempString[j]);
				else
					Load_Mesh(meshType, tempPath, tempString[j]);
			}
			tempString.clear();
		}
	}


}

void CObjectDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_OBJ_COMBO, m_ObjCom);
	DDX_Control(pDX, IDC_OBJ_COMBO2, m_TypeCombo);
	DDX_Control(pDX, IDC_CHECK_NAVI, m_NaviButton);
	DDX_Control(pDX, IDC_OBJ_COMBO3, m_ComboFolder);
}


BEGIN_MESSAGE_MAP(CObjectDialog, CDialogEx)
	ON_CBN_SELCHANGE(IDC_OBJ_COMBO, &CObjectDialog::OnCbnSelchangeComCombo)
	ON_BN_CLICKED(ID_OBJ_OK, &CObjectDialog::OnBnClickedComOk)
	ON_BN_CLICKED(ID_OBJ_CANCEL, &CObjectDialog::OnBnClickedComCancel)
	ON_CBN_SELCHANGE(IDC_OBJ_COMBO2, &CObjectDialog::OnCbnSelchangeObjCombo2)
	ON_CBN_SELCHANGE(IDC_OBJ_COMBO3, &CObjectDialog::OnCbnSelchangeObjCombo3)
END_MESSAGE_MAP()


// CObjectDialog 메시지 처리기


void CObjectDialog::OnCbnSelchangeComCombo()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	switch (m_ObjCom.GetCurSel())
	{
	case Engine::OBJ_TERRAIN:
		m_TypeCombo.ResetContent();
		m_ComboFolder.ResetContent();
		if (m_vecTerrainFolderList.size() > 0)
		{
			for (int i = 0; i < m_vecTerrainFolderList.size(); i++)
			{
				m_ComboFolder.AddString(m_vecTerrainFolderList[i]);
			}
		}
		break;
	case Engine::OBJ_WALL:
		m_TypeCombo.ResetContent();
		m_ComboFolder.ResetContent();
		if (m_vecObjectFolderList.size() > 0)
		{
			for (int i = 0; i < m_vecObjectFolderList.size(); i++)
			{
				m_ComboFolder.AddString(m_vecObjectFolderList[i]);
			}
		}
		break;
	case Engine::OBJ_PLAYER:
		m_TypeCombo.ResetContent();
		m_ComboFolder.ResetContent();
		break;
	case Engine::OBJ_MONSTER:
		m_TypeCombo.ResetContent();
		m_ComboFolder.ResetContent();
		m_TypeCombo.AddString(L"Mesh_Leech");
		m_TypeCombo.AddString(L"Mesh_Janitor");
		m_TypeCombo.AddString(L"Mesh_Janitor_Arm");
		break;
	case Engine::OBJ_COL:
		m_TypeCombo.ResetContent();
		m_ComboFolder.ResetContent();
		m_TypeCombo.AddString(L"Col");
		m_TypeCombo.AddString(L"Trigger");
		break;
	}
}


void CObjectDialog::OnBnClickedComOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame* pMain = (CMainFrame*)AfxGetMainWnd();
	CToolView* pToolView = nullptr;	
	Engine::CGameObject* Object = nullptr;
	switch (m_ObjCom.GetCurSel())
	{
	case Engine::OBJ_TERRAIN:
		break;

	case Engine::OBJ_PLAYER:
		 pToolView = (CToolView*)((CMainFrame*)(AfxGetApp()->m_pMainWnd))->GetActiveView();
		if (pToolView == nullptr)
		{
			return;
		}
		Object = pToolView->GetSceneObj(L"Obj_Player");
		if (Object == nullptr)
		{
			break;
		}
		else
		{
			AfxMessageBox(L"이미 있는 오브젝트입니다");
			return;
		}
		break;
	case Engine::OBJ_MONSTER:
		break;

	}
	CreateObject((Engine::OBJECTTYPE)m_ObjCom.GetCurSel());

}


void CObjectDialog::OnBnClickedComCancel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_ObjCom.Clear();
	CToolView* pToolView = (CToolView*)((CMainFrame*)(AfxGetApp()->m_pMainWnd))->GetActiveView();
	pToolView->m_DObjEnable = false;
	//this->DestroyWindow();
}


void CObjectDialog::OnCbnSelchangeObjCombo2()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}


void CObjectDialog::OnCbnSelchangeObjCombo3()//폴더 변경
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	vector<CString> tempString;
	switch (m_ObjCom.GetCurSel())
	{
	case Engine::OBJ_TERRAIN:
		m_TypeCombo.ResetContent();
		if (m_vecTerrainFolderList.size() > 0)
		{
			string tempPath = CT2CA(m_vecTerrainFolderList[m_ComboFolder.GetCurSel()].operator LPCWSTR());
			tempPath = "../../Client/Bin/Resource/Mesh/StaticMesh/Stage/" + tempPath + "/";
			GetfileList(tempPath, "%s*.x", tempString);

			for (int i = 0; i < tempString.size(); i++)
			{
				m_TypeCombo.AddString(tempString[i]);
			}

		}
		break;
	case Engine::OBJ_WALL:
		m_TypeCombo.ResetContent();
		if (m_vecObjectFolderList.size() > 0)
		{
			string tempPath = CT2CA(m_vecObjectFolderList[m_ComboFolder.GetCurSel()].operator LPCWSTR());
			tempPath = "../../Client/Bin/Resource/Mesh/Object/" + tempPath + "/";
			GetfileList(tempPath, "%s*.x", tempString);

			for (int i = 0; i < tempString.size(); i++)
			{
				m_TypeCombo.AddString(tempString[i]);
			}

		}
		break;
	case Engine::OBJ_PLAYER:
		m_TypeCombo.ResetContent();
		break;
	case Engine::OBJ_MONSTER:
		m_TypeCombo.ResetContent();
		break;
	case Engine::OBJ_COL:
		m_TypeCombo.ResetContent();
		break;
	}
}
