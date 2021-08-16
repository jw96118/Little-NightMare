#ifndef GameObject_h__
#define GameObject_h__

#include "Engine_Defines.h"
#include "Base.h"

#include "Component.h"

BEGIN(Engine)
class CTexture;
class CLightTex;
class CTransform;
class CRenderer;
class CStaticMesh;
class CDynamicMesh;
class CCalculator;
class CCollider;
class CShader;
class ENGINE_DLL CGameObject : public CBase
{
protected:
	explicit CGameObject(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CGameObject();

public:
	_float				Get_ViewZ(void) const { return m_fViewZ; }
	void				Compute_ViewZ(const _vec3* pPos);

	virtual void		Set_Enable(bool bEnable);
	virtual void        Set_ObjType(OBJECTTYPE objType);
	virtual void        Set_MonsterType(MONSTERTYPE monsterType);
	virtual void        Set_ItemType(ITEMTYPE itemType);
	virtual void        Set_ObjTypeName(const _tchar* objName);
	virtual void        Set_ObjName(const _tchar* objName);
	void        Set_MeshType(MESHTYPE eMeshType);
public:
	CComponent*			Get_Component(const _tchar* pComponentTag, COMPONENTID eID);

	bool				Get_Enable(void) const;
	bool*				Get_EnableP(void);
	OBJECTTYPE          Get_ObjType();
	MONSTERTYPE         Get_MonsterType();
	CCalculator*        Get_Calculator();
	ITEMTYPE            Get_ItemType();
	MESHTYPE            Get_MeshType();
	wstring             Get_ObjTypeName();
	wstring             Get_ObjName();
	wstring             Get_MeshName();
public:
	virtual HRESULT		Ready_GameObject(void);
	virtual HRESULT		LateReady_GameObject(void);
	virtual _int		Update_GameObject(const _float& fTimeDelta);
	virtual _int		LateUpdate_GameObject(const _float& fTimeDelta);
	virtual void		Render_GameObject(void);
	virtual void		Render_ShadowDepth(void) { }
	virtual _vec3       Get_LightPos();
	virtual	void		OnEnable_GameObject(void);

	//툴에서 사용
public:
	HRESULT             Insert_Component(int index);
	HRESULT             Set_Insert_Component(const _tchar* str, CComponent* Com);
	const map<wstring, CComponent*>* Get_MapComponent(COMPONENTID eID) const;
	const int&          Get_TexCount() const;
	const _tchar*       Get_TexName() const;
	void                Save_Component(DWORD& dwBytes, const HANDLE& hFile,bool flag);
	CGameObject*        Load_Component(DWORD& dwBytes, const HANDLE& hFile, bool flag);
	Engine::CTexture*   GetTextureCom();
	Engine::CTransform* GetTransformCom();
	map<wstring, CComponent*>&	GetMapCom(int i);
	static  CGameObject* Create(LPDIRECT3DDEVICE9 device);
private:
	CComponent*			Find_Component(const _tchar* pComponentTag, COMPONENTID eID);

protected:
	LPDIRECT3DDEVICE9						m_pGraphicDev;
	map<wstring, CComponent*>			    m_mapComponent[ID_END];
	bool									m_bEnable;
	//오브젝트 종류 구분
	wstring                                 m_ObjTypeName = L"";
	wstring                                 m_ObjName = L"";
	wstring                                 m_sMeshName = L"";
	OBJECTTYPE                              m_ObjType;
	MONSTERTYPE                             m_MonsterType = NO_MONSTER;
	ITEMTYPE                                m_ItemType;
	MESHTYPE                                m_eMeshType = NOT_MESH;
	_float									m_fViewZ;
	Engine::CTexture*                       m_pTextureCom = nullptr;
	Engine::CLightTex*	                    m_pBufferCom = nullptr;
	Engine::CTransform*		                m_pTransformCom = nullptr;
	Engine::CCalculator*                    m_pCalculatorCom = nullptr;
	Engine::CRenderer*		                m_pRendererCom = nullptr;
	Engine::CStaticMesh*                    m_pStaticMeshCom = nullptr;
	Engine::CDynamicMesh*                   m_pDynamicMeshCom = nullptr;
	Engine::CShader*		m_pShaderCom = nullptr;

	vector<Engine::CCollider*>              m_vecColliderCom;
	bool                                    m_bSelect = false;    //툴에서 체크용
	int                                     m_iTexCount = 0;      //현재 그릴 텍스쳐 넘버
	_tchar                                  m_TexName[255] = L"";  
	_tchar                                  m_tempObjName[255] = L"";
	_tchar                                  m_LoadTempKey[255];    //로드용
public:
	virtual void Free(void);
	//툴에서만 사용
public:
	void SetSelect(bool flag);

};

END
#endif // GameObject_h__
