#include "pch.h"
#include "TempObject.h"
#include "MainFrm.h"
#include "Export_Function.h"

CTempObject::CTempObject(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CGameObject(pGraphicDev)
{

}

CTempObject::CTempObject(LPDIRECT3DDEVICE9 pGraphicDev, Engine::CGameObject * tempObject) : Engine::CGameObject(pGraphicDev)
{

	for (int i = 0; i < Engine::ID_END; i++)
	{
		auto iter = tempObject->GetMapCom(i).begin();
		for (; iter != tempObject->GetMapCom(i).end(); iter++)
		{
			if (wcscmp(iter->first.c_str(), L"Com_Buffer") == 0)
			{
				m_pBufferCom = dynamic_cast<Engine::CLightTex*>(Engine::Clone(RESOURCE_STATIC, L"Buffer_LightTex"));
				m_mapComponent[i].emplace(iter->first, m_pBufferCom);
			}
			else if(wcscmp(iter->first.c_str(), L"Com_Transform") == 0)
			{
				m_pTransformCom = Engine::CTransform::Clone(tempObject->GetTransformCom());
				m_mapComponent[i].emplace(iter->first, m_pTransformCom);
			}
			else if (wcscmp(iter->first.c_str(), L"Com_Renderer") == 0)
			{
				m_pRendererCom = Engine::Get_Renderer();
				NULL(m_pRendererCom, E_FAIL);
				m_pRendererCom->AddRef();
				m_mapComponent[i].emplace(iter->first, m_pRendererCom);
			}
			else if (wcscmp(iter->first.c_str(), L"Com_Texture") == 0)
			{
				m_pTextureCom = dynamic_cast<Engine::CTexture*>(Engine::Clone(iter->second));
				m_mapComponent[i].emplace(iter->first, m_pTextureCom);
				m_iTexCount = tempObject->Get_TexCount();
				
				lstrcpy(m_TexName, tempObject->Get_TexName());
			}
		}
	}
	m_ObjName = tempObject->Get_ObjName();
	m_ObjType = tempObject->Get_ObjType();
	m_MonsterType = tempObject->Get_MonsterType();
	m_ItemType = tempObject->Get_ItemType();
}

CTempObject::~CTempObject(void)
{
}

HRESULT CTempObject::Ready_GameObject(void)
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	D3DXCreateLine(m_pGraphicDev, &m_pLine);
	m_pLine->SetWidth(5.f);
	m_pLine->SetAntialias(true);

	return S_OK;
}

_int CTempObject::Update_GameObject(const _float& fTimeDelta)
{
	Engine::CGameObject::Update_GameObject(fTimeDelta);

	m_pRendererCom->Add_RenderGroup(Engine::RENDER_PRIORITY, this);

	return 0;
}

void CTempObject::Render_GameObject(void)
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, &m_pTransformCom->m_matWorld);
	if (m_pTextureCom != nullptr)
	{
		m_pTextureCom->Render_Texture(m_iTexCount);
	}
	else
	{
		m_pGraphicDev->SetTexture(0,nullptr);
	}
	m_pBufferCom->Render_Buffer();

	DrawLine();
}

HRESULT CTempObject::Ready_CopyGameObject(void)
{
	D3DXCreateLine(m_pGraphicDev, &m_pLine);
	m_pLine->SetWidth(5.f);
	m_pLine->SetAntialias(true);

	LPDIRECT3DDEVICE9 pd3dDevice = m_pGraphicDev;
	ULONG refCount = pd3dDevice->AddRef();
	pd3dDevice->Release();
	refCount--;
	cout << "레퍼런스 카운트 복사: "<< refCount << endl;

	return S_OK;
}

HRESULT CTempObject::Add_Component(void)
{

	Engine::CComponent*		pComponent = nullptr;

	pComponent = m_pBufferCom = dynamic_cast<Engine::CLightTex*>(Engine::Clone(RESOURCE_STATIC, L"Buffer_LightTex"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Buffer", pComponent);

	pComponent = m_pTransformCom = Engine::CTransform::Create();
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_DYNAMIC].emplace(L"Com_Transform", pComponent);


	//pComponent = m_pTextureCom = dynamic_cast<Engine::CTexture*>(Engine::Clone(RESOURCE_STAGE, L"Texture_Test"));
	//NULL_CHECK_RETURN(pComponent, E_FAIL);
	//m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Texture", pComponent);

	pComponent = m_pRendererCom = Engine::Get_Renderer();
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	pComponent->AddRef();
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Renderer", pComponent);

	return S_OK;
}

void CTempObject::DrawLine()
{
	float fLineLength = 70.f;

	if (m_bSelect)
	{
		_matrix m_matView, m_matProj;
		m_pGraphicDev->GetTransform(D3DTS_VIEW, &m_matView);
		m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &m_matProj);

		D3DXVECTOR3 pX[2] = { {0,0,0 },
						  {fLineLength,0,0} };
		D3DXVECTOR3 pY[2] = { {0,0,0 },
						  {0,fLineLength,0} };
		D3DXVECTOR3 pZ[2] = { {0,0,0 },
						  {0,0,fLineLength} };

		m_pLine->Begin();
		m_pLine->DrawTransform(pX, 2, &(m_pTransformCom->m_matWorld *m_matView* m_matProj), D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f)); 
		m_pLine->DrawTransform(pY, 2, &(m_pTransformCom->m_matWorld *m_matView* m_matProj), D3DXCOLOR(0.0f, 1.0f, 0.0f, 1.0f)); 
		m_pLine->DrawTransform(pZ, 2, &(m_pTransformCom->m_matWorld *m_matView* m_matProj), D3DXCOLOR(0.0f, 0.0f, 1.0f, 1.0f)); 
		m_pLine->End();
	}
}

void CTempObject::SetTexCount(int count)
{
	m_iTexCount = count;
}

void CTempObject::SetTestTexure(Engine::OBJECTTYPE objType)
{
	Engine::CComponent* pComponent = nullptr;
	CString tempstr;
	switch (objType)
	{
	case Engine::OBJ_TERRAIN:
		break;

	case Engine::OBJ_PLAYER:
		pComponent = m_pTextureCom = dynamic_cast<Engine::CTexture*>(Engine::Clone(RESOURCE_STAGE, L"Test_Player"));
		m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Texture", pComponent);
		break;
	case Engine::OBJ_MONSTER:
		switch (m_MonsterType)
		{
		case Engine::NO_MONSTER:
			break;
		case Engine::MONSTER_BAT:
			tempstr = L"Texture_Bat_Idle";
			break;
		case Engine::MONSTER_SPIDER:
			tempstr = L"Texture_Spider_Idle";
			break;
		case Engine::MONSTER_SHIELD:
			tempstr = L"Texture_Crusader_Idle";
			break;
		case Engine::MONSTER_ICE:
			tempstr = L"Texture_Glacier_Move";
			break;
		case Engine::MONSTER_MIDDLEBOSS:
			tempstr = L"Texture_Newshambler_Idle";
			break;
		case Engine::MONSTER_BOSS:
			break;
		case Engine::MONSTER_END:
			break;
		default:
			break;
		}
		pComponent = m_pTextureCom = dynamic_cast<Engine::CTexture*>(Engine::Clone(RESOURCE_STAGE, tempstr));
		m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Texture", pComponent);
		break;

	}
}


CTempObject* CTempObject::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CTempObject* pInstance = new CTempObject(pGraphicDev);
	if (FAILED(pInstance->Ready_GameObject()))
		Engine::Safe_Release(pInstance);

	
	return pInstance;
}

CTempObject * CTempObject::Create(LPDIRECT3DDEVICE9 pGraphicDev, Engine::CGameObject * object)
{
	CTempObject* pInstance = new CTempObject(pGraphicDev,object);
	if (FAILED(pInstance->Ready_CopyGameObject()))
		Engine::Safe_Release(pInstance);

	return pInstance;
}

const CString CTempObject::GetString() const
{
	// TODO: 여기에 반환 구문을 삽입합니다.
	return m_TexName;
}



void CTempObject::SetString(CString str)
{
	lstrcpy(m_TexName, str);
}


void CTempObject::Free(void)
{
	if(m_pLine != nullptr)
		m_pLine->Release();
	Engine::CGameObject::Free();

}

