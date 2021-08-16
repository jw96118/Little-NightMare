#ifndef NaviObject_h__
#define NaviObject_h__

#include "Defines.h"
#include "GameObject.h"

BEGIN(Engine)

class CLightTex;
class CTexture;
class CTransform;
class CRenderer;

END

class CNaviObject : public Engine::CGameObject
{
private:
	explicit CNaviObject(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CNaviObject(LPDIRECT3DDEVICE9 pGraphicDev, Engine::CGameObject* pGameObject);
	virtual ~CNaviObject(void);

public:
	virtual HRESULT Ready_GameObject(void) override;
	virtual _int Update_GameObject(const _float& fTimeDelta) override;
	virtual void Render_GameObject(void) override;
	_vec3*      Get_Navi();
	HRESULT Ready_CopyObject(void);

	void        Set_Vertex(int i,int j,float Val);

	const float & CNaviObject::Get_fDist() const;
	_bool Picking();
	void Save(DWORD& dwBytes, const HANDLE& hFile);
	void Load(DWORD& dwBytes, const HANDLE& hFile);
private:
	HRESULT		Add_Component(void);
	void DrawLine();
	void DrawNaviLine();

private:
	_vec3					m_vDir;
	_float					m_fSpeed = 20.f;
	ID3DXLine*              m_pLine;
	ID3DXLine*              m_pNaviLine;
	_vec3                   m_NaviVertex[3];
	_float                  m_fDist = 0.f;
public:
	static CNaviObject*		Create(LPDIRECT3DDEVICE9 pGraphicDev);
	static CNaviObject*		Create(LPDIRECT3DDEVICE9 pGraphicDev , Engine::CGameObject* pGameObject);

private:
	virtual void Free(void) override;
};

#endif // NaviObject_h__
