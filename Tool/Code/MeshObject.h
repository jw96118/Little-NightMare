#ifndef MeshObject_h__
#define MeshObject_h__

#include "Defines.h"
#include "GameObject.h"

BEGIN(Engine)

class CLightTex;
class CTexture;
class CTransform;
class CRenderer;
class COptimization;
END

class CMeshObject : public Engine::CGameObject
{
private:
	explicit CMeshObject(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CMeshObject(LPDIRECT3DDEVICE9 pGraphicDev,Engine::CGameObject* tempObject);
	virtual ~CMeshObject(void);

public:
	virtual HRESULT Ready_GameObject(void) override;
	virtual _int Update_GameObject(const _float& fTimeDelta) override;
	virtual void Render_GameObject(void) override;
	HRESULT Ready_CopyGameObject(void);
	HRESULT Load_GameObject(void);
	void    Set_sMeshName(CString sMeshName);
	void    Set_TestCol();
private:
	HRESULT		Add_Component(void);
	void DrawLine();

private:
	_vec3					m_vDir;
	_float					m_fSpeed = 20.f;
	ID3DXLine*              m_pLine;

	_bool                   m_bPick = false;
	_float                  m_fDist = 0.f;

	Engine::COptimization*	m_pOptimizationCom = nullptr;

	_bool					m_bDraw = false;

public:
	static CMeshObject*		Create(LPDIRECT3DDEVICE9 pGraphicDev, Engine::MESHTYPE eMeshType);
	static CMeshObject*		Create(LPDIRECT3DDEVICE9 pGraphicDev, Engine::MESHTYPE eMeshType, CString sMeshName);

	static CMeshObject*		Create(LPDIRECT3DDEVICE9 pGraphicDev,Engine::CGameObject* object);
	_bool   Picking();
	const float& Get_fDist() const;
	const CString GetString() const;
	const	Engine::MESHTYPE& GetMeshType() const;
	void SetString(CString str);
	void SetTexCount(int count);
	void SetbPick(_bool flag);
private:
	virtual void Free(void) override;
};

#endif // Terrain_h__
