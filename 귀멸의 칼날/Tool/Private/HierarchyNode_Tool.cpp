#include "AssimpFIleLoad_ Tool.h" 
#include "HierarchyNode_Tool.h"

CHierarchyNode_Tool::CHierarchyNode_Tool()
{
	XMStoreFloat4x4(&m_SaveTransformMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_TransformMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_OffsetMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_CombinedTransformMatrix, XMMatrixIdentity());
}

HRESULT CHierarchyNode_Tool::NativeConstruct(CHierarchyNode_Tool* _pParents, const _char* _pName, _fmatrix _TransformMatrix, const _uint& _iDepth)
{
	strcpy_s(m_szName, _pName);

	m_pParents = _pParents;
	Safe_AddRef(m_pParents);

	XMStoreFloat4x4(&m_TransformMatrix, XMMatrixTranspose(_TransformMatrix));
	m_SaveTransformMatrix = m_TransformMatrix;
	m_fDepth = (_float)_iDepth;

	return S_OK;
}

void CHierarchyNode_Tool::Update_CombinedTransformationMatrix()
{
	if (nullptr != m_pParents)
	{
		XMStoreFloat4x4(&m_CombinedTransformMatrix, XMLoadFloat4x4(&m_TransformMatrix) *
			XMLoadFloat4x4(&m_pParents->m_CombinedTransformMatrix));
	}
	else
	{
		m_CombinedTransformMatrix = m_TransformMatrix;
	}
}

void CHierarchyNode_Tool::Get_Data(HNODEDESC& _tNodeDesc)
{
	_tNodeDesc.TransformMatrix = m_SaveTransformMatrix;
	_tNodeDesc.fDepth = m_fDepth;
	_tNodeDesc.OffsetMatrix = m_OffsetMatrix;
	string str = m_szName;
	_tNodeDesc.strName.assign(str.begin(), str.end());
	if (nullptr != m_pParents)
	{
		_string parentsName = m_pParents->Get_Name();
		_tNodeDesc.strParentsName.assign(parentsName.begin(), parentsName.end());
	}
}

CHierarchyNode_Tool* CHierarchyNode_Tool::Create(CHierarchyNode_Tool* _pParents, const _char* _pName, _fmatrix _TransformMatrix, const _uint& _iDepth)
{
	CHierarchyNode_Tool* pInstance = new CHierarchyNode_Tool();
	if (FAILED(pInstance->NativeConstruct(_pParents, _pName, _TransformMatrix, _iDepth)))
	{
		Safe_Release(pInstance);
		MSG_BOX("CHierachyNode::Create To Failed");
	}
	return pInstance;
}

void CHierarchyNode_Tool::Free()
{
	Safe_Release(m_pParents);
}
