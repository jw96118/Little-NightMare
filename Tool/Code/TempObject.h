#ifndef TempObject_h__
#define TempObject_h__

#include "Defines.h"
#include "GameObject.h"

BEGIN(Engine)

class CLightTex;
class CTexture;
class CTransform;
class CRenderer;

END

class CTempObject : public Engine::CGameObject
{
private:
	explicit CTempObject(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CTempObject(LPDIRECT3DDEVICE9 pGraphicDev,Engine::CGameObject* tempObject);
	virtual ~CTempObject(void);

public:
	virtual HRESULT Ready_GameObject(void) override;
	virtual _int Update_GameObject(const _float& fTimeDelta) override;
	virtual void Render_GameObject(void) override;
	HRESULT Ready_CopyGameObject(void);
private:
	HRESULT		Add_Component(void);
	void DrawLine();

private:
	_vec3					m_vDir;
	_float					m_fSpeed = 20.f;
	ID3DXLine*              m_pLine;
public:
	static CTempObject*		Create(LPDIRECT3DDEVICE9 pGraphicDev);
	static CTempObject*		Create(LPDIRECT3DDEVICE9 pGraphicDev,Engine::CGameObject* object);
	const CString GetString() const;
	void SetString(CString str);
	void SetTexCount(int count);
	void SetTestTexure(Engine::OBJECTTYPE objType);
private:
	virtual void Free(void) override;
};

#endif // Terrain_h__
