#include "stdafx.h"
#include "Terrain.h"
#include "NaviMesh.h"
#include "Export_Function.h"

CTerrain::CTerrain(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CGameObject(pGraphicDev)
{

}

CTerrain::~CTerrain(void)
{

}

HRESULT CTerrain::Ready_GameObject(void)
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);
	return S_OK;
}

HRESULT CTerrain::LateReady_GameObject(void)
{
	return S_OK;
}

_int CTerrain::Update_GameObject(const _float& fTimeDelta)
{
	_int iExit = Engine::CGameObject::Update_GameObject(fTimeDelta);


	m_pRendererCom->Add_RenderGroup(Engine::RENDER_NONALPHA, this);

	return iExit;
}

void CTerrain::Render_GameObject(void)
{
	//m_pGraphicDev->SetTransform(D3DTS_WORLD, &m_pTransformCom->m_matWorld);

	//m_pTextureCom->Render_Texture();
	////m_pGraphicDev->SetTexture(0, nullptr);

	//if (FAILED(SetUp_Material()))
	//	return;
	//

	//m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, TRUE);
	//LPD3DXEFFECT	pEffect = m_pShaderCom->Get_EffectHandle();
	//NULL_CHECK(pEffect);
	//pEffect->AddRef();

	//_uint	iPassMax = 0;

	////m_pBufferCom->Copy_Indieces(m_pIndex, m_dwTriCnt);

	//FAILED_CHECK_RETURN(SetUp_ConstantTable(pEffect), );

	//pEffect->Begin(&iPassMax, 0);	// 1인자 : 현재 쉐이더 파일 갖고 있는 최대 패스의 개수

	//pEffect->BeginPass(0);

	////m_pBufferCom->Render_Buffer();
	//for (int i = 0; i < m_pNaviCom.size(); i++)
	//{
	//	//m_pNaviCom[i]->Render_NaviMesh();
	//}
	//pEffect->EndPass();

	//pEffect->End();

	//Safe_Release(pEffect);


}

void CTerrain::LoadNaviMesh(DWORD& dwBytes, const HANDLE& hFile,wstring comName)
{
	Engine::CComponent*		pComponent = nullptr;

	pComponent = dynamic_cast<Engine::CNaviMesh*>(Engine::Clone(RESOURCE_STAGE, L"Mesh_Navi"));
	NULL_CHECK(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_STATIC].emplace(comName, pComponent);
	m_pNaviCom.push_back(dynamic_cast<Engine::CNaviMesh*>(pComponent));
	
	_bool tempbool = false;
	_int  LoadCount = 0;
	_int  naviCount = 0;

	ReadFile(hFile, &LoadCount, sizeof(int), &dwBytes, nullptr);
	for (int i = 0; i < LoadCount; i++)
	{

		_vec3 vtlCol[3];
			ReadFile(hFile, &vtlCol[0], sizeof(_vec3), &dwBytes, nullptr);
			ReadFile(hFile, &vtlCol[1], sizeof(_vec3), &dwBytes, nullptr);
			ReadFile(hFile, &vtlCol[2], sizeof(_vec3), &dwBytes, nullptr);
	/*		for (int j = 0; j < 3; j++)
			{
				if (vtlCol[j].y <= 0.01f)
				{
					vtlCol[j].y = 0.f;
				}
			}*/
			Engine::CCell* tempCCell = Engine::CCell::Create(m_pGraphicDev, i, &vtlCol[0], &vtlCol[1], &vtlCol[2]);
			m_pNaviCom.back()->PushBack_Cell(tempCCell);
	}
	m_pNaviCom.back()->Link_Cell();
}

HRESULT CTerrain::Add_Component(void)
{
	Engine::CComponent*		pComponent = nullptr;

	pComponent = m_pBufferCom = dynamic_cast<Engine::CTerrainTex*>(Engine::Clone(RESOURCE_STATIC, L"Buffer_TerrainTex"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Buffer", pComponent);

	pComponent = m_pTransformCom = Engine::CTransform::Create();
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_DYNAMIC].emplace(L"Com_Transform", pComponent);



	pComponent = m_pRendererCom = Engine::Get_Renderer();
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	pComponent->AddRef();
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Renderer", pComponent);

	pComponent = m_pShaderCom = dynamic_cast<Engine::CShader*>(Engine::Clone_Prototype(L"Shader_Terrain"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Shader", pComponent);


	return S_OK;
}



HRESULT CTerrain::SetUp_ConstantTable(LPD3DXEFFECT & pEffect)
{
	_matrix			matWorld, matView, matProj;

	m_pTransformCom->Get_WorldMatrix(&matWorld);
	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
	m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &matProj);

	pEffect->SetMatrix("g_matWorld", &matWorld);
	pEffect->SetMatrix("g_matView", &matView);
	pEffect->SetMatrix("g_matProj", &matProj);

	m_pTextureCom->Set_Texture(pEffect, "g_BaseTexture");
	m_pTextureCom->Set_Texture(pEffect, "g_BaseTexture1", 1);
	m_pFilterCom->Set_Texture(pEffect, "g_FilterTexture");
	m_pAuraCom->Set_Texture(pEffect, "g_AuraTexture");

	pEffect->SetFloat("g_fDetail", 20.f);


	//const D3DLIGHT9*		pLight = Engine::Get_LightInfo(0);
	//NULL_CHECK_RETURN(pLight, S_OK);


	//pEffect->SetVector("g_vLightDir", &_vec4(pLight->Direction, 0.f));
	//pEffect->SetVector("g_vLightDiffuse", (_vec4*)&pLight->Diffuse);
	//pEffect->SetVector("g_vLightSpecular", (_vec4*)&pLight->Specular);
	//pEffect->SetVector("g_vLightAmbient", (_vec4*)&pLight->Ambient);


	//D3DMATERIAL9		tMtrlInfo;
	//ZeroMemory(&tMtrlInfo, sizeof(D3DMATERIAL9));

	//tMtrlInfo.Diffuse = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
	//tMtrlInfo.Specular = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
	//tMtrlInfo.Ambient = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
	//tMtrlInfo.Emissive = D3DXCOLOR(0.f, 0.f, 0.f, 1.f);
	//tMtrlInfo.Power = 100.f;

	//pEffect->SetVector("g_vMtrlDiffuse", (_vec4*)&tMtrlInfo.Diffuse);
	//pEffect->SetVector("g_vMtrlSpecular", (_vec4*)&tMtrlInfo.Specular);
	//pEffect->SetVector("g_vMtrlAmbient", (_vec4*)&tMtrlInfo.Ambient);

	//_vec4		vCamPos;
	//D3DXMatrixInverse(&matView, NULL, &matView);
	//memcpy(&vCamPos, &matView.m[3][0], sizeof(_vec4));

	//pEffect->SetVector("g_vCamPos", &vCamPos);
	//pEffect->SetFloat("g_fPower", tMtrlInfo.Power);


	_vec4		vPosition = _vec4(5.f, 0.f, 5.f, 1.f);

	Engine::CTransform*		pPlayerTransform = dynamic_cast<Engine::CTransform*>(Engine::Get_Component(L"GameLogic", L"Player", L"Com_Transform", Engine::ID_DYNAMIC));


	pEffect->SetVector("g_vAuraPos", &vPosition);

	_float		fRange = 5.f;
	pEffect->SetFloat("g_fRange", fRange);

	return S_OK;
}

CTerrain* CTerrain::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CTerrain*	pInstance = new CTerrain(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject()))
		Engine::Safe_Release(pInstance);

	return pInstance;
}

void CTerrain::Free(void)
{
	Engine::Safe_Delete_Array(m_pIndex);

	Engine::CGameObject::Free();
}

