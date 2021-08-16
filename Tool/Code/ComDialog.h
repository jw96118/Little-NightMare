#pragma once
#include <afxwin.h>
class CComDialog :
	public CDialog
{
	DECLARE_DYNAMIC(CComDialog)

public:
	CComDialog(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CComDialog();

	// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum {
		IDD = IDD_DIALOG_COM
};
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
	
public:
	virtual BOOL OnInitDialog();
public:
	const int& GetTile() const;
	const float& GetAngle() const;
	const float& GetChangeX() const;
	const float& GetChangeY() const;
	const RECT* GetCol() const;
	afx_msg void OnLbnSelchangeTile();
	afx_msg void OnBnClickedSave();
	afx_msg void OnBnClickedLoad();
private:
	void TileRender();
public: // Control Variables



};

