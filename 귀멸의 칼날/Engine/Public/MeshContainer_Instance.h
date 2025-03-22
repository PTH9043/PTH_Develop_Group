#pragma once
#include "VIBuffer.h"

BEGIN(Engine)
class CHierarchyNode_Instance;

class ENGINE_DLL CMeshContainer_Instance final : public CVIBuffer
{
public:
	typedef struct tagMeshInfo
	{
		_uint iType = 0;
		_uint iInstanceNum = 0;
		_uint iGroupNum = 0;
		class CModel_Instance* pInstance = nullptr;
	}MESHINFO;

	typedef vector<vector<_uint>> GROUPIDS;
protected:
	explicit CMeshContainer_Instance(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	explicit CMeshContainer_Instance(const CMeshContainer_Instance& _rhs);
	virtual ~CMeshContainer_Instance() = default;

public:
	// 한 매시컨테이너의 가장 끝부분 두 점을 알기 위한 함수, 변수 
	_float3 Get_MinVertex(void) { return m_vMinVertex; }
	_float3 Get_MaxVertex(void) { return m_vMaxVertex; }
public:
	const _uint& Get_MaterialIndex() const { return m_iMaterialIndex; }
	const _uint& Get_NumBones() const { return m_iNumBones; }
	const _string& Get_Name() const { return m_strName; }
public:
	virtual HRESULT NativeConstruct_Prototype() override;
	HRESULT NativeConstruct_Prototype(class CModel_Instance* _pModel, const MESHDESC& _tData, 
		 _fmatrix _mPivotMatrix = XMMatrixIdentity());
	HRESULT NativeConstruct_Prototype_Anim(class CModel_Instance* _pModel, const ANIMMESHDESC& _tData, 
		_fmatrix _mPivotMatrix = XMMatrixIdentity());
	virtual HRESULT NativeConstruct_Clone(void* _pArg) override;
	virtual HRESULT Render();
	// Set Up Bon Matrices
	HRESULT Update_Value( const _uint& _iType, TRANSFORMVECTOR Transforms, _fmatrix _PivotMatrix, const _uint& _iIndex = 0);
	HRESULT Update_Value(const _uint& _iType, F4POSVECTOR f4PosVector, _fmatrix _PivotMatrix, const _uint& _iIndex = 0);
	HRESULT Update_Value(const _uint& _iType, MATRIXES _Matrix, _fmatrix _PivotMatrix, const _uint& _iIndex = 0);
	HRESULT SetUp_BoneMatrices(const _uint& _iIndex, _float4x4* _pBoneMatrices, _fmatrix _PivotMatrix);
private:
	// Create Mesh
	HRESULT SetUp_Vertices(class CModel_Instance* _pModel, const MESHDESC& _tMeshDesc,  _fmatrix _mPivotMatrix);
	// Create Anim Mesh
	HRESULT SetUp_AnimVertices(class CModel_Instance* _pModel, const ANIMMESHDESC& _tAnimMeshDesc, _fmatrix _mPivotMatrix);
	// Create Mesh
	HRESULT SetUp_Vertices_Instance(const _uint& _iInstanceCount);
	// Create Anim Mesh
	HRESULT SetUp_AnimVertices_Instance(const _uint& _iInstanceCount);
	// Create Indices 
	HRESULT SetUp_Indices(const _uint& _iInstanceCount, const _uint& _iIndicesCount,  FACEINDICES32* _pData);
private:
	HRESULT Divine_Group(const _uint& _iModelInstanceNum, const _uint& _iGroupNum);
private:
	vector<CHierarchyNode_Instance*>						m_HierarchyNodes;
	vector<vector<CHierarchyNode_Instance*>>		m_HierackyGroup;
	_uint																					m_iMaterialIndex = 0;
	_uint																					m_iNumBones = 0;
	string																					m_strName = "";
	// VB Instance
	ID3D11Buffer*																	m_pVBInstance = nullptr;
	_uint																					m_iStrideInstance = 0;
	_uint																					m_iNumInstance = 0;
private:
	_float3																				m_vMinVertex{};
	_float3																				m_vMaxVertex{};
	_uint																					m_iNumIndices = 0;
private:
	_uint																					m_iGroupNum = 0;
	GROUPIDS																			m_GroupIDS;
public:
	static CMeshContainer_Instance* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext,
		class CModel_Instance* _pModel, const MESHDESC& _tData,  _fmatrix _mPivotMatrix = XMMatrixIdentity());
	static CMeshContainer_Instance* Create_Anim(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext,
		class CModel_Instance* _pModel, const ANIMMESHDESC& _tData, _fmatrix _mPivotMatrix = XMMatrixIdentity());
	virtual CComponent* Clone(void* _pArg) override;
	virtual void Free() override;
};

END