#ifndef Terrain_h__
#define Terrain_h__

#include "Defines.h"
#include "GameObject.h"

BEGIN(Engine)

class CLightTex;
class CTexture;
class CTransform;
class CRenderer;
class CTerrainCol;
END

class CTerrain : public Engine::CGameObject
{
private:
	explicit CTerrain(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CTerrain(void);

public:
	virtual HRESULT Ready_GameObject(void) override;
	virtual _int Update_GameObject(const _float& fTimeDelta) override;
	virtual void Render_GameObject(void) override;
	vector<_vec3>         GetNaviPos();
	void     Set_Vertex(int i, int j, float val);
private:
	HRESULT		Add_Component(void);
	

private:
	_vec3					m_vDir;
	_float					m_fSpeed = 20.f;
	Engine::CTerrainCol*     m_pTerrain = nullptr;

	LPDIRECT3DTEXTURE9		m_pTexture[2];
	vector<_vec3>           m_vNaviPos;
	int                     m_iVertexNum = 0;
public:
	static CTerrain*		Create(LPDIRECT3DDEVICE9 pGraphicDev);
	const CString GetString() const;
	void SetString(CString str);
	void Save_NaviTerrain(DWORD& dwBytes, const HANDLE& hFile);
	void Load_NaviTerrain(DWORD& dwBytes, const HANDLE& hFile);
private:
	virtual void Free(void) override;
};

#endif // Terrain_h__
