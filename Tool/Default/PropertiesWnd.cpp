
#include "pch.h"
#include "framework.h"

#include "PropertiesWnd.h"
#include "Resource.h"
#include "MainFrm.h"
#include "Tool.h"
#include "ClassView.h"
#include "ToolView.h"
#include "Transform.h"
#include "TempObject.h"
#include "Terrain.h"
#include "NaviObject.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//PropertyEx
//#define AFX_PROP_HAS_LIST 0x0001
#define AFX_PROP_HAS_BUTTON 0x0002
//#define AFX_PROP_HAS_SPIN 0x0004

// CMFCPropertyGridPropertyEx

CMFCPropertyGridPropertyEx::CMFCPropertyGridPropertyEx(const CString& strName, const COleVariant& varValue, LPCTSTR lpszDescr, BOOL bButton, int nDlgType)
	:CMFCPropertyGridProperty(strName, varValue, lpszDescr, 0, NULL, NULL, NULL)
{
	m_nDlgType = nDlgType;

	if (bButton)
		m_dwFlags = AFX_PROP_HAS_BUTTON;
}

CMFCPropertyGridPropertyEx::~CMFCPropertyGridPropertyEx()
{
}


// CMFCPropertyGridPropertyEx member functions
void CMFCPropertyGridPropertyEx::OnClickButton(CPoint point)
{
	ASSERT_VALID(this);
	ASSERT_VALID(m_pWndList);
	ASSERT_VALID(m_pWndInPlace);
	ASSERT(::IsWindow(m_pWndInPlace->GetSafeHwnd()));

	m_bButtonIsDown = TRUE;
	Redraw();

	//하고자 하는 내용 구현
	CMainFrame* pMain = (CMainFrame*)AfxGetMainWnd();
	pMain->m_wndProperties.m_TexTool.m_TexCombo.Clear();
	pMain->m_wndProperties.m_TexTool.ShowWindow(SW_SHOW);

	m_bButtonIsDown = FALSE;
	Redraw();

	if (m_pWndInPlace != NULL)
	{
		//m_pWndInPlace->SetFocus();
	}
	else
	{
		//m_pWndList->SetFocus();
	}

}


/////////////////////////////////////////////////////////////////////////////
// CResourceViewBar

CPropertiesWnd::CPropertiesWnd() noexcept
{
	m_nComboHeight = 0;
}

void CPropertiesWnd::SetObjectInfo(Engine::CGameObject * object)
{
	m_pNowGameObjcet = object;
	wstring temp = L"이름 : ";
	temp += m_pNowGameObjcet->Get_ObjName();
	CString nameStr = temp.c_str();

	if (m_pNaviName != nullptr)
		m_wndPropList.DeleteProperty(m_pNaviName);
	
	if (m_pObjName != nullptr)
		m_wndPropList.DeleteProperty(m_pObjName);
	 m_pObjName = new CMFCPropertyGridProperty(nameStr);

	if (m_pGroupComponunt != nullptr)
		m_wndPropList.DeleteProperty(m_pGroupComponunt);

	m_pGroupComponunt = new CMFCPropertyGridProperty(_T("Componunt"));

	for (int i = 0; i < Engine::ID_END; i++)
	{
		if (object->Get_MapComponent((Engine::COMPONENTID)i) != nullptr)
		{
			auto iter = object->Get_MapComponent((Engine::COMPONENTID)i)[i].begin();

			for (; iter != object->Get_MapComponent((Engine::COMPONENTID)i)[i].end(); iter++)
			{
				if (wcscmp(iter->first.c_str(), L"Com_Transform") == 0)
				{
					CMFCPropertyGridProperty* pGroupTransForm = new CMFCPropertyGridProperty(_T("Transform"));
					m_pGroupComponunt->AddSubItem(pGroupTransForm);

					CMFCPropertyGridProperty* pGroupScale = new CMFCPropertyGridProperty(_T("Scale"));
					_vec3 vScale = dynamic_cast<Engine::CTransform*>(object->Get_Component(L"Com_Transform", Engine::ID_DYNAMIC))->m_vScale;
					pGroupTransForm->AddSubItem(pGroupScale);
					pGroupScale->AddSubItem(new CMFCPropertyGridProperty(_T("x"), (_variant_t)vScale.x, _T("Scale x")));
					pGroupScale->AddSubItem(new CMFCPropertyGridProperty(_T("y"), (_variant_t)vScale.y, _T("Scale y")));
					pGroupScale->AddSubItem(new CMFCPropertyGridProperty(_T("z"), (_variant_t)vScale.z, _T("Scale z")));

					
					CMFCPropertyGridProperty* pGroupPosition = new CMFCPropertyGridProperty(_T("Position"));
					pGroupTransForm->AddSubItem(pGroupPosition);
					_vec3 vPos = *dynamic_cast<Engine::CTransform*>(object->Get_Component(L"Com_Transform", Engine::ID_DYNAMIC))->Get_Info(Engine::INFO_POS);
					pGroupPosition->AddSubItem(new CMFCPropertyGridProperty(_T("x"), (_variant_t)vPos.x, _T("Position x")));
					pGroupPosition->AddSubItem(new CMFCPropertyGridProperty(_T("y"), (_variant_t)vPos.y, _T("Position y")));
					pGroupPosition->AddSubItem(new CMFCPropertyGridProperty(_T("z"), (_variant_t)vPos.z, _T("Position z")));


					CMFCPropertyGridProperty* pGroupRotation = new CMFCPropertyGridProperty(_T("Rotation"));
					_vec3 vAngle = dynamic_cast<Engine::CTransform*>(object->Get_Component(L"Com_Transform", Engine::ID_DYNAMIC))->m_vAngle;
					pGroupTransForm->AddSubItem(pGroupRotation);
					pGroupRotation->AddSubItem(new CMFCPropertyGridProperty(_T("x"), (_variant_t)D3DXToDegree(vAngle.x), _T("Rotation x")));
					pGroupRotation->AddSubItem(new CMFCPropertyGridProperty(_T("y"), (_variant_t)D3DXToDegree(vAngle.y), _T("Rotation y")));
					pGroupRotation->AddSubItem(new CMFCPropertyGridProperty(_T("z"), (_variant_t)D3DXToDegree(vAngle.z), _T("Rotation z")));

				}
				else if (wcscmp(iter->first.c_str(), L"Com_Texture") == 0)
				{
					//SetTextureName(L"텍스쳐 추가 버튼");
					//if (pGroupTexture != nullptr)
						//m_wndPropList.DeleteProperty(pGroupTexture);
				
					if (wcscmp(dynamic_cast<CTempObject*>(object)->GetString(), L"") == 0)
					{
						SetTextureName(L"텍스쳐 추가 버튼",true);
					}
					else
					{
						SetTextureName(dynamic_cast<CTempObject*>(object)->GetString(), true);
					}
				}
				else
				{
					CMFCPropertyGridProperty* pGroupCom = new CMFCPropertyGridProperty(iter->first.c_str());
					m_pGroupComponunt->AddSubItem(pGroupCom);
		
				}
			}
		}
	}


	m_wndPropList.AddProperty(m_pObjName);
	m_wndPropList.AddProperty(m_pGroupComponunt);

}

void CPropertiesWnd::SetTerrainInfo(Engine::CGameObject * object)
{
	wstring temp = L"이름 : ";
	temp += object->Get_ObjName();
	CString nameStr = temp.c_str();

	if (m_pNaviName != nullptr)
		m_wndPropList.DeleteProperty(m_pNaviName);
	
	if (m_pObjName != nullptr)
		m_wndPropList.DeleteProperty(m_pObjName);
	m_pObjName = new CMFCPropertyGridProperty(nameStr);
	if (m_pGroupComponunt != nullptr)
		m_wndPropList.DeleteProperty(m_pGroupComponunt);

	m_pNowTerrainObjcet = object;
	m_pNaviName = new CMFCPropertyGridProperty(_T("Navi"));


	_vec3* vNaviPos = dynamic_cast<CNaviObject*>(object)->Get_Navi();

	if (vNaviPos != nullptr)
	{
		CMFCPropertyGridProperty* pGroupPos1 = new CMFCPropertyGridProperty(_T("1번정점"));
		m_pNaviName->AddSubItem(pGroupPos1);

		pGroupPos1->AddSubItem(new CMFCPropertyGridProperty(_T("x"), (_variant_t)vNaviPos[0].x, _T("Scale x")));
		pGroupPos1->AddSubItem(new CMFCPropertyGridProperty(_T("y"), (_variant_t)vNaviPos[0].y, _T("Scale y")));
		pGroupPos1->AddSubItem(new CMFCPropertyGridProperty(_T("z"), (_variant_t)vNaviPos[0].z, _T("Scale z")));


		CMFCPropertyGridProperty* pGroupPos2 = new CMFCPropertyGridProperty(_T("2번정점"));
		m_pNaviName->AddSubItem(pGroupPos2);
		pGroupPos2->AddSubItem(new CMFCPropertyGridProperty(_T("x"), (_variant_t)vNaviPos[1].x, _T("Scale x")));
		pGroupPos2->AddSubItem(new CMFCPropertyGridProperty(_T("y"), (_variant_t)vNaviPos[1].y, _T("Scale y")));
		pGroupPos2->AddSubItem(new CMFCPropertyGridProperty(_T("z"), (_variant_t)vNaviPos[1].z, _T("Scale z")));

		CMFCPropertyGridProperty* pGroupPos3 = new CMFCPropertyGridProperty(_T("3번정점"));
		m_pNaviName->AddSubItem(pGroupPos3);
		pGroupPos3->AddSubItem(new CMFCPropertyGridProperty(_T("x"), (_variant_t)vNaviPos[2].x, _T("Scale x")));
		pGroupPos3->AddSubItem(new CMFCPropertyGridProperty(_T("y"), (_variant_t)vNaviPos[2].y, _T("Scale y")));
		pGroupPos3->AddSubItem(new CMFCPropertyGridProperty(_T("z"), (_variant_t)vNaviPos[2].z, _T("Scale z")));
	}
	m_wndPropList.AddProperty(m_pObjName);
	m_wndPropList.AddProperty(m_pNaviName);
}

void CPropertiesWnd::SetTextureName(CString str,bool flag)
{

	if (!flag)
	{
		if (m_pGroupTexture != nullptr)
			m_wndPropList.DeleteProperty(m_pGroupTexture);
	}
	m_pGroupTexture = new CMFCPropertyGridProperty(_T("Com_Texture"));
	m_pGroupComponunt->AddSubItem(m_pGroupTexture);

	CMFCPropertyGridProperty* pGroupTexDialog = new CMFCPropertyGridPropertyEx(_T("텍스쳐"), str);

	m_pGroupTexture->AddSubItem(pGroupTexDialog);
	m_pGroupComponunt->Expand(FALSE);
	m_pGroupComponunt->Expand(TRUE);
}

CPropertiesWnd::~CPropertiesWnd()
{
}

BEGIN_MESSAGE_MAP(CPropertiesWnd, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_COMMAND(ID_EXPAND_ALL, OnExpandAllProperties)
	ON_UPDATE_COMMAND_UI(ID_EXPAND_ALL, OnUpdateExpandAllProperties)
	ON_COMMAND(ID_SORTPROPERTIES, OnSortProperties)
	ON_UPDATE_COMMAND_UI(ID_SORTPROPERTIES, OnUpdateSortProperties)
	ON_COMMAND(ID_PROPERTIES1, OnProperties1)
	ON_UPDATE_COMMAND_UI(ID_PROPERTIES1, OnUpdateProperties1)
	ON_COMMAND(ID_PROPERTIES2, OnProperties2)
	ON_UPDATE_COMMAND_UI(ID_PROPERTIES2, OnUpdateProperties2)
	ON_WM_SETFOCUS()
	ON_WM_SETTINGCHANGE()
	ON_REGISTERED_MESSAGE(AFX_WM_PROPERTY_CHANGED, &CPropertiesWnd::OnPropertyChanged)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CResourceViewBar 메시지 처리기

void CPropertiesWnd::AdjustLayout()
{
	if (GetSafeHwnd () == nullptr || (AfxGetMainWnd() != nullptr && AfxGetMainWnd()->IsIconic()))
	{
		return;
	}

	CRect rectClient;
	GetClientRect(rectClient);

	int cyTlb = m_wndToolBar.CalcFixedLayout(FALSE, TRUE).cy;

	m_wndObjectCombo.SetWindowPos(nullptr, rectClient.left, rectClient.top, rectClient.Width(), m_nComboHeight, SWP_NOACTIVATE | SWP_NOZORDER);
	m_wndToolBar.SetWindowPos(nullptr, rectClient.left, rectClient.top + m_nComboHeight, rectClient.Width(), cyTlb, SWP_NOACTIVATE | SWP_NOZORDER);
	m_wndPropList.SetWindowPos(nullptr, rectClient.left, rectClient.top + m_nComboHeight + cyTlb, rectClient.Width(), rectClient.Height() -(m_nComboHeight+cyTlb), SWP_NOACTIVATE | SWP_NOZORDER);
}
int CPropertiesWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rectDummy;
	rectDummy.SetRectEmpty();

	// 콤보 상자를 만듭니다.
	const DWORD dwViewStyle = WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST | WS_BORDER | CBS_SORT | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;

	if (!m_wndObjectCombo.Create(dwViewStyle, rectDummy, this, 1))
	{
		TRACE0("속성 콤보 상자를 만들지 못했습니다. \n");
		return -1;      // 만들지 못했습니다.
	}

	m_wndObjectCombo.AddString(_T("응용 프로그램"));
	m_wndObjectCombo.AddString(_T("속성 창"));
	m_wndObjectCombo.SetCurSel(0);

	CRect rectCombo;
	m_wndObjectCombo.GetClientRect (&rectCombo);

	m_nComboHeight = rectCombo.Height();

	if (!m_wndPropList.Create(WS_VISIBLE | WS_CHILD, rectDummy, this, 2))
	{
		TRACE0("속성 표를 만들지 못했습니다. \n");
		return -1;      // 만들지 못했습니다.
	}

	InitPropList();

	m_wndToolBar.Create(this, AFX_DEFAULT_TOOLBAR_STYLE, IDR_PROPERTIES);
	m_wndToolBar.LoadToolBar(IDR_PROPERTIES, 0, 0, TRUE /* 잠금 */);
	m_wndToolBar.CleanUpLockedImages();
	m_wndToolBar.LoadBitmap(theApp.m_bHiColorIcons ? IDB_PROPERTIES_HC : IDR_PROPERTIES, 0, 0, TRUE /* 잠금 */);

	m_wndToolBar.SetPaneStyle(m_wndToolBar.GetPaneStyle() | CBRS_TOOLTIPS | CBRS_FLYBY);
	m_wndToolBar.SetPaneStyle(m_wndToolBar.GetPaneStyle() & ~(CBRS_GRIPPER | CBRS_SIZE_DYNAMIC | CBRS_BORDER_TOP | CBRS_BORDER_BOTTOM | CBRS_BORDER_LEFT | CBRS_BORDER_RIGHT));
	m_wndToolBar.SetOwner(this);

	// 모든 명령은 부모 프레임이 아닌 이 컨트롤을 통해 라우팅됩니다.
	m_wndToolBar.SetRouteCommandsViaFrame(FALSE);
	m_TexTool.Create(IDD_DIALOG_TEX);
	AdjustLayout();
	return 0;
}

void CPropertiesWnd::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);
	AdjustLayout();
}

void CPropertiesWnd::OnExpandAllProperties()
{
	m_wndPropList.ExpandAll();
}

void CPropertiesWnd::OnUpdateExpandAllProperties(CCmdUI* /* pCmdUI */)
{
}

void CPropertiesWnd::OnSortProperties()
{
	m_wndPropList.SetAlphabeticMode(!m_wndPropList.IsAlphabeticMode());
}

void CPropertiesWnd::OnUpdateSortProperties(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_wndPropList.IsAlphabeticMode());
}

void CPropertiesWnd::OnProperties1()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
}

void CPropertiesWnd::OnUpdateProperties1(CCmdUI* /*pCmdUI*/)
{
	// TODO: 여기에 명령 업데이트 UI 처리기 코드를 추가합니다.
}

void CPropertiesWnd::OnProperties2()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
}

void CPropertiesWnd::OnUpdateProperties2(CCmdUI* /*pCmdUI*/)
{
	// TODO: 여기에 명령 업데이트 UI 처리기 코드를 추가합니다.
}

void CPropertiesWnd::InitPropList()
{
	SetPropListFont();

	m_wndPropList.EnableHeaderCtrl(FALSE);
	m_wndPropList.EnableDescriptionArea();
	m_wndPropList.SetVSDotNetLook();
	m_wndPropList.MarkModifiedProperties();

	//m_pGroupComponunt = new CMFCPropertyGridProperty(_T("Transform"));
	//m_wndPropList.AddProperty(m_pGroupTransform);


	//CMFCPropertyGridProperty* pGroupScale = new CMFCPropertyGridProperty(_T("Scale"));
	//m_pGroupTransform->AddSubItem(pGroupScale);
	//pGroupScale->AddSubItem(new CMFCPropertyGridProperty(_T("x"), (_variant_t)0, _T("Scale x")));
	//pGroupScale->AddSubItem(new CMFCPropertyGridProperty(_T("y"), (_variant_t)0, _T("Scale y")));
	//pGroupScale->AddSubItem(new CMFCPropertyGridProperty(_T("z"), (_variant_t)0, _T("Scale z")));

	//m_wndPropList.DeleteProperty(m_pGroupTransform);

	//m_pGroupTransform = new CMFCPropertyGridProperty(_T("Transform"));
	//m_wndPropList.AddProperty(m_pGroupTransform);
	//pGroupScale = new CMFCPropertyGridProperty(_T("Scale"));
	//m_pGroupTransform->AddSubItem(pGroupScale);
	//pGroupScale->AddSubItem(new CMFCPropertyGridProperty(_T("x"), (_variant_t)0, _T("Scale x")));
	//pGroupScale->AddSubItem(new CMFCPropertyGridProperty(_T("y"), (_variant_t)0, _T("Scale y")));
	//pGroupScale->AddSubItem(new CMFCPropertyGridProperty(_T("z"), (_variant_t)0, _T("Scale z")));

	//CMFCPropertyGridProperty* pGroup1 = new CMFCPropertyGridProperty(_T("모양"));

	//pGroup1->AddSubItem(new CMFCPropertyGridProperty(_T("3D 모양"), (_variant_t)false, _T("창에 굵은 글꼴이 아닌 글꼴을 지정하고, 컨트롤에 3D 테두리를 지정합니다.")));

	//CMFCPropertyGridProperty* pProp = new CMFCPropertyGridProperty(_T("테두리"), _T("Dialog Frame"), _T("None, Thin, Resizable 또는 Dialog Frame 중 하나를 지정합니다."));
	//pProp->AddOption(_T("None"));
	//pProp->AddOption(_T("Thin"));
	//pProp->AddOption(_T("Resizable"));
	//pProp->AddOption(_T("Dialog Frame"));
	//pProp->AllowEdit(FALSE);

	//pGroup1->AddSubItem(pProp);
	//pGroup1->AddSubItem(new CMFCPropertyGridProperty(_T("캡션"), (_variant_t)_T("정보"), _T("창의 제목 표시줄에 표시되는 텍스트를 지정합니다.")));

	//m_wndPropList.AddProperty(pGroup1);

	//CMFCPropertyGridProperty* pSize = new CMFCPropertyGridProperty(_T("창 크기"), 0, TRUE);

	//pProp = new CMFCPropertyGridProperty(_T("높이"), (_variant_t) 250l, _T("창의 높이를 지정합니다."));
	//pProp->EnableSpinControl(TRUE, 50, 300);
	//pSize->AddSubItem(pProp);

	//pProp = new CMFCPropertyGridProperty( _T("너비"), (_variant_t) 150l, _T("창의 너비를 지정합니다."));
	//pProp->EnableSpinControl(TRUE, 50, 200);
	//pSize->AddSubItem(pProp);

	//m_wndPropList.AddProperty(pSize);

	//CMFCPropertyGridProperty* pGroup2 = new CMFCPropertyGridProperty(_T("글꼴"));

	//LOGFONT lf;
	//CFont* font = CFont::FromHandle((HFONT) GetStockObject(DEFAULT_GUI_FONT));
	//font->GetLogFont(&lf);

	//_tcscpy_s(lf.lfFaceName, _T("맑은 고딕"));

	//pGroup2->AddSubItem(new CMFCPropertyGridFontProperty(_T("글꼴"), lf, CF_EFFECTS | CF_SCREENFONTS, _T("창의 기본 글꼴을 지정합니다.")));
	//pGroup2->AddSubItem(new CMFCPropertyGridProperty(_T("시스템 글꼴을 사용합니다."), (_variant_t) true, _T("창에서 MS Shell Dlg 글꼴을 사용하도록 지정합니다.")));

	//m_wndPropList.AddProperty(pGroup2);

	//CMFCPropertyGridProperty* pGroup3 = new CMFCPropertyGridProperty(_T("기타"));
	//pProp = new CMFCPropertyGridProperty(_T("(이름)"), _T("응용 프로그램"));
	//pProp->Enable(FALSE);
	//pGroup3->AddSubItem(pProp);

	//CMFCPropertyGridColorProperty* pColorProp = new CMFCPropertyGridColorProperty(_T("창 색상"), RGB(210, 192, 254), nullptr, _T("창의 기본 색상을 지정합니다."));
	//pColorProp->EnableOtherButton(_T("기타..."));
	//pColorProp->EnableAutomaticButton(_T("기본값"), ::GetSysColor(COLOR_3DFACE));
	//pGroup3->AddSubItem(pColorProp);

	//static const TCHAR szFilter[] = _T("아이콘 파일(*.ico)|*.ico|모든 파일(*.*)|*.*||");
	//pGroup3->AddSubItem(new CMFCPropertyGridFileProperty(_T("아이콘"), TRUE, _T(""), _T("ico"), 0, szFilter, _T("창 아이콘을 지정합니다.")));

	//pGroup3->AddSubItem(new CMFCPropertyGridFileProperty(_T("폴더"), _T("c:\\")));

	//m_wndPropList.AddProperty(pGroup3);

	//CMFCPropertyGridProperty* pGroup4 = new CMFCPropertyGridProperty(_T("계층"));

	//CMFCPropertyGridProperty* pGroup41 = new CMFCPropertyGridProperty(_T("첫번째 하위 수준"));
	//pGroup4->AddSubItem(pGroup41);

	//CMFCPropertyGridProperty* pGroup411 = new CMFCPropertyGridProperty(_T("두 번째 하위 수준"));
	//pGroup41->AddSubItem(pGroup411);

	//pGroup411->AddSubItem(new CMFCPropertyGridProperty(_T("항목 1"), (_variant_t) _T("값 1"), _T("설명입니다.")));
	//pGroup411->AddSubItem(new CMFCPropertyGridProperty(_T("항목 2"), (_variant_t) _T("값 2"), _T("설명입니다.")));
	//pGroup411->AddSubItem(new CMFCPropertyGridProperty(_T("항목 3"), (_variant_t) _T("값 3"), _T("설명입니다.")));

	//pGroup4->Expand(FALSE);
	//m_wndPropList.AddProperty(pGroup4);
}

void CPropertiesWnd::OnSetFocus(CWnd* pOldWnd)
{
	CDockablePane::OnSetFocus(pOldWnd);
	m_wndPropList.SetFocus();
}

void CPropertiesWnd::OnSettingChange(UINT uFlags, LPCTSTR lpszSection)
{
	CDockablePane::OnSettingChange(uFlags, lpszSection);
	SetPropListFont();
}

LRESULT CPropertiesWnd::OnPropertyChanged(WPARAM wp, LPARAM lp)
{
	CMFCPropertyGridProperty* pProp = NULL;
	pProp = (CMFCPropertyGridProperty*)lp;

	if (!pProp)
		return 0L;

	_variant_t var = pProp->GetValue();

	int id = pProp->GetData();

	CString str;
	CString strParent;
	str = pProp->GetName();
	strParent =	pProp->GetParent()->GetName();

	CMainFrame* pMain = (CMainFrame*)AfxGetMainWnd();
	Engine::CGameObject* Object;
	CToolView* pToolView = (CToolView*)((CMainFrame*)(AfxGetApp()->m_pMainWnd))->GetActiveView();
	if (!pToolView->m_DObj.m_NaviButton.GetCheck())
	{
		CString strViewItem = m_pNowGameObjcet->Get_ObjName().c_str();//pMain->m_wndClassView.m_wndClassView.GetItemText(pMain->m_wndClassView.m_wndClassView.GetSelectedItem());


		if (pToolView == nullptr)
		{
			cout << "안됨" << endl;
			return 0L;
		}
		Object = pToolView->GetSceneObj(strViewItem);

		if (Object == nullptr)
		{
			cout << "없음" << endl;
			return 0L;
		}
	}
	else
	{
		Object = m_pNowTerrainObjcet;
	}
	switch (var.vt)
	{
	case VT_UINT:
		str.Format(_T("Name(ID:%d):%s, Value:%d"), id, str, var.uintVal);
	case VT_INT:
		str.Format(_T("Name(ID:%d):%s, Value:%d"), id, str, var.intVal);
	case VT_I2:
		str.Format(_T("Name(ID:%d):%s, Value:%d"), id, str, var.iVal);
	case VT_I4:
		str.Format(_T("Name(ID:%d):%s, Value:%d"), id, str, var.lVal);
		break;
	case VT_R4:	
		if (strParent == L"Scale")
		{
			if (str == L"x")
			{
				dynamic_cast<Engine::CTransform*>(Object->Get_Component(L"Com_Transform", Engine::ID_DYNAMIC))->m_vScale.x = var.fltVal;
			}
			else if (str == L"y")
			{
				dynamic_cast<Engine::CTransform*>(Object->Get_Component(L"Com_Transform", Engine::ID_DYNAMIC))->m_vScale.y = var.fltVal;
			}
			else if (str == L"z")
			{
				dynamic_cast<Engine::CTransform*>(Object->Get_Component(L"Com_Transform", Engine::ID_DYNAMIC))->m_vScale.z = var.fltVal;
			}
		}
		if (strParent == L"Position")
		{
			if (str == L"x")
			{
				dynamic_cast<Engine::CTransform*>(Object->Get_Component(L"Com_Transform", Engine::ID_DYNAMIC))->m_vInfo[Engine::INFO_POS].x = var.fltVal;
			}
			else if (str == L"y")
			{
				dynamic_cast<Engine::CTransform*>(Object->Get_Component(L"Com_Transform", Engine::ID_DYNAMIC))->m_vInfo[Engine::INFO_POS].y = var.fltVal;
			}
			else if (str == L"z")
			{
				dynamic_cast<Engine::CTransform*>(Object->Get_Component(L"Com_Transform", Engine::ID_DYNAMIC))->m_vInfo[Engine::INFO_POS].z = var.fltVal;
			}
		}
		if (strParent == L"Rotation")
		{
			if (str == L"x")
			{
				dynamic_cast<Engine::CTransform*>(Object->Get_Component(L"Com_Transform", Engine::ID_DYNAMIC))->m_vAngle.x = D3DXToRadian(var.fltVal);
			}
			else if (str == L"y")
			{
				dynamic_cast<Engine::CTransform*>(Object->Get_Component(L"Com_Transform", Engine::ID_DYNAMIC))->m_vAngle.y = D3DXToRadian(var.fltVal);
			}
			else if (str == L"z")
			{
				dynamic_cast<Engine::CTransform*>(Object->Get_Component(L"Com_Transform", Engine::ID_DYNAMIC))->m_vAngle.z = D3DXToRadian(var.fltVal);
			}
		}
		if (strParent == L"1번정점")
		{
			if (str == L"x")
			{
				dynamic_cast<CNaviObject*>(m_pNowTerrainObjcet)->Set_Vertex(0, 0, var.fltVal);
			}
			else if (str == L"y")
			{
				dynamic_cast<CNaviObject*>(m_pNowTerrainObjcet)->Set_Vertex(0, 1, var.fltVal);
			}
			else if (str == L"z")
			{
				dynamic_cast<CNaviObject*>(m_pNowTerrainObjcet)->Set_Vertex(0, 2, var.fltVal);
			}
		}
		if (strParent == L"2번정점")
		{
			if (str == L"x")
			{
				dynamic_cast<CNaviObject*>(m_pNowTerrainObjcet)->Set_Vertex(1, 0, var.fltVal);
			}
			else if (str == L"y")
			{
				dynamic_cast<CNaviObject*>(m_pNowTerrainObjcet)->Set_Vertex(1, 1, var.fltVal);
			}
			else if (str == L"z")
			{
				dynamic_cast<CNaviObject*>(m_pNowTerrainObjcet)->Set_Vertex(1, 2, var.fltVal);
			}
		}
		if (strParent == L"3번정점")
		{
			if (str == L"x")
			{
				dynamic_cast<CNaviObject*>(m_pNowTerrainObjcet)->Set_Vertex(2, 0, var.fltVal);
			}
			else if (str == L"y")
			{
				dynamic_cast<CNaviObject*>(m_pNowTerrainObjcet)->Set_Vertex(2, 1, var.fltVal);
			}
			else if (str == L"z")
			{
				dynamic_cast<CNaviObject*>(m_pNowTerrainObjcet)->Set_Vertex(2, 2, var.fltVal);
			}
		}
		break;
	case VT_R8:
		str.Format(_T("Name(ID:%d):%s, Value:%f"), id, str, var.dblVal);
		break;
	case VT_BSTR:
		str.Format(_T("Name(ID:%d):%s, Value:%s"), id, str, var.bstrVal);
		break;
	case VT_BOOL:
		str.Format(_T("Name(ID:%d):%s, Value:%d"), id, str, var.boolVal);
		break;
	}

	//AfxMessageBox(str);

	return 0L;
}

void CPropertiesWnd::SetPropListFont()
{
	::DeleteObject(m_fntPropList.Detach());

	LOGFONT lf;
	afxGlobalData.fontRegular.GetLogFont(&lf);

	NONCLIENTMETRICS info;
	info.cbSize = sizeof(info);

	afxGlobalData.GetNonClientMetrics(info);

	lf.lfHeight = info.lfMenuFont.lfHeight;
	lf.lfWeight = info.lfMenuFont.lfWeight;
	lf.lfItalic = info.lfMenuFont.lfItalic;

	m_fntPropList.CreateFontIndirect(&lf);

	m_wndPropList.SetFont(&m_fntPropList);
	m_wndObjectCombo.SetFont(&m_fntPropList);
}
