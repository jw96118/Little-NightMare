
#include "Export_Utility.h"

USING(Engine)
IMPLEMENT_SINGLETON(CRenderer)

Engine::CRenderer::CRenderer(void) : m_pVB(nullptr), m_pIB(nullptr)
{


}

Engine::CRenderer::~CRenderer(void)
{
	Safe_Release(m_pVB);
	Safe_Release(m_pIB);
	Free();
}

void Engine::CRenderer::Add_RenderGroup(RENDERID eGroup, CGameObject* pGameObject)
{
	if (RENDER_END <= eGroup)
		return;

	m_RenderGroup[eGroup].push_back(pGameObject);

	pGameObject->AddRef();
}
HRESULT Engine::CRenderer::Ready_Renderer(LPDIRECT3DDEVICE9& pGraphicDev)
{
	FAILED_CHECK_RETURN(pGraphicDev->CreateVertexBuffer(sizeof(VTXSCREEN) * 4,
		0,
		FVF_SCREEN,
		D3DPOOL_MANAGED,
		&m_pVB,
		NULL),
		E_FAIL);


	FAILED_CHECK_RETURN(pGraphicDev->CreateIndexBuffer(sizeof(INDEX16) * 2,	// 생성할 인덱스 버퍼의 크기
		0,					// 버퍼의 종류(숫자가 0인 경우 정적 버퍼)
		D3DFMT_INDEX16,			// 생성하는 인덱스의 속성값
		D3DPOOL_MANAGED,	// 메모리 풀의 설정
		&m_pIB,
		NULL),
		E_FAIL);

	D3DVIEWPORT9		ViewPort;
	pGraphicDev->GetViewport(&ViewPort);

	VTXSCREEN*		pVertex = nullptr;

	m_pVB->Lock(0, 0, (void**)&pVertex, 0);

	pVertex[0].vPos = _vec4(0.f, 0.f, 0.f, 1.f);
	pVertex[0].vTexUV = _vec2(0.f, 0.f);

	pVertex[1].vPos = _vec4(_float(ViewPort.Width), 0.f, 0.f, 1.f);
	pVertex[1].vTexUV = _vec2(1.f, 0.f);

	pVertex[2].vPos = _vec4(_float(ViewPort.Width), _float(ViewPort.Height), 0.f, 1.f);
	pVertex[2].vTexUV = _vec2(1.f, 1.f);

	pVertex[3].vPos = _vec4(0.f, _float(ViewPort.Height), 0.f, 1.f);
	pVertex[3].vTexUV = _vec2(0.f, 1.f);

	m_pVB->Unlock();

	INDEX16*		pIndex = nullptr;

	m_pIB->Lock(0, 0, (void**)&pIndex, 0);

	// 0
	pIndex[0]._0 = 0;
	pIndex[0]._1 = 1;
	pIndex[0]._2 = 2;

	// 1
	pIndex[1]._0 = 0;
	pIndex[1]._1 = 2;
	pIndex[1]._2 = 3;

	m_pIB->Unlock();
	FAILED_CHECK_RETURN(pGraphicDev->CreateDepthStencilSurface(1920*4,
		1080*4,
		D3DFMT_D24S8,
		D3DMULTISAMPLE_NONE,
		0,
		TRUE,
		&m_pTargetSurface,
		NULL),
		E_FAIL);

	return S_OK;
}



void Engine::CRenderer::Render_GameObject(LPDIRECT3DDEVICE9& pGraphicDev)
{
	Render_Priority(pGraphicDev);

	Render_ShadowDepth(pGraphicDev);

	Render_Defferd(pGraphicDev);
	Render_LightAcc(pGraphicDev);

	Render_Blend(pGraphicDev);

	Render_Alpha(pGraphicDev);
	Render_UI(pGraphicDev);

	Engine::Render_DebugBuffer(L"MRT_Defferd");
	Engine::Render_DebugBuffer(L"MRT_LightAcc");
	Engine::Render_DebugBuffer(L"MRT_LightDepth");

	Clear_RenderGroup();
}

void Engine::CRenderer::Clear_RenderGroup(void)
{
	for (_uint i = 0; i < RENDER_END; ++i)
	{
		for_each(m_RenderGroup[i].begin(), m_RenderGroup[i].end(), CDeleteObj());
		m_RenderGroup[i].clear();
	}
}

void CRenderer::Render_Priority(LPDIRECT3DDEVICE9 & pGraphicDev)
{
	pGraphicDev->SetRenderState(D3DRS_LIGHTING, FALSE);
	pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);

	for (auto& iter : m_RenderGroup[RENDER_PRIORITY])
		iter->Render_GameObject();

	pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
	pGraphicDev->SetRenderState(D3DRS_LIGHTING, TRUE);
}

void CRenderer::Render_NonAlpha(LPDIRECT3DDEVICE9 & pGraphicDev)
{
	for (auto& iter : m_RenderGroup[RENDER_NONALPHA])
		iter->Render_GameObject();
}

_bool	Compare_Z(CGameObject* pDest, CGameObject* pSour)
{
	return pDest->Get_ViewZ() > pSour->Get_ViewZ();
}

void CRenderer::Render_Alpha(LPDIRECT3DDEVICE9 & pGraphicDev)
{
	m_RenderGroup[RENDER_ALPHA].sort(Compare_Z);

	for (auto& iter : m_RenderGroup[RENDER_ALPHA])
		iter->Render_GameObject();
}

void CRenderer::Render_UI(LPDIRECT3DDEVICE9 & pGraphicDev)
{
	pGraphicDev->SetRenderState(D3DRS_LIGHTING, FALSE);
	pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
	pGraphicDev->SetRenderState(D3DRS_ZENABLE, FALSE);
	pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	pGraphicDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	pGraphicDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	m_RenderGroup[RENDER_UI].sort(Compare_Z);
	for (auto& iter : m_RenderGroup[RENDER_UI])
		iter->Render_GameObject();

	pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
	pGraphicDev->SetRenderState(D3DRS_ZENABLE, TRUE);
	pGraphicDev->SetRenderState(D3DRS_LIGHTING, TRUE);
}

void Engine::CRenderer::Render_Defferd(LPDIRECT3DDEVICE9& pGraphicDev)
{
	Engine::Begin_MRT(L"MRT_Defferd");

	Render_NonAlpha(pGraphicDev);

	Engine::End_MRT(L"MRT_Defferd");
}
void Engine::CRenderer::Render_Blend(LPDIRECT3DDEVICE9& pGraphicDev)
{
	CShader*	pShader = dynamic_cast<Engine::CShader*>(Engine::Clone_Prototype(L"Shader_Blend"));
	NULL_CHECK(pShader);

	LPD3DXEFFECT pEffect = pShader->Get_EffectHandle();
	pEffect->AddRef();

	Engine::SetUp_OnShader(pEffect, L"Target_Albedo", "g_AlbedoTexture");
//	Engine::SetUp_OnShader(pEffect, L"Target_Light", "g_LightTexture");
	Engine::SetUp_OnShader(pEffect, L"Target_Shade", "g_ShadeTexture");
	Engine::SetUp_OnShader(pEffect, L"Target_Specular", "g_SpecularTexture");

	Engine::SetUp_OnShader(pEffect, L"Target_Depth", "g_DepthTexture");
	Engine::SetUp_OnShader(pEffect, L"Target_ShadowDepth", "g_ShadowDepthTexture");

	_matrix			matView, matProj;
	_matrix			matLightView;

	pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
	pGraphicDev->GetTransform(D3DTS_PROJECTION, &matProj);

	_matrix			LightProjMatrix;

	pEffect->SetMatrix("g_matViewInv", D3DXMatrixInverse(&matView, nullptr, &matView));
	pEffect->SetMatrix("g_matProjInv", D3DXMatrixInverse(&matProj, nullptr, &matProj));

	Engine::CGameObject* pCam = Engine::Get_GameObject(L"01.Environment", L"MainCamera");
	Engine::CGameObject* pPlayer = Engine::Get_GameObject(L"02.GameLogic", L"Dyn_Player");
	
	_vec3		vLightEye = _vec3(-5.f, 15.f, -5.f);
	_vec3		vLightAt = _vec3(60.f, 0.f, 60.f);
	_vec3		vLightUp = _vec3(0.f, 1.f, 0.f);
	if (pCam != nullptr && pPlayer != nullptr)
	{

		//_vec3 tempPos = pPlayer->Get_LightPos();
		_vec3 tempPos = Engine::Get_LightInfo(1)->Position;
		_vec3 test = pPlayer->Get_LightPos();
		D3DXVec3Normalize(&test, &test);
		test.y = 0.3f;
		vLightEye = pPlayer->Get_LightPos();
		//vLightEye = Engine::Get_LightInfo(2)->Position;
		vLightAt = *pPlayer->GetTransformCom()->Get_Info(Engine::INFO_POS);

	}
	D3DXMatrixLookAtLH(&matLightView, &vLightEye, &vLightAt, &vLightUp);
	pEffect->SetMatrix("g_matLightView", &matLightView);

	D3DXMatrixPerspectiveFovLH(&LightProjMatrix, D3DXToRadian(100.f), _float(1920) / _float(1080), 0.2f, 10000.f);
	pEffect->SetMatrix("g_matProj", &LightProjMatrix);


	pEffect->Begin(NULL, 0);
	pEffect->BeginPass(0);

	pGraphicDev->SetStreamSource(0, m_pVB, 0, sizeof(VTXSCREEN));
	pGraphicDev->SetFVF(FVF_SCREEN);
	pGraphicDev->SetIndices(m_pIB);
	pGraphicDev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 4, 0, 2);

	pEffect->EndPass();
	pEffect->End();

	Safe_Release(pEffect);
	Safe_Release(pShader);
}

void CRenderer::Render_ShadowDepth(LPDIRECT3DDEVICE9 & pGraphicDev)
{
	Engine::Begin_MRT(L"MRT_LightDepth");

	pGraphicDev->GetDepthStencilSurface(&m_pOldTargetSurface);
	pGraphicDev->SetDepthStencilSurface(m_pTargetSurface);

	pGraphicDev->Clear(0, 0, D3DCLEAR_ZBUFFER, D3DXCOLOR(1.f, 1.f, 1.f, 1.f), 1.f, 0);

	for (auto& iter : m_RenderGroup[RENDER_SHADOWDEPTH])
		iter->Render_ShadowDepth();

	Engine::End_MRT(L"MRT_LightDepth");

	pGraphicDev->SetDepthStencilSurface(m_pOldTargetSurface);
	Safe_Release(m_pOldTargetSurface);

}


void Engine::CRenderer::Free(void)
{
	Safe_Release(m_pTargetSurface);
	Safe_Release(m_pOldTargetSurface);
	Clear_RenderGroup();
}

void Engine::CRenderer::Render_LightAcc(LPDIRECT3DDEVICE9& pGraphicDev)
{
	Engine::Begin_MRT(L"MRT_LightAcc");

	CShader*	pShader = dynamic_cast<Engine::CShader*>(Engine::Clone_Prototype(L"Shader_Light"));
	NULL_CHECK(pShader);

	LPD3DXEFFECT pEffect = pShader->Get_EffectHandle();
	pEffect->AddRef();

	Engine::SetUp_OnShader(pEffect, L"Target_Normal", "g_NormalTexture");
	Engine::SetUp_OnShader(pEffect, L"Target_Depth", "g_DepthTexture");
	Engine::SetUp_OnShader(pEffect, L"Target_Pos", "g_PositionTexture");

	pEffect->Begin(NULL, 0);

	Engine::Render_Light(pEffect);

	pEffect->End();

	Engine::End_MRT(L"MRT_LightAcc");

	Safe_Release(pEffect);
	Safe_Release(pShader);
}
