#ifndef Player_h__
#define Player_h__

#include "Defines.h"
#include "GameObject.h"

BEGIN(Engine)

class CNaviMesh;
class CDynamicMesh;
class CTransform;
class CRenderer;
class CCalculator;
class CCollider;

END
class CInteractionObject;
class CFirstPersonCamera;
enum PLAYER_DIR
{
	DIR_LEFT,
	DIR_RIGHT,
	DIR_TOP,
	DIR_BOTTOM

};

class CPlayer : public Engine::CGameObject
{
private:
	explicit CPlayer(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CPlayer(void);

public:
	virtual HRESULT Ready_GameObject(void) override;
	virtual HRESULT			LateReady_GameObject(void) override;
	virtual _int Update_GameObject(const _float& fTimeDelta) override;
	virtual _int LateUpdate_GameObject(const _float& fTimeDelta) override;
	virtual void Render_GameObject(void) override;
	virtual void Render_ShadowDepth(void);
	virtual _vec3         Get_LightPos();
	void Set_NaviMesh(const _tchar* comName);
	void Set_Shader();
	Engine::CCollider*     Get_BoxCollider();
	_matrix                Get_ColWorld();
	void                   Set_Trigger(const _tchar* name);
	void                   Set_CatchLeech(const _bool& flag);
	void                   Set_CatchJanitor(const _bool& flag,const _int& index,const _bool& bHandFlag,Engine::CGameObject* pObject);
	void                   Set_OnCol(const _bool& flag);
	void                   Delete_TriggerName(wstring str);
	void                   Land();
	void                   Trigger_Event();
	void                   Set_ColWorld(_vec3 vScale, _vec3 vPos);
	const _bool&           Get_ChaseBool() const;
	const _bool&           Get_Falling() const;
	const _bool&           Get_CatchLeech() const;
	const _bool&           Get_LoadScene() const;
	const _bool&           Get_HatchOpenAni() const;
	const _bool&           Get_PullHatch() const;
	const _bool&           Get_HatchOpen() const;
	const _bool&           Get_WoodFloor() const;
	const _bool&           Get_DoorPush() const;
	const _bool&           Get_LeverPush() const;
	const _bool&           Get_BoxMove() const;
	const _bool&           Get_NaviFlag() const;
	const _bool&           Get_Turn() const;
	const _bool&           Get_SpawnJionterBedRoom() const;
	const _bool&           Get_JionterCatch() const;
	const _bool&           Get_LastStage() const;
	const _bool&           Get_LighterFlag() const;
	const _vec3&           Get_AniPos() const;
	const _vec3&           Get_PreDir() const;
	const _vec3&           Get_Player_Hand() const;
	const _vec3&           Get_ColScale() const;
	const _matrix&         Get_LighterPos() const;
	const _float&          Get_NaviY() const;
	const _float&          Get_ShpereRadius() const;
	const _float&          Get_HatchCount() const;
	const _float&          Get_AniTime() const;
	const _bool&           GetAniflag() const;

private:
	HRESULT		Add_Component(void);
	void		Key_Input(const _float& fTimeDelta);
	void		SetUp_OnTerrain(void);
	HRESULT     SetUp_ConstantTable(LPD3DXEFFECT& pEffect);
	void        Player_Animation();
	void        Gravity(const _float& fTimeDelta);
	void        Turn();

	void        Draw_Sphere();
	void        Set_SavePoint();
	void        Play_MoveSound(const _tchar* name);
	void        TurnOffLighter(_bool flag =false);
private:

	Engine::CNaviMesh*		m_pNaviCom = nullptr;
	Engine::CCollider*		m_pColliderCom = nullptr;
	Engine::CDynamicMesh*   m_pDynmicRIght = nullptr;
	_int                    m_ianimationKey = 0;
	_double                 m_dAnimationCount = 0.25;

	_int                    m_iRightAnimationKey = 0;
	_double                 m_dRightAnimationCount = 0.25;


	_int                    aniCount = 0;
	_vec3					m_vDir;
	_float					m_fSpeed   = 3.f;
	_float                  m_fJumpPower = 0.f;
	_float                  m_fAcc = 0.f;
	_bool                   m_bPushCtrl = false;
	_bool                   m_bPushCtrlTrigger = false;
	_bool                   m_bTurn     = false;
	_bool                   m_bTurnEnd  = false;
	_bool                   m_bMove     = false;
	_bool                   m_bJump     = false;
	_bool                   m_bLanding = false;
	_bool                   m_bRun  = false;
	_bool                   m_bClime = false;
	_bool                   m_bClimeReady = false;
	_bool                   m_bClimeAir = false;
	_bool                   m_bClimeEnd = false;
	_bool                   m_bFalling = false;
	_bool                   m_bCantMove = false;
	_bool                   m_bEndAniPos = false;
	_bool                   m_bLeechChase = false;
	_bool                   m_bCatchLeech = false;
	_bool                   m_bLeechDeath = false;
	_bool                   m_bLeechDeathEnd = false;
	_bool                   m_bLoadScene = false;
	_bool                   m_bCatchGrip = false;
	_bool                   m_bPullGrip = false;
	_bool                   m_bPullDownGrip = false;
	_bool                   m_bPullDownGripEnd = false;
	_bool                   m_bTriggerGrip = false;
	_bool                   m_bHatchOpen = false;
	_bool                   m_bWoodFloor = false;
	_bool                   m_bPushDoorTrigger = false;
	_bool                   m_bPushDoor = false;
	_bool                   m_bPushDoorOpen = false;
	_bool                   m_bFallDown = false;
	_bool                   m_bPushLeverTrigger = false;
	_bool                   m_bPushLever = false;
	_bool                   m_bBoxCarryTrigger = false;
	_bool                   m_bBoxCarry = false;
	_bool                   m_bBoxCarryMove = false;
	_bool                   m_bNaviFlag = false;
	_bool                   m_bBoxUpTrigger = false;
	_bool                   m_bOnCol = false;
	_bool                   m_bSpawnJionterBedRoom = false;
	_bool                   m_bJumpFalling = false;
	_bool                   m_bUpPipeTrigger = false;
	_bool                   m_bJanitorDeath = false;
	_bool                   m_bJanitorDeathEnd = false;
	_bool                   m_bJanitorHand = false;
	_bool                   m_bLastStage = false;
	_bool                   m_bLeftCageTrigger = false;
	_bool                   m_bRightCageTrigger = false;
	_bool                   m_bLeftCageCatch = false;
	_bool                   m_bRightCageCatch = false;
	_bool                   m_bLeftCagePull = false;
	_bool                   m_bRightCagePull = false;
	_bool                   m_bLeftCagePullDown = false;
	_bool                   m_bRightCagePullDown = false;
	_bool                   m_bLeftCagePullDownReset = false;
	_bool                   m_bRightCagePullDownReset = false;	
	_bool                   m_bLighter = false;
	_bool                   m_bLighterIgnight = false;
	_bool                   m_bMakeBreak = false;
	_int                    m_iSlidingCount = 0;



	_bool                   m_bStartAni = true;
	_bool                   m_bStartAniEnd = true;

	//Sound
	_float                  m_bPipeSceneOut = 1.f;
	_bool                   m_bWalkSound = false;
	_bool                   m_bWalkSoundHalf = false;
	_bool                   m_bJanitorArmSound = false;
	_bool                   m_bJanitorAirEffect = false;
	_bool                   m_bJumpSound = false;
	_bool                   m_bLandSound = false;
	_bool                   m_bHatchOpenSound = false;
	_bool                   m_bHatchOpenSoundEnd = false;
	_bool                   m_bWoodFloorSound = false;
	_bool                   m_bSceondBgm = false;
	_bool                   m_bLeechDeathSound = false;
	_bool                   m_bPipeBgmSound = false;
	_bool                   m_bClimbSound = false;
	_bool                   m_bWakeUpSound = false;
	_bool                   m_bLighterOnSound = false;
	_bool                   m_bLighterOffSound = false;
	

	_bool                   m_bResetBGM = false;


	_vec3                   m_vCheckPoint;
	_vec3                   m_vGrabfirstPos = {};
	_float                  m_fGrabCount = 0.f;
	_int                    m_iJanitorHand = 0;
	PLAYER_DIR              m_eDir = DIR_BOTTOM;
	PLAYER_DIR              m_ePreDir = DIR_BOTTOM;
	PLAYER_DIR              m_eBoxCarryDir = DIR_LEFT;
	_matrix                 m_ColWorld;
	_bool					m_bColl = false;
	vector<wstring>         m_wTriggerName;
	Engine::CGameObject*    m_pJanitor = nullptr;
	CFirstPersonCamera*     m_pCam = nullptr;
	_float                  m_fSphereCol = 10.25f;
	_float                  m_fAngle = 0.f;
	_vec3                   m_vPrePos;
	_vec3                   m_vColScale;
	_vec3                   m_vTest;
	_vec3                   m_aniPos;
	_vec3                   m_vPreDir = {0.f,0.f,0.f};
	LPD3DXMESH              m_mSphere;
	CInteractionObject*     m_pLever;
	
public:
	static CPlayer*		Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	virtual void Free(void) override;
};

#endif // Player_h__
