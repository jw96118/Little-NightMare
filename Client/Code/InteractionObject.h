#ifndef InteractionObject_h__
#define InteractionObject_h__

#include "Defines.h"
#include "GameObject.h"

BEGIN(Engine)

class CLightTex;
class CTexture;
class CTransform;
class CRenderer;

END
class CPlayer;
enum CAGESTATE {
	crush1 ,
    crush2 ,
    crush3 ,
    wobble1,
    wobble2,
	CAGE_END
};
class CInteractionObject : public Engine::CGameObject
{
public:
	enum OBJTYPE {HATCH, WOODFLOOR, PANELDOOR, SHIPDOOR,LEVER,LEVERFRAME,BOX,SPAWNDOOR,LASTDOOR,CAGE,CAGE_LEFT, CAGE_RIGHT, CAGE_LEFT_OUT, CAGE_RIGHT_OUT};
private:
	explicit CInteractionObject(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CInteractionObject(void);

public:
	virtual HRESULT Ready_GameObject(void) override;
	virtual HRESULT LateReady_GameObject(void) override;
	virtual _int Update_GameObject(const _float& fTimeDelta) override;
	virtual void Render_GameObject(void) override;
	HRESULT Load_GameObject(void);
	Engine::CCollider*     Get_BoxCollider();
	const _bool&           Get_ReadyLever() const;
	const _bool&           Get_CollWall() const;
	const _bool&           Get_NotRender() const;
	void                   Set_CollWall(const _bool& flag);
	void                   Set_NotRender(const _bool& flag);
	void                   Set_CageState(CAGESTATE eState);
	CAGESTATE              Get_CageState();
private:
	HRESULT		Add_Component(void);
	HRESULT     SetUp_ConstantTable(LPD3DXEFFECT& pEffect);
	void        Mesh_Animation(const _float& fdeltaTime);
	void 		Set_SavePoint();
	void        Render_Mesh();
	const _float&  Get_Angle() const;
public:
	void        SetTestCol();

private:
	CPlayer*                m_pPlayerObj;
	_vec3					m_vDir;
	_float					m_fSpeed = 20.f;
	ID3DXLine*              m_pLine;
	Engine::COLLTYPE        m_eType[5] = { Engine::COL_FALSE,Engine::COL_FALSE,Engine::COL_FALSE,Engine::COL_FALSE,Engine::COL_FALSE };
	_bool                   m_bPick = false;
	_float                  m_fDist = 0.f;
	_bool                   m_bDraw = false;
	_bool                   m_bCollWall = false;
	OBJTYPE                 m_eObjType;
	_float                  m_fAngle = 0.f;
	_bool                   m_bNotDraw = false;

	_bool                   m_bWoodFloorSound = false;
	_bool                   m_bPanelDoorSound = false;
	//레버
	_vec3                   m_vPreDir = {};
	_bool                   m_iLeverFlag = false;
	_bool                   m_bLeverReady = false;
	_int                    m_iLeverCount = 0;
	_bool                   m_bLeverSound = false;
	_bool                   m_bLeverSoundEnd = false;

	//배 문
	_bool                   m_bShipSound = false;
	_bool                   m_bShipSoundEnd = false;
	_float                  m_fShipDoorY = 0.f;
	//box
	_bool                   m_bBoxSound = false;
	//스폰 문
	_bool                   m_bSpawnDoorAniFlag = false;
	_bool                   m_bSpawnDoorFlag = false;
	_bool                   m_bSpawnDoorLastFlag = false;
	_bool                   m_bSpawnDoorhandleSound = false;
	_bool                   m_bSpawnDoorOpenSound = false;
	_bool                   m_bSpawnDoorCloseSound = false;
	_bool                   m_bSpawnDoorCloseEndSound = false;
	_bool                   m_bAniSpeed = 1.f;
	//Cage
	_int                    m_iAnimation;
	_bool                   m_bEndAni = false;
	_bool                   m_bCageSound[CAGE_END] = {};
	_bool                   m_bCageBarSound[2] = {};
	CAGESTATE               m_eCageState = wobble1;
	_bool                   m_bMonsterScream = false;
	_bool                   m_bMonsterChase = false;
	_vec3                   m_vFirstPos;
	_vec3                   m_vFirstAngle;

public:
	static CInteractionObject*		Create(LPDIRECT3DDEVICE9 pGraphicDev, Engine::MESHTYPE eMeshType, OBJTYPE eObjType);

	const float& Get_fDist() const;

	const	Engine::MESHTYPE& GetMeshType() const;
	void SetTexCount(int count);
	void SetbPick(_bool flag);
private:
	virtual void Free(void) override;
};

#endif // Terrain_h__
