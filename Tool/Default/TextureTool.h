#pragma once

#include "Component.h"
// CTextureTool 대화 상자
class CToolView;
class CTextureTool : public CDialogEx
{
	DECLARE_DYNAMIC(CTextureTool)

public:
	CTextureTool(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CTextureTool();
	virtual BOOL OnInitDialog();
// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_TEX };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	CStatic m_TexImg;
	CComboBox m_TexCombo;
	CListBox m_TexList;
	CButton m_BOk;
	CToolView* pToolView;

	LPD3DXSPRITE m_pSprite = nullptr;
	bool Init = false;
	Engine::CComponent* m_pTextureCom = nullptr;
	CString m_TexName;
	CString tempName;
	afx_msg void OnBnClickedComOk();
	afx_msg void OnLbnSelchangeTextureList();
	afx_msg void OnCbnSelchangeTextureCombo();
private:
	void TileRender();
public:
	afx_msg void OnBnClickedComCancel();
	virtual BOOL DestroyWindow();
	afx_msg void OnDestroy();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
};
