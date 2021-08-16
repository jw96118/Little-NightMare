#ifndef DynamicMesh_h__
#define DynamicMesh_h__

#include "Mesh.h"
#include "HierarchyLoader.h"
#include "AniCtrl.h"

BEGIN(Engine)

class ENGINE_DLL	CDynamicMesh : public CMesh
{
private:
	explicit CDynamicMesh(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CDynamicMesh(const CDynamicMesh& rhs);
	virtual ~CDynamicMesh(void);

public:
	HRESULT			Ready_Meshes(const _tchar* pFilePath, const _tchar* pFileName);
	void			Render_Meshes(void);
	void			Render_Meshes(LPD3DXEFFECT& pEffect);
	const  D3DXFRAME_DERIVED*	Get_FrameByName(const char* pFrameName);
	_bool							Is_AnimationSetEnd(void);
	_float          Get_AniPercent();
public:
	void			Set_AnimationSet(const _uint& iIndex, const _double& AniCount = 0.25);
	void			Play_Animation(const _float& fTimeDelta);
	void			Reset_Animation();
	void            Set_Matrix(const _float& fY);
	void            Set_Test(CDynamicMesh * pMesh);
private:

	// ��� ������ ��ȸ�ϸ鼭 ������ ���� �ִ� TransformationMatrix�� pParentMatrix�� �����Ͽ� CombinedTransformationMatrix�� ������ִ� �Լ�
	void			Update_FrameMatrices(D3DXFRAME_DERIVED* pFrame, const _matrix* pParentMatrix);

	// CombinedTransformationMatrix�� �ּҰ��� ���ͼ� ä���ִ� �Լ�
	void			SetUp_FrameMatrixPointer(D3DXFRAME_DERIVED* pFrame);

private:
	D3DXFRAME*				m_pRootFrame;
	CHierarchyLoader*		m_pLoader;
	CAniCtrl*				m_pAniCtrl;
	list<D3DXMESHCONTAINER_DERIVED*>			m_MeshContainerList;
	CDynamicMesh*           m_pPlayerRIght = nullptr;
	_int                    m_uAniNum = 0;
	_int                    m_iTestIndex = 0;
	_vec3                   m_vPrePos = {0.f,0.f,0.f};
public:
	static	CDynamicMesh*		Create(LPDIRECT3DDEVICE9 pGraphicDev, const _tchar* pFilePath, const _tchar* pFileName);
	virtual CResources*			Clone(void);
	virtual void				Free(void);

};

END
#endif // DynamicMesh_h__


//typedef struct _D3DXFRAME
//{
//	LPSTR                   Name;						// ���� �̸��� �����ϱ� ���� ���ڿ�(�ƽ�Ű�ڵ�)
//	D3DXMATRIX              TransformationMatrix;		// ���� ���� �ε� ������ �������
//
//	LPD3DXMESHCONTAINER     pMeshContainer;
//
//	struct _D3DXFRAME       *pFrameSibling;				// ���� ���� �ּҸ� �����ϴ� ������
//	struct _D3DXFRAME       *pFrameFirstChild;			// �ڽ� ���� �ּҸ� �����ϴ� ������
//		
//} D3DXFRAME, *LPD3DXFRAME;
//
//
//typedef struct _D3DXMESHCONTAINER
//{
//	LPSTR                   Name;					// �޽� �����̳��� �̸��� �����ϱ� ���� ���ڿ�(�ƽ�Ű�ڵ� : �ٵ� ��κ� ���ų� �Ű澲�� �ʴ´�)
//	D3DXMESHDATA            MeshData;				// �޽��� ������ �����ϱ� ���� ����ü��, �İ�ü�� �����ϰ� �ִ� ����ü
//	LPD3DXMATERIAL          pMaterials;				// ���� ������ �����ϴ� ������
//	LPD3DXEFFECTINSTANCE    pEffects;				// �޽��� ���� Ư���� ����Ʈ(�츮�� ������� ���Ѵ�)
//	DWORD                   NumMaterials;			// ������ ����
//	DWORD                  *pAdjacency;				// ������ �������� ù ��° �ּҰ��� �����ϴ� ������
//	LPD3DXSKININFO          pSkinInfo;				// ��Ű�� �ִϸ��̼ǽ� �ʿ��� ���� �Լ��� �����ϴ� �İ�ü
//
//	struct _D3DXMESHCONTAINER *pNextMeshContainer;	// ���� ����� �޽� �����̳��� �ּ�(��ǻ� ������� ����)
//} D3DXMESHCONTAINER, *LPD3DXMESHCONTAINER;