#include "stdafx.h"
#include "TestPlayer.h"

#include "Export_Function.h"

CTestPlayer::CTestPlayer(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CGameObject(pGraphicDev)
	, m_vDir(0.f, 0.f, 0.f)
	, m_fSpeed(0.f)
{

}

CTestPlayer::~CTestPlayer(void)
{

}

void CTestPlayer::Set_Camera(Engine::CGameObject* pCamera)
{
	m_pCamera = pCamera;
}

HRESULT CTestPlayer::Ready_GameObject(void)
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	m_fSpeed = 20.f;

	// temp
	m_vPos = { 0.f, 0.f, 0.f };
	m_vTarget = { 50.f, 0.f, 0.f };

	return S_OK;
}

_int CTestPlayer::Update_GameObject(const _float& fTimeDelta)
{
	SetUp_OnTerrain();
	Key_Input(fTimeDelta);

	Engine::CGameObject::Update_GameObject(fTimeDelta);


	m_pRendererCom->Add_RenderGroup(Engine::RENDER_UI, this);


	// temp
	//if (fabsf(D3DXVec3Length(&m_vTarget) - D3DXVec3Length(&m_vPos)) > 1.f)
	//{
	//	_vec3 vPos = m_vTarget - m_vPos;
	//	m_vPos = m_vPos + vPos * fTimeDelta * 4.f;
	//}
	//else
	//{
	//	m_vPos = m_vTarget;
	//	int i = 4;
	//	i = -5;
	//}
		

	return NO_EVENT;
}

void CTestPlayer::Render_GameObject(void)
{
	_matrix matView;
	D3DXMatrixScaling(&matView, 128.f, 128.f, 1.f);
	
	memcpy(&matView._41, &m_vPos, sizeof(_vec3));



	m_pGraphicDev->SetTransform(D3DTS_VIEW, &matView);


	//
	m_pGraphicDev->SetTransform(D3DTS_WORLD, &m_pTransformCom->m_matWorld);
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	/*m_pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	m_pGraphicDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	m_pGraphicDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);*/

	//Engine::ReceiveToEngine(Engine::RESOURCE_STATIC, L"Buffer_RcTex", m_pConvertVtx);


	m_pTextureCom->Render_Texture();

	m_pBufferCom->Render_Buffer();

	//m_pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

HRESULT CTestPlayer::Add_Component(void)
{
	Engine::CComponent*		pComponent = nullptr;

	pComponent = m_pBufferCom = dynamic_cast<Engine::CRcTex*>(Engine::Clone(RESOURCE_STATIC, L"Buffer_RcTex"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Buffer", pComponent);

	pComponent = m_pTransformCom = Engine::CTransform::Create();
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_DYNAMIC].emplace(L"Com_Transform", pComponent);

	pComponent = m_pTextureCom = dynamic_cast<Engine::CTexture*>(Engine::Clone(RESOURCE_STAGE, L"Texture_Player"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Texture", pComponent);

	pComponent = m_pRendererCom = Engine::Get_Renderer();
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	pComponent->AddRef();
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Renderer", pComponent);

	pComponent = m_pCalculatorCom = Engine::CCalculator::Create(m_pGraphicDev);
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Calculator", pComponent);
	return S_OK;
}

void CTestPlayer::Key_Input(const _float& fTimeDelta)
{
	m_pTransformCom->Get_Info(Engine::INFO_LOOK, &m_vDir);

	if (GetAsyncKeyState(VK_UP) & 0x8000)
	{
		D3DXVec3Normalize(&m_vDir, &m_vDir);
		m_pTransformCom->Move_Pos(&(m_vDir * m_fSpeed * fTimeDelta));
	}

	if (GetAsyncKeyState(VK_DOWN) & 0x8000)
	{
		D3DXVec3Normalize(&m_vDir, &m_vDir);
		m_pTransformCom->Move_Pos(&(m_vDir * -m_fSpeed * fTimeDelta));
	}

	if (GetAsyncKeyState(VK_LEFT) & 0x8000)
		m_pTransformCom->Rotation(Engine::ROT_Y, D3DXToRadian(90.f * fTimeDelta));

	if (GetAsyncKeyState(VK_RIGHT) & 0x8000)
		m_pTransformCom->Rotation(Engine::ROT_Y, D3DXToRadian(-90.f * fTimeDelta));


}

CTestPlayer* CTestPlayer::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CTestPlayer*	pInstance = new CTestPlayer(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject()))
		Engine::Safe_Release(pInstance);

	return pInstance;
}

void CTestPlayer::Free(void)
{
	Engine::CGameObject::Free();
}

void CTestPlayer::SetUp_OnTerrain(void)
{
	_vec3		vPos;
	m_pTransformCom->Get_Info(Engine::INFO_POS, &vPos);

	Engine::CTerrainTex*		pTerrainBufferCom = dynamic_cast<Engine::CTerrainTex*>
		(Engine::Get_Component(L"GameLogic", L"Terrain", L"Com_Buffer", Engine::ID_STATIC));
	NULL_CHECK(pTerrainBufferCom);

	_float	fHeight = m_pCalculatorCom->Compute_HeightOnTerrain(&vPos,
		pTerrainBufferCom->Get_VtxPos(),
		pTerrainBufferCom->Get_VtxCntX(),
		pTerrainBufferCom->Get_VtxCntZ());

	m_pTransformCom->Set_Pos(vPos.x, fHeight, vPos.z);
}
