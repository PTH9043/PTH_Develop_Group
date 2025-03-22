#pragma once
#include "VIBuffer.h"

BEGIN(Engine)
class CHierarchyNode;

class ENGINE_DLL CMeshContainer  : public CVIBuffer
{
protected:
	explicit CMeshContainer(ID3D11Device * _pDevice, ID3D11DeviceContext * _pContext);
	explicit CMeshContainer(const CMeshContainer& _rhs);
	virtual ~CMeshContainer() = default;
public:
	const _uint& Get_MaterialIndex() const { return m_iMaterialIndex; }
	const _uint& Get_NumBones() const { return m_iNumBones; }
	const _string& Get_Name() const { return m_strName; }
public:
	// 한 매시컨테이너의 가장 끝부분 두 점을 알기 위한 함수, 변수 
	_float3 Get_MinVertex(void) { return m_vMinVertex; }
	_float3 Get_MaxVertex(void) { return m_vMaxVertex; }
public:
	virtual HRESULT NativeConstruct_Prototype() override;
	HRESULT NativeConstruct_Prototype(class CModel* _pModel, const MESHDESC& _tData,  _fmatrix _mPivotMatrix = XMMatrixIdentity());
	HRESULT NativeConstruct_Prototype_Anim(class CModel* _pModel, const ANIMMESHDESC& _tData,  _fmatrix _mPivotMatrix = XMMatrixIdentity());
	virtual HRESULT NativeConstruct_Clone(void* _pArg) override;
	virtual HRESULT Render();
	// Set Up Bon Matrices
	HRESULT SetUp_BoneMatrices(_float4x4* _pBoneMatrices, _fmatrix _PivotMatrix);
private:
	// Create Mesh
	HRESULT SetUp_Vertices(class CModel* _pModel, const MESHDESC& _tMeshDesc, _fmatrix _mPivotMatrix);
	// Create Anim Mesh
	HRESULT SetUp_AnimVertices(class CModel* _pModel, const ANIMMESHDESC& _tAnimMeshDesc,  _fmatrix _mPivotMatrix);
	// Creat Indices 
	HRESULT SetUp_Indices(FACEINDICES32* _pData);
private:
	vector<CHierarchyNode*>					m_HierarchyNodes;
	_uint															m_iMaterialIndex = 0;
	_uint															m_iNumBones = 0;
	string															m_strName = "";
private:
	_float3														m_vMinVertex{};
	_float3														m_vMaxVertex{};
public:
	static CMeshContainer* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext,
		class CModel* _pModel, const MESHDESC& _tData,  _fmatrix _mPivotMatrix = XMMatrixIdentity());
	static CMeshContainer* Create_Anim(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext,
		class CModel* _pModel, const ANIMMESHDESC& _tData, _fmatrix _mPivotMatrix = XMMatrixIdentity());
	virtual CComponent* Clone(void* _pArg) override;
	virtual void Free() override;
};

END