#ifndef JanitorArm_h__
#define JanitorArm_h__

#include "Defines.h"
#include "GameObject.h"

BEGIN(Engine)

class CRcTex;
class CTexture;
class CTransform;
class CRenderer;
class CCalculator;
class CNaviMesh;
class CCollider;
END

class CPlayer;

class CJanitorArm : public Engine::CGameObject
{
public:
	enum JANITOR_STATE
	{
		IDLE,
		START,
		LOOP,
		CATCH_PLAYER
	};
protected:
	explicit CJanitorArm(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CJanitorArm(void);

public:

public:
	void                    Set_Shader();
	static CJanitorArm*          Create(LPDIRECT3DDEVICE9 pGraphicDev, const _bool& flag);
public:
	virtual HRESULT			Ready_GameObject(void) override;
	virtual _int			Update_GameObject(const _float& fTimeDelta) override;
	virtual _int			LateUpdate_GameObject(const _float& fTimeDelta) override;
	virtual void			Render_GameObject(void) override;
	virtual HRESULT			LateReady_GameObject(void);
	HRESULT                 SetUp_ConstantTable(LPD3DXEFFECT& pEffect);
	Engine::CCollider*      Get_BoxCollider();
	_matrix                 Get_ColWorld();
	virtual	HRESULT			Add_Component(void);
	const _vec3&            Get_ColPos(int i = 0) const;
	const _matrix&          Get_BoneMat() const;
	const _vec3&            Get_GrabPos(int i) const;
	const _float&           Get_ShpereRadius(int i) const;
	const _float&           Get_AniTime() const;
	const _bool&            Get_CatchFlag() const;
	const _bool&            Get_CatchRIght() const;
	const _bool&            Get_CatchLeft() const;
	void                    Set_CatchPlayer(const _bool& flag);
	void                    Set_TurnChase(const _bool& flag);
	void                    Set_State(JANITOR_STATE eState);
	const JANITOR_STATE&    Get_State() const;
	void                    Reset_Ani();
private:
	void                    Janitor_Animation(const _float& fTimeDelta);
	void                    Check_Player();
	void                    AniEvent();
	void                    Draw_Sphere(int i);
	void                    Set_First_BedRoom();	
	void                    Set_ColWorld(_vec3 vScale, _vec3 vPos);
protected:
	Engine::CNaviMesh*		m_pNaviCom = nullptr;
	Engine::CCollider*		m_pColliderCom = nullptr;
	CPlayer*                m_pPlayerObj;
	Engine::CTransform*		m_pTargetTransformCom = nullptr;
	const _tchar*			m_pLayerTag;
	_int                    m_ianimationKey = 0;
	_matrix					m_matWorld;
	_matrix					m_matView;
	_matrix                 m_ColWorld;
	_bool                   m_bChase_Player = false;
	_bool                   m_bCatch_Player = false;
	_bool                   m_AniChange = true;
	_bool                   m_bTestEnd = false;
	_bool                   m_bPlayerDeath = false;
	_bool                   m_bCatchMove= false;
	_bool                   m_bStartAni = false;
	_bool                   m_bTurnChase = false;
	_bool                   m_bCatchRight = false;
	_bool                   m_bCatchLeft = false;

	_vec3					m_vTargetPos;

	_vec3					m_vPos;
	_vec3					m_vDir;
	_float                  m_fSpeed = 1.5f;
	_vec3                   m_vFirstPos;
	_vec3                   m_vFirstAngle;
	_int                    m_iTurnMax = 0;
	_int                    m_iTurnCount = 0;
	JANITOR_STATE           m_eState = IDLE;
	_vec3                   m_vPrePos;
	_int                    aniCount = 0;
	_float                  m_fSphereCol[2];
	_bool                   m_bCatchAni = false;
	//false면 오른쪽
	_bool                   m_bLRFlag = false;
	ID3DXLine*              m_pLine;
	LPD3DXMESH              m_mSphere1;
	LPD3DXMESH              m_mSphere2;

	_vec3                   m_vRHandPos;
	_vec3                   m_vLHandPos;
protected:
	virtual void Free(void) override;


};

#endif // Janitor_h__
