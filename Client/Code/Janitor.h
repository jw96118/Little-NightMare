#ifndef Janitor_h__
#define Janitor_h__

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

class CJanitor : public Engine::CGameObject
{
public:
	enum JANITOR_STATE
	{
		BED_SPAWN,
		WALK,
		TURN_180,
		CHASE_PLAYER,
		CATCH_PLAYER,
		CATCH_FAIL,
		STOP,
		LAST_SPAWN,
		LAST_START,
		LAST_LOOP,
		LAST_CATCH,
		LAST_BREAK,
		DEATH
	};
protected:
	explicit CJanitor(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CJanitor(void);

public:

public:
	void                    Set_Shader();
	static CJanitor*          Create(LPDIRECT3DDEVICE9 pGraphicDev);
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
	void                    Set_AnimationKey(int iKey);
	const JANITOR_STATE&    Get_State() const;
	void                    Reset_Ani();
	_vec3                   Get_BloodEffectPos(int i);
private:
	void                    Janitor_Animation(const _float& fTimeDelta);
	void                    Check_Player();
	void                    AniEvent();
	void                    Draw_Sphere(int i);
	void                    Set_First_BedRoom();	
	void                    Set_ColWorld(_vec3 vScale, _vec3 vPos);
	void                    TestAstar(float fTimeDelta);
	void PathFinding(AstarNode startNode, AstarNode endNode, list<AstarNode> & openList, list<AstarNode> & closeList);
	void OpenListAdd(int checkX, int checkY, Engine::CNaviMesh*	m_pNaviCom, list<AstarNode> & openList, list<AstarNode> & closeList, AstarNode currentNode);
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
	JANITOR_STATE           m_eState = BED_SPAWN;
	_vec3                   m_vPrePos;
	_int                    aniCount = 0;
	_float                  m_fSphereCol[3];
	_bool                   m_bCatchAni = false;
	ID3DXLine*              m_pLine;
	LPD3DXMESH              m_mSphere1;
	LPD3DXMESH              m_mSphere2;
	LPD3DXMESH              m_mSphere3;
	_float                  m_AniSpeed = 1.f;
	_float                  m_fEndCount = 0.f;
	//사운드
	_bool m_bLastSpawnSound = false;
	_bool m_bLastStartSound = false;
	_bool m_bTeethSound = false;
	_bool m_bBreathSound = false;
	_bool m_bHeartBeatSound = false;
	_bool m_bFindSound = false;
	_bool m_bWorkingSound = false;
	_bool m_bWalkSoundHalf = false;
	_bool m_bBgmSound = false;
	_bool m_bStopSound = false;
	_float m_fPipeSceneOut = 1.f;
protected:
	virtual void Free(void) override;


};

class AstarNode
{
public:
	AstarNode* ParentNode;
	int H;     // 시작노드부터 목표지점까지 거리(타일 칸 수)
	int G;     // 시작노드부터 이동하려는 위치 거리
	int F = G + H;
	int x, y;
	AstarNode() {};
	AstarNode(int x, int y)
	{
		this->x = x;
		this->y = y;
	}
};


#endif // Janitor_h__
