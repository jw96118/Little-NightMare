#ifndef CollisionMgr_h__
#define CollisionMgr_h__

#include "Defines.h"
#include "GameObject.h"
#include "Layer.h"

class CollisionMgr
{
private:
	typedef	struct tagOBB
	{
		_vec3		vPoint[8];		// ť�긦 �̷�� ����
		_vec3		vCenter;		// ť���� ����
		_vec3		vProjAxis[3];	// �������� �� ���� ���� ������� ���� ���� 
		_vec3		vAxis[3];		// ť���� ��� ������ ��, ���� ����

	}OBB;

private:
	explicit CollisionMgr(void);
	virtual ~CollisionMgr(void);

public:
	static void		CollisionWallToPlayer(Engine::CLayer* pLayer, 
										  Engine::CGameObject* pObject, _bool& flag);
	static void		CollisionWallToObject(Engine::CLayer* pLayer,
		Engine::CGameObject* pObject);

	static void		CollisionObjectToPlayerObb(Engine::CLayer* pLayer,
		Engine::CGameObject* pObject);
	static void		CollisionObjectToPlayerAABB(Engine::CLayer* pLayer,
		Engine::CGameObject* pObject,_bool flag = false);
	static void     CollisionObjectToPlayerAABB(Engine::CGameObject* pObject1,
		Engine::CGameObject* pObject2);
	static _bool    CollisionObjectPlayerAABB_Y(_vec3 vPos, Engine::CGameObject* pObject2);
	static _bool    CollisionColPlayerAABB_Y(_vec3 vPos,Engine::CGameObject* pObject2);

	static void		CollisionLeechToPlayer(Engine::CLayer* pLayer, Engine::CGameObject* pObject);
	static void		CollisionJanitorToPlayer(Engine::CLayer* pLayer, Engine::CGameObject* pObject);
	static void		CollisionJanitorArmToPlayer(Engine::CLayer* pLayer, Engine::CGameObject* pObject);
	static void		CollisionJanitorToPlayerAABB(Engine::CLayer* pLayer, Engine::CGameObject* pObject);


private:
	static void Set_Axis(OBB* pObb);
	static void Set_Point(OBB* pObb, const _vec3* pMin, const _vec3* pMax);
public: // �浹 ����
	// AABB �浹
	static _bool	IsCollisionBoxToBox(const _vec3& vDestMin,
									    const _vec3& vDestMax,
									    const _vec3& vSrcMin,
									    const _vec3& vSrcMax);

	// �� �浹
	static _bool	IsCollisionSphereToSphere(const SPHERECOLLIDER& tDest, 
											  const SPHERECOLLIDER& tSrc);
	static _bool Collision_OBB(const _vec3* pDestMin, const _vec3* pDestMax, const _matrix* pDestWorld, const _vec3* pSourMin, const _vec3* pSourMax, const _matrix* pSourWorld);
	// ����-�� �浹
	static _bool	IsCollisionRayToSphere(_vec3& vIntersection, 
										   _float& fDist, 
										   const LPDIRECT3DDEVICE9 pGraphicDev, 
										   const SPHERECOLLIDER & tDest);

	static void		Ray(const LPDIRECT3DDEVICE9 pGraphicDev, _vec3& vRayPos, _vec3& vRayDir);
};


#endif // WeaponMgr_h__