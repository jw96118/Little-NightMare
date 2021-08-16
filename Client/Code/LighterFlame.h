#ifndef MeshObject_h__
#define MeshObject_h__

#include "Defines.h"
#include "GameObject.h"

BEGIN(Engine)

class CLightTex;
class CTexture;
class CTransform;
class CRenderer;

END
class CPlayer;
class CMeshObject : public Engine::CGameObject
{
private:
	explicit CMeshObject(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CMeshObject(void);

public:
	virtual HRESULT Ready_GameObject(void) override;
	virtual HRESULT LateReady_GameObject(void) override;
	virtual _int Update_GameObject(const _float& fTimeDelta) override;
	virtual void Render_GameObject(void) override;

	HRESULT Load_GameObject(void);
private:
	HRESULT		Add_Component(void);
	HRESULT     SetUp_ConstantTable(LPD3DXEFFECT& pEffect);
public:
	void        SetTestCol();

private:
	_vec3					m_vDir;
	_float					m_fSpeed = 20.f;
	ID3DXLine*              m_pLine;

	_bool                   m_bPick = false;
	_float                  m_fDist = 0.f;
	_bool                   m_bDraw = false;
	CPlayer*                m_pPlayer = nullptr;
public:
	static CMeshObject*		Create(LPDIRECT3DDEVICE9 pGraphicDev, Engine::MESHTYPE eMeshType);

	const float& Get_fDist() const;

	const	Engine::MESHTYPE& GetMeshType() const;
	void SetTexCount(int count);
	void SetbPick(_bool flag);
private:
	virtual void Free(void) override;
};

#endif // Terrain_h__
