#ifndef TerrainCol_h__
#define TerrainCol_h__

#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CTerrainCol : public CVIBuffer
{
private:
	explicit CTerrainCol(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CTerrainCol(const CTerrainCol& rhs);
	virtual ~CTerrainCol(void);

public:
	const _vec3*			Get_VtxPos(void) const;
	_ulong					Get_VtxCntX(void) const;
	_ulong					Get_VtxCntZ(void) const;
	void                    Save_Navi(DWORD& dwBytes, const HANDLE& hFile);
	void                    Load_Navi(DWORD& dwBytes, const HANDLE& hFile);
public:
	HRESULT					Ready_Buffer(const _ulong& dwCntX, 
										 const _ulong& dwCntZ,	
										 const _ulong dwVtxItv = 1,  _vec3 vPos = _vec3(0.f,0.f,0.f)) ;
	void                   Set_Select(int iIndex, vector<_vec3>&  vPos);
    void                   Set_bNavi(int iIndex,bool flag);
	void                   Set_Vertex(int iIndex, int i, int j, float Val);
private:
	HANDLE					m_hFile;
	BITMAPFILEHEADER		m_fH;
	BITMAPINFOHEADER		m_iH;

	_vec3*					m_pPos;
	_vec3                   m_vStartPos;
	_bool					m_bClone;
	vector<_bool>           m_bNavi;

	_ulong                  m_dwCntZ = 0, m_dwCntX = 0;
public:
	static CTerrainCol*		Create(LPDIRECT3DDEVICE9 pGraphicDev,
							const _ulong& dwCntX, 
							const _ulong& dwCntZ, 
							const _ulong& dwVtxItv, _vec3 vPos);

	virtual CResources*		Clone(void) override;

private:
	virtual void			Free(void);
};

END
#endif // TerrainTex_h__
