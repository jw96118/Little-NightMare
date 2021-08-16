
// ToolView.cpp: CToolView 클래스의 구현
//

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS는 미리 보기, 축소판 그림 및 검색 필터 처리기를 구현하는 ATL 프로젝트에서 정의할 수 있으며
// 해당 프로젝트와 문서 코드를 공유하도록 해 줍니다.
#ifndef SHARED_HANDLERS
#include "Tool.h"
#endif

#include "MainFrm.h"
#include "Export_System.h"
#include "Export_Function.h"
#include "Export_Utility.h"
#include "Engine_Macro.h"
#include "ToolView.h"
#include "ClassView.h"
#include "TestStage.h"
#include "ToolDoc.h"
#ifdef _DEBUG

#define new DEBUG_NEW
#endif
#define MAX_STR 256

// 전역변수
HWND g_hWnd;
HINSTANCE g_hInst;                                // 현재 인스턴스입니다.

// CToolView

IMPLEMENT_DYNCREATE(CToolView, CView)

BEGIN_MESSAGE_MAP(CToolView, CView)
	// 표준 인쇄 명령입니다.
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CToolView::OnFilePrintPreview)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
	ON_COMMAND(ID_COM_INSERT, &CToolView::InsertComponunt)
	ON_WM_LBUTTONDOWN()
	ON_COMMAND(ID_TOOL_SAVE, &CToolView::OnToolSave)
	ON_COMMAND(ID_TOOL_LOAD, &CToolView::OnToolLoad)
	ON_COMMAND(ID_OBJ_INSERT, &CToolView::OnObjInsert)
	ON_COMMAND(ID_COL_SAVE, &CToolView::OnColSave)
	ON_COMMAND(ID_COL_LOAD, &CToolView::OnColLoad)
	ON_COMMAND(ID_NAVI_SAVE, &CToolView::OnNaviSave)
	ON_COMMAND(ID_NAVI_LOAD, &CToolView::OnNaviLoad)
END_MESSAGE_MAP()

// CToolView 생성/소멸

CToolView::CToolView() noexcept
{
	// TODO: 여기에 생성 코드를 추가합니다.

}

CToolView::~CToolView()
{
	Engine::Safe_Release(m_pDevice);
	Engine::Safe_Release(m_pDeviceClass);
	Engine::Safe_Release(m_pManagement);

	Engine::Release_Resources();
	Engine::Release_Utility();
	Engine::Release_System();

}

BOOL CToolView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: CREATESTRUCT cs를 수정하여 여기에서
	//  Window 클래스 또는 스타일을 수정합니다.

	return CView::PreCreateWindow(cs);
}

// CToolView 그리기

void CToolView::OnDraw(CDC* /*pDC*/)
{
	CToolDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: 여기에 원시 데이터에 대한 그리기 코드를 추가합니다.
}


// CToolView 인쇄


void CToolView::OnFilePrintPreview()
{
#ifndef SHARED_HANDLERS
	AFXPrintPreview(this);
#endif
}

BOOL CToolView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// 기본적인 준비
	return DoPreparePrinting(pInfo);
}

void CToolView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 인쇄하기 전에 추가 초기화 작업을 추가합니다.
}

void CToolView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 인쇄 후 정리 작업을 추가합니다.
}

void CToolView::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
	ClientToScreen(&point);
	//OnContextMenu(this, point);
}

void CToolView::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}


// CToolView 진단

#ifdef _DEBUG
void CToolView::AssertValid() const
{
	CView::AssertValid();
}

void CToolView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

HRESULT CToolView::View_Update(const _float& fTimeDelta)
{
	Engine::Set_InputDev();
	m_pKeyMgr->Update();
	if (m_pManagement != nullptr)
		m_pManagement->Update_Scene(fTimeDelta);
	CMainFrame* pMain = (CMainFrame*)AfxGetMainWnd();
	if (pMain->m_bWindowActive)
	{
		if (!m_DObj.m_NaviButton.GetCheck())
		{
			if (m_pKeyMgr->KeyPressing(Engine::KEY_LCTRL) && m_pKeyMgr->KeyDown(Engine::KEY_A))
			{
				CString str = L"Col_Object";
				CString strCount;
				strCount.Format(_T("%d"), ColCount);
				str += strCount;

				//CreateColObject(str);

				pMain->m_wndClassView.UpdateColList(str);
				ColCount++;
				cout << "오브젝트 생성" << endl;
			}
			if (m_pKeyMgr->KeyPressing(Engine::KEY_LCTRL) && m_pKeyMgr->KeyDown(Engine::KEY_D))
			{
				HTREEITEM NowItem = pMain->m_wndClassView.m_wndClassView.GetSelectedItem();
				CString strItem = pMain->m_wndClassView.m_wndClassView.GetItemText(NowItem);

				if (strItem.Left(wcslen(L"Obj")) == L"Obj")
				{
					if (pMain->m_wndProperties.m_pNowGameObjcet != nullptr)
					{

						CString str = pMain->m_wndProperties.m_pNowGameObjcet->Get_ObjName().c_str();
						CString strCount;
						strCount.Format(_T("%d"), m_DObj.m_iObjNameCount[pMain->m_wndProperties.m_pNowGameObjcet->Get_ObjType()]);
						str += strCount;

						CopyGameObject(pMain->m_wndProperties.m_pNowGameObjcet, str);

						pMain->m_wndClassView.UpdateList(pMain->m_wndProperties.m_pNowGameObjcet->Get_ObjType(), pMain->m_wndProperties.m_pNowGameObjcet->Get_MonsterType(), str);

						m_DObj.m_iObjCount[pMain->m_wndProperties.m_pNowGameObjcet->Get_ObjType()]++;
						m_DObj.m_iObjNameCount[pMain->m_wndProperties.m_pNowGameObjcet->Get_ObjType()]++;

						//cout << "오브젝트 복사" << endl;
					}
				}
			}
			if (m_pKeyMgr->KeyPressing(Engine::KEY_LCTRL) && m_pKeyMgr->KeyDown(Engine::KEY_W))
			{
				HTREEITEM NowItem = pMain->m_wndClassView.m_wndClassView.GetSelectedItem();
				CString strItem = pMain->m_wndClassView.m_wndClassView.GetItemText(NowItem);

				if (strItem.Left(wcslen(L"Obj")) == L"Obj")
				{
					if (pMain->m_wndProperties.m_pNowGameObjcet != nullptr)
					{
						for (int i = 0; i < 20; i++)
						{
							//for (int j = 0; j < 31; j++)
							//{
							CString str = pMain->m_wndProperties.m_pNowGameObjcet->Get_ObjName().c_str();
							CString strCount;
							strCount.Format(_T("%d"), m_DObj.m_iObjNameCount[pMain->m_wndProperties.m_pNowGameObjcet->Get_ObjType()]);
							str += strCount;

							CopyGameObject(pMain->m_wndProperties.m_pNowGameObjcet, str, _vec3(0, 0, (i + 1) * 2));


							pMain->m_wndClassView.UpdateList(pMain->m_wndProperties.m_pNowGameObjcet->Get_ObjType(), pMain->m_wndProperties.m_pNowGameObjcet->Get_MonsterType(), str);

							m_DObj.m_iObjCount[pMain->m_wndProperties.m_pNowGameObjcet->Get_ObjType()]++;
							m_DObj.m_iObjNameCount[pMain->m_wndProperties.m_pNowGameObjcet->Get_ObjType()]++;
							//}
						}
						//cout << "오브젝트 복사" << endl;
					}
				}
			}
			if (m_pKeyMgr->KeyPressing(Engine::KEY_LCTRL) && m_pKeyMgr->KeyDown(Engine::KEY_Q))
			{
				HTREEITEM NowItem = pMain->m_wndClassView.m_wndClassView.GetSelectedItem();
				CString strItem = pMain->m_wndClassView.m_wndClassView.GetItemText(NowItem);

				if (strItem.Left(wcslen(L"Obj")) == L"Obj")
				{
					if (pMain->m_wndProperties.m_pNowGameObjcet != nullptr)
					{
						for (int i = 0; i < 20; i++)
						{
							//for (int j = 0; j < 31; j++)
							//{
							if (i % 5 != 4)
							{
								CString str = pMain->m_wndProperties.m_pNowGameObjcet->Get_ObjName().c_str();
								CString strCount;
								strCount.Format(_T("%d"), m_DObj.m_iObjNameCount[pMain->m_wndProperties.m_pNowGameObjcet->Get_ObjType()]);
								str += strCount;

								CopyGameObject(pMain->m_wndProperties.m_pNowGameObjcet, str, _vec3((i + 1) * 2, 0, 0));


								pMain->m_wndClassView.UpdateList(pMain->m_wndProperties.m_pNowGameObjcet->Get_ObjType(), pMain->m_wndProperties.m_pNowGameObjcet->Get_MonsterType(), str);

								m_DObj.m_iObjCount[pMain->m_wndProperties.m_pNowGameObjcet->Get_ObjType()]++;
								m_DObj.m_iObjNameCount[pMain->m_wndProperties.m_pNowGameObjcet->Get_ObjType()]++;
							}
							//}
						}
						//cout << "오브젝트 복사" << endl;
					}
				}
			}
			if (m_pKeyMgr->KeyDown(Engine::KEY_DELETE))
			{
				HTREEITEM NowItem = pMain->m_wndClassView.m_wndClassView.GetSelectedItem();
				CString strItem = pMain->m_wndClassView.m_wndClassView.GetItemText(NowItem);

					if (m_DeleteBool)
					{
						DeleteGameObject(strItem);
						pMain->m_wndProperties.m_pNowGameObjcet = nullptr;
						m_DeleteBool = false;

						if (pMain->m_wndProperties.m_pGroupComponunt != nullptr)
							pMain->m_wndProperties.m_wndPropList.DeleteProperty(pMain->m_wndProperties.m_pGroupComponunt);
						pMain->m_wndProperties.m_pGroupComponunt = new CMFCPropertyGridProperty(_T("Componunt"));

					}
			
			}
		}
		else
		{
			if (m_pKeyMgr->KeyPressing(Engine::KEY_LCTRL) && m_pKeyMgr->KeyDown(Engine::KEY_A))
			{
				CString str = L"Navi_Object";
				CString strCount;
				strCount.Format(_T("%d"), NaviNowCount);
				str += strCount;

				CreateNaviObject(str);

				pMain->m_wndClassView.UpdateNaviList(str);
				NaviNowCount++;
				NaviAllCount++;
				cout << "오브젝트 생성" << endl;
			}
			if (m_pKeyMgr->KeyPressing(Engine::KEY_LCTRL) && m_pKeyMgr->KeyDown(Engine::KEY_D))
			{
				HTREEITEM NowItem = pMain->m_wndClassView.m_wndClassView.GetSelectedItem();
				CString strItem = pMain->m_wndClassView.m_wndClassView.GetItemText(NowItem);

				if (strItem.Find(L"Navi") != -1)
				{
					if (pMain->m_wndProperties.m_pNowTerrainObjcet != nullptr)
					{
						CString str = L"Navi_Object";
						CString strCount;
						strCount.Format(_T("%d"), NaviNowCount);
						str += strCount;
						
						dynamic_cast<CTestStage*>(m_pScene)->Copy_NaviObject(pMain->m_wndProperties.m_pNowTerrainObjcet, str);
						
						pMain->m_wndClassView.UpdateNaviList(str);
						NaviNowCount++;
						NaviAllCount++;
						//cout << "오브젝트 복사" << endl;
					}
				}
			}

			if (m_pKeyMgr->KeyDown(Engine::KEY_DELETE))
			{
				HTREEITEM NowItem = pMain->m_wndClassView.m_wndClassView.GetSelectedItem();
				CString strItem = pMain->m_wndClassView.m_wndClassView.GetItemText(NowItem);
				if (strItem.Find(L"Navi") != -1)
				{
					if (m_DeleteBool)
					{
						dynamic_cast<CTestStage*>(m_pScene)->Delete_Navi_Object(strItem, NaviAllCount);
						pMain->m_wndProperties.m_pNowGameObjcet = nullptr;
						m_DeleteBool = false;

						if (pMain->m_wndProperties.m_pGroupComponunt != nullptr)
							pMain->m_wndProperties.m_wndPropList.DeleteProperty(pMain->m_wndProperties.m_pGroupComponunt);
						pMain->m_wndProperties.m_pGroupComponunt = new CMFCPropertyGridProperty(_T("Componunt"));
					}
				}
			}
		}
	}
	return S_OK;
}

void CToolView::View_Render()
{
	m_pDeviceClass->Render_Begin(D3DXCOLOR(0.5f, 0.5f, 0.5f, 1.f));
	m_pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	if (m_pManagement != nullptr)
		m_pManagement->Render_Scene(m_pDevice);
	//m_pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	m_pDeviceClass->Render_End();
}

Engine::CGameObject * CToolView::GetSceneObj(CString str)
{
	if (m_pManagement->Get_GameObject(L"GameObject", str) != nullptr)
		return m_pManagement->Get_GameObject(L"GameObject", str);
	else if (m_pManagement->Get_GameObject(L"ColObject", str) != nullptr)
		return m_pManagement->Get_GameObject(L"ColObject", str);
	else
		return m_pManagement->Get_GameObject(L"NaviObject", str);
}

Engine::CGraphicDev * CToolView::GetDeviceClass()
{
	if (m_pDeviceClass == nullptr)
		return  nullptr;
	return m_pDeviceClass;
}



LPDIRECT3DDEVICE9 & CToolView::GetDevice()
{
	// TODO: 여기에 반환 구문을 삽입합니다.
	return m_pDevice;
}

POINT CToolView::Get_WinSize()
{

	CClientDC dc(this);

	CRect rect;
	dc.GetClipBox(&rect);
	int x = rect.Width();
	int y = rect.Height();
	//cout << x << " , " << y << endl;
	return POINT{ x, y };
}

CToolDoc* CToolView::GetDocument() const // 디버그되지 않은 버전은 인라인으로 지정됩니다.
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CToolDoc)));
	return (CToolDoc*)m_pDocument;
}


HRESULT CToolView::SetUp_DefaultSetting(LPDIRECT3DDEVICE9 * ppGraphicDev)
{
	FAILED(Engine::Ready_GraphicDev(g_hWnd,
		Engine::MODE_WIN,
		WINCX,
		WINCY,
		&m_pDeviceClass),
		E_FAIL);

	m_pDeviceClass->AddRef();

	(*ppGraphicDev) = m_pDeviceClass->Get_Device();
	(*ppGraphicDev)->AddRef();
	// INPUTDEV
	FAILED(Engine::Ready_InputDev(g_hInst, g_hWnd), E_FAIL);
	return S_OK;
}

HRESULT CToolView::Ready_Scene(LPDIRECT3DDEVICE9& pGraphicDev,
	Engine::CManagement ** ppManagement)
{
	m_pScene = nullptr;

	FAILED(Engine::Create_Management(pGraphicDev, ppManagement, false), E_FAIL);
	(*ppManagement)->AddRef();

	m_pScene = CTestStage::Create(pGraphicDev, this);
	NULL_CHECK_RETURN(m_pScene, E_FAIL);

	m_DObj.Set_Scene(m_pScene);

	FAILED((*ppManagement)->SetUp_Scene(m_pScene), E_FAIL);

	return S_OK;
}

HRESULT CToolView::CreateGameObject(CString cstr)
{
	//return dynamic_cast<CTestStage*>(m_pScene)->Add_GameObject(cstr);
	return S_OK;
}

HRESULT CToolView::DeleteGameObject(CString cstr)
{
	CMainFrame* pMain = (CMainFrame*)AfxGetMainWnd();
	//m_DObj.m_iObjNameCount[pMain->m_wndProperties.m_pNowGameObjcet->Get_ObjType()] = 0;
	return dynamic_cast<CTestStage*>(m_pScene)->Delete_Object(cstr, m_DObj.m_iObjCount[pMain->m_wndProperties.m_pNowGameObjcet->Get_ObjType()]);
}

HRESULT CToolView::CopyGameObject(Engine::CGameObject * object, CString cstr)
{
	return dynamic_cast<CTestStage*>(m_pScene)->Copy_GameObject(object, cstr);
}

HRESULT CToolView::CopyGameObject(Engine::CGameObject * object, CString cstr, _vec3 tempPos)
{
	return dynamic_cast<CTestStage*>(m_pScene)->Copy_GameObject(object, cstr, tempPos);
}

//HRESULT CToolView::CreateColObject(CString cstr)
//{
//	return dynamic_cast<CTestStage*>(m_pScene)->Add_ColObject(cstr);
//}

HRESULT CToolView::CreateNaviObject(CString cstr)
{
	return dynamic_cast<CTestStage*>(m_pScene)->Add_NaviObject(cstr);
}

//HRESULT CToolView::CopyNaviObject(CString cstr)
//{
//	return dynamic_cast<CTestStage*>(m_pScene)->Add_ColObject(cstr);
//}

void CToolView::OnInitialUpdate()
{
	g_hWnd = m_hWnd;

	g_hInst = AfxGetInstanceHandle();

	// View 크기를 조정하기 위해서는 프레임 원도우의 크기를 건드린다.
	// 프레임 윈도우의 포인터 얻어오는 방법.
	CMainFrame* pFrameWnd = dynamic_cast<CMainFrame*>(::AfxGetApp()->GetMainWnd());
	NULL_CHECK(pFrameWnd, L"pFrameWnd is null");

	RECT rcFrame = {};

	// 윈도우의 크기(테두리 + 클라이언트 영역) 얻어오는 함수.(스크린 좌표 기준)
	pFrameWnd->GetWindowRect(&rcFrame);
	::SetRect(&rcFrame, 0, 0, rcFrame.right - rcFrame.left, rcFrame.bottom - rcFrame.top);

	RECT rcView = {};

	// 윈도우의 클라이언트 영역(테두리를 포함하지 않은) 크기 얻어오는 함수. (클라이언트 좌표 기준)
	GetClientRect(&rcView);

	// 현재 프레임윈도우와 View 사이의 갭들을 구한다.
	int iGapX = rcFrame.right - rcView.right;
	int iGapY = rcFrame.bottom - rcView.bottom;

	// 프레임윈도우의 크기를 새로 설정.
	pFrameWnd->SetWindowPos(nullptr, 0, 0, WINCX + iGapX, WINCY + iGapY, SWP_NOZORDER | SWP_NOMOVE);

	FAILED(SetUp_DefaultSetting(&m_pDevice), E_FAIL);
	FAILED(Ready_Scene(m_pDevice, &m_pManagement), E_FAIL);

	m_pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);

	m_pKeyMgr = Engine::Get_KeyMgr();



	m_DCom.Create(IDD_DIALOG_COM);
	m_DObj.Create(IDD_DIALOG_OBJ);
}


void CToolView::OnLButtonDown(UINT nFlags, CPoint point)
{

}

#endif //_DEBUG


// CToolView 메시지 처리기


void CToolView::InsertComponunt()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	NULL(m_DCom.GetSafeHwnd(), L"TileTool is null");
	m_DCom.ComboClaer();
	m_DCom.ShowWindow(SW_SHOW); // SW_SHOW: 창모양으로 출력.
}






void CToolView::OnToolSave()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	CFileDialog Dlg(FALSE, L".dat", L"제목 없음.dat", OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		L"Data Files(*.dat)|*.dat|Text Files(*.txt)|*.txt||", this);

	TCHAR szCurrentDir[MAX_STR] = L"";

	::GetCurrentDirectory(MAX_STR, szCurrentDir);
	::PathRemoveFileSpec(szCurrentDir);
	::PathCombine(szCurrentDir, szCurrentDir, L"Data");

	Dlg.m_ofn.lpstrInitialDir = szCurrentDir;

	if (IDOK == Dlg.DoModal())
	{
		dynamic_cast<CTestStage*>(m_pScene)->Save(Dlg.GetPathName());
	}
}


void CToolView::OnToolLoad()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CFileDialog Dlg(TRUE, L".dat", L"제목 없음.dat", OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		L"Data Files(*.dat)|*.dat|Text Files(*.txt)|*.txt||", this);

	TCHAR szCurrentDir[MAX_STR] = L"";

	::GetCurrentDirectory(MAX_STR, szCurrentDir);
	::PathRemoveFileSpec(szCurrentDir);
	::PathCombine(szCurrentDir, szCurrentDir, L"Data");

	Dlg.m_ofn.lpstrInitialDir = szCurrentDir;

	if (IDOK == Dlg.DoModal())
	{
		int Count = 0;
		dynamic_cast<CTestStage*>(m_pScene)->Load(Dlg.GetPathName(), Count);
		Invalidate(FALSE);
	}
}


void CToolView::OnObjInsert()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	NULL(m_DObj.GetSafeHwnd(), L"TileTool is null");
	m_DObj.ShowWindow(SW_SHOW); // SW_SHOW: 창모양으로 출력.
	m_DObjEnable = true;
}


void CToolView::OnColSave()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	CFileDialog Dlg(FALSE, L".dat", L"제목 없음.dat", OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		L"Data Files(*.dat)|*.dat|Text Files(*.txt)|*.txt||", this);

	TCHAR szCurrentDir[MAX_STR] = L"";

	::GetCurrentDirectory(MAX_STR, szCurrentDir);
	::PathRemoveFileSpec(szCurrentDir);
	::PathCombine(szCurrentDir, szCurrentDir, L"Col");

	Dlg.m_ofn.lpstrInitialDir = szCurrentDir;

	if (IDOK == Dlg.DoModal())
	{
		dynamic_cast<CTestStage*>(m_pScene)->Col_Save(Dlg.GetPathName());
	}
}


void CToolView::OnColLoad()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	CFileDialog Dlg(TRUE, L".dat", L"제목 없음.dat", OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		L"Data Files(*.dat)|*.dat|Text Files(*.txt)|*.txt||", this);

	TCHAR szCurrentDir[MAX_STR] = L"";

	::GetCurrentDirectory(MAX_STR, szCurrentDir);
	::PathRemoveFileSpec(szCurrentDir);
	::PathCombine(szCurrentDir, szCurrentDir, L"Col");

	Dlg.m_ofn.lpstrInitialDir = szCurrentDir;

	if (IDOK == Dlg.DoModal())
	{
		CMainFrame* pMain = (CMainFrame*)AfxGetMainWnd();
		int iColCount = 0;


		dynamic_cast<CTestStage*>(m_pScene)->Col_Load(Dlg.GetPathName(), iColCount);

		m_DObj.m_iObjNameCount[Engine::OBJ_COL] = iColCount;
		m_DObj.m_iObjCount[Engine::OBJ_COL]     = iColCount;
		Invalidate(FALSE);
	}
}


void CToolView::OnNaviSave()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	CFileDialog Dlg(FALSE, L".dat", L"제목 없음.dat", OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		L"Data Files(*.dat)|*.dat|Text Files(*.txt)|*.txt||", this);

	TCHAR szCurrentDir[MAX_STR] = L"";

	::GetCurrentDirectory(MAX_STR, szCurrentDir);
	::PathRemoveFileSpec(szCurrentDir);
	::PathCombine(szCurrentDir, szCurrentDir, L"Navi");

	Dlg.m_ofn.lpstrInitialDir = szCurrentDir;

	if (IDOK == Dlg.DoModal())
	{
		dynamic_cast<CTestStage*>(m_pScene)->Navi_Save(Dlg.GetPathName());
	}
}


void CToolView::OnNaviLoad()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	CFileDialog Dlg(TRUE, L".dat", L"제목 없음.dat", OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		L"Data Files(*.dat)|*.dat|Text Files(*.txt)|*.txt||", this);

	TCHAR szCurrentDir[MAX_STR] = L"";

	::GetCurrentDirectory(MAX_STR, szCurrentDir);
	::PathRemoveFileSpec(szCurrentDir);
	::PathCombine(szCurrentDir, szCurrentDir, L"Navi");

	Dlg.m_ofn.lpstrInitialDir = szCurrentDir;

	if (IDOK == Dlg.DoModal())
	{
		NaviAllCount = 0;
		dynamic_cast<CTestStage*>(m_pScene)->Navi_Load(Dlg.GetPathName(), NaviAllCount);
		Invalidate(FALSE);
	}
}
