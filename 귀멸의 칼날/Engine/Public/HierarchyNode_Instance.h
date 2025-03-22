#pragma once
#include "Engine_Defines.h"
#include "Base.h"

BEGIN(Engine)
class CModel_Instance;

class ENGINE_DLL CHierarchyNode_Instance final : public CBase
{ 
private:
	explicit CHierarchyNode_Instance();
	virtual ~CHierarchyNode_Instance() = default;
public:
	const _float& Get_Depth() const { return m_fDepth; }
	const _string& Get_Name() const { return m_strName; }
	const _string& Get_ParentName() const { return m_strPatentsName; }

	_matrix Get_CombinedTransformationMatrix() { return XMLoadFloat4x4(&m_CombinedTransformMatrix); }
	_matrix Get_OffsetMatrix() { return XMLoadFloat4x4(&m_OffsetMatrix); }
	_matrix Get_TransformMatrix() { return XMLoadFloat4x4(&m_TransformMatrix); }
	void Set_TransformationMatrix(_fmatrix _TransformationMatrix) { XMStoreFloat4x4(&m_TransformMatrix, _TransformationMatrix); }
	void Set_OffsetMatrix(_float4x4 _OffsetMatrix) { m_OffsetMatrix = _OffsetMatrix; }
	void Add_ChildNode(CHierarchyNode_Instance* _pNode);
public:
	HRESULT NativeConstruct(CModel_Instance* _pModel, const HNODEDESC& _tDesc);
	void Update_CombinedTransformationMatrix();
	// Clone Node
	CHierarchyNode_Instance* Clone_Node(const _uint& _iGroupID = 0);
	// Find Parents
	void Find_Parents(CModel_Instance* _pModel);
private:
	CHierarchyNode_Instance*					m_pParents = nullptr;
private:
	list<CHierarchyNode_Instance*>		m_ChildNodes;
	string															m_strName = "";
	string															m_strPatentsName = "";
	_float4x4													m_OffsetMatrix;
	_float4x4													m_TransformMatrix;
	_float4x4													m_CombinedTransformMatrix;
	_float															m_fDepth = 0.f;
	_uint															m_iGroupID = 10000000;
public:
	static CHierarchyNode_Instance* Create(CModel_Instance* _pModel, const HNODEDESC& _tDesc);
	virtual void Free() override;
};

END
