#pragma once
#include "Engine_Defines.h"
#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CHierarchyNode final : public CBase
{
private:
	explicit CHierarchyNode();
	virtual ~CHierarchyNode() = default;
public:
	const _float& Get_Depth() const { return m_fDepth; }
	const _string& Get_Name() const { return m_strName; }
	const _string& Get_ParentName() const { return m_strPatentsName; }
	CHierarchyNode* Get_Parent() { return m_pParents; }

	list<CHierarchyNode*> Get_ChildNodes() { return m_ChildNodes; }
public:
	_matrix Get_CombinedTransformationMatrix() { return XMLoadFloat4x4(&m_CombinedTransformMatrix); }
	_matrix Get_RootTransformationMatrix() { return XMLoadFloat4x4(&m_RootTransformMatrix); }
	_matrix Get_OffsetMatrix() { return XMLoadFloat4x4(&m_OffsetMatrix); }
	_matrix Get_TransformMatrix() { return XMLoadFloat4x4(&m_TransformMatrix); }
	_float4x4 Get_CombinedTransformMatrix4x4() { return m_CombinedTransformMatrix; }
	_float4x4 Get_RootTransformMatrix4x4() { return m_RootTransformMatrix; }


	void Set_TransformationMatrix(_fmatrix _TransformationMatrix) { XMStoreFloat4x4(&m_TransformMatrix, _TransformationMatrix); }
	void Set_OffsetMatrix(_float4x4 _OffsetMatrix) { m_OffsetMatrix = _OffsetMatrix; }
	void Add_ChildNode(CHierarchyNode* _pNode);
public:
	HRESULT NativeConstruct(class CModel* _pModel, const HNODEDESC& _tDesc);
	void Update_CombinedTransformationMatrix();
	// Clone Node
	CHierarchyNode* Clone_Node();
	// Find Parents
	void Find_Parents(CModel* _pModel);
private:
	CHierarchyNode* m_pParents = nullptr;
private:
	list<CHierarchyNode*>		m_ChildNodes;

	string			m_strName = "";
	string			m_strPatentsName = "";
	_float4x4		m_OffsetMatrix;
	_float4x4		m_TransformMatrix;
	_float4x4		m_CombinedTransformMatrix;
	// Root 이동값이 있는 경우 Root의 이동값을 담는 행렬
	_float4x4		m_RootTransformMatrix;
	_float			m_fDepth = 0.f;
	CModel*			m_pModelCom = nullptr;
public:
	static CHierarchyNode* Create(class CModel* _pModel, const HNODEDESC& _tDesc);
	virtual void Free() override;
};

END