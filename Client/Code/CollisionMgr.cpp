#include "stdafx.h"
#include "CollisionMgr.h"

#include "Player.h"
#include "ColObject.h"
#include "FirstPersonCamera.h"
#include "Monster.h"
#include "Leech.h"
#include "Collider.h"
#include "Janitor.h"
#include "Janitor_Arm.h"
#include "Export_Function.h"
#include "InteractionObject.h"

CollisionMgr::CollisionMgr(void)
{
}

CollisionMgr::~CollisionMgr(void)
{
}

void CollisionMgr::CollisionWallToPlayer(Engine::CLayer* pLayer,
	Engine::CGameObject* pObject, _bool& flag)
{
	CPlayer* pPlayer = dynamic_cast<CPlayer*>(pObject);
	NULL_CHECK(pPlayer);
	auto mapObject = pLayer->GetMapObj();
	if (!pPlayer->Get_JionterCatch())
	{
		for (auto iter : mapObject)
		{
			CColObject* pColObject = dynamic_cast<CColObject*>(iter.second);
			NULL_CHECK(pColObject);
			_vec3		vDestMin, vDestMax, vSourMin, vSourMax;

			_matrix     matPlayerColWorld = pPlayer->Get_ColWorld();
			Engine::CTransform* pColTransform = pColObject->GetTransformCom();



			D3DXVec3TransformCoord(&vDestMin, pPlayer->Get_BoxCollider()->Get_Min(), &matPlayerColWorld);
			D3DXVec3TransformCoord(&vDestMax, pPlayer->Get_BoxCollider()->Get_Max(), &matPlayerColWorld);

			D3DXVec3TransformCoord(&vSourMin, pColObject->Get_BoxCollider()->Get_Min(), pColTransform->Get_WorldMatrixPointer());
			D3DXVec3TransformCoord(&vSourMax, pColObject->Get_BoxCollider()->Get_Max(), pColTransform->Get_WorldMatrixPointer());

			if (IsCollisionBoxToBox(vDestMin, vDestMax,
				vSourMin, vSourMax))
			{
				//pPlayer->Set_Trigger(L"");
				if (iter.first.find(L"Trigger", 0) == wstring::npos)
				{
					flag = true;
					// x축의 반지름 합과 거리를 구한다.
					// 이 때 x축의 반지름 합이 x축의 거리보다 크면 x쪽으로 겹침.
					_float PlayerMaxX = vDestMax.x;
					_float PlayerMinX = vDestMin.x;
					_float ColMaxX = vSourMax.x;
					_float ColMinX = vSourMin.x;
					_float fSumX = (PlayerMaxX - PlayerMinX) * 0.5f +
						(ColMaxX - ColMinX) * 0.5f;
					_float fDistX = fabs((PlayerMaxX + PlayerMinX) * 0.5f - (ColMaxX + ColMinX) * 0.5f);


					_float PlayerMaxZ = vDestMax.z;
					_float PlayerMinZ = vDestMin.z;
					_float ColMaxZ = vSourMax.z;
					_float ColMinZ = vSourMin.z;

					// z축의 반지름 합과 거리를 구한다.
					// 이 때 z축의 반지름 합이 z축의 거리보다 크면 z쪽으로 겹침.
					_float fSumZ = (PlayerMaxZ - PlayerMinZ) * 0.5f + (ColMaxZ - ColMinZ) * 0.5f;
					_float fDistZ = fabs((PlayerMaxZ + PlayerMinZ) * 0.5f - (ColMaxZ + ColMinZ) * 0.5f);

					// 겹친영역의 크기 구하기.
					_float fMoveX = fSumX - fDistX;
					_float fMoveZ = fSumZ - fDistZ;

					Engine::CTransform* pPlayerTransform = pPlayer->GetTransformCom();
					_float fX = (PlayerMaxX + PlayerMinX) * 0.5f;
					_float fZ = (PlayerMaxZ + PlayerMinZ) * 0.5f;
					_vec3 vPos = *pPlayerTransform->Get_Info(Engine::INFO_POS);
					_float testy = vSourMax.y - abs(vSourMax.y * 0.2f);
					pPlayer->Set_OnCol(false);
					if ((pPlayer->GetTransformCom()->Get_Info(Engine::INFO_POS)->x > 96.f && pPlayer->GetTransformCom()->Get_Info(Engine::INFO_POS)->x<180.f)
						|| pPlayer->GetTransformCom()->Get_Info(Engine::INFO_POS)->x < 10.f)
					{
						if (vDestMin.y > testy)
						{
							vPos = { vPos.x, vSourMax.y , vPos.z };
							pPlayer->Land();
							pPlayer->Set_OnCol(true);
							pPlayerTransform->Set_Pos(&vPos);
						}
						else if (fMoveX > fMoveZ) // Z축으로 밀어냄
						{
							if ((ColMaxZ + ColMinZ) * 0.5f < fZ)
							{
								vPos = { vPos.x, vPos.y, vPos.z + fMoveZ };
								pPlayerTransform->Set_Pos(&vPos);
							}
							else
							{
								vPos = { vPos.x, vPos.y, vPos.z - fMoveZ };
								pPlayerTransform->Set_Pos(&vPos);
							}
						}
						else // X축으로 밀어냄
						{
							if ((ColMaxX + ColMinX) * 0.5f < fX)
							{
								vPos = { vPos.x + fMoveX, vPos.y, vPos.z };
								pPlayerTransform->Set_Pos(&vPos);
							}
							else
							{
								vPos = { vPos.x - fMoveX, vPos.y, vPos.z };
								pPlayerTransform->Set_Pos(&vPos);
							}
						}
					}
					else
					{
						if (fMoveX > fMoveZ) // Z축으로 밀어냄
						{
							if ((ColMaxZ + ColMinZ) * 0.5f < fZ)
							{
								vPos = { vPos.x, vPos.y, vPos.z + fMoveZ };
								pPlayerTransform->Set_Pos(&vPos);
							}
							else
							{
								vPos = { vPos.x, vPos.y, vPos.z - fMoveZ };
								pPlayerTransform->Set_Pos(&vPos);
							}
						}
						else // X축으로 밀어냄
						{
							if ((ColMaxX + ColMinX) * 0.5f < fX)
							{
								vPos = { vPos.x + fMoveX, vPos.y, vPos.z };
								pPlayerTransform->Set_Pos(&vPos);
							}
							else
							{
								vPos = { vPos.x - fMoveX, vPos.y, vPos.z };
								pPlayerTransform->Set_Pos(&vPos);
							}
						}
					}
				}
				else
					pPlayer->Set_Trigger(iter.first.c_str());
			}
			else
			{
				pPlayer->Delete_TriggerName(iter.first);
			}
		}
	}
}
void CollisionMgr::CollisionWallToObject(Engine::CLayer* pLayer,
	Engine::CGameObject* pObject)
{
	CInteractionObject* pInterObject = dynamic_cast<CInteractionObject*>(pObject);
	NULL_CHECK(pInterObject);
	auto mapObject = pLayer->GetMapObj();

	for (auto iter : mapObject)
	{
		CColObject* pColObject = dynamic_cast<CColObject*>(iter.second);
		NULL_CHECK(pColObject);
		_vec3		vDestMin, vDestMax, vSourMin, vSourMax;

		_matrix             matObjectColWorld = *pInterObject->GetTransformCom()->Get_WorldMatrixPointer();
		Engine::CTransform* pColTransform = pColObject->GetTransformCom();



		D3DXVec3TransformCoord(&vDestMin, pInterObject->Get_BoxCollider()->Get_Min(), &matObjectColWorld);
		D3DXVec3TransformCoord(&vDestMax, pInterObject->Get_BoxCollider()->Get_Max(), &matObjectColWorld);

		D3DXVec3TransformCoord(&vSourMin, pColObject->Get_BoxCollider()->Get_Min(), pColTransform->Get_WorldMatrixPointer());
		D3DXVec3TransformCoord(&vSourMax, pColObject->Get_BoxCollider()->Get_Max(), pColTransform->Get_WorldMatrixPointer());

		if (IsCollisionBoxToBox(vDestMin, vDestMax,
			vSourMin, vSourMax))
		{
			//pPlayer->Set_Trigger(L"");
			if (iter.first.find(L"Trigger", 0) == wstring::npos)
			{
				// x축의 반지름 합과 거리를 구한다.
				// 이 때 x축의 반지름 합이 x축의 거리보다 크면 x쪽으로 겹침.
				_float ObjectMaxX = vDestMax.x;
				_float ObjectMinX = vDestMin.x;
				_float ColMaxX = vSourMax.x;
				_float ColMinX = vSourMin.x;
				_float fSumX = (ObjectMaxX - ObjectMinX) * 0.5f +
					(ColMaxX - ColMinX) * 0.5f;
				_float fDistX = fabs((ObjectMaxX + ObjectMinX) * 0.5f - (ColMaxX + ColMinX) * 0.5f);


				_float ObjectMaxZ = vDestMax.z;
				_float ObjectMinZ = vDestMin.z;
				_float ColMaxZ = vSourMax.z;
				_float ColMinZ = vSourMin.z;

				// z축의 반지름 합과 거리를 구한다.
				// 이 때 z축의 반지름 합이 z축의 거리보다 크면 z쪽으로 겹침.
				_float fSumZ = (ObjectMaxZ - ObjectMinZ) * 0.5f + (ColMaxZ - ColMinZ) * 0.5f;
				_float fDistZ = fabs((ObjectMaxZ + ObjectMinZ) * 0.5f - (ColMaxZ + ColMinZ) * 0.5f);

				// 겹친영역의 크기 구하기.
				_float fMoveX = fSumX - fDistX;
				_float fMoveZ = fSumZ - fDistZ;

				Engine::CTransform* pObjectTransform = pObject->GetTransformCom();
				_float fX = (ObjectMaxX + ObjectMinX) * 0.5f;
				_float fZ = (ObjectMaxZ + ObjectMinZ) * 0.5f;
				Engine::CGameObject* m_pPlayer = dynamic_cast<CPlayer*>(Engine::Get_GameObject(L"02.GameLogic", L"Dyn_Player"));
				if (fMoveX > fMoveZ) // Z축으로 밀어냄
				{
					if ((ColMaxZ + ColMinZ) * 0.5f < fZ)
					{
						_vec3 vPos = *pObjectTransform->Get_Info(Engine::INFO_POS);
						vPos = { vPos.x, vPos.y, vPos.z + fMoveZ };

						pObjectTransform->Set_Pos(&vPos);

						_vec3 tempPos = *m_pPlayer->GetTransformCom()->Get_Info(Engine::INFO_POS);
						tempPos = { tempPos.x, tempPos.y, tempPos.z + fMoveZ };
						m_pPlayer->GetTransformCom()->Set_Pos(&tempPos);
					}
					else
					{
						_vec3 vPos = *pObjectTransform->Get_Info(Engine::INFO_POS);
						vPos = { vPos.x, vPos.y, vPos.z - fMoveZ };

						pObjectTransform->Set_Pos(&vPos);

						_vec3 tempPos = *m_pPlayer->GetTransformCom()->Get_Info(Engine::INFO_POS);
						tempPos = { tempPos.x, tempPos.y, tempPos.z - fMoveZ };
						m_pPlayer->GetTransformCom()->Set_Pos(&tempPos);
					}
				}
				else // X축으로 밀어냄
				{
					if ((ColMaxX + ColMinX) * 0.5f < fX)
					{
						_vec3 vPos = *pObjectTransform->Get_Info(Engine::INFO_POS);
						vPos = { vPos.x + fMoveX, vPos.y, vPos.z };

						pObjectTransform->Set_Pos(&vPos);

						_vec3 tempPos = *m_pPlayer->GetTransformCom()->Get_Info(Engine::INFO_POS);
						tempPos = { tempPos.x + fMoveX, tempPos.y, tempPos.z };
						m_pPlayer->GetTransformCom()->Set_Pos(&tempPos);
					}
					else
					{
						_vec3 vPos = *pObjectTransform->Get_Info(Engine::INFO_POS);
						vPos = { vPos.x - fMoveX, vPos.y, vPos.z };

						pObjectTransform->Set_Pos(&vPos);

						_vec3 tempPos = *m_pPlayer->GetTransformCom()->Get_Info(Engine::INFO_POS);
						tempPos = { tempPos.x - fMoveX, tempPos.y, tempPos.z };
						m_pPlayer->GetTransformCom()->Set_Pos(&tempPos);
					}
				}
			}
		}
	}
}
void CollisionMgr::CollisionObjectToPlayerObb(Engine::CLayer * pLayer, Engine::CGameObject * pObject)
{
	CPlayer* pPlayer = dynamic_cast<CPlayer*>(pObject);
	NULL_CHECK(pPlayer);

	auto mapObject = pLayer->GetMapObj();
	if (pPlayer)
		for (auto iter : mapObject)
		{
			if (iter.first.find(L"Hatch", 0) != wstring::npos && !pPlayer->Get_PullHatch())
			{
				CInteractionObject* pColObject = dynamic_cast<CInteractionObject*>(iter.second);
				NULL_CHECK(pColObject);
				_vec3		vDestMin = *pPlayer->Get_BoxCollider()->Get_Min();
				_vec3   	vDestMax = *pPlayer->Get_BoxCollider()->Get_Max();
				_vec3       vSourMin = *pColObject->Get_BoxCollider()->Get_Min();
				_vec3       vSourMax = *pColObject->Get_BoxCollider()->Get_Max();

				Engine::CTransform* pColTransform = pColObject->GetTransformCom();
				_matrix     matPlayerColWorld = pPlayer->Get_ColWorld();
				_matrix     matColWorld = *pColTransform->Get_WorldMatrixPointer();
				matColWorld._11 *= 0.5f;

				if (Collision_OBB(&vDestMin, &vDestMax, &matPlayerColWorld, &vSourMin, &vSourMax, &matColWorld))
				{
					pPlayer->GetTransformCom()->Set_Pos(*pPlayer->GetTransformCom()->Get_Info(Engine::INFO_POS) - pPlayer->Get_PreDir());
				}
				else
				{
				}
			}
		}
}

void CollisionMgr::CollisionObjectToPlayerAABB(Engine::CLayer * pLayer, Engine::CGameObject * pObject, _bool flag)
{
	CPlayer* pPlayer = dynamic_cast<CPlayer*>(pObject);
	NULL_CHECK(pPlayer);
	auto mapObject = pLayer->GetMapObj();

	for (auto iter : mapObject)
	{
		if ((iter.first.find(L"Door", 0) != wstring::npos && !pPlayer->Get_DoorPush()) || iter.first.find(L"LeverBase", 0) != wstring::npos
			|| iter.first.find(L"ShipDoor", 0) != wstring::npos || iter.first.find(L"Box", 0) != wstring::npos)
		{
			_int iCount = 1;
			if (iter.first.find(L"Box", 0) != wstring::npos)
				iCount = 3;

			for (int i = 0; i < iCount; i++)
			{
				CInteractionObject* pColObject = dynamic_cast<CInteractionObject*>(iter.second);
				NULL_CHECK(pColObject);
				_vec3		vDestMin, vDestMax, vSourMin, vSourMax;

				_matrix     matPlayerColWorld = pPlayer->Get_ColWorld();
				_matrix     matColWorld = *pColObject->GetTransformCom()->Get_WorldMatrixPointer();



				D3DXVec3TransformCoord(&vDestMin, pPlayer->Get_BoxCollider()->Get_Min(), &matPlayerColWorld);
				D3DXVec3TransformCoord(&vDestMax, pPlayer->Get_BoxCollider()->Get_Max(), &matPlayerColWorld);

				if (iter.first.find(L"Box", 0) != wstring::npos && i == 0)
				{
					matColWorld._11 *= 1.2f;
					matColWorld._33 *= 1.2f;
				}
				else if ((iter.first.find(L"LeverBase", 0) != wstring::npos) && i == 0)
				{
					matColWorld._11 *= 0.5f;
				}
				if (i == 1)
				{
					matColWorld._11 *= 0.75f;
					matColWorld._22 *= 0.3f;
					matColWorld._33 *= 1.2f;
				}
				else if (i == 2)
				{
					matColWorld._11 *= 1.2f;
					matColWorld._22 *= 0.3f;
					matColWorld._33 *= 0.75f;
				}
				D3DXVec3TransformCoord(&vSourMin, pColObject->Get_BoxCollider()->Get_Min(), &matColWorld);
				D3DXVec3TransformCoord(&vSourMax, pColObject->Get_BoxCollider()->Get_Max(), &matColWorld);

				if (IsCollisionBoxToBox(vDestMin, vDestMax,
					vSourMin, vSourMax))
				{
					if (i == 0)
					{
						// x축의 반지름 합과 거리를 구한다.
						// 이 때 x축의 반지름 합이 x축의 거리보다 크면 x쪽으로 겹침.
						_float PlayerMaxX = vDestMax.x;
						_float PlayerMinX = vDestMin.x;
						_float ColMaxX = vSourMax.x;
						_float ColMinX = vSourMin.x;
						_float fSumX = (PlayerMaxX - PlayerMinX) * 0.5f +
							(ColMaxX - ColMinX) * 0.5f;
						_float fDistX = fabs((PlayerMaxX + PlayerMinX) * 0.5f - (ColMaxX + ColMinX) * 0.5f);


						_float PlayerMaxZ = vDestMax.z;
						_float PlayerMinZ = vDestMin.z;
						_float ColMaxZ = vSourMax.z;
						_float ColMinZ = vSourMin.z;

						// z축의 반지름 합과 거리를 구한다.
						// 이 때 z축의 반지름 합이 z축의 거리보다 크면 z쪽으로 겹침.
						_float fSumZ = (PlayerMaxZ - PlayerMinZ) * 0.5f + (ColMaxZ - ColMinZ) * 0.5f;
						_float fDistZ = fabs((PlayerMaxZ + PlayerMinZ) * 0.5f - (ColMaxZ + ColMinZ) * 0.5f);

						// 겹친영역의 크기 구하기.
						_float fMoveX = fSumX - fDistX;
						_float fMoveZ = fSumZ - fDistZ;

						Engine::CTransform* pPlayerTransform = pPlayer->GetTransformCom();
						_float fX = (PlayerMaxX + PlayerMinX) * 0.5f;
						_float fZ = (PlayerMaxZ + PlayerMinZ) * 0.5f;
						if (!flag)
						{
							_vec3 vPos = *pPlayerTransform->Get_Info(Engine::INFO_POS);
							_float testy = vSourMax.y - abs(vSourMax.y * 0.1f);
							if (iter.first.find(L"Box", 0) != wstring::npos)
							{
								pPlayer->Set_OnCol(false);
								if (vDestMin.y > testy)
								{
									vPos = { vPos.x, vSourMax.y , vPos.z };
									pPlayer->Land();
									pPlayer->Set_OnCol(true);
									pPlayerTransform->Set_Pos(&vPos);
								}
								else if (fMoveX > fMoveZ) // Z축으로 밀어냄
								{
									if ((ColMaxZ + ColMinZ) * 0.5f < fZ)
									{
										vPos = { vPos.x, vPos.y, vPos.z + fMoveZ };

										pPlayerTransform->Set_Pos(&vPos);
									}
									else
									{
										vPos = { vPos.x, vPos.y, vPos.z - fMoveZ };

										pPlayerTransform->Set_Pos(&vPos);
									}
								}
								else // X축으로 밀어냄
								{
									if ((ColMaxX + ColMinX) * 0.5f < fX)
									{
										vPos = { vPos.x + fMoveX, vPos.y, vPos.z };

										pPlayerTransform->Set_Pos(&vPos);
									}
									else
									{
										vPos = { vPos.x - fMoveX, vPos.y, vPos.z };

										pPlayerTransform->Set_Pos(&vPos);
									}
								}
							}
							else
							{
								if (fMoveX > fMoveZ) // Z축으로 밀어냄
								{
									if ((ColMaxZ + ColMinZ) * 0.5f < fZ)
									{
										vPos = { vPos.x, vPos.y, vPos.z + fMoveZ };

										pPlayerTransform->Set_Pos(&vPos);
									}
									else
									{
										vPos = { vPos.x, vPos.y, vPos.z - fMoveZ };

										pPlayerTransform->Set_Pos(&vPos);
									}
								}
								else // X축으로 밀어냄
								{
									if ((ColMaxX + ColMinX) * 0.5f < fX)
									{
										vPos = { vPos.x + fMoveX, vPos.y, vPos.z };

										pPlayerTransform->Set_Pos(&vPos);
									}
									else
									{
										vPos = { vPos.x - fMoveX, vPos.y, vPos.z };

										pPlayerTransform->Set_Pos(&vPos);
									}
								}
							}
						}
						else
						{
							_float testy = vSourMax.y - abs(vSourMax.y * 0.2f);
							if (iter.first.find(L"Box", 0) != wstring::npos)
							{
								_vec3 vPlayerPos = *pPlayerTransform->Get_Info(Engine::INFO_POS);
								if (vDestMin.y > testy)
								{
									vPlayerPos = { vPlayerPos.x, vSourMax.y , vPlayerPos.z };
									pPlayer->Land();
									pPlayer->Set_OnCol(true);
									pPlayerTransform->Set_Pos(&vPlayerPos);
								}
								else
								{
									if (!pPlayer->Get_Turn())
									{
										_vec3 vPos = *pColObject->GetTransformCom()->Get_Info(Engine::INFO_POS);
										pColObject->GetTransformCom()->Set_Pos(vPos - pPlayer->Get_PreDir());
									}
								}
							}

						}
					}
					else if (i == 1)
					{
						pPlayer->Set_Trigger(L"Box_Trigger_Z");
					}
					else if (i == 2)
					{
						pPlayer->Set_Trigger(L"Box_Trigger_X");
					}
				}
				else
				{
					if (i == 1)
					{
						pPlayer->Delete_TriggerName(L"Box_Trigger_Z");
					}
					else if (i == 2)
					{
						pPlayer->Delete_TriggerName(L"Box_Trigger_X");
					}
				}
			}
		}

	}



}

void CollisionMgr::CollisionObjectToPlayerAABB(Engine::CGameObject * pObject1, Engine::CGameObject * pObject2)
{
	CPlayer* pPlayer = dynamic_cast<CPlayer*>(pObject1);
	NULL_CHECK(pPlayer);

	CInteractionObject* pColObject = dynamic_cast<CInteractionObject*>(pObject2);
	NULL_CHECK(pColObject);
	_vec3		vDestMin, vDestMax, vSourMin, vSourMax;

	_matrix     matPlayerColWorld = pPlayer->Get_ColWorld();
	_matrix     matColWorld = *pColObject->GetTransformCom()->Get_WorldMatrixPointer();



	D3DXVec3TransformCoord(&vDestMin, pPlayer->Get_BoxCollider()->Get_Min(), &matPlayerColWorld);
	D3DXVec3TransformCoord(&vDestMax, pPlayer->Get_BoxCollider()->Get_Max(), &matPlayerColWorld);
	//matColWorld._11 *= 0.5f;

	D3DXVec3TransformCoord(&vSourMin, pColObject->Get_BoxCollider()->Get_Min(), &matColWorld);
	D3DXVec3TransformCoord(&vSourMax, pColObject->Get_BoxCollider()->Get_Max(), &matColWorld);

	if (IsCollisionBoxToBox(vDestMin, vDestMax,
		vSourMin, vSourMax))
	{

		// x축의 반지름 합과 거리를 구한다.
		// 이 때 x축의 반지름 합이 x축의 거리보다 크면 x쪽으로 겹침.
		_float PlayerMaxX = vDestMax.x;
		_float PlayerMinX = vDestMin.x;
		_float ColMaxX = vSourMax.x;
		_float ColMinX = vSourMin.x;
		_float fSumX = (PlayerMaxX - PlayerMinX) * 0.5f +
			(ColMaxX - ColMinX) * 0.5f;
		_float fDistX = fabs((PlayerMaxX + PlayerMinX) * 0.5f - (ColMaxX + ColMinX) * 0.5f);


		_float PlayerMaxZ = vDestMax.z;
		_float PlayerMinZ = vDestMin.z;
		_float ColMaxZ = vSourMax.z;
		_float ColMinZ = vSourMin.z;

		// z축의 반지름 합과 거리를 구한다.
		// 이 때 z축의 반지름 합이 z축의 거리보다 크면 z쪽으로 겹침.
		_float fSumZ = (PlayerMaxZ - PlayerMinZ) * 0.5f + (ColMaxZ - ColMinZ) * 0.5f;
		_float fDistZ = fabs((PlayerMaxZ + PlayerMinZ) * 0.5f - (ColMaxZ + ColMinZ) * 0.5f);

		// 겹친영역의 크기 구하기.
		_float fMoveX = fSumX - fDistX;
		_float fMoveZ = fSumZ - fDistZ;

		Engine::CTransform* pPlayerTransform = pPlayer->GetTransformCom();
		_float fX = (PlayerMaxX + PlayerMinX) * 0.5f;
		_float fZ = (PlayerMaxZ + PlayerMinZ) * 0.5f;

		_vec3 vPos = *pPlayerTransform->Get_Info(Engine::INFO_POS);
		_float testy = vSourMax.y - abs(vSourMax.y * 0.1f);

		if (vDestMin.y > testy)
		{
			vPos = { vPos.x, vSourMax.y , vPos.z };
			pPlayer->Land();
			pPlayerTransform->Set_Pos(&vPos);
		}
		else if (fMoveX > fMoveZ) // Z축으로 밀어냄
		{
			if ((ColMaxZ + ColMinZ) * 0.5f < fZ)
			{
				vPos = { vPos.x, vPos.y, vPos.z + fMoveZ };

				pPlayerTransform->Set_Pos(&vPos);
				pPlayer->Set_ColWorld(pPlayer->Get_ColScale(), vPos);
			}
			else
			{
				vPos = { vPos.x, vPos.y, vPos.z - fMoveZ };

				pPlayerTransform->Set_Pos(&vPos);
			}
		}
		else // X축으로 밀어냄
		{
			if ((ColMaxX + ColMinX) * 0.5f < fX)
			{
				vPos = { vPos.x + fMoveX, vPos.y, vPos.z };

				pPlayerTransform->Set_Pos(&vPos);
			}
			else
			{
				vPos = { vPos.x - fMoveX, vPos.y, vPos.z };

				pPlayerTransform->Set_Pos(&vPos);
				pPlayer->Set_ColWorld(pPlayer->Get_ColScale(), vPos);
			}
		}
	}
}

_bool CollisionMgr::CollisionColPlayerAABB_Y(_vec3 vPos, Engine::CGameObject * pObject2)
{
	CColObject* pColObject = dynamic_cast<CColObject*>(pObject2);
	NULL_CHECK_RETURN(pColObject, false);
	_vec3	 vSourMin, vSourMax;

	_matrix     matColWorld = *pColObject->GetTransformCom()->Get_WorldMatrixPointer();

	matColWorld._11 *= 1.2f;
	matColWorld._33 *= 1.2f;

	D3DXVec3TransformCoord(&vSourMin, pColObject->Get_BoxCollider()->Get_Min(), &matColWorld);
	D3DXVec3TransformCoord(&vSourMax, pColObject->Get_BoxCollider()->Get_Max(), &matColWorld);

	if (((vSourMax.x > vPos.x) && (vSourMin.x < vPos.x)) && ((vSourMax.z > vPos.z) && (vSourMin.z < vPos.z)))
	{
		if (vPos.y < vSourMax.y)
		{
			return true;
		}
	}


	return false;
}

_bool CollisionMgr::CollisionObjectPlayerAABB_Y(_vec3 vPos, Engine::CGameObject* pObject2)
{
	CInteractionObject* pColObject = dynamic_cast<CInteractionObject*>(pObject2);
	NULL_CHECK_RETURN(pColObject,false);
	_vec3	 vSourMin, vSourMax;

	_matrix     matColWorld = *pColObject->GetTransformCom()->Get_WorldMatrixPointer();

	matColWorld._11 *= 1.2f;
	matColWorld._33 *= 1.2f;

	D3DXVec3TransformCoord(&vSourMin, pColObject->Get_BoxCollider()->Get_Min(), &matColWorld);
	D3DXVec3TransformCoord(&vSourMax, pColObject->Get_BoxCollider()->Get_Max(), &matColWorld);

	if (((vSourMax.x > vPos.x) && (vSourMin.x < vPos.x)) && ((vSourMax.z > vPos.z) && (vSourMin.z < vPos.z)))
	{
		if (vPos.y < vSourMax.y)
		{
			return true;
		}
	}


	return false;
}


void CollisionMgr::Set_Point(OBB* pObb, const _vec3* pMin, const _vec3* pMax)
{
	pObb->vPoint[0] = _vec3(pMin->x, pMax->y, pMin->z);
	pObb->vPoint[1] = _vec3(pMax->x, pMax->y, pMin->z);
	pObb->vPoint[2] = _vec3(pMax->x, pMin->y, pMin->z);
	pObb->vPoint[3] = _vec3(pMin->x, pMin->y, pMin->z);

	pObb->vPoint[4] = _vec3(pMin->x, pMax->y, pMax->z);
	pObb->vPoint[5] = _vec3(pMax->x, pMax->y, pMax->z);
	pObb->vPoint[6] = _vec3(pMax->x, pMin->y, pMax->z);
	pObb->vPoint[7] = _vec3(pMin->x, pMin->y, pMax->z);

	pObb->vCenter = (*pMin + *pMax) * 0.5f; // 나누기 2하는거에요...
}

void CollisionMgr::Set_Axis(OBB* pObb)
{
	pObb->vProjAxis[0] = (pObb->vPoint[2] + pObb->vPoint[5]) * 0.5f - pObb->vCenter;
	pObb->vProjAxis[1] = (pObb->vPoint[0] + pObb->vPoint[5]) * 0.5f - pObb->vCenter;
	pObb->vProjAxis[2] = (pObb->vPoint[7] + pObb->vPoint[5]) * 0.5f - pObb->vCenter;

	pObb->vAxis[0] = pObb->vPoint[2] - pObb->vPoint[3];
	pObb->vAxis[1] = pObb->vPoint[0] - pObb->vPoint[3];
	pObb->vAxis[2] = pObb->vPoint[7] - pObb->vPoint[3];

	for (_uint i = 0; i < 3; ++i)
		D3DXVec3Normalize(&pObb->vAxis[i], &pObb->vAxis[i]);
}








void CollisionMgr::CollisionLeechToPlayer(Engine::CLayer * pLayer, Engine::CGameObject * pObject)
{

	CPlayer* pPlayer = dynamic_cast<CPlayer*>(pObject);
	NULL_CHECK(pPlayer);


	auto mapObject = pLayer->GetMapObj();
	if (pPlayer->Get_ChaseBool())
	{
		for (auto iter : mapObject)
		{
			if (!pPlayer->Get_CatchLeech())
			{
				if (iter.first.find(L"Leech", 0) != wstring::npos)
				{
					CLeech* pColObject = dynamic_cast<CLeech*>(iter.second);
					NULL_CHECK(pColObject);
					_vec3 vPlayerPos = {};
					_vec3 vLeechPos = {};
					//Engine::CTransform* pColTransform = pColObject->GetTransformCom();
					vPlayerPos = *pPlayer->GetTransformCom()->Get_Info(Engine::INFO_POS);
					vLeechPos = pColObject->Get_ColPos();
					if ((D3DXVec3Length(&(vPlayerPos - vLeechPos))) < pPlayer->Get_ShpereRadius() + pColObject->Get_ShpereRadius())
					{
						//cout << "충돌" << endl;
						pPlayer->Set_CatchLeech(true);
						pColObject->Set_CatchPlayer(true);
					}
				}
			}
		}
	}

}

void CollisionMgr::CollisionJanitorToPlayer(Engine::CLayer * pLayer, Engine::CGameObject * pObject)
{
	CPlayer* pPlayer = dynamic_cast<CPlayer*>(pObject);
	NULL_CHECK(pPlayer);


	auto mapObject = pLayer->GetMapObj();

	for (auto iter : mapObject)
	{

		if (iter.first.find(L"Janitor", 0) != wstring::npos)
		{
			CJanitor* pColObject = dynamic_cast<CJanitor*>(iter.second);
			NULL_CHECK(pColObject);
			_vec3 vPlayerPos = {};
			_vec3 vJanitorPos = {};
			//Engine::CTransform* pColTransform = pColObject->GetTransformCom();
			vPlayerPos = *pPlayer->GetTransformCom()->Get_Info(Engine::INFO_POS);
			_matrix tempMat = pColObject->Get_BoneMat();
			_vec3 BoneLook = { tempMat._21 ,tempMat._22,tempMat._23 };
			if (!pPlayer->Get_JionterCatch())
			{
				for (int i = 0; i < 3; i++)
				{
					vJanitorPos = pColObject->Get_ColPos(i);
					if (pColObject->Get_State() != CJanitor::LAST_START && pColObject->Get_State() != CJanitor::LAST_LOOP && pColObject->Get_State() != CJanitor::LAST_CATCH 
						&&pColObject->Get_State() != CJanitor::LAST_BREAK)
					{
						if (i == 0)
						{
							D3DXVec3Normalize(&BoneLook, &BoneLook);
							vJanitorPos = _vec3(tempMat._41 + BoneLook.x * 2.f, tempMat._42 + BoneLook.y* 2.f, tempMat._43 + BoneLook.z* 2.f);
						}
						if (i > 0 && pColObject->Get_CatchFlag())
						{

							if (pColObject->Get_AniTime() > 0.125f && pColObject->Get_AniTime() < 0.7f)
							{
								if ((i == 1 && pColObject->Get_CatchLeft()) || (i == 2 && pColObject->Get_CatchRIght()) || (pColObject->Get_CatchRIght() && pColObject->Get_CatchLeft()))
									if ((D3DXVec3Length(&(vPlayerPos - vJanitorPos))) < pPlayer->Get_ShpereRadius() + pColObject->Get_ShpereRadius(i)*1.5f)
									{
										_vec3 vDir = *pColObject->GetTransformCom()->Get_Info(Engine::INFO_POS) - *pPlayer->GetTransformCom()->Get_Info(Engine::INFO_POS);
										_vec3 vPlayerLook = *pPlayer->GetTransformCom()->Get_Info(Engine::INFO_LOOK);
										vDir.y = vPlayerLook.y = 0.f;
										D3DXVec3Normalize(&vDir, &vDir);
										D3DXVec3Normalize(&vPlayerLook, &vPlayerLook);

										pPlayer->GetTransformCom()->Rotation(Engine::ROT_Y, acosf(D3DXVec3Dot(&vPlayerLook, &vDir)));
										pPlayer->Set_CatchJanitor(true, i, false, pColObject);
									}
							}

						}
						else if ((D3DXVec3Length(&(vPlayerPos - vJanitorPos))) < pPlayer->Get_ShpereRadius() + pColObject->Get_ShpereRadius(i))
						{
							//cout << "충돌" << endl;
							//pPlayer->Set_CatchLeech(true);	
							pColObject->Set_State(CJanitor::CATCH_PLAYER);

						}
					}
					else if (pColObject->Get_State() == CJanitor::LAST_LOOP)
					{
						if (i > 0 && (D3DXVec3Length(&(vPlayerPos - vJanitorPos))) < pPlayer->Get_ShpereRadius() + pColObject->Get_ShpereRadius(i))
						{
							if (pColObject->Get_AniTime() > 0.9f)
							{
								pColObject->Set_AnimationKey(13);
							}
							else if (pColObject->Get_AniTime() > 0.8f)
							{
								pColObject->Set_AnimationKey(12);
							}
							else if (pColObject->Get_AniTime() > 0.55f)
							{
								pColObject->Set_AnimationKey(11);
							}
							else if (pColObject->Get_AniTime() > 0.48f)
							{
								pColObject->Set_AnimationKey(10);
							}
							else if (pColObject->Get_AniTime() > 0.43f)
							{
								pColObject->Set_AnimationKey(9);
							}
							else if (pColObject->Get_AniTime() > 0.35f)
							{
								pColObject->Set_AnimationKey(8);
							}
							else if (pColObject->Get_AniTime() > 0.2f)
							{
								pColObject->Set_AnimationKey(7);
							}
							else if (pColObject->Get_AniTime() > 0.15f)
							{
								pColObject->Set_AnimationKey(6);
							}
							else if (pColObject->Get_AniTime() > 0.1f)
							{
								pColObject->Set_AnimationKey(5);
							}
							else if (pColObject->Get_AniTime() > 0.05f)
							{
								pColObject->Set_AnimationKey(4);
							}
							pColObject->Set_State(CJanitor::LAST_CATCH);
						}
					}
					else if (pColObject->Get_State() == CJanitor::LAST_CATCH)
					{
						if (i > 0)
						{
							if (pColObject->Get_AniTime() > 0.05f && pColObject->Get_AniTime() < 0.7f)
							{

								if ((D3DXVec3Length(&(vPlayerPos - vJanitorPos))) < pPlayer->Get_ShpereRadius() + pColObject->Get_ShpereRadius(i)*1.5f)
								{
									_vec3 vDir = *pColObject->GetTransformCom()->Get_Info(Engine::INFO_POS) - *pPlayer->GetTransformCom()->Get_Info(Engine::INFO_POS);
									_vec3 vPlayerLook = *pPlayer->GetTransformCom()->Get_Info(Engine::INFO_LOOK);
									vDir.y = vPlayerLook.y = 0.f;
									D3DXVec3Normalize(&vDir, &vDir);
									D3DXVec3Normalize(&vPlayerLook, &vPlayerLook);

									pPlayer->GetTransformCom()->Rotation(Engine::ROT_Y, acosf(D3DXVec3Dot(&vPlayerLook, &vDir)));
									pPlayer->Set_CatchJanitor(true, i, false, pColObject);
								}
							}

						}
					}
				}
			}

		}
	}

}

void CollisionMgr::CollisionJanitorArmToPlayer(Engine::CLayer * pLayer, Engine::CGameObject * pObject)
{

	CPlayer* pPlayer = dynamic_cast<CPlayer*>(pObject);
	NULL_CHECK(pPlayer);


	auto mapObject = pLayer->GetMapObj();

	for (auto iter : mapObject)
	{

		if (iter.first.find(L"Janitor_Arm", 0) != wstring::npos)
		{
			CJanitorArm* pColObject = dynamic_cast<CJanitorArm*>(iter.second);
			NULL_CHECK(pColObject);
			_vec3 vPlayerPos = {};
			_vec3 vJanitorPos = {};
			vPlayerPos = *pPlayer->GetTransformCom()->Get_Info(Engine::INFO_POS);

			for (int i = 0; i < 2; i++)
			{
				if (!pPlayer->Get_JionterCatch())
				{
					vJanitorPos = pColObject->Get_ColPos(i);

					if (pColObject->Get_CatchFlag())
					{
						if ((D3DXVec3Length(&(vPlayerPos - vJanitorPos))) < pPlayer->Get_ShpereRadius() + pColObject->Get_ShpereRadius(i))
						{
							if (pColObject->Get_AniTime() > 0.05f && pColObject->Get_AniTime() < 0.7f)
							{
								_vec3 vDir = *pColObject->GetTransformCom()->Get_Info(Engine::INFO_POS) - *pPlayer->GetTransformCom()->Get_Info(Engine::INFO_POS);
								_vec3 vPlayerLook = *pPlayer->GetTransformCom()->Get_Info(Engine::INFO_LOOK);
								vDir.y = vPlayerLook.y = 0.f;
								D3DXVec3Normalize(&vDir, &vDir);
								D3DXVec3Normalize(&vPlayerLook, &vPlayerLook);

								pPlayer->GetTransformCom()->Rotation(Engine::ROT_Y, acosf(D3DXVec3Dot(&vPlayerLook, &vDir)));
								pPlayer->Set_CatchJanitor(true, i + 1, true, pColObject);
							}
						}
					}
					else if ((D3DXVec3Length(&(vPlayerPos - vJanitorPos))) < pPlayer->Get_ShpereRadius() + pColObject->Get_ShpereRadius(i))
					{
						pColObject->Set_State(CJanitorArm::CATCH_PLAYER);
					}
					float test1 = D3DXVec3Length(&(vPlayerPos - vJanitorPos));
					float test2 = pPlayer->Get_ShpereRadius() + pColObject->Get_ShpereRadius(i);
					test1 - test2;
				}
			}

		}
	}

}

void CollisionMgr::CollisionJanitorToPlayerAABB(Engine::CLayer * pLayer, Engine::CGameObject * pObject)
{

	CPlayer* pPlayer = dynamic_cast<CPlayer*>(pObject);
	NULL_CHECK(pPlayer);


	auto mapObject = pLayer->GetMapObj();

	for (auto iter : mapObject)
	{
		if (!pPlayer->Get_JionterCatch())
		{
			if (iter.first.find(L"Janitor", 0) != wstring::npos)
			{
				CJanitor* pColObject = dynamic_cast<CJanitor*>(iter.second);
				NULL_CHECK(pColObject);
				_vec3 vPlayerPos = {};
				_vec3 vJanitorPos = {};
				//Engine::CTransform* pColTransform = pColObject->GetTransformCom();
				vPlayerPos = *pPlayer->GetTransformCom()->Get_Info(Engine::INFO_POS);

				_vec3		vDestMin, vDestMax, vSourMin, vSourMax;

				_matrix     matPlayerColWorld = pPlayer->Get_ColWorld();
				_matrix     matJanitorWorld = pColObject->Get_ColWorld();
				Engine::CTransform* pColTransform = pColObject->GetTransformCom();



				D3DXVec3TransformCoord(&vDestMin, pPlayer->Get_BoxCollider()->Get_Min(), &matPlayerColWorld);
				D3DXVec3TransformCoord(&vDestMax, pPlayer->Get_BoxCollider()->Get_Max(), &matPlayerColWorld);

				D3DXVec3TransformCoord(&vSourMin, pColObject->Get_BoxCollider()->Get_Min(), &matJanitorWorld);
				D3DXVec3TransformCoord(&vSourMax, pColObject->Get_BoxCollider()->Get_Max(), &matJanitorWorld);

				if (IsCollisionBoxToBox(vDestMin, vDestMax,
					vSourMin, vSourMax))
				{

					_float PlayerMaxX = vDestMax.x;
					_float PlayerMinX = vDestMin.x;
					_float ColMaxX = vSourMax.x;
					_float ColMinX = vSourMin.x;
					_float fSumX = (PlayerMaxX - PlayerMinX) * 0.5f +
						(ColMaxX - ColMinX) * 0.5f;
					_float fDistX = fabs((PlayerMaxX + PlayerMinX) * 0.5f - (ColMaxX + ColMinX) * 0.5f);


					_float PlayerMaxZ = vDestMax.z;
					_float PlayerMinZ = vDestMin.z;
					_float ColMaxZ = vSourMax.z;
					_float ColMinZ = vSourMin.z;

					// z축의 반지름 합과 거리를 구한다.
					// 이 때 z축의 반지름 합이 z축의 거리보다 크면 z쪽으로 겹침.
					_float fSumZ = (PlayerMaxZ - PlayerMinZ) * 0.5f + (ColMaxZ - ColMinZ) * 0.5f;
					_float fDistZ = fabs((PlayerMaxZ + PlayerMinZ) * 0.5f - (ColMaxZ + ColMinZ) * 0.5f);

					// 겹친영역의 크기 구하기.
					_float fMoveX = fSumX - fDistX;
					_float fMoveZ = fSumZ - fDistZ;

					Engine::CTransform* pPlayerTransform = pPlayer->GetTransformCom();
					_float fX = (PlayerMaxX + PlayerMinX) * 0.5f;
					_float fZ = (PlayerMaxZ + PlayerMinZ) * 0.5f;
					_vec3 vPos = *pPlayerTransform->Get_Info(Engine::INFO_POS);


					if (pColObject->Get_State() == CJanitor::WALK || pColObject->Get_State() == CJanitor::BED_SPAWN)
					{
						pColObject->Reset_Ani();
						_matrix matBone = pColObject->Get_BoneMat();
						//_vec3 vBonePos = { matBone._41,matBone._42,matBone._43 };
						//_vec3 vDir = *pPlayer->GetTransformCom()->Get_Info(Engine::INFO_POS) - vBonePos;
						//_vec3 vLook = { matBone._31,matBone._32,matBone._33 };
						_vec3 vDir = *pPlayer->GetTransformCom()->Get_Info(Engine::INFO_POS) - *pColObject->GetTransformCom()->Get_Info(Engine::INFO_POS);
						_vec3 vLook = *pColObject->GetTransformCom()->Get_Info(Engine::INFO_LOOK);
						D3DXVec3Normalize(&vDir, &vDir);
						D3DXVec3Normalize(&vLook, &vLook);

						if (D3DXToDegree(acosf(D3DXVec3Dot(&vDir, &vLook))) > 120.f)
						{
							pColObject->Set_State(CJanitor::TURN_180);
							pColObject->Set_TurnChase(true);
						}
						else
							pColObject->Set_State(CJanitor::CHASE_PLAYER);
					}
					if (fMoveX > fMoveZ) // Z축으로 밀어냄
					{
						if ((ColMaxZ + ColMinZ) * 0.5f < fZ)
						{
							vPos = { vPos.x, vPos.y, vPos.z + fMoveZ };
							pPlayerTransform->Set_Pos(&vPos);
						}
						else
						{
							vPos = { vPos.x, vPos.y, vPos.z - fMoveZ };
							pPlayerTransform->Set_Pos(&vPos);
						}
					}
					else // X축으로 밀어냄
					{
						if ((ColMaxX + ColMinX) * 0.5f < fX)
						{
							vPos = { vPos.x + fMoveX, vPos.y, vPos.z };
							pPlayerTransform->Set_Pos(&vPos);
						}
						else
						{
							vPos = { vPos.x - fMoveX, vPos.y, vPos.z };
							pPlayerTransform->Set_Pos(&vPos);
						}
					}
				}
			}
		}
	}

}



//void CollisionMgr::CollisionWallToMonster(Engine::CLayer * pLayerWall, Engine::CLayer * pLayerMonster)
//{
//	auto mapWalls = pLayerWall->GetMapObj();
//	auto mapMonsters = pLayerMonster->GetMapObj();
//
//	for (auto iterWall : mapWalls)
//	{
//		CColObject * pWall = dynamic_cast<CColObject*>(iterWall.second);
//		NULL_CHECK(pWall);
//
//		const BOXCOLLIDER& tWallCollider = *pWall->Get_Collider();
//
//		for (auto iterMonsters : mapMonsters)
//		{
//			CMonster * pMonster = dynamic_cast<CMonster*>(iterMonsters.second);
//			NULL_CHECK(pMonster);
//			if (pMonster->Get_CurHP()<=0.f)
//				continue;
//			const BOXCOLLIDER& tMonsterCollider = pMonster->Get_BoxCollider();
//
//			if (IsCollisionBoxToBox(tMonsterCollider.vMin, tMonsterCollider.vMax, tWallCollider.vMin, tWallCollider.vMax))
//			{
//
//				// x축의 반지름 합과 거리를 구한다.
//				// 이 때 x축의 반지름 합이 x축의 거리보다 크면 x쪽으로 겹침.
//				_float fSumX = (tMonsterCollider.vMax.x - tMonsterCollider.vMin.x) * 0.5f + (tWallCollider.vMax.x - tWallCollider.vMin.x) * 0.5f;
//				_float fDistX = fabs((tMonsterCollider.vMax.x + tMonsterCollider.vMin.x) * 0.5f - (tWallCollider.vMax.x + tWallCollider.vMin.x) * 0.5f);
//
//				// z축의 반지름 합과 거리를 구한다.
//				// 이 때 z축의 반지름 합이 z축의 거리보다 크면 z쪽으로 겹침.
//				_float fSumZ = (tMonsterCollider.vMax.z - tMonsterCollider.vMin.z) * 0.5f + (tWallCollider.vMax.z - tWallCollider.vMin.z) * 0.5f;
//				_float fDistZ = fabs((tMonsterCollider.vMax.z + tMonsterCollider.vMin.z) * 0.5f - (tWallCollider.vMax.z + tWallCollider.vMin.z) * 0.5f);
//
//				// 겹친영역의 크기 구하기.
//				_float fMoveX = fSumX - fDistX;
//				_float fMoveZ = fSumZ - fDistZ;
//
//				Engine::CTransform* pMonsterTransform = dynamic_cast<Engine::CTransform*>(
//					pMonster->Get_Component(L"Com_Transform", Engine::ID_DYNAMIC));
//				NULL_CHECK(pMonsterTransform);
//
//
//				_float fX = (tMonsterCollider.vMax.x + tMonsterCollider.vMin.x) * 0.5f;
//				_float fZ = (tMonsterCollider.vMax.z + tMonsterCollider.vMin.z) * 0.5f;
//
//				if (fMoveX > fMoveZ) // Z축으로 밀어냄
//				{
//					if ((tWallCollider.vMax.z + tWallCollider.vMin.z) * 0.5f < fZ)
//					{
//						_vec3 vPos = *pMonsterTransform->Get_Info(Engine::INFO_POS);
//						vPos = { vPos.x, pMonsterTransform->Get_WorldMatrixPointer()->_42, vPos.z + fMoveZ };
//
//						pMonsterTransform->Set_Pos(&vPos);
//						pMonsterTransform->m_vInfo[Engine::INFO_POS] = vPos;
//						pMonster->Update_BoxCollider();
//					}
//					else
//					{
//						_vec3 vPos = *pMonsterTransform->Get_Info(Engine::INFO_POS);
//						vPos = { vPos.x, pMonsterTransform->Get_WorldMatrixPointer()->_42, vPos.z - fMoveZ };
//
//						pMonsterTransform->Set_Pos(&vPos);
//						pMonsterTransform->m_vInfo[Engine::INFO_POS] = vPos;
//						pMonster->Update_BoxCollider();
//					}
//				}
//				else // X축으로 밀어냄
//				{
//					if ((tWallCollider.vMax.x + tWallCollider.vMin.x) * 0.5f < fX)
//					{
//						_vec3 vPos = *pMonsterTransform->Get_Info(Engine::INFO_POS);
//						vPos = { vPos.x + fMoveX, pMonsterTransform->Get_WorldMatrixPointer()->_42, vPos.z };
//
//						pMonsterTransform->Set_Pos(&vPos);
//						pMonsterTransform->m_vInfo[Engine::INFO_POS] = vPos;
//						pMonster->Update_BoxCollider();
//					}
//					else
//					{
//						_vec3 vPos = *pMonsterTransform->Get_Info(Engine::INFO_POS);
//						vPos = { vPos.x - fMoveX, pMonsterTransform->Get_WorldMatrixPointer()->_42, vPos.z };
//
//						pMonsterTransform->Set_Pos(&vPos);
//						pMonsterTransform->m_vInfo[Engine::INFO_POS] = vPos;
//						pMonster->Update_BoxCollider();
//					}
//				}
//			}
//
//		}
//	}
//}

_bool CollisionMgr::IsCollisionBoxToBox(const _vec3 & vDestMin,
	const _vec3 & vDestMax,
	const _vec3 & vSrcMin,
	const _vec3 & vSrcMax)
{
	if (vDestMin.x <= vSrcMax.x && vDestMax.x >= vSrcMin.x &&
		vDestMin.y <= vSrcMax.y && vDestMax.y >= vSrcMin.y &&
		vDestMin.z <= vSrcMax.z && vDestMax.z >= vSrcMin.z)
	{
		return true;
	}

	return false;
}

_bool CollisionMgr::IsCollisionSphereToSphere(const SPHERECOLLIDER & tDest, const SPHERECOLLIDER & tSrc)
{
	_float fLength = D3DXVec3Length(&(tDest.vCenter - tSrc.vCenter));

	if (fLength <= tDest.fRadius + tSrc.fRadius)
		return true;

	return false;
}

_bool CollisionMgr::Collision_OBB(const _vec3* pDestMin, const _vec3* pDestMax, const _matrix* pDestWorld, const _vec3* pSourMin, const _vec3* pSourMax, const _matrix* pSourWorld)
{

	OBB		tObb[2];
	ZeroMemory(&tObb, sizeof(OBB) * 2);

	Set_Point(&tObb[0], pDestMin, pDestMax);
	Set_Point(&tObb[1], pSourMin, pSourMax);

	for (_uint i = 0; i < 8; ++i)
	{
		D3DXVec3TransformCoord(&tObb[0].vPoint[i], &tObb[0].vPoint[i], pDestWorld);
		D3DXVec3TransformCoord(&tObb[1].vPoint[i], &tObb[1].vPoint[i], pSourWorld);
	}
	D3DXVec3TransformCoord(&tObb[0].vCenter, &tObb[0].vCenter, pDestWorld);
	D3DXVec3TransformCoord(&tObb[1].vCenter, &tObb[1].vCenter, pSourWorld);

	for (_uint i = 0; i < 2; ++i)
		Set_Axis(&tObb[i]);


	// 구 충돌과 비슷한 방식으로 충돌처리를 한다.

	_float		fDistance[3]{};			// 0번 : 첫 번째 박스의 각 면으로 뻗어나간 방향벡터의 임의의 축으로 투영한 길이의 합
										// 1번 : 두 번째 박스의 각 면으로 뻗어나간 방향벡터의 임의의 축으로 투영한 길이의 합
										// 2번 : 두 박스의 중점 간의 길이를 임의의 축으로 투영한 길이


	for (_uint i = 0; i < 2; ++i)
	{
		for (_uint j = 0; j < 3; ++j)
		{
			fDistance[0] = fabs(D3DXVec3Dot(&tObb[0].vProjAxis[0], &tObb[i].vAxis[j])) +
				fabs(D3DXVec3Dot(&tObb[0].vProjAxis[1], &tObb[i].vAxis[j])) +
				fabs(D3DXVec3Dot(&tObb[0].vProjAxis[2], &tObb[i].vAxis[j]));

			fDistance[1] = fabs(D3DXVec3Dot(&tObb[1].vProjAxis[0], &tObb[i].vAxis[j])) +
				fabs(D3DXVec3Dot(&tObb[1].vProjAxis[1], &tObb[i].vAxis[j])) +
				fabs(D3DXVec3Dot(&tObb[1].vProjAxis[2], &tObb[i].vAxis[j]));

			fDistance[2] = fabs(D3DXVec3Dot(&(tObb[1].vCenter - tObb[0].vCenter), &tObb[i].vAxis[j]));

			if (fDistance[0] + fDistance[1] < fDistance[2])
				return false;
		}
	}

	return true;
}

_bool CollisionMgr::IsCollisionRayToSphere(_vec3 & vIntersection,
	_float & fDist,
	const LPDIRECT3DDEVICE9 pGraphicDev,
	const SPHERECOLLIDER & tDest)
{
	//SetCursorPos(WINCX * 0.5f, WINCY * 0.5f);

	//if (Engine::Get_KeyMgr()->KeyDown(Engine::KEY_LBUTTON))
	//{
	POINT			ptMouse{};

	// 윈도우 영역 == 뷰포트 영역
	GetCursorPos(&ptMouse);
	ScreenToClient(g_hWnd, &ptMouse);

	// 뷰 포트에서 투영으로

	D3DVIEWPORT9			ViewPort;
	ZeroMemory(&ViewPort, sizeof(D3DVIEWPORT9));
	pGraphicDev->GetViewport(&ViewPort);

	_vec3	vMousePos;

	vMousePos.x = ptMouse.x / (ViewPort.Width * 0.5f) - 1.f;
	vMousePos.y = -ptMouse.y / (ViewPort.Height * 0.5f) + 1.f;
	vMousePos.z = 0.f;

	// 투영에서 뷰 스페이스로
	_matrix		matProj;

	pGraphicDev->GetTransform(D3DTS_PROJECTION, &matProj);
	D3DXMatrixInverse(&matProj, NULL, &matProj);
	D3DXVec3TransformCoord(&vMousePos, &vMousePos, &matProj);

	_vec3	vRayPos, vRayDir;

	vRayPos = _vec3(0.f, 0.f, 0.f);
	vRayDir = vMousePos - vRayPos;

	// 뷰 스페이스에서 월드 스페이스로
	_matrix		matView;

	pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
	D3DXMatrixInverse(&matView, NULL, &matView);
	D3DXVec3TransformCoord(&vRayPos, &vRayPos, &matView);
	D3DXVec3TransformNormal(&vRayDir, &vRayDir, &matView);
	D3DXVec3Normalize(&vRayDir, &vRayDir);

	_vec3 l = tDest.vCenter - vRayPos;

	_float s = D3DXVec3Dot(&l, &vRayDir);

	_float l2 = D3DXVec3Dot(&l, &l);

	_float r2 = pow(tDest.fRadius, 2);

	if (s < 0 && l2 > r2)
		return false;                       // 광선이 구의 반대 방향을 향하거나 구를 지나친 경우

	_float m2 = l2 - pow(s, 2);


	if (m2 > r2)
		return false;                      // 광선이 구를 비껴가는 경우

	_float q = sqrt(r2 - m2);

	// 두 개의 교차점 중 어느 것을 구하는가?   
	fDist = 0.f;

	if (l2 > r2)
		fDist = s - q;
	else
		fDist = s + q;

	//static int iCnt = 1000;

	//wstring wstrProjectile = L"Bat_spit" + to_wstring(iCnt);
	//Engine::CGameObject * pGo = CProjectile::Create(pGraphicDev, L"Texture_Bat_Spit", vRayPos, vRayDir);


	//Engine::Get_Layer(L"02.GameLogic")->Add_GameObject(wstrProjectile.c_str(), pGo);
	//++iCnt;

	vIntersection = vRayPos + vRayDir * fDist;

	return true;

	//}

	//return false;
}

void CollisionMgr::Ray(const LPDIRECT3DDEVICE9 pGraphicDev, _vec3 & vRayPos, _vec3 & vRayDir)
{
	POINT			ptMouse{};

	// 윈도우 영역 == 뷰포트 영역
	GetCursorPos(&ptMouse);
	ScreenToClient(g_hWnd, &ptMouse);

	// 뷰 포트에서 투영으로

	D3DVIEWPORT9			ViewPort;
	ZeroMemory(&ViewPort, sizeof(D3DVIEWPORT9));
	pGraphicDev->GetViewport(&ViewPort);

	_vec3	vMousePos;

	vMousePos.x = ptMouse.x / (ViewPort.Width * 0.5f) - 1.f;
	vMousePos.y = -ptMouse.y / (ViewPort.Height * 0.5f) + 1.f;
	vMousePos.z = 0.f;

	// 투영에서 뷰 스페이스로
	_matrix		matProj;

	pGraphicDev->GetTransform(D3DTS_PROJECTION, &matProj);
	D3DXMatrixInverse(&matProj, NULL, &matProj);
	D3DXVec3TransformCoord(&vMousePos, &vMousePos, &matProj);

	vRayPos = _vec3(0.f, 0.f, 0.f);
	vRayDir = vMousePos - vRayPos;

	// 뷰 스페이스에서 월드 스페이스로
	_matrix		matView;

	pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
	D3DXMatrixInverse(&matView, NULL, &matView);
	D3DXVec3TransformCoord(&vRayPos, &vRayPos, &matView);
	D3DXVec3TransformNormal(&vRayDir, &vRayDir, &matView);
	D3DXVec3Normalize(&vRayDir, &vRayDir);
}
