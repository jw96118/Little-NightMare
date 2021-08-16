#ifndef Leech_h__
#define Leech_h__

#include "Defines.h"
#include "GameObject.h"

BEGIN(Engine)

class CRcTex;
class CTexture;
class CTransform;
class CRenderer;
class CCalculator;
class CNaviMesh;
END

class CPlayer;

class CLeech : public Engine::CGameObject
{
	enum LEECH_DIR
	{
		DIR_FRONT,
		DIR_RIGHT,
		DIR_LEFT
	};
protected:
	explicit CLeech(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CLeech(void);

public:

public:
	void                    Set_Shader();
	static CLeech*          Create(LPDIRECT3DDEVICE9 pGraphicDev);
public:
	virtual HRESULT			Ready_GameObject(void) override;
	virtual _int			Update_GameObject(const _float& fTimeDelta) override;
	virtual _int			LateUpdate_GameObject(const _float& fTimeDelta) override;
	virtual void			Render_GameObject(void) override;
	virtual HRESULT			LateReady_GameObject(void);
	HRESULT                 SetUp_ConstantTable(LPD3DXEFFECT& pEffect);

	virtual	HRESULT			Add_Component(void);
	const _vec3&            Get_ColPos() const;
	const _float&           Get_ShpereRadius() const;
	void                    Set_CatchPlayer(const _bool& flag);
private:
	void                    Leech_Animation();
	void                    Leech_Chase_Player();
	void                    Leech_AniEvent();
	void                    Draw_Sphere();
	void                    Set_First();
protected:
	Engine::CNaviMesh*		m_pNaviCom = nullptr;
	Engine::CCollider*		m_pColliderCom = nullptr;
	CPlayer*                m_pPlayerObj;
	Engine::CTransform*		m_pTargetTransformCom = nullptr;
	const _tchar*			m_pLayerTag;
	_int                    m_ianimationKey = 0;
	_matrix					m_matWorld;
	_matrix					m_matView;

	_bool                   m_bChase_Player = false;
	_bool                   m_bCatch_Player = false;
	_bool                   m_AniChange = true;
	_bool                   m_bTestEnd = false;
	_bool                   m_bPlayerDeath = false;
	_bool                   m_bCatchMove = false;
	_vec3					m_vTargetPos;
	_vec3					m_vPos;
	_vec3					m_vDir;
	
	_vec3                   m_vFirstPos;
	_vec3                   m_vFirstAngle;
	LEECH_DIR               m_eDIr;
	_vec3                   m_vPrePos;
	_int                    aniCount = 0;
	_float                  m_fSphereCol = 0.4f;
	_vec3                   tempBonedir = { 0.f,0.f,0.f };
	_vec3                   tempdir = { 0.f,0.f,0.f };
	ID3DXLine*              m_pLine;
	LPD3DXMESH              m_mSphere;


	_bool                   m_bMoveSound = false;
	_bool                   m_bFindSound = false;
	_bool                   m_bCatchSound = false;
protected:
	virtual void Free(void) override;


};

#endif // Leech_h__
