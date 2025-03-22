#pragma once
#include "AssimpFIleLoad_ Tool.h" 
#include "Tool_Defines.h"
#include "VIBuffer.h"

BEGIN(Tool)
class CModel_Tool;
class CHierarchyNode_Tool;

class CMeshContainer_Tool final : public CVIBuffer
{
protected:
	explicit CMeshContainer_Tool(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	explicit CMeshContainer_Tool(const CMeshContainer_Tool& _rhs);
	virtual ~CMeshContainer_Tool() = default;
public:
	const _uint& Get_MaterialIndex() const { return m_iMaterialIndex; }
	const _uint& Get_NumBones() const { return m_iNumBones; }
	_float2 Get_UVTexCoords(const _uint& _iIndex);
public:
	virtual HRESULT NativeConstruct_Prototype() override;
	HRESULT NativeConstruct_Prototype(struct aiMesh* _pAIMesh, const _uint& _iType,
		CModel_Tool* _pModel, const _fmatrix& _mPivotMatrix = XMMatrixIdentity());
	virtual HRESULT NativeConstruct_Clone(void* _pArg) override;
	virtual HRESULT Render();
	// Set Up Bon Matrices
	HRESULT SetUp_BoneMatrices(_float4x4* _pBoneMatrices, const _fmatrix& _PivotMatrix);
private:
	// Create Mesh
	HRESULT SetUp_Vertices(struct aiMesh* _pMesh, CModel_Tool* _pModel, const _fmatrix& _mPivotMatrix);
	// Create Anim Mesh
	HRESULT SetUp_AnimVertices(struct aiMesh* _pMesh, CModel_Tool* _pModel);
	// Creat Indices 
	HRESULT SetUp_Indices(struct aiMesh* _pMesh);
private:
	_uint																	m_iMaterialIndex = 0;
	_uint																	m_iNumBones = 0;
	_uint																	m_iHierachyNodesNum = 0;
	vector<class CHierarchyNode_Tool*>		m_HierarchyNodes;
	string																	m_strName = "";
private:
	void*																	m_pVertexData = nullptr;
	vector<_float2>												m_UVTexCoords;
public:
	// Get Data 
	void Get_Data(MESHDESC& _tMeshDesc);
	void Get_Data(ANIMMESHDESC& _tAnimMeshDesc);
public:
	static CMeshContainer_Tool* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, struct aiMesh* _pAIMesh,
		const _uint& _iType, CModel_Tool* _pModel, const _fmatrix& _mPivotMatrix = XMMatrixIdentity());
	virtual CComponent* Clone(void* _pArg) override;
	virtual void Free() override;
};

END