#ifndef Struct_h__
#define Struct_h__

typedef struct tagFrame
{
	_uint  uiAnimationID;
	_float fFrameCnt;
	_float fMaxFrameCnt;
	_float fTimeScale;
	_float fWidth;
	_float fHeight;
	_vec3  vOffset;

}FRAME;
typedef struct tagObjectState
{
	_uint	uiLev;
	_float	fCurHP;
	_float	fMaxHP;
	_float	fCurEXE;
	_float	fMaxEXE;
	_float	fDamage;
}OBJ_DATA;

typedef struct tagBoxCollider
{
	_vec3	vMax;
	_vec3	vMin;
}BOXCOLLIDER;

typedef struct tagSphereCollider
{
	_vec3	vCenter;
	_float	fRadius;
}SPHERECOLLIDER;



#endif // Struct_h__
