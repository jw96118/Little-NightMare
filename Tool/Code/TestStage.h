#ifndef Stage_h__
#define Stage_h__

#include "Defines.h"
#include "Scene.h"
#include "ToolView.h"

#include "DynamicCamera.h"

#include "ToolCamera.h"
class CTerrain;
class CTestStage : public Engine::CScene
{
private:
	explicit CTestStage(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CTestStage(void);


public:
	virtual HRESULT Ready_Scene(void) override;
	virtual HRESULT LateReady_Scene(void) override;
	virtual _int Update_Scene(const _float& fTimeDelta) override;
	virtual void Render_Scene(void) override;

private:
	HRESULT		Ready_Environment_Layer(const _tchar* pLayerTag) { return S_OK; }
	HRESULT		Ready_GameLogic_Layer(const _tchar* pLayerTag);
	HRESULT		Ready_UI_Layer(const _tchar* pLayerTag) { return S_OK; }
	HRESULT		Ready_Resource(LPDIRECT3DDEVICE9& pGraphicDev, RESOURCEID eMax);
	void        StaticMesh_Picking();
	void        NaviMesh_Picking();
public:
	HRESULT     Add_GameObject(Engine::OBJECTTYPE objType, int monsterType, const _tchar* pObjName, const _tchar* pObjTypeName);
	HRESULT     Add_GameObject_Mesh(Engine::OBJECTTYPE objType, Engine::MESHTYPE eMeshType, CString sMeshName,const _tchar* pObjName, const _tchar* pObjTypeName);
	HRESULT     Copy_GameObject(Engine::CGameObject* object, const _tchar* pObjName);
	HRESULT     Copy_GameObject(Engine::CGameObject* object,const _tchar* pObjName,_vec3 tempPos);


	HRESULT     Add_ColObject(const _tchar * pObjName, Engine::COLLTYPE coltype);
	HRESULT     Add_NaviObject(const _tchar * pObjName);
	HRESULT     Copy_NaviObject(Engine::CGameObject* object,const _tchar * pObjName);

	void        Save(const TCHAR * pFilePath);
	void        Load(const TCHAR * pFilePath,  int &count);
	void        Navi_Save(const TCHAR * pFilePath);
	void        Navi_Load(const TCHAR * pFilePath, int &count);
	void        Col_Save(const TCHAR * pFilePath);
	void        Col_Load(const TCHAR * pFilePath, int &count);
	HRESULT     Delete_Object(const _tchar* pObjectName, int &count);
	HRESULT     Delete_Col_Object(const _tchar* pObjectName, int &count);
	HRESULT     Delete_Navi_Object(const _tchar* pObjectName, int &count);
private:
	_matrix						m_matView, m_matProj;
	Engine::CLayer*		        m_pLayer;
	Engine::CLayer*		        m_pColLayer;
	Engine::CLayer*		        m_pNaviLayer;
	Engine::CGameObject*        m_preSelectObject = nullptr;
	Engine::CKeyMgr*            m_pKeyMgr = nullptr;
	CTerrain*                   m_pTerrain = nullptr;
	D3DXVECTOR3             m_vOrigin;
	D3DXVECTOR3             m_vDirection;

public:
	static CTestStage*		Create(LPDIRECT3DDEVICE9 pGraphicDev,CToolView* m_pView);
private:
	CToolView*			m_pView;
private:
	virtual void Free(void) override;
};


#endif // Stage_h__
