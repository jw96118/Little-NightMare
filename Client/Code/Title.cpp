#include "stdafx.h"
#include "Export_Function.h"
#include "DynamicCamera.h"
#include "FirstPersonCamera.h"
#include "Title.h"

CTitle::CTitle(LPDIRECT3DDEVICE9 pGraphicDev, wstring wstrTex,_vec2 fScale, _vec3 vPos)
	:Engine::CGameObject(pGraphicDev),m_wstrTexture(wstrTex),m_vScale(fScale),m_vPos(vPos), m_fAlpha(1.f)
{

	D3DXMatrixIdentity(&m_MatView);
	D3DXMatrixIdentity(&m_MatView);
	m_fViewZ = vPos.z;
	m_fX = vPos.x;
	m_fY = vPos.y;
}

CTitle::~CTitle(void)
{
}

void CTitle::Set_TexAlpha(_float fAlpha)
{
	m_fAlpha = fAlpha;
}

void CTitle::Set_TexX(_float fTextureX)
{
	m_fTextureX = fTextureX;
}

void CTitle::Set_Pos(_vec2 vPos)
{
	m_fX = vPos.x;
	m_fY = vPos.y;
}

HRESULT CTitle::Ready_GameObject(void)
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);


	

	m_fTextureX = 1.f;
	m_fAlpha = 1.f;

	m_fSizeX = m_vScale.x;
	m_fSizeY = m_vScale.y;

	return S_OK;
}

HRESULT CTitle::LateReady_GameObject(void)
{
	m_pCam = dynamic_cast<CFirstPersonCamera*>(Engine::Get_GameObject(L"01.Environment", L"MainCamera"));
	
	return S_OK;
}

_int CTitle::Update_GameObject(const _float& fTimeDelta)
{
	Engine::CGameObject::Update_GameObject(fTimeDelta);

	m_pRendererCom->Add_RenderGroup(Engine::RENDER_ALPHA, this);
	//D3DXMatrixOrthoLH(&m_matProj, WINCX, WINCY, 0.f, 1.f);
	m_fSizeX = m_vScale.x * m_fTextureX;

	return 0;
}

void CTitle::Render_GameObject(void)
{
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


}

HRESULT CTitle::Add_Component(void)
{
	Engine::CComponent*		pComponent = nullptr;

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



HRESULT CTitle::SetUp_ConstantTable(LPD3DXEFFECT& pEffect)
{

	_matrix			matWorld, matView, matProj;

	//m_pTransformCom->Get_WorldMatrix(&matWorld);
	//m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
	D3DXMatrixIdentity(&matView);
	D3DXMatrixIdentity(&matWorld);
	matWorld._11 = m_fSizeX;
	matWorld._22 = m_fSizeY;
	matWorld._33 = 1.f;
	matWorld._41 = m_fX - WINCX * 0.5f;
	matWorld._42 = -m_fY + WINCY * 0.5f;
	//m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &matProj);
	D3DXMatrixOrthoLH(&matProj, WINCX, WINCY, 0.f, 1.f);
	pEffect->SetMatrix("g_matWorld", &matWorld);
	pEffect->SetMatrix("g_matView", &matView);
	pEffect->SetMatrix("g_matProj", &matProj);
	pEffect->SetFloat("g_Alpha", m_fAlpha);
	pEffect->SetFloat("g_Load", false);

	m_pTextureCom->Set_Texture(pEffect, "g_BaseTexture", 0);

	return S_OK;
}

CTitle* CTitle::Create(LPDIRECT3DDEVICE9 pGraphicDev, wstring wstr,_vec2 vScale, _vec3 vPos)
{
	CTitle*	pInstance = new CTitle(pGraphicDev, wstr, vScale,vPos);

	if (FAILED(pInstance->Ready_GameObject()))
		Engine::Safe_Release(pInstance);

	return pInstance;
}

void CTitle::Free(void)
{


	Engine::CGameObject::Free();
}

