#include "pch.h"

#include "Tool.h"
#include "MainFrm.h"
#include "ToolView.h"
#include "ComDialog.h"
#include "afxdialogex.h"

// CUnitTool 대화 상자입니다.

IMPLEMENT_DYNAMIC(CComDialog, CDialog)

CComDialog::CComDialog(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_DIALOG_COM, pParent)
{

}



CComDialog::~CComDialog()
{
}

void CComDialog::DoDataExchange(CDataExchange * pDX)
{
	CDialog::DoDataExchange(pDX);

}

BEGIN_MESSAGE_MAP(CComDialog, CDialog)


END_MESSAGE_MAP()

BOOL CComDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	CString wstring;
	for (int i = 0; i < 10; i++)
	{
		for (int j = 0; j < 10; j++)
		{
			wstring = to_string ( (i * 10) + j).c_str();
			m_ListBox.AddString(wstring);
		}
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다
}







