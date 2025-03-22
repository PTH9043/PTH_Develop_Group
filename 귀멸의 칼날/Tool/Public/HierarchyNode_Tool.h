#pragma once
#include "AssimpFIleLoad_ Tool.h" 
#include "Tool_Defines.h"
#include "Base.h"

BEGIN(Tool)

class CHierarchyNode_Tool final : public CBase
{
private:
	explicit CHierarchyNode_Tool();
	virtual ~CHierarchyNode_Tool() = default;
public:
	const _float& Get_Depth() const { return m_fDepth; }

	const _char* Get_Name() const { return m_szName; }

	_matrix Get_CombinedTransformationMatrix() { return XMLoadFloat4x4(&m_CombinedTransformMatrix); }
	_matrix Get_OffsetMatrix() { return XMLoadFloat4x4(&m_OffsetMatrix); }
	void Set_TransformationMatrix(_fmatrix _TransformationMatrix) { XMStoreFloat4x4(&m_TransformMatrix, _TransformationMatrix); }
	void Set_OffsetMatrix(const _float4x4& _OffsetMatrix) { m_OffsetMatrix = _OffsetMatrix; }
public:
	HRESULT NativeConstruct(CHierarchyNode_Tool* _pParents, const _char* _pName, _fmatrix _TransformMatrix, const _uint& _iDepth);
	void Update_CombinedTransformationMatrix();
private:
	CHierarchyNode_Tool* m_pParents = nullptr;
private:
	_char					 m_szName[MAX_PATH] = "";
	_float4x4			m_TransformMatrix;
	_float4x4			m_SaveTransformMatrix;
	_float4x4			m_CombinedTransformMatrix;
	_float4x4			m_OffsetMatrix;
	_float					m_fDepth;

public:
	void Get_Data(HNODEDESC& _tNodeDesc);
public:
	static CHierarchyNode_Tool* Create(CHierarchyNode_Tool* _pParents, const _char* _pName, _fmatrix _TransformMatrix, const _uint& _iDepth);
	virtual void Free() override;
};

END