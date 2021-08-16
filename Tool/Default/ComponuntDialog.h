#pragma once


// CComponuntDialog 대화 상자

class CComponuntDialog : public CDialog
{
	DECLARE_DYNAMIC(CComponuntDialog)

public:
	CComponuntDialog(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CComponuntDialog();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_COM };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
public:
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()
public:
	CComboBox m_Combo;
	afx_msg void OnBnClickedComOk();
public:
	void ComboClaer();
};
