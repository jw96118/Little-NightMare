
// ToolView.h: CToolView 클래스의 인터페이스
//

#pragma once

#include "Defines.h"
#include "Base.h"
#include "Engine_Defines.h"
#include "Export_Function.h"
#include "GameObject.h"
#include "ComponuntDialog.h"
#include "ObjectDialog.h"
BEGIN(Engine)

class CGraphicDev;
class CManagement;

class CTransform;
class CKeyMgr;
class CGameObject;

END

class CToolDoc;
class CClassView;

class CToolView : public CView
{
protected: // serialization에서만 만들어집니다.
	CToolView() noexcept;
	DECLARE_DYNCREATE(CToolView)

// 특성입니다.
public:
	CToolDoc* GetDocument() const;

// 작업입니다.
public:

// 재정의입니다.
public:
	virtual void OnDraw(CDC* pDC);  // 이 뷰를 그리기 위해 재정의되었습니다.
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// 구현입니다.
public:
	virtual ~CToolView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

// 생성된 메시지 맵 함수
protected:
	afx_msg void OnFilePrintPreview();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	DECLARE_MESSAGE_MAP()


public:
	HRESULT View_Update(const _float& fTimeDelta);
	void View_Render();
	Engine::CGameObject* GetSceneObj(CString str);
	Engine::CGraphicDev* GetDeviceClass();
	LPDIRECT3DDEVICE9 &  GetDevice();
	POINT Get_WinSize();
private:
	HRESULT		SetUp_DefaultSetting(LPDIRECT3DDEVICE9* ppGraphicDev);

	HRESULT		Ready_Scene(LPDIRECT3DDEVICE9& pGraphicDev,
		Engine::CManagement** ppManagement);
	HRESULT CreateGameObject(CString cstr);
	HRESULT DeleteGameObject(CString cstr);
	HRESULT CopyGameObject(Engine::CGameObject* object, CString cstr);
	HRESULT CopyGameObject(Engine::CGameObject* object,CString cstr,_vec3 tempPos);
	//HRESULT CreateColObject(CString cstr);
	HRESULT CreateNaviObject(CString cstr);
	//HRESULT CopyNaviObject(CString cstr);
private:
	Engine::CGraphicDev*			m_pDeviceClass = nullptr;
	Engine::CManagement*			m_pManagement = nullptr;
public:
	LPDIRECT3DDEVICE9				m_pDevice = nullptr;

	Engine::CKeyMgr*        m_pKeyMgr = nullptr;
	Engine::CScene*		    m_pScene = nullptr;
public:
	int ColCount = 0;
	int NaviNowCount = 0;
	int NaviAllCount = 0;
public:
	CComponuntDialog   m_DCom;
	CObjectDialog      m_DObj;
	_bool              m_DObjEnable = false;
public:
	bool                 m_DeleteBool = false;
public:
	virtual void OnInitialUpdate();
	afx_msg void InsertComponunt();

	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnToolSave();
	afx_msg void OnToolLoad();
	afx_msg void OnObjInsert();
	afx_msg void OnColSave();
	afx_msg void OnColLoad();
	afx_msg void OnNaviSave();
	afx_msg void OnNaviLoad();
};

#ifndef _DEBUG  // ToolView.cpp의 디버그 버전
inline CToolDoc* CToolView::GetDocument() const
   { return reinterpret_cast<CToolDoc*>(m_pDocument); }
#endif

