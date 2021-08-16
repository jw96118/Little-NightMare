// TextureTool.cpp: 구현 파일
//

#include "pch.h"
#include "Tool.h"
#include "TextureTool.h"
#include "afxdialogex.h"

#include "Texture.h"
#include "Export_Resources.h"
#include "Export_System.h"
#include "MainFrm.h"
#include "ToolView.h"
#include "ToolTex.h"
#include "TempObject.h"
// CTextureTool 대화 상자

IMPLEMENT_DYNAMIC(CTextureTool, CDialogEx)

CTextureTool::CTextureTool(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_TEX, pParent)
{

}

CTextureTool::~CTextureTool()
{
	
}

BOOL CTextureTool::OnInitDialog()
{
	CDialog::OnInitDialog();

	CString wstring;

	m_TexCombo.AddString(L"Terrain");
	m_TexCombo.AddString(L"Wall");
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다
}



void CTextureTool::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TEXTURE_IMG, m_TexImg);
	DDX_Control(pDX, IDC_TEXTURE_COMBO, m_TexCombo);
	DDX_Control(pDX, IDC_TEXTURE_LIST, m_TexList);
	DDX_Control(pDX, ID_COM_OK, m_BOk);
}


BEGIN_MESSAGE_MAP(CTextureTool, CDialogEx)
	ON_BN_CLICKED(ID_COM_OK, &CTextureTool::OnBnClickedComOk)
	ON_LBN_SELCHANGE(IDC_TEXTURE_LIST, &CTextureTool::OnLbnSelchangeTextureList)
	ON_CBN_SELCHANGE(IDC_TEXTURE_COMBO, &CTextureTool::OnCbnSelchangeTextureCombo)
	ON_BN_CLICKED(ID_COM_CANCEL, &CTextureTool::OnBnClickedComCancel)
	ON_WM_DESTROY()
	ON_WM_SYSCOMMAND()
END_MESSAGE_MAP()


// CTextureTool 메시지 처리기




void CTextureTool::OnBnClickedComOk()
{
	CMainFrame* pMain = (CMainFrame*)AfxGetMainWnd();
	if (m_TexList.GetCurSel() != -1)
	{
		m_TexName = tempName;
		CString tempNum;
		tempNum.Format(_T("%d"), m_TexList.GetCurSel());
		m_TexName += tempNum;

		dynamic_cast<CTempObject*>(pMain->m_wndProperties.m_pNowGameObjcet)->SetTexCount(m_TexList.GetCurSel());
		dynamic_cast<CTempObject*>(pMain->m_wndProperties.m_pNowGameObjcet)->SetString(m_TexName);
		pMain->m_wndProperties.m_pNowGameObjcet->Set_Insert_Component(L"Com_Texture", m_pTextureCom);
		pMain->m_wndProperties.SetTextureName(m_TexName,false);
	}
}


void CTextureTool::OnLbnSelchangeTextureList()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다. 
	cout << m_TexList.GetCurSel() << endl;
	TileRender();
}


void CTextureTool::OnCbnSelchangeTextureCombo()
{
	if (m_pSprite == nullptr)
	{
		pToolView = (CToolView*)((CMainFrame*)(AfxGetApp()->m_pMainWnd))->GetActiveView();
		HRESULT hr = D3DXCreateSprite(pToolView->GetDevice(), &m_pSprite);
		FAILED(hr, L"D3DXCreateSprite Failed", E_FAIL);
		//m_TexName = new _tchar[20];
		Init = true;
	}
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_TexList.ResetContent();
	m_pTextureCom = nullptr;
	//ZeroMemory(&m_TexName, sizeof(CString));
	
	switch (m_TexCombo.GetCurSel())
	{
	case 0:
		m_pTextureCom =	dynamic_cast<Engine::CTexture*>(Engine::Clone(RESOURCE_STAGE, L"Texture_Terrain"));
		dynamic_cast<Engine::CResources*>(m_pTextureCom)->SetSaveType(RESOURCE_STAGE, L"Texture_Terrain");
		tempName = L"Terrain";
	
		break;
	case 1:
		m_pTextureCom = dynamic_cast<Engine::CTexture*>(Engine::Clone(RESOURCE_STAGE, L"Texture_Wall"));
		dynamic_cast<Engine::CResources*>(m_pTextureCom)->SetSaveType(RESOURCE_STAGE, L"Texture_Wall");
		tempName = L"Wall";
		break;
	default:
		break;
	}
	if (m_pTextureCom != nullptr)
	{
		for (int i = 0; i < dynamic_cast<Engine::CTexture*>(m_pTextureCom)->GetVecSize(); i++)
		{
			m_TexName = tempName;
			CString tempNum;
			tempNum.Format(_T("%d"), i);
			m_TexName+=tempNum;
			m_TexList.AddString(m_TexName);
			
		}
	}
}

void CTextureTool::TileRender()
{
	pToolView->GetDeviceClass()->Render_Begin(D3DXCOLOR(0.5f, 0.5f, 0.5f, 1.f));

	if (m_pSprite != nullptr)
	{
		if (m_TexList.GetCurSel() != -1)
		{
			m_pSprite->Begin(D3DXSPRITE_ALPHABLEND);
			D3DXMATRIX matScale, matTrans;
		
			LPDIRECT3DTEXTURE9 texInfo = (LPDIRECT3DTEXTURE9)(dynamic_cast<Engine::CTexture*>(m_pTextureCom)->Get_Texture(m_TexList.GetCurSel()));
			D3DXIMAGE_INFO	   tImgInfo = dynamic_cast<Engine::CTexture*>(m_pTextureCom)->Get_ImageInfo(m_TexList.GetCurSel());

			D3DXMatrixScaling(&matScale,
				(float)WINCX / tImgInfo.Width,
				(float)WINCY / tImgInfo.Height,
				0.f);

			D3DXMatrixTranslation(&matTrans,
				(float)WINCX * 0.5f,
				(float)WINCY * 0.5f,
				0.f);
		
			float fCenterX = tImgInfo.Width * 0.5f;
			float fCenterY = tImgInfo.Height * 0.5f;
			cout << fCenterX << " , " << fCenterY << endl;
			m_pSprite->SetTransform(&(matScale * matTrans));
			m_pSprite->Draw(texInfo, nullptr,
				&D3DXVECTOR3(fCenterX, fCenterY, 0.f), nullptr, D3DCOLOR_ARGB(255, 255, 255, 255));
		
			m_pSprite->End();
			texInfo->Release();
			
		}
	}
	//m_pDeviceMgr->Render_End(m_Picture.m_hWnd);
	pToolView->GetDeviceClass()->Render_End(m_TexImg.m_hWnd);


}


void CTextureTool::OnBnClickedComCancel()
{
	if (m_pTextureCom != nullptr)
		Engine::Safe_Release(m_pTextureCom);
	if (m_pSprite != nullptr)
	{
		m_pSprite->Release();
		m_pSprite = nullptr;
	}
	m_TexList.ResetContent();
	m_TexCombo.Clear();
	this->DestroyWindow();
}


BOOL CTextureTool::DestroyWindow()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	return CDialogEx::DestroyWindow();

}


void CTextureTool::OnDestroy()
{
	CDialogEx::OnDestroy();
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
}


void CTextureTool::OnSysCommand(UINT nID, LPARAM lParam)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	CDialogEx::OnSysCommand(nID, lParam);
	if (m_pTextureCom != nullptr)
		Engine::Safe_Release(m_pTextureCom);
	if (m_pSprite != nullptr)
	{
		m_pSprite->Release();
		m_pSprite = nullptr;
	}

	m_TexList.ResetContent();
	m_TexCombo.Clear();
}
