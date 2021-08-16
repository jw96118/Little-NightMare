#include "GameObject.h"
#include "Transform.h"
#include "Renderer.h"
#include "Calculator.h"

#include "Export_Resources.h"


USING(Engine)
enum COMPONENT { COM_TEXTURE };
Engine::CGameObject::CGameObject(LPDIRECT3DDEVICE9 pGraphicDev)
	: m_pGraphicDev(pGraphicDev)
{
	m_pGraphicDev->AddRef();
	m_fViewZ = 0.5f;
	Set_Enable(true);
}

Engine::CGameObject::~CGameObject()
{

}

void CGameObject::Compute_ViewZ(const _vec3 * pPos)
{
	_matrix		matView;
	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
	D3DXMatrixInverse(&matView, NULL, &matView);

	_vec3	vCamPos;
	memcpy(&vCamPos, &matView.m[3][0], sizeof(_vec3));

	m_fViewZ = D3DXVec3Length(&(vCamPos - *pPos));
}

void CGameObject::Set_Enable(bool bEnable)
{
	m_bEnable = bEnable;

	if (m_bEnable)
		OnEnable_GameObject();
}

void CGameObject::Set_ObjType(OBJECTTYPE objType)
{
	m_ObjType = objType;
}

void CGameObject::Set_MonsterType(MONSTERTYPE monsterType)
{
	m_MonsterType = monsterType;
}

void CGameObject::Set_ItemType(ITEMTYPE itemType)
{
	m_ItemType = itemType;
}

void CGameObject::Set_ObjTypeName(const _tchar * objName)
{
	m_ObjTypeName = objName;
}

void CGameObject::Set_ObjName(const _tchar * objName)
{
	m_ObjName = objName;
}

void CGameObject::Set_MeshType(MESHTYPE eMeshType)
{
	m_eMeshType = eMeshType;
}

CComponent * CGameObject::Find_Component(const _tchar * pComponentTag, COMPONENTID eID)
{
	//auto	iter = find_if(m_mapComponent[eID].begin(), m_mapComponent[eID].end(), CTag_Finder(pComponentTag));
	auto	iter = m_mapComponent[eID].begin();

	for (; iter != m_mapComponent[eID].end(); iter++)
	{
		if (iter->first == pComponentTag)
			return iter->second;
	}
	return nullptr;
}

CComponent * CGameObject::Get_Component(const _tchar * pComponentTag, COMPONENTID eID)
{
	CComponent*		pComponent = Find_Component(pComponentTag, eID);
	NULL_CHECK_RETURN(pComponent, nullptr);

	return pComponent;
}

bool CGameObject::Get_Enable(void) const
{
	return m_bEnable;
}

bool * CGameObject::Get_EnableP(void)
{
	// TODO: 여기에 반환 구문을 삽입합니다.
	return &m_bEnable;
}

OBJECTTYPE CGameObject::Get_ObjType()
{
	return m_ObjType;
}

MONSTERTYPE CGameObject::Get_MonsterType()
{
	return m_MonsterType;
}

CCalculator * CGameObject::Get_Calculator()
{
	if (m_pCalculatorCom != nullptr)
		return m_pCalculatorCom;
	return nullptr;
}

ITEMTYPE CGameObject::Get_ItemType()
{
	return m_ItemType;
}

MESHTYPE CGameObject::Get_MeshType()
{
	return m_eMeshType;
}

wstring CGameObject::Get_ObjTypeName()
{
	return m_ObjTypeName;
}

wstring CGameObject::Get_ObjName()
{
	return m_ObjName;
}

wstring CGameObject::Get_MeshName()
{
	return m_sMeshName;
}

_vec3 CGameObject::Get_LightPos()
{
	return _vec3();
}

HRESULT Engine::CGameObject::Ready_GameObject(void)
{
	return S_OK;
}

HRESULT CGameObject::LateReady_GameObject(void)
{
	return S_OK;
}

_int Engine::CGameObject::Update_GameObject(const _float& fTimeDelta)
{
	for (auto& iter : m_mapComponent[ID_DYNAMIC])
		iter.second->Update_Component(fTimeDelta);

	return 0;
}

_int CGameObject::LateUpdate_GameObject(const _float & fTimeDelta)
{
	for (auto& iter : m_mapComponent[ID_DYNAMIC])
		iter.second->LateUpdate_Component(fTimeDelta);

	return 0;
}

void Engine::CGameObject::Render_GameObject(void)
{

}

void CGameObject::OnEnable_GameObject(void)
{
}

HRESULT CGameObject::Insert_Component(int index)
{
	CComponent* pComponent = nullptr;
	Engine::COMPONENTID tempComID;
	TCHAR tempStr[20] = {};
	switch (index)
	{
	case COM_TEXTURE:
		wcscpy_s(tempStr, L"Com_Texture");
		break;
	}
	for (int i = 0; i < Engine::ID_END; i++)
	{
		if (!m_mapComponent[i].empty())
		{
			auto iter = m_mapComponent[i].begin();

			for (; iter != m_mapComponent[i].end(); iter++)
			{
				if (iter->first == tempStr)
				{
					return E_FAIL;
				}
			}
		}
	}
	switch (index)
	{
	case COM_TEXTURE:
		pComponent = m_pTextureCom = nullptr;
		tempComID = Engine::ID_STATIC;
		m_mapComponent[tempComID].emplace(L"Com_Texture", pComponent);
		break;
	}
	return S_OK;
}

HRESULT CGameObject::Set_Insert_Component(const _tchar * str, CComponent * Com)
{
	bool failflag = false;
	for (int i = 0; i < Engine::ID_END; i++)
	{
		if (!m_mapComponent[i].empty())
		{
			auto iter = m_mapComponent[i].begin();

			for (; iter != m_mapComponent[i].end(); iter++)
			{
				if (iter->first == str)
				{
					if (iter->first == L"Com_Texture")
					{
					    iter->second = m_pTextureCom = dynamic_cast<Engine::CTexture*>(Engine::Clone(Com));
						failflag = false;
					}
				}
				else
				{
					failflag = true;
				}
			}
		}
	}
	if (failflag)
	{
		return E_FAIL;
	}
	return S_OK;
}

const map<wstring, CComponent*>* CGameObject::Get_MapComponent(COMPONENTID eID) const
{
	if (m_mapComponent[eID].empty())
	{
		return nullptr;
	}
	return m_mapComponent;
}
const int & CGameObject::Get_TexCount() const
{
	// TODO: 여기에 반환 구문을 삽입합니다.
	return m_iTexCount;
}

const _tchar * CGameObject::Get_TexName() const
{
	return m_TexName;
}

void CGameObject::Save_Component(DWORD& dwBytes, const HANDLE& hFile, bool bObjflag)
{
	size_t tempNameLen = (wcslen(m_ObjTypeName.c_str()) * sizeof(_tchar));
	WriteFile(hFile, &tempNameLen, sizeof(size_t), &dwBytes, nullptr);
	WriteFile(hFile, m_ObjTypeName.c_str(), tempNameLen * sizeof(_tchar), &dwBytes, nullptr);
	WriteFile(hFile, &m_ObjType, sizeof(OBJECTTYPE), &dwBytes, nullptr);
	WriteFile(hFile, &m_MonsterType, sizeof(MONSTERTYPE), &dwBytes, nullptr);
	WriteFile(hFile, &m_eMeshType, sizeof(MESHTYPE), &dwBytes, nullptr);
	for (int i = 0; i < ID_END; i++)
	{
		auto iter = m_mapComponent[i].begin();

		size_t tempSize = m_mapComponent[i].size();
		WriteFile(hFile, &tempSize, sizeof(size_t), &dwBytes, nullptr);
		for (; iter != m_mapComponent[i].end(); iter++)
		{
			size_t tempLen = (wcslen(iter->first.c_str()) * sizeof(_tchar));
			WriteFile(hFile, &tempLen, sizeof(size_t), &dwBytes, nullptr);
			WriteFile(hFile, iter->first.c_str(), tempLen * sizeof(_tchar), &dwBytes, nullptr);
			if (iter->first == L"Com_Transform")
			{
				m_pTransformCom->Save_Transform(dwBytes, hFile);
			}
			else if (iter->first == L"Com_Texture")
			{
				size_t tempTexLen = (wcslen(m_TexName) * sizeof(_tchar));

				WriteFile(hFile, &m_iTexCount, sizeof(size_t), &dwBytes, nullptr);
				WriteFile(hFile, &tempTexLen, sizeof(size_t), &dwBytes, nullptr);
				WriteFile(hFile, m_TexName, tempTexLen * sizeof(_tchar), &dwBytes, nullptr);
				bool flag = false;
				if (m_pTextureCom != nullptr)
				{
					flag = true;
					WriteFile(hFile, &flag, sizeof(bool), &dwBytes, nullptr);
					_ushort tempTexIdx = dynamic_cast<CTexture*>(iter->second)->m_wContainerIdx;
					size_t tempTagLen = (wcslen(dynamic_cast<CTexture*>(iter->second)->m_pResourceTag) * sizeof(_tchar));
					WriteFile(hFile, &tempTexIdx, sizeof(_ushort), &dwBytes, nullptr);
					WriteFile(hFile, &tempTagLen, sizeof(size_t), &dwBytes, nullptr);
					WriteFile(hFile, dynamic_cast<CTexture*>(iter->second)->m_pResourceTag, tempTagLen * sizeof(_tchar), &dwBytes, nullptr);
				}
				else
				{
					WriteFile(hFile, &flag, sizeof(bool), &dwBytes, nullptr);
				}
			}
			else if (iter->first == L"Com_Mesh")
			{
				size_t tempMeshLen = (wcslen(m_sMeshName.c_str()) * sizeof(_tchar));
				WriteFile(hFile, &tempMeshLen, sizeof(size_t), &dwBytes, nullptr);
				WriteFile(hFile, m_sMeshName.c_str(), tempMeshLen * sizeof(_tchar), &dwBytes, nullptr);
			}
		}
	}
}

CGameObject* CGameObject::Load_Component(DWORD& dwBytes, const HANDLE& hFile, bool bObjflag)
{
	wstring StempKey = L"";
	size_t tempNameLen = 0;
	size_t templen = 0;
	size_t tempSize = 0;
	size_t TexLen = 0;
	bool flag = false;
	_tchar* tempstr = nullptr;
	_ushort tempTexIdx = 0;
	size_t tempTagLen = 0;
	ReadFile(hFile, &tempNameLen, sizeof(size_t), &dwBytes, nullptr);
	ZeroMemory(m_tempObjName, 255 * sizeof(_tchar));
	ReadFile(hFile, m_tempObjName, tempNameLen * sizeof(_tchar), &dwBytes, nullptr);
	m_ObjTypeName = m_tempObjName;
	ReadFile(hFile, &m_ObjType, sizeof(OBJECTTYPE), &dwBytes, nullptr);
	ReadFile(hFile, &m_MonsterType, sizeof(MONSTERTYPE), &dwBytes, nullptr);
	ReadFile(hFile, &m_eMeshType, sizeof(MESHTYPE), &dwBytes, nullptr);

	for (int i = 0; i < ID_END; i++)
	{
		if (!m_mapComponent[i].empty())
		{
			for_each(m_mapComponent[i].begin(), m_mapComponent[i].end(), CDeleteMap());
			m_mapComponent[i].clear();
		}

		ReadFile(hFile, &tempSize, sizeof(size_t), &dwBytes, nullptr);
		for (int j = 0; j < tempSize; j++)
		{
			ReadFile(hFile, &templen, sizeof(size_t), &dwBytes, nullptr);

			ZeroMemory(m_LoadTempKey, 255 * sizeof(_tchar));

			ReadFile(hFile, m_LoadTempKey, templen * sizeof(_tchar), &dwBytes, nullptr);
			StempKey = m_LoadTempKey;
			if (StempKey == L"Com_Buffer")
			{
				m_pBufferCom = dynamic_cast<Engine::CLightTex*>(Engine::Clone(RESOURCE_STATIC, L"Buffer_LightTex"));
				m_mapComponent[i].emplace(StempKey, m_pBufferCom);
			}
			else if (StempKey == L"Com_Transform")
			{
				m_pTransformCom = Engine::CTransform::Create();
				m_pTransformCom->Load_Transform(dwBytes, hFile);
				m_mapComponent[i].emplace(StempKey, m_pTransformCom);
			}
			else if (StempKey == L"Com_Renderer")
			{
				m_pRendererCom = CRenderer::GetInstance();
				m_pRendererCom->AddRef();
				m_mapComponent[i].emplace(StempKey, m_pRendererCom);
			}
			else if (StempKey == L"Com_Texture")
			{
				ReadFile(hFile, &m_iTexCount, sizeof(size_t), &dwBytes, nullptr);

				ReadFile(hFile, &TexLen, sizeof(size_t), &dwBytes, nullptr);

				ZeroMemory(m_TexName, 255 * sizeof(_tchar));
				ReadFile(hFile, m_TexName, TexLen * sizeof(_tchar), &dwBytes, nullptr);

				ReadFile(hFile, &flag, sizeof(bool), &dwBytes, nullptr);
				if (flag)
				{

					ReadFile(hFile, &tempTexIdx, sizeof(_ushort), &dwBytes, nullptr);
					ReadFile(hFile, &tempTagLen, sizeof(size_t), &dwBytes, nullptr);
					tempstr = new _tchar[255];
					ZeroMemory(tempstr, 255 * sizeof(_tchar));
					ReadFile(hFile, tempstr, tempTagLen * sizeof(_tchar), &dwBytes, nullptr);

					m_pTextureCom = dynamic_cast<Engine::CTexture*>(Engine::Clone(tempTexIdx, tempstr));
					m_pTextureCom->SetSaveType(tempTexIdx, tempstr);
					m_mapComponent[i].emplace(StempKey, m_pTextureCom);
					delete[] tempstr;
				}
				else
				{
					m_mapComponent[i].emplace(StempKey, m_pTextureCom);
				}
			}
			else if (StempKey == L"Com_Calculator")
			{
				m_pCalculatorCom = CCalculator::Create(m_pGraphicDev);
				NULL_CHECK_RETURN(m_pCalculatorCom, nullptr);
				m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Calculator", m_pCalculatorCom);
			}
			else if (StempKey == L"Com_Mesh")
			{
				size_t tempMeshLen = 0;
				_tchar* tempMeshName = new _tchar[255];
				ReadFile(hFile, &tempMeshLen, sizeof(size_t), &dwBytes, nullptr);
				ReadFile(hFile, tempMeshName, tempMeshLen * sizeof(_tchar), &dwBytes, nullptr);
				m_sMeshName = tempMeshName;
				delete[] tempMeshName;
				switch (m_eMeshType)
				{
				case Engine::TYPE_STATIC:
					m_pStaticMeshCom = dynamic_cast<Engine::CStaticMesh*>(Engine::Clone(RESOURCE_STATIC, m_sMeshName.c_str()));
					NULL_CHECK_RETURN(m_pStaticMeshCom, nullptr);
					m_mapComponent[i].emplace(StempKey, m_pStaticMeshCom);
					break;
				case Engine::TYPE_DYNAMIC:
					m_pDynamicMeshCom = dynamic_cast<Engine::CDynamicMesh*>(Engine::Clone(RESOURCE_STATIC, m_sMeshName.c_str()));
					NULL_CHECK_RETURN(m_pDynamicMeshCom, nullptr);
					m_mapComponent[i].emplace(StempKey, m_pDynamicMeshCom);
					break;
				case Engine::TYPE_NAVI:
					break;
				}
			}
		}
	}



	return this;
}





//Engine::CTexture * CGameObject::GetTextureCom()
//{
//	if (m_pTextureCom != nullptr)
//		return m_pTextureCom;
//	return nullptr;
//}

Engine::CTransform * CGameObject::GetTransformCom()
{
	if (m_pTransformCom != nullptr)
		return m_pTransformCom;
	return nullptr;
}

map<wstring, CComponent*>& CGameObject::GetMapCom(int i)
{
	// TODO: 여기에 반환 구문을 삽입합니다.
	return m_mapComponent[i];
}

CGameObject * CGameObject::Create(LPDIRECT3DDEVICE9 device)
{
	CGameObject* pInstance = new CGameObject(device);
	if (FAILED(pInstance->Ready_GameObject()))
		return nullptr;
	return pInstance;
}

void Engine::CGameObject::Free(void)
{

	for (_uint i = 0; i < ID_END; ++i)
	{
		for_each(m_mapComponent[i].begin(), m_mapComponent[i].end(), CDeleteMap());
		m_mapComponent[i].clear();
	}
	Safe_Release(m_pGraphicDev);
}

void CGameObject::SetSelect(bool flag)
{
	m_bSelect = flag;
}

