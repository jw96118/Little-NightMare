// ComponuntDialog.cpp: 구현 파일
//

#include "pch.h"
#include "Tool.h"
#include "ComponuntDialog.h"
#include "afxdialogex.h"
#include "MainFrm.h"
#include <windows.h>
#include "ToolView.h"
#include "GameObject.h"
// CComponuntDialog 대화 상자

IMPLEMENT_DYNAMIC(CComponuntDialog, CDialog)

CComponuntDialog::CComponuntDialog(CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_DIALOG_COM, pParent)
{

}

CComponuntDialog::~CComponuntDialog()
{
}

void CComponuntDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COM_COMBO, m_Combo);
}


BEGIN_MESSAGE_MAP(CComponuntDialog, CDialog)
	ON_BN_CLICKED(ID_COM_OK, &CComponuntDialog::OnBnClickedComOk)
END_MESSAGE_MAP()

BOOL CComponuntDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	CString wstring = L"Com_Texture";

	m_Combo.AddString(wstring);
		
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다
}
// CComponuntDialog 메시지 처리기


void CComponuntDialog::OnBnClickedComOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame* pMain = (CMainFrame*)AfxGetMainWnd();

	CString tempStr = pMain->m_wndClassView.m_wndClassView.GetItemText(pMain->m_wndClassView.m_wndClassView.GetSelectedItem());

	if (tempStr.Left(3) == L"Obj")
	{
		CToolView* pToolView = (CToolView*)((CMainFrame*)(AfxGetApp()->m_pMainWnd))->GetActiveView();
		if (pToolView == nullptr)
		{
			return;
		}
		Engine::CGameObject* Object = pToolView->GetSceneObj(tempStr);
		if (Object == nullptr)
		{
			return;
		}
		HRESULT hr = Object->Insert_Component(m_Combo.GetCurSel());
		if (FAILED(hr))
		{
			AfxMessageBox(L"이미 있는 컴포넌트입니다");
			return;
		}
		CMainFrame* pMain = (CMainFrame*)AfxGetMainWnd();
		pMain->m_wndProperties.SetObjectInfo(Object);
	}
	else
	{
		AfxMessageBox(L"오브젝트가 선택되지 않음");
		return;
	}
}

void CComponuntDialog::ComboClaer()
{
	m_Combo.Clear();
}
