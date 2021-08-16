#include "DynamicMesh.h"
#include "iostream"
using namespace std;
USING(Engine)

Engine::CDynamicMesh::CDynamicMesh(LPDIRECT3DDEVICE9 pGraphicDev)
	: CMesh(pGraphicDev)
	, m_pRootFrame(nullptr)
	, m_pLoader(nullptr)
	, m_pAniCtrl(nullptr)
{

}

Engine::CDynamicMesh::CDynamicMesh(const CDynamicMesh& rhs)
	: CMesh(rhs)
	, m_pRootFrame(rhs.m_pRootFrame)
	, m_pLoader(rhs.m_pLoader)
	, m_MeshContainerList(rhs.m_MeshContainerList)

{
	m_pAniCtrl = CAniCtrl::Create(*rhs.m_pAniCtrl);
}

Engine::CDynamicMesh::~CDynamicMesh(void)
{

}

HRESULT Engine::CDynamicMesh::Ready_Meshes(const _tchar* pFilePath, const _tchar* pFileName)
{
	_tchar		szFullPath[256] = L"";

	lstrcpy(szFullPath, pFilePath);
	lstrcat(szFullPath, pFileName);

	m_pLoader = CHierarchyLoader::Create(m_pGraphicDev, pFilePath);
	NULL_CHECK_RETURN(m_pLoader, E_FAIL);

	LPD3DXANIMATIONCONTROLLER		pAniCtrl = nullptr;

	FAILED_CHECK_RETURN(D3DXLoadMeshHierarchyFromX(szFullPath,
		D3DXMESH_MANAGED,
		m_pGraphicDev,
		m_pLoader,	// HierarchyLoader
		NULL,
		&m_pRootFrame,
		&pAniCtrl),	// AniCtrl
		E_FAIL);

	m_pAniCtrl = CAniCtrl::Create(pAniCtrl);
	NULL_CHECK_RETURN(m_pAniCtrl, E_FAIL);
	//int i = 0;
	//while (true)
	//{
	//	string str = m_pAniCtrl->Get_AnimationName(i);
	//	if (str == "")
	//	{
	//		break;
	//	}
	//	string num ;
	//	num = to_string(i) + ". ";
	//	num += str;
	//	cout << num << endl;
	//	i++;
	//}
	Safe_Release(pAniCtrl);

	_matrix		matTemp;

	Update_FrameMatrices((D3DXFRAME_DERIVED*)m_pRootFrame, D3DXMatrixRotationY(&matTemp, D3DXToRadian(180.f)));

	SetUp_FrameMatrixPointer((D3DXFRAME_DERIVED*)m_pRootFrame);

	return S_OK;
}

void Engine::CDynamicMesh::Render_Meshes(void)
{
	for (auto& iter : m_MeshContainerList)
	{
		D3DXMESHCONTAINER_DERIVED*			pMeshContainer = iter;

		for (_ulong i = 0; i < pMeshContainer->dwNumBones; ++i)
		{
			if(m_iTestIndex == 0)
				pMeshContainer->pRenderingMatrix[i] = pMeshContainer->pFrameOffsetMatrix[i] * (*pMeshContainer->ppFrameCombinedMatrix[i]);
			else
			{
				if (i == 22 || i == 19 || i == 29 || i == 30 || i == 32 || i == 34
					|| i == 15 || i == 7 || i == 25 || i == 20 || i == 24 || i == 28 ||
					i == 11 || i == 12 || i == 14 || i == 8 || i == 9 || i == 26)
				{

					pMeshContainer->pFrameOffsetMatrix[i] = pMeshContainer->pFrameOffsetMatrix[6] * (*pMeshContainer->ppFrameCombinedMatrix[6]);
				}
				else
				{
					pMeshContainer->pRenderingMatrix[i] = pMeshContainer->pFrameOffsetMatrix[i] * (*pMeshContainer->ppFrameCombinedMatrix[i]);
				}
			}
		}

		void*		pSrcVtx = nullptr;
		void*		pDestVtx = nullptr;


		pMeshContainer->pOriMesh->LockVertexBuffer(0, &pSrcVtx);
		pMeshContainer->MeshData.pMesh->LockVertexBuffer(0, &pDestVtx);

		// 소프트웨어 스키닝을 수행하는 함수(스키닝 뿐 아니라 애니메이션 변경 시, 뼈대들과 정점 정보들의 변경을 동시에 수행하기도 함)
		pMeshContainer->pSkinInfo->UpdateSkinnedMesh(pMeshContainer->pRenderingMatrix,	// 최종 뼈의 변환상태 행렬
			nullptr,						// 원래 상태로 되돌리기 위한 상태 행렬(원래는 위 행렬의 역행렬을 구해서 넣어줘야 하지만 안넣어줘도 상관 없음)
			pSrcVtx,						// 변하지 않는 원본 메쉬의 정점 정보
			pDestVtx);						// 변환된 정보를 담기 위한 메쉬의 정점 정보


		for (_ulong i = 0; i < pMeshContainer->NumMaterials; ++i)
		{
			m_pGraphicDev->SetTexture(0, pMeshContainer->ppTexture[i]);
			pMeshContainer->MeshData.pMesh->DrawSubset(i);
		}

		pMeshContainer->pOriMesh->UnlockVertexBuffer();
		pMeshContainer->MeshData.pMesh->UnlockVertexBuffer();
	}
}

const Engine::D3DXFRAME_DERIVED* Engine::CDynamicMesh::Get_FrameByName(const char* pFrameName)
{
	return (D3DXFRAME_DERIVED*)D3DXFrameFind(m_pRootFrame, pFrameName);
}

_bool Engine::CDynamicMesh::Is_AnimationSetEnd(void)
{
	return m_pAniCtrl->Is_AnimationSetEnd();
}

_float CDynamicMesh::Get_AniPercent()
{
	return m_pAniCtrl->Get_AniPercent();
}

void Engine::CDynamicMesh::Render_Meshes(LPD3DXEFFECT& pEffect)
{
	D3DXMESHCONTAINER_DERIVED*			pRightMeshContainer =nullptr;
	if (m_pPlayerRIght != nullptr)
	{
		for (auto& iter : m_pPlayerRIght->m_MeshContainerList)
		{
			pRightMeshContainer = iter;
		}

	}
	for (auto& iter : m_MeshContainerList)
	{
		D3DXMESHCONTAINER_DERIVED*			pMeshContainer = iter;

		for (_ulong i = 0; i < pMeshContainer->dwNumBones; ++i)
		{
			if (m_iTestIndex == 0)
				pMeshContainer->pRenderingMatrix[i] = pMeshContainer->pFrameOffsetMatrix[i] * (*pMeshContainer->ppFrameCombinedMatrix[i]);
			else
			{
				if (i == 22 || i == 19 || i == 29 || i == 30 || i == 32 || i == 34
					|| i == 15 || i == 7 || i == 25 || i == 20 || i == 24 || i == 12 ||
					i == 11 || i == 16 || i == 14 || i == 8 || i == 9 || i == 26 || i == 6
					|| i ==10)
				{
					Play_Animation(0.f);
					pMeshContainer->pRenderingMatrix[i] = pMeshContainer->pFrameOffsetMatrix[i] * (*pMeshContainer->ppFrameCombinedMatrix[i]);
			
				}
				else if (i == 37 || i == 31 || i == 46 || i == 50 || i == 45 || i == 27 || i == 35 || i == 36 ||
					i == 38 || i == 28 || i == 48 || i == 49 || i == 44 || i == 47 || i == 51 || i == 41 || i == 42 || i == 43 || i == 39 || i == 33)
				{
					if (pRightMeshContainer != nullptr)
					{
						m_pPlayerRIght->Play_Animation(0.f);

						pMeshContainer->pRenderingMatrix[i] = pRightMeshContainer->pFrameOffsetMatrix[i] * *pRightMeshContainer->ppFrameCombinedMatrix[i];

					}
				}
				else
				{
					Play_Animation(0.f);
					pMeshContainer->pRenderingMatrix[i] = pMeshContainer->pFrameOffsetMatrix[i] * (*pMeshContainer->ppFrameCombinedMatrix[i]);
				}
			}
		}

		void*		pSrcVtx = nullptr;
		void*		pDestVtx = nullptr;


		pMeshContainer->pOriMesh->LockVertexBuffer(0, &pSrcVtx);
		pMeshContainer->MeshData.pMesh->LockVertexBuffer(0, &pDestVtx);

		// 소프트웨어 스키닝을 수행하는 함수(스키닝 뿐 아니라 애니메이션 변경 시, 뼈대들과 정점 정보들의 변경을 동시에 수행하기도 함)
		pMeshContainer->pSkinInfo->UpdateSkinnedMesh(pMeshContainer->pRenderingMatrix,	// 최종 뼈의 변환상태 행렬
			nullptr,						// 원래 상태로 되돌리기 위한 상태 행렬(원래는 위 행렬의 역행렬을 구해서 넣어줘야 하지만 안넣어줘도 상관 없음)
			pSrcVtx,						// 변하지 않는 원본 메쉬의 정점 정보
			pDestVtx);						// 변환된 정보를 담기 위한 메쉬의 정점 정보


		for (_ulong i = 0; i < pMeshContainer->NumMaterials; ++i)
		{
			pEffect->SetTexture("g_BaseTexture", pMeshContainer->ppTexture[i]);
			pEffect->CommitChanges();
			pMeshContainer->MeshData.pMesh->DrawSubset(i);
		}

		pMeshContainer->pOriMesh->UnlockVertexBuffer();
		pMeshContainer->MeshData.pMesh->UnlockVertexBuffer();
	}
}

void CDynamicMesh::Set_AnimationSet(const _uint & iIndex,const _double& AniCount)
{
	m_uAniNum = iIndex;
	m_pAniCtrl->Set_AnimationSet(m_uAniNum, AniCount);
}

void CDynamicMesh::Play_Animation(const _float & fTimeDelta)
{
	m_pAniCtrl->Play_Animation(fTimeDelta);

	_matrix		matTemp;
	Update_FrameMatrices((D3DXFRAME_DERIVED*)m_pRootFrame, D3DXMatrixRotationY(&matTemp, D3DXToRadian(180.f)));
}

void CDynamicMesh::Reset_Animation()
{
	m_pAniCtrl->Reset();
}

void CDynamicMesh::Set_Matrix(const _float& fY)
{
	_vec3 vPos, vScale;
	D3DXQUATERNION	vRot;
	D3DXMatrixDecompose(&vScale,&vRot,&vPos, &m_pRootFrame->TransformationMatrix);

	_matrix mScale, mPos,mRotY;

	D3DXMatrixScaling(&mScale, vScale.x, vScale.y, vScale.z);
	D3DXMatrixRotationY(&mRotY, D3DXToRadian(fY));
	D3DXMatrixTranslation(&mPos, vPos.x,
		vPos.y,
		vPos.z);

	m_pRootFrame->TransformationMatrix = mScale * mRotY * mPos;
}

void CDynamicMesh::Set_Test(CDynamicMesh* pMesh)
{
	m_iTestIndex = 1;
	m_pPlayerRIght = pMesh;
	//const Engine::D3DXFRAME_DERIVED* pBone = (D3DXFRAME_DERIVED*)D3DXFrameFind(m_pRootFrame, "skinned_l_radius_bn");
	//pBone->CombinedTransformationMatrix;
	//int count = 0;
	//for (auto& iter : m_MeshContainerList)
	//{
	//	D3DXMESHCONTAINER_DERIVED*			pMeshContainer = iter;
	//	for (_ulong i = 0; i < pMeshContainer->dwNumBones; ++i)
	//	{
	//		
	//		if (count == 22 || count == 19 || count == 29 || count == 30 || count == 32 || count == 34
	//			|| count == 15 || count == 7 || count == 25 || count == 20 || count == 24 || count == 28 ||
	//			count == 11 || count == 12 || count == 14 || count == 8 || count == 9 || count == 26)
	//		{

	//			pMeshContainer->pFrameOffsetMatrix[i] = pBone->CombinedTransformationMatrix;
	//		}
	//		count++;
	//	}
	//}

}

void Engine::CDynamicMesh::Update_FrameMatrices(D3DXFRAME_DERIVED* pFrame, const _matrix* pParentMatrix)
{
	if (nullptr == pFrame)
		return;

	pFrame->CombinedTransformationMatrix = pFrame->TransformationMatrix * (*pParentMatrix);

	if (nullptr != pFrame->pFrameSibling)
		Update_FrameMatrices((D3DXFRAME_DERIVED*)pFrame->pFrameSibling, pParentMatrix);

	if (nullptr != pFrame->pFrameFirstChild)
		Update_FrameMatrices((D3DXFRAME_DERIVED*)pFrame->pFrameFirstChild, &pFrame->CombinedTransformationMatrix);

}

Engine::CDynamicMesh* Engine::CDynamicMesh::Create(LPDIRECT3DDEVICE9 pGraphicDev, const _tchar* pFilePath, const _tchar* pFileName)
{
	CDynamicMesh*	pInstance = new CDynamicMesh(pGraphicDev);

	if (FAILED(pInstance->Ready_Meshes(pFilePath, pFileName)))
		Safe_Release(pInstance);

	return pInstance;
}

Engine::CResources* Engine::CDynamicMesh::Clone(void)
{
	return new CDynamicMesh(*this);
}

void Engine::CDynamicMesh::Free(void)
{
	CMesh::Free();
	if (m_pPlayerRIght != nullptr)
		Safe_Release(m_pPlayerRIght);
	Safe_Release(m_pAniCtrl);

	if (false == m_bClone)
	{
		m_pLoader->DestroyFrame(m_pRootFrame);
		Safe_Release(m_pLoader);
	}
	m_MeshContainerList.clear();
}

void Engine::CDynamicMesh::SetUp_FrameMatrixPointer(D3DXFRAME_DERIVED* pFrame)
{
	if (nullptr != pFrame->pMeshContainer)
	{
		D3DXMESHCONTAINER_DERIVED*		pDerivedMeshContainer = (D3DXMESHCONTAINER_DERIVED*)pFrame->pMeshContainer;

		for (_ulong i = 0; i < pDerivedMeshContainer->dwNumBones; ++i)
		{
			const char*		pBoneName = pDerivedMeshContainer->pSkinInfo->GetBoneName(i);
			D3DXFRAME_DERIVED*	pBone = (D3DXFRAME_DERIVED*)D3DXFrameFind(m_pRootFrame, pBoneName);

			pDerivedMeshContainer->ppFrameCombinedMatrix[i] = &pBone->CombinedTransformationMatrix;

			//cout << i << " : " << pBoneName << endl;
		}

		m_MeshContainerList.push_back(pDerivedMeshContainer);
	}

	if (nullptr != pFrame->pFrameSibling)
		SetUp_FrameMatrixPointer((D3DXFRAME_DERIVED*)pFrame->pFrameSibling);

	if (nullptr != pFrame->pFrameFirstChild)
		SetUp_FrameMatrixPointer((D3DXFRAME_DERIVED*)pFrame->pFrameFirstChild);

}

