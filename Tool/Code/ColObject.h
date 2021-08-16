#ifndef Col_h__
#define Col_h__

#include "Defines.h"
#include "GameObject.h"

BEGIN(Engine)

class CLightTex;
class CTexture;
class CTransform;
class CRenderer;

END

class CColObject : public Engine::CGameObject
{
private:
	explicit CColObject(LPDIRECT3DDEVICE9 pGraphicDev, Engine::COLLTYPE Coltype);
	virtual ~CColObject(void);

public:
	virtual HRESULT Ready_GameObject(void) override;
	virtual _int Update_GameObject(const _float& fTimeDelta) override;
	virtual void Render_GameObject(void) override;
	Engine::COLLTYPE GetColType();
	void    SetColType(Engine::COLLTYPE colType);
	void    Set_TestCol();
private:
	HRESULT		Add_Component(void);
	void DrawLine();
	
private:
	_vec3					m_vDir;
	_float					m_fSpeed = 20.f;
	ID3DXLine*              m_pLine;
	LPD3DXMESH              m_pBoxMesh;
	Engine::COLLTYPE        type = Engine::COL_FALSE;
public:
	static CColObject*		Create(LPDIRECT3DDEVICE9 pGraphicDev, Engine::COLLTYPE Coltype);

private:
	virtual void Free(void) override;
};

#endif // Col_h__
