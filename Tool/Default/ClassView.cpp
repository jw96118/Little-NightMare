
#include "pch.h"
#include "framework.h"
#include "MainFrm.h"
#include "ClassView.h"
#include "Resource.h"
#include "Tool.h"
#include "TestStage.h"
#include "Export_Utility.h"
class CClassViewMenuButton : public CMFCToolBarMenuButton
{
	friend class CClassView;

	DECLARE_SERIAL(CClassViewMenuButton)

public:
	CClassViewMenuButton(HMENU hMenu = nullptr) noexcept : CMFCToolBarMenuButton((UINT)-1, hMenu, -1)
	{
	}

	virtual void OnDraw(CDC* pDC, const CRect& rect, CMFCToolBarImages* pImages, BOOL bHorz = TRUE,
		BOOL bCustomizeMode = FALSE, BOOL bHighlight = FALSE, BOOL bDrawBorder = TRUE, BOOL bGrayDisabledButtons = TRUE)
	{
		pImages = CMFCToolBar::GetImages();

		CAfxDrawState ds;
		pImages->PrepareDrawImage(ds);

		CMFCToolBarMenuButton::OnDraw(pDC, rect, pImages, bHorz, bCustomizeMode, bHighlight, bDrawBorder, bGrayDisabledButtons);

		pImages->EndDrawImage(ds);
	}
};

IMPLEMENT_SERIAL(CClassViewMenuButton, CMFCToolBarMenuButton, 1)

//////////////////////////////////////////////////////////////////////
// 생성/소멸
//////////////////////////////////////////////////////////////////////
CClassView::CClassView() noexcept
{
	m_nCurrSort = ID_SORTING_GROUPBYTYPE;
}

CClassView::~CClassView()
{
}

BEGIN_MESSAGE_MAP(CClassView, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_CLASS_ADD_MEMBER_FUNCTION, OnClassAddMemberFunction)
	ON_COMMAND(ID_CLASS_ADD_MEMBER_VARIABLE, OnClassAddMemberVariable)
	ON_COMMAND(ID_CLASS_DEFINITION, OnClassDefinition)
	ON_COMMAND(ID_CLASS_PROPERTIES, OnClassProperties)
	ON_COMMAND(ID_NEW_FOLDER, OnNewFolder)
	ON_WM_PAINT()
	ON_WM_SETFOCUS()
	ON_COMMAND_RANGE(ID_SORTING_GROUPBYTYPE, ID_SORTING_SORTBYACCESS, OnSort)
	ON_UPDATE_COMMAND_UI_RANGE(ID_SORTING_GROUPBYTYPE, ID_SORTING_SORTBYACCESS, OnUpdateSort)
	ON_NOTIFY(NM_DBLCLK, 2, OnDblClickTree)
	ON_NOTIFY(NM_CLICK, 2, OnClickTree)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CClassView 메시지 처리기

int CClassView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rectDummy;
	rectDummy.SetRectEmpty();

	// 뷰를 만듭니다.
	const DWORD dwViewStyle = WS_CHILD | WS_VISIBLE | TVS_HASLINES | TVS_LINESATROOT | TVS_HASBUTTONS | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;

	if (!m_wndClassView.Create(dwViewStyle, rectDummy, this, 2))
	{
		TRACE0("클래스 뷰를 만들지 못했습니다.\n");
		return -1;      // 만들지 못했습니다.
	}

	// 이미지를 로드합니다.
	m_wndToolBar.Create(this, AFX_DEFAULT_TOOLBAR_STYLE, IDR_SORT);
	m_wndToolBar.LoadToolBar(IDR_SORT, 0, 0, TRUE /* 잠금 */);

	OnChangeVisualStyle();

	m_wndToolBar.SetPaneStyle(m_wndToolBar.GetPaneStyle() | CBRS_TOOLTIPS | CBRS_FLYBY);
	m_wndToolBar.SetPaneStyle(m_wndToolBar.GetPaneStyle() & ~(CBRS_GRIPPER | CBRS_SIZE_DYNAMIC | CBRS_BORDER_TOP | CBRS_BORDER_BOTTOM | CBRS_BORDER_LEFT | CBRS_BORDER_RIGHT));

	m_wndToolBar.SetOwner(this);

	// 모든 명령은 부모 프레임이 아닌 이 컨트롤을 통해 라우팅됩니다.
	m_wndToolBar.SetRouteCommandsViaFrame(FALSE);

	CMenu menuSort;
	menuSort.LoadMenu(IDR_POPUP_SORT);

	m_wndToolBar.ReplaceButton(ID_SORT_MENU, CClassViewMenuButton(menuSort.GetSubMenu(0)->GetSafeHmenu()));

	CClassViewMenuButton* pButton = DYNAMIC_DOWNCAST(CClassViewMenuButton, m_wndToolBar.GetButton(0));

	if (pButton != nullptr)
	{
		pButton->m_bText = FALSE;
		pButton->m_bImage = TRUE;
		pButton->SetImage(GetCmdMgr()->GetCmdImage(m_nCurrSort));
		pButton->SetMessageWnd(this);
	}

	// 정적 트리 뷰 데이터를 더미 코드로 채웁니다.
	FillClassView();

	return 0;
}

void CClassView::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);
	AdjustLayout();
}

void CClassView::FillClassView()
{
	hRootTop = m_wndClassView.InsertItem(_T("생성된 오브젝트"), 0, 0);
	m_wndClassView.SetItemState(hRootTop, TVIS_BOLD, TVIS_BOLD);

	m_hTerrainClass = m_wndClassView.InsertItem(_T("Terrain"), 1, 1, hRootTop);

	m_hWallClass = m_wndClassView.InsertItem(_T("Wall"), 1, 1, hRootTop);

	hStaticMeshTop = m_wndClassView.InsertItem(_T("생성된 StaticMesh"), 0, 0);
	m_wndClassView.SetItemState(hStaticMeshTop, TVIS_BOLD, TVIS_BOLD);

	m_hBuildingClass = m_wndClassView.InsertItem(_T("Building"), 1, 1, hStaticMeshTop);

	hDynamicMeshTop = m_wndClassView.InsertItem(_T("생성된 DynamicMesh"), 0, 0);
	m_wndClassView.SetItemState(hDynamicMeshTop, TVIS_BOLD, TVIS_BOLD);

	m_hPlayerClass = m_wndClassView.InsertItem(_T("Player"), 1, 1, hDynamicMeshTop);

	m_hMonsterClass = m_wndClassView.InsertItem(_T("Monster"), 1, 1, hDynamicMeshTop);

	//m_hMonster_test1 = m_wndClassView.InsertItem(_T("임시 몬스터"), 3, 3, m_hMonsterClass);

	hColTop = m_wndClassView.InsertItem(_T("생성된 충돌 오브젝트"), 0, 0);
	m_wndClassView.SetItemState(hColTop, TVIS_BOLD, TVIS_BOLD);

	hnaviTop = m_wndClassView.InsertItem(_T("생성된 네비 메쉬"), 0, 0);
	m_wndClassView.SetItemState(hnaviTop, TVIS_BOLD, TVIS_BOLD);

	/*HTREEITEM hClass = m_wndClassView.InsertItem(_T("CFakeAboutDlg"), 1, 1, hRoot);
	m_wndClassView.InsertItem(_T("CFakeAboutDlg()"), 3, 3, hClass);*/

	//m_wndClassView.Expand(hRoot, TVE_EXPAND);

	//hClass = m_wndClassView.InsertItem(_T("CFakeApp"), 1, 1, hRoot);
	//m_wndClassView.InsertItem(_T("CFakeApp()"), 3, 3, hClass);
	//m_wndClassView.InsertItem(_T("InitInstance()"), 3, 3, hClass);
	//m_wndClassView.InsertItem(_T("OnAppAbout()"), 3, 3, hClass);

	//hClass = m_wndClassView.InsertItem(_T("CFakeAppDoc"), 1, 1, hRoot);
	//m_wndClassView.InsertItem(_T("CFakeAppDoc()"), 4, 4, hClass);
	//m_wndClassView.InsertItem(_T("~CFakeAppDoc()"), 3, 3, hClass);
	//m_wndClassView.InsertItem(_T("OnNewDocument()"), 3, 3, hClass);

	//hClass = m_wndClassView.InsertItem(_T("CFakeAppView"), 1, 1, hRoot);
	//m_wndClassView.InsertItem(_T("CFakeAppView()"), 4, 4, hClass);
	//m_wndClassView.InsertItem(_T("~CFakeAppView()"), 3, 3, hClass);
	//m_wndClassView.InsertItem(_T("GetDocument()"), 3, 3, hClass);
	//m_wndClassView.Expand(hClass, TVE_EXPAND);

	//hClass = m_wndClassView.InsertItem(_T("CFakeAppFrame"), 1, 1, hRoot);
	//m_wndClassView.InsertItem(_T("CFakeAppFrame()"), 3, 3, hClass);
	//m_wndClassView.InsertItem(_T("~CFakeAppFrame()"), 3, 3, hClass);
	//m_wndClassView.InsertItem(_T("m_wndMenuBar"), 6, 6, hClass);
	//m_wndClassView.InsertItem(_T("m_wndToolBar"), 6, 6, hClass);
	//m_wndClassView.InsertItem(_T("m_wndStatusBar"), 6, 6, hClass);

	//hClass = m_wndClassView.InsertItem(_T("Globals"), 2, 2, hRoot);
	//m_wndClassView.InsertItem(_T("theFakeApp"), 5, 5, hClass);
	m_wndClassView.Expand(hRootTop, TVE_EXPAND);
	m_wndClassView.Expand(hStaticMeshTop, TVE_EXPAND);
	m_wndClassView.Expand(hDynamicMeshTop, TVE_EXPAND);
	m_wndClassView.Expand(hColTop, TVE_EXPAND);
}

void CClassView::UpdateList(Engine::OBJECTTYPE objTag, int testTag, CString str)
{
	HTREEITEM NowItem = m_wndClassView.GetSelectedItem();

	switch (objTag)
	{
	case Engine::OBJ_TERRAIN:
		m_wndClassView.InsertItem(str, 3, 3, m_hTerrainClass);
		m_wndClassView.Expand(m_hTerrainClass, TVE_EXPAND);
		break;

	case Engine::OBJ_PLAYER:
		m_wndClassView.InsertItem(str, 3, 3, m_hPlayerClass);
		m_wndClassView.Expand(m_hPlayerClass, TVE_EXPAND);
		break;
	case Engine::OBJ_MONSTER:
		switch (testTag)
		{
		case Engine::NO_MONSTER:
			break;
		case Engine::MONSTER_BAT:
			m_wndClassView.InsertItem(str, 4, 4, m_hMonster_test1);
			m_wndClassView.Expand(m_hMonsterClass, TVE_EXPAND);
			break;

		case Engine::MONSTER_END:
			break;
		}
		break;

	case Engine::OBJ_END:
		break;
	}
}

void CClassView::UpdateMeshList(Engine::OBJECTTYPE objTag, CString str)
{

	HTREEITEM NowItem = m_wndClassView.GetSelectedItem();

	switch (objTag)
	{
	case Engine::OBJ_TERRAIN:
		m_wndClassView.InsertItem(str, 3, 3, m_hTerrainClass);
		m_wndClassView.Expand(m_hTerrainClass, TVE_EXPAND);
		break;
	case Engine::OBJ_WALL:
		m_wndClassView.InsertItem(str, 3, 3, m_hBuildingClass);
		m_wndClassView.Expand(m_hBuildingClass, TVE_EXPAND);
		break;
	case Engine::OBJ_PLAYER:
		m_wndClassView.InsertItem(str, 3, 3, m_hPlayerClass);
		m_wndClassView.Expand(m_hPlayerClass, TVE_EXPAND);
		break;
	case Engine::OBJ_MONSTER:
		m_wndClassView.InsertItem(str, 3, 3, m_hMonsterClass);
		m_wndClassView.Expand(m_hMonsterClass, TVE_EXPAND);
		break;
	case Engine::OBJ_COL:
		m_wndClassView.InsertItem(str, 1, 1, hColTop);
		m_wndClassView.Expand(hColTop, TVE_EXPAND);
		break;
	case Engine::OBJ_END:
		break;
	}

}

void CClassView::UpdateDyanmicList(Engine::OBJECTTYPE objTag, CString str)
{
}

void CClassView::DeleteList(CString str)
{
	HTREEITEM NowItem = m_wndClassView.GetSelectedItem();
	m_wndClassView.DeleteItem(NowItem);
}

void CClassView::UpdateNaviList(CString str)
{
	m_wndClassView.InsertItem(str, 1, 1, hnaviTop);
	m_wndClassView.Expand(hnaviTop, TVE_EXPAND);
}

void CClassView::DeleteNaviList(CString str)
{
	HTREEITEM NowItem = m_wndClassView.GetSelectedItem();
	m_wndClassView.DeleteItem(NowItem);
}

void CClassView::UpdateColList(CString str)
{
	m_wndClassView.InsertItem(str, 1, 1, hColTop);
	m_wndClassView.Expand(hColTop, TVE_EXPAND);
}

void CClassView::DeleteColList(CString str)
{
	HTREEITEM NowItem = m_wndClassView.GetSelectedItem();
	m_wndClassView.DeleteItem(NowItem);
}

void CClassView::OnContextMenu(CWnd* pWnd, CPoint point)
{
	CTreeCtrl* pWndTree = (CTreeCtrl*)&m_wndClassView;
	ASSERT_VALID(pWndTree);

	if (pWnd != pWndTree)
	{
		CDockablePane::OnContextMenu(pWnd, point);
		return;
	}

	if (point != CPoint(-1, -1))
	{
		// 클릭한 항목을 선택합니다.
		CPoint ptTree = point;
		pWndTree->ScreenToClient(&ptTree);

		UINT flags = 0;
		HTREEITEM hTreeItem = pWndTree->HitTest(ptTree, &flags);
		if (hTreeItem != nullptr)
		{
			pWndTree->SelectItem(hTreeItem);
		}
	}

	pWndTree->SetFocus();
	CMenu menu;
	menu.LoadMenu(IDR_POPUP_SORT);

	CMenu* pSumMenu = menu.GetSubMenu(0);

	if (AfxGetMainWnd()->IsKindOf(RUNTIME_CLASS(CMDIFrameWndEx)))
	{
		CMFCPopupMenu* pPopupMenu = new CMFCPopupMenu;

		if (!pPopupMenu->Create(this, point.x, point.y, (HMENU)pSumMenu->m_hMenu, FALSE, TRUE))
			return;

		((CMDIFrameWndEx*)AfxGetMainWnd())->OnShowPopupMenu(pPopupMenu);
		UpdateDialogControls(this, FALSE);
	}
}

void CClassView::AdjustLayout()
{
	if (GetSafeHwnd() == nullptr)
	{
		return;
	}

	CRect rectClient;
	GetClientRect(rectClient);

	int cyTlb = m_wndToolBar.CalcFixedLayout(FALSE, TRUE).cy;

	m_wndToolBar.SetWindowPos(nullptr, rectClient.left, rectClient.top, rectClient.Width(), cyTlb, SWP_NOACTIVATE | SWP_NOZORDER);
	m_wndClassView.SetWindowPos(nullptr, rectClient.left + 1, rectClient.top + cyTlb + 1, rectClient.Width() - 2, rectClient.Height() - cyTlb - 2, SWP_NOACTIVATE | SWP_NOZORDER);
}

BOOL CClassView::PreTranslateMessage(MSG* pMsg)
{
	return CDockablePane::PreTranslateMessage(pMsg);
}

void CClassView::OnSort(UINT id)
{
	if (m_nCurrSort == id)
	{
		return;
	}

	m_nCurrSort = id;

	CClassViewMenuButton* pButton = DYNAMIC_DOWNCAST(CClassViewMenuButton, m_wndToolBar.GetButton(0));

	if (pButton != nullptr)
	{
		pButton->SetImage(GetCmdMgr()->GetCmdImage(id));
		m_wndToolBar.Invalidate();
		m_wndToolBar.UpdateWindow();
	}
}

void CClassView::OnUpdateSort(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(pCmdUI->m_nID == m_nCurrSort);
}

void CClassView::OnDblClickTree(NMHDR * pNMHDR, LRESULT * pResult)
{
	HTREEITEM NowItem = m_wndClassView.GetSelectedItem();

	CString str = m_wndClassView.GetItemText(NowItem);
	CToolView* pToolView = (CToolView*)((CMainFrame*)(AfxGetApp()->m_pMainWnd))->GetActiveView();
	if (pToolView == nullptr)
	{
		return;
	}
	Engine::CGameObject* Object = pToolView->GetSceneObj(str);
	if (Object == nullptr)
	{
		return;
	}
	CMainFrame* pMain = (CMainFrame*)AfxGetMainWnd();


	BOOL chk = false;

	if (pToolView->m_DObjEnable)
	{
		chk = pToolView->m_DObj.m_NaviButton.GetCheck();
	}
	if (!chk)
	{
		pMain->m_wndProperties.SetObjectInfo(Object);
		if (preSelectObj != nullptr)
		{
			if (preSelectObj != Object)
			{
				preSelectObj->SetSelect(false);
			}
		}
		Object->SetSelect(true);
		preSelectObj = Object;
	}
	else
	{
		pMain->m_wndProperties.SetTerrainInfo(Object);
		if (preSelectObj != nullptr)
		{
			if (preSelectObj != Object)
			{
				preSelectObj->SetSelect(false);
			}
		}
		Object->SetSelect(true);
		preSelectObj = Object;
	}
	pToolView->m_DeleteBool = true;
}

void CClassView::OnClickTree(NMHDR * pNMHDR, LRESULT * pResult)
{
	//HTREEITEM NowItem = m_wndClassView.GetSelectedItem();

	//CString str = m_wndClassView.GetItemText(NowItem);
	//wcout << str << endl;
	//CToolView* pToolView = (CToolView*)((CMainFrame*)(AfxGetApp()->m_pMainWnd))->GetActiveView();
	//if (pToolView == nullptr)
	//{
	//	cout << "안됨" << endl;
	//	return;
	//}
	//Engine::CGameObject* Object = pToolView->GetSceneObj(str);
	//if (Object == nullptr)
	//{
	//	cout << "없음" << endl;
	//	return;
	//}

	//CMainFrame* pMain = (CMainFrame*)AfxGetMainWnd();
	//pMain->m_wndProperties.SetObjectInfo(Object);
}

void CClassView::OnClassAddMemberFunction()
{
	AfxMessageBox(_T("멤버 함수 추가..."));
}

void CClassView::OnClassAddMemberVariable()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
}

void CClassView::OnClassDefinition()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
}

void CClassView::OnClassProperties()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
}

void CClassView::OnNewFolder()
{
	AfxMessageBox(_T("새 폴더..."));
}

void CClassView::OnPaint()
{
	CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.

	CRect rectTree;
	m_wndClassView.GetWindowRect(rectTree);
	ScreenToClient(rectTree);

	rectTree.InflateRect(1, 1);
	dc.Draw3dRect(rectTree, ::GetSysColor(COLOR_3DSHADOW), ::GetSysColor(COLOR_3DSHADOW));
}

void CClassView::OnSetFocus(CWnd* pOldWnd)
{
	CDockablePane::OnSetFocus(pOldWnd);

	m_wndClassView.SetFocus();
}

void CClassView::OnChangeVisualStyle()
{
	m_ClassViewImages.DeleteImageList();

	UINT uiBmpId = theApp.m_bHiColorIcons ? IDB_CLASS_VIEW_24 : IDB_CLASS_VIEW;

	CBitmap bmp;
	if (!bmp.LoadBitmap(uiBmpId))
	{
		TRACE(_T("비트맵을 로드할 수 없습니다. %x\n"), uiBmpId);
		ASSERT(FALSE);
		return;
	}

	BITMAP bmpObj;
	bmp.GetBitmap(&bmpObj);

	UINT nFlags = ILC_MASK;

	nFlags |= (theApp.m_bHiColorIcons) ? ILC_COLOR24 : ILC_COLOR4;

	m_ClassViewImages.Create(16, bmpObj.bmHeight, nFlags, 0, 0);
	m_ClassViewImages.Add(&bmp, RGB(255, 0, 0));

	m_wndClassView.SetImageList(&m_ClassViewImages, TVSIL_NORMAL);

	m_wndToolBar.CleanUpLockedImages();
	m_wndToolBar.LoadBitmap(theApp.m_bHiColorIcons ? IDB_SORT_24 : IDR_SORT, 0, 0, TRUE /* 잠금 */);
}

void CClassView::ClearGameObject()
{

	m_wndClassView.DeleteItem(hRootTop);
	m_wndClassView.DeleteItem(hStaticMeshTop);
	m_wndClassView.DeleteItem(hDynamicMeshTop);

	hRootTop = m_wndClassView.InsertItem(_T("생성된 오브젝트"), 0, 0);
	m_wndClassView.SetItemState(hRootTop, TVIS_BOLD, TVIS_BOLD);

	m_hTerrainClass = m_wndClassView.InsertItem(_T("Terrain"), 1, 1, hRootTop);

	m_hWallClass = m_wndClassView.InsertItem(_T("Wall"), 1, 1, hRootTop);

	hStaticMeshTop = m_wndClassView.InsertItem(_T("생성된 StaticMesh"), 0, 0);
	m_wndClassView.SetItemState(hStaticMeshTop, TVIS_BOLD, TVIS_BOLD);

	m_hBuildingClass = m_wndClassView.InsertItem(_T("Building"), 1, 1, hStaticMeshTop);

	hDynamicMeshTop = m_wndClassView.InsertItem(_T("생성된 DynamicMesh"), 0, 0);
	m_wndClassView.SetItemState(hDynamicMeshTop, TVIS_BOLD, TVIS_BOLD);

	m_hPlayerClass = m_wndClassView.InsertItem(_T("Player"), 1, 1, hDynamicMeshTop);

	m_hMonsterClass = m_wndClassView.InsertItem(_T("Monster"), 1, 1, hDynamicMeshTop);




	m_wndClassView.Expand(hRootTop, TVE_EXPAND);
	m_wndClassView.Expand(hStaticMeshTop, TVE_EXPAND);
	m_wndClassView.Expand(hDynamicMeshTop, TVE_EXPAND);
}

void CClassView::ClearColObject()
{
	m_wndClassView.DeleteItem(hColTop);
	hColTop = m_wndClassView.InsertItem(_T("생성된 충돌 오브젝트"), 0, 0);
	m_wndClassView.SetItemState(hColTop, TVIS_BOLD, TVIS_BOLD);
}
