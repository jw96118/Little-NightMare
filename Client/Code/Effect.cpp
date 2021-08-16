#include "stdafx.h"
#include "Player.h"
#include "Janitor.h"
#include "Lighter.h"
#include "Effect.h"
#include "InteractionObject.h"
#include "Export_Function.h"

CEffect::CEffect(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CGameObject(pGraphicDev)
{

}

CEffect::~CEffect(void)
{

}

HRESULT CEffect::Ready_GameObject(void)
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);


}

HRESULT CEffect::LateReady_GameObject(void)
{
	switch (m_EffectType)
	{
	case CEffect::FIRE:
		m_pPlayer = dynamic_cast<CPlayer*>(Engine::Get_GameObject(L"02.GameLogic", L"Dyn_Player"));
		m_pLighter = dynamic_cast<CLighter*>(Engine::Get_GameObject(L"Lighter", L"Mesh_Lighter"));
		m_pTransformCom->Set_Scale(0.1f, 0.1f, 0.1f);

		break;
	case CEffect::BLOOD1:
	case CEffect::BLOOD2:
		m_pJanitor = dynamic_cast<CJanitor*>(Engine::Get_GameObject(L"02.GameLogic", L"Dyn_Monster_Mesh_Janitor_6"));
	    m_pCage = dynamic_cast<CInteractionObject*>(Engine::Get_GameObject(L"02.GameLogic", L"Dyn_Obj_BossCage_13"));
		m_fFrameMax = 32.f;
		m_pTransformCom->Set_Scale(10.f, 10.f, 10.f);
		m_vRGB = { 1.f, 0.f, 0.f, 0.f };
		break;
	}
	return S_OK;
}

_int CEffect::Update_GameObject(const _float& fTimeDelta)
{
	switch (m_EffectType)
	{
	case CEffect::FIRE:
		if (m_pPlayer->Get_LighterFlag())
		{
			m_bDraw = true;
			m_pTransformCom->Set_Pos(m_pLighter->Get_EffectPos());
		}
		else
			m_bDraw = false;
		break;
	case CEffect::BLOOD1:
		if (m_pCage->Get_CageState() == CAGESTATE::crush3)
		{
			if (!m_bEffectEnd)
			{
				m_bDraw = true;
				m_pTransformCom->Set_Pos(m_pJanitor->Get_BloodEffectPos((int)m_EffectType));
				//m_pTransformCom->Set_Pos(m_pLighter->Get_EffectPos());
				m_fFrameCnt += m_fFrameMax * fTimeDelta;
				if (m_fFrameCnt >= m_fFrameMax)
				{
					m_bEffectEnd = true;
					m_fFrameCnt = 0;
					m_bDraw = false;
				}
			}
			else
				m_bDraw = false;
		}
		break;
	case CEffect::BLOOD2:
		if (m_pCage->Get_CageState() == CAGESTATE::crush3)
		{
			if (!m_bEffectEnd)
			{
				m_bDraw = true;
				m_pTransformCom->Set_Pos(m_pJanitor->Get_BloodEffectPos((int)m_EffectType));
				m_fFrameCnt += m_fFrameMax * fTimeDelta;
				if (m_fFrameCnt >= m_fFrameMax)
				{
					m_bEffectEnd = true;
					m_fFrameCnt = 0;
					m_bDraw = false;
				}
			}
			else
				m_bDraw = false;
		}
		break;
	}
	



	Engine::CGameObject::Update_GameObject(fTimeDelta);	



	Engine::CGameObject::Compute_ViewZ(&m_pTransformCom->m_vInfo[Engine::INFO_POS]);

	m_pRendererCom->Add_RenderGroup(Engine::RENDER_ALPHA, this);

	return 0;
}

void CEffect::Render_GameObject(void)
{
	if (!m_bDraw)
		return;
	LPD3DXEFFECT	pEffect = m_pShaderCom->Get_EffectHandle();
	NULL_CHECK(pEffect);
	pEffect->AddRef();

	_uint	iPassMax = 0;

	FAILED_CHECK_RETURN(SetUp_ConstantTable(pEffect), );

	pEffect->Begin(&iPassMax, 0);

	pEffect->BeginPass(0);

	m_pBufferCom->Render_Buffer();

	pEffect->EndPass();

	pEffect->End();

	Safe_Release(pEffect);
}

HRESULT CEffect::Add_Component(void)
{
	Engine::CComponent*		pComponent = nullptr;

	pComponent = m_pBufferCom = dynamic_cast<Engine::CRcTex*>(Engine::Clone(RESOURCE_STATIC, L"Buffer_RcTex"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Buffer", pComponent);

	pComponent = m_pTransformCom = Engine::CTransform::Create();
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_DYNAMIC].emplace(L"Com_Transform", pComponent);
	switch (m_EffectType)
	{
	case CEffect::FIRE:
		pComponent = m_pTextureCom = dynamic_cast<Engine::CTexture*>(Engine::Clone(RESOURCE_STAGE, L"Texture_Effect_Fire"));
		NULL_CHECK_RETURN(pComponent, E_FAIL);
		m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Texture", pComponent);
		break;
	case CEffect::BLOOD1:
	case CEffect::BLOOD2:
		pComponent = m_pTextureCom = dynamic_cast<Engine::CTexture*>(Engine::Clone(RESOURCE_STAGE, L"Texture_Effect_Blood"));
		NULL_CHECK_RETURN(pComponent, E_FAIL);
		m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Texture", pComponent);
		break;
	}

	pComponent = m_pRendererCom = Engine::Get_Renderer();
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	pComponent->AddRef();
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Renderer", pComponent);
	
	pComponent = m_pShaderCom = dynamic_cast<Engine::CShader*>(Engine::Clone_Prototype(L"Shader_Effect"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Shader", pComponent);
	
	return S_OK;
}



HRESULT CEffect::SetUp_ConstantTable(LPD3DXEFFECT& pEffect)
{
	_matrix			matWorld, matView, matProj;

	_matrix	        matBill;

	m_pTransformCom->Get_WorldMatrix(&matWorld);
	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
	m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &matProj);

	D3DXMatrixIdentity(&matBill);

	matBill._11 = matView._11;
	matBill._13 = matView._13;
	matBill._31 = matView._31;
	matBill._33 = matView._33;

	D3DXMatrixInverse(&matBill, NULL, &matBill);


	pEffect->SetMatrix("g_matWorld", &(matBill * matWorld));
	pEffect->SetMatrix("g_matView", &matView);
	pEffect->SetMatrix("g_matProj", &matProj);

	m_pTextureCom->Set_Texture(pEffect, "g_BaseTexture", _uint(m_fFrameCnt));
	Engine::SetUp_OnShader(pEffect, L"Target_Depth", "g_DepthTexture");
	pEffect->SetVector("g_vEffectRGB", &m_vRGB);
	return S_OK;
}

CEffect* CEffect::Create(LPDIRECT3DDEVICE9 pGraphicDev, TYPE type  )
{
	CEffect*	pInstance = new CEffect(pGraphicDev);
	pInstance->m_EffectType = type;
	if (FAILED(pInstance->Ready_GameObject()))
		Engine::Safe_Release(pInstance);
	
	return pInstance;
}

void CEffect::Free(void)
{


	Engine::CGameObject::Free();
}

