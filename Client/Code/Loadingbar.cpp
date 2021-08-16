#include "stdafx.h"
#include "LoadingBar.h"
#include "Export_Function.h"
#include "DynamicCamera.h"
#include "Camera.h"
CLoadingBar::CLoadingBar(LPDIRECT3DDEVICE9 pGraphicDev, wstring wstrTex, _vec3 vPos, _float fScale, _int iPivot, _float fLoad)
	:Engine::CGameObject(pGraphicDev),
	m_vPos(vPos),
	m_fScale(fScale),
	m_iPivot(iPivot),
	m_wstrTexture(wstrTex),
	m_fLoad(fLoad)
{

	D3DXMatrixIdentity(&m_MatWorld);
	D3DXMatrixIdentity(&m_MatView);
	m_fViewZ = vPos.z;
}

CLoadingBar::~CLoadingBar(void)
{
}

void CLoadingBar::Set_Load(const float & fLoad)
{
	m_fLoad = fLoad;
}



HRESULT CLoadingBar::Ready_GameObject(void)
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);
	CalculateTransform();

	return S_OK;
}

_int CLoadingBar::Update_GameObject(const _float & fTimeDelta)
{
	Engine::CGameObject::Update_GameObject(fTimeDelta);

	m_pRendererCom->Add_RenderGroup(Engine::RENDER_UI, this);
	



	return 0;
}

void CLoadingBar::Render_GameObject(void)
{

	m_pBufferCom->SendToClient(0.0f, m_fLoad, 0.0f, 1.0f);

	LPD3DXEFFECT	pEffect = m_pShaderCom->Get_EffectHandle();
	NULL_CHECK(pEffect);
	pEffect->AddRef();

	_uint	iPassMax = 0;


	FAILED_CHECK_RETURN(SetUp_ConstantTable(pEffect), );

	pEffect->Begin(&iPassMax, 0);	// 1인자 : 현재 쉐이더 파일 갖고 있는 최대 패스의 개수

	pEffect->BeginPass(0);

	m_pBufferCom->Render_Buffer();

	pEffect->EndPass();

	pEffect->End();

	Engine::Safe_Release(pEffect);

	m_pBufferCom->SendToClient(0.0f, 1.0f, 0.0f, 1.0f);


}



HRESULT CLoadingBar::SetUp_ConstantTable(LPD3DXEFFECT& pEffect)
{

	_matrix			matWorld, matView, matProj;

	D3DXMatrixIdentity(&matView);
	D3DXMatrixIdentity(&matWorld);


	matWorld._11 = m_pTextureCom->Get_ImageInfo().Width  * m_fScale;
	matWorld._22 = m_pTextureCom->Get_ImageInfo().Height * m_fScale;
	matWorld._33 = 1.f;

	matWorld._41 = m_vPos.x;
	matWorld._42 = m_vPos.y;
	matWorld._43 = m_vPos.z;

	matWorld._41 += (matWorld._11*0.5f);
	matWorld._42 -= (matWorld._22*0.5f);



	D3DXMatrixOrthoLH(&matProj, WINCX, WINCY, 0.f, 1.f);
	pEffect->SetMatrix("g_matWorld", &matWorld);
	pEffect->SetMatrix("g_matView", &matView);
	pEffect->SetMatrix("g_matProj", &matProj);
	pEffect->SetFloat("g_Alpha", 1.f);
	pEffect->SetFloat("g_Load", true);

	m_pTextureCom->Set_Texture(pEffect, "g_BaseTexture", 0);

	return S_OK;
}






HRESULT CLoadingBar::Add_Component(void)
{
	Engine::CComponent*		pComponent = nullptr;

	//pComponent = m_pBufferCom = dynamic_cast<Engine::CRcTex*>(Engine::Clone(RESOURCE_STATIC, L"Buffer_RcTex"));
	//NULL_CHECK_RETURN(pComponent, E_FAIL);
	//m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Buffer", pComponent);


	//pComponent = m_pTransformCom = Engine::CTransform::Create();
	//NULL_CHECK_RETURN(pComponent, E_FAIL);
	//m_mapComponent[Engine::ID_DYNAMIC].emplace(L"Com_Transform", pComponent);


	//pComponent = m_pTextureCom = dynamic_cast<Engine::CTexture*>(Engine::Clone(RESOURCE_LOGO, m_wstrTexture.c_str()));
	//NULL_CHECK_RETURN(pComponent, E_FAIL);
	//m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Texture", pComponent);

	//pComponent = m_pRendererCom = Engine::Get_Renderer();
	//NULL_CHECK_RETURN(pComponent, E_FAIL);
	//pComponent->AddRef();
	//m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Renderer", pComponent);
	//
	//pComponent = m_pShaderCom = dynamic_cast<Engine::CShader*>(Engine::Clone_Prototype(L"Shader_Sample"));
	//NULL_CHECK_RETURN(pComponent, E_FAIL);
	//m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Shader", pComponent);





	pComponent = m_pBufferCom = dynamic_cast<Engine::CRcTex*>(Engine::Clone(RESOURCE_STATIC, L"Buffer_RcTex"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Buffer", pComponent);

	pComponent = m_pTransformCom = Engine::CTransform::Create();
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_DYNAMIC].emplace(L"Com_Transform", pComponent);

	pComponent = m_pTextureCom = dynamic_cast<Engine::CTexture*>(Engine::Clone(RESOURCE_LOGO, m_wstrTexture.c_str()));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Texture", pComponent);

	pComponent = m_pRendererCom = Engine::Get_Renderer();
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	pComponent->AddRef();
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Renderer", pComponent);

	pComponent = m_pShaderCom = dynamic_cast<Engine::CShader*>(Engine::Clone_Prototype(L"Shader_Sample"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Shader", pComponent);







	return S_OK;

}

void CLoadingBar::CalculateTransform()
{
	SetScale();
	SetPos();
	SetPivot();
}

void CLoadingBar::SetPivot()
{
	switch (m_iPivot)
	{
	case 0:
		m_MatView._41 += (m_MatView._11*0.5f);
		m_MatView._42 -= (m_MatView._22*0.5f);
		break;
	case 1:
		m_MatView._41 -= (m_MatView._11*0.5f);
		m_MatView._42 -= (m_MatView._22*0.5f);
		break;
	case 2:
		m_MatView._41 -= (m_MatView._11*0.5f);
		m_MatView._42 += (m_MatView._22*0.5f);
		break;
	case 3:
		m_MatView._41 += (m_MatView._11*0.5f);
		m_MatView._42 += (m_MatView._22*0.5f);
		break;
	default:
		break;
	}
}

void CLoadingBar::SetScale()
{
	m_MatView._11 *= m_pTextureCom->Get_ImageInfo().Width  * m_fScale;
	m_MatView._22 *= m_pTextureCom->Get_ImageInfo().Height * m_fScale;
	m_MatView._33 = 1.f;
}

void CLoadingBar::SetPos()
{	
	memcpy(&(m_MatView._41), m_vPos, sizeof(_float) * 3);
}

CLoadingBar * CLoadingBar::Create(LPDIRECT3DDEVICE9 pGraphicDev, wstring wstrTex , _vec3 vPos, _float fScale,_int iPivot ,_float Load)
{
	CLoadingBar*	pInstance = new CLoadingBar(pGraphicDev,wstrTex, vPos, fScale,iPivot,Load);

	if (FAILED(pInstance->Ready_GameObject()))
		Engine::Safe_Release(pInstance);

	return pInstance;
}



void CLoadingBar::Free(void)
{
	Engine::CGameObject::Free();

}
