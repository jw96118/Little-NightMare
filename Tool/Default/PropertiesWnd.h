 
#pragma once
#include "GameObject.h"
#include "TextureTool.h"
BEGIN(Engine)
class CGameObject;
END
class CPropertiesToolBar : public CMFCToolBar
{
public:
	virtual void OnUpdateCmdUI(CFrameWnd* /*pTarget*/, BOOL bDisableIfNoHndler)
	{
		CMFCToolBar::OnUpdateCmdUI((CFrameWnd*) GetOwner(), bDisableIfNoHndler);
	}

	virtual BOOL AllowShowOnList() const { return FALSE; }
};

class CMFCPropertyGridPropertyEx : public CMFCPropertyGridProperty
{
public:
	CMFCPropertyGridPropertyEx(const CString& strName, const COleVariant& varValue, LPCTSTR lpszDescr = NULL, BOOL bButton = TRUE, int nDlgType = 0);
	virtual ~CMFCPropertyGridPropertyEx();

public:
	int m_nDlgType;//다이얼로그 타입

public:
	virtual void OnClickButton(CPoint point);
};


class CPropertiesWnd : public CDockablePane
{
// 생성입니다.
public:
	CPropertiesWnd() noexcept;

	void AdjustLayout();

// 특성입니다.
public:
	void SetVSDotNetLook(BOOL bSet)
	{
		m_wndPropList.SetVSDotNetLook(bSet);
		m_wndPropList.SetGroupNameFullWidth(bSet);
	}
public:
	void SetObjectInfo(Engine::CGameObject* object);
	void SetTerrainInfo(Engine::CGameObject* object);
	//void Set
	void SetTextureName(CString str, bool flag);
public:
	CMFCPropertyGridProperty* m_pGroupComponunt = nullptr;
	CMFCPropertyGridProperty* m_pGroupTexture   = nullptr;
	CMFCPropertyGridProperty* m_pObjName      = nullptr;

	CMFCPropertyGridProperty* m_pNaviName = nullptr;

	CString                   m_TexName;
public:
	CTextureTool              m_TexTool;
	Engine::CGameObject*      m_pNowGameObjcet = nullptr;
	Engine::CGameObject*      m_pNowTerrainObjcet = nullptr;
	CMFCPropertyGridCtrl      m_wndPropList;

protected:
	CFont m_fntPropList;
	CComboBox m_wndObjectCombo;
	CPropertiesToolBar m_wndToolBar;

	
// 구현입니다.
public:
	virtual ~CPropertiesWnd();

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnExpandAllProperties();
	afx_msg void OnUpdateExpandAllProperties(CCmdUI* pCmdUI);
	afx_msg void OnSortProperties();
	afx_msg void OnUpdateSortProperties(CCmdUI* pCmdUI);
	afx_msg void OnProperties1();
	afx_msg void OnUpdateProperties1(CCmdUI* pCmdUI);
	afx_msg void OnProperties2();
	afx_msg void OnUpdateProperties2(CCmdUI* pCmdUI);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnSettingChange(UINT uFlags, LPCTSTR lpszSection);
	afx_msg LRESULT OnPropertyChanged(WPARAM wp, LPARAM lp);
	DECLARE_MESSAGE_MAP()

	void InitPropList();
	void SetPropListFont();

	int m_nComboHeight;
};

