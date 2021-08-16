#pragma once
#include "Export_Utility.h"
#include <iostream>
#include <fstream>
#include <string>
#include <list>
#include <string>
#include <algorithm>
#include <io.h>
#include <vector>
using namespace std;

typedef struct _finddata_t  FILE_SEARCH;



// CObjectDialog 대화 상자
BEGIN(Engine)
END
enum LOAD_STAGE
{
	STAGE1_1,
	STAGE1_2,
	STAGE1_3,
	STAGE2_1,
	STAGE2_2,
	STAGE2_3,
	STAGE_End
};
class CObjectDialog : public CDialogEx
{
	DECLARE_DYNAMIC(CObjectDialog)

public:
	CObjectDialog(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CObjectDialog();
	virtual BOOL OnInitDialog();
// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_OBJ };
#endif
public:
	int m_iObjNameCount[Engine::OBJ_END] = {};
	int m_iObjCount[Engine::OBJ_END] = {};
	//CString StaticMeshName = L"";
	//CString DynamicMeshName = L"";
	vector<CString>    m_vecTerrainFolderList;
	vector<CString>    m_vecObjectFolderList;
private:
	Engine::CScene*		    m_pScene = nullptr;
public:
	void Set_Scene(Engine::CScene* pScene);
private:
	void CreateObject(Engine::OBJECTTYPE objTag);
	void GetfileList(string path, const char * option, vector<CString>& filelist);
	HRESULT Load_Mesh(Engine::MESHTYPE meshType ,string path, CString filename);
	void Start_Load(Engine::MESHTYPE meshType, vector<CString>& filelist , string path1,string path2);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnCbnSelchangeComCombo();
	afx_msg void OnBnClickedComOk();
	afx_msg void OnBnClickedComCancel();
	CComboBox m_ObjCom;
	CComboBox m_TypeCombo;
	afx_msg void OnCbnSelchangeObjCombo2();
	CButton m_NaviButton;
	_bool   m_bLoad[STAGE_End];
	CComboBox m_ComboFolder;
	afx_msg void OnCbnSelchangeObjCombo3();
	
};
