#pragma once
#include "ModelDefines.h"
#include "CVIBuffer.h"

BEGIN(Model)
class CModel_FBX;
class CHierarchyNode_FBX;

class CMeshContainer_FBX final : public CVIBuffer
{
protected:
	explicit CMeshContainer_FBX(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	explicit CMeshContainer_FBX(const CMeshContainer_FBX& _rhs);
	virtual ~CMeshContainer_FBX() = default;
public:
	const _uint& Get_MaterialIndex() const { return m_iMaterialIndex;}
	const _uint& Get_NumBones() const { return m_iNumBones; }
public:
	// Get Data 
	void Get_Data(MESHDESC& _tMeshDesc);
	void Get_Data(ANIMMESHDESC& _tAnimMeshDesc);
public:
	virtual HRESULT NativeConstruct_Prototype() override;
	HRESULT NativeConstruct_Prototype(aiMesh* _pAIMesh, const _uint& _iType, 
		CModel_FBX* _pModel, const _fmatrix& _mPivotMatrix = XMMatrixIdentity());
	virtual HRESULT NativeConstruct_Clone(void* _pArg) override;
	virtual HRESULT Render();
	// Set Up Bon Matrices
	HRESULT SetUp_BoneMatrices(_float4x4* _pBoneMatrices, const _fmatrix& _PivotMatrix);
private:
	// Create Mesh
	HRESULT SetUp_Vertices(aiMesh* _pMesh, CModel_FBX* _pModel, const _fmatrix& _mPivotMatrix);
	// Create Anim Mesh
	HRESULT SetUp_AnimVertices(aiMesh* _pMesh, CModel_FBX* _pModel);
	// Creat Indices 
	HRESULT SetUp_Indices(aiMesh* _pMesh);
private:
	_uint																	m_iMaterialIndex = 0;
	_uint																	m_iNumBones = 0;
	_uint																	m_iHierachyNodesNum = 0;
	vector<class CHierarchyNode_FBX*>		m_HierarchyNodes;
	string																	m_strName = "";
private:
	void*			m_pVertexData = nullptr;
public:
	static CMeshContainer_FBX* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, aiMesh* _pAIMesh, 
		const _uint& _iType, CModel_FBX* _pModel, const _fmatrix& _mPivotMatrix = XMMatrixIdentity());
	virtual CComponent* Clone(void* _pArg) override;
	virtual void Free() override;
};

END