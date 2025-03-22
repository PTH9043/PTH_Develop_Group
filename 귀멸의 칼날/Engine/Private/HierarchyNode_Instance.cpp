#include "HierarchyNode_Instance.h"
#include "Model_Instance.h"
#include "Method.h"

CHierarchyNode_Instance::CHierarchyNode_Instance()
{
	XMStoreFloat4x4(&m_OffsetMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_TransformMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_CombinedTransformMatrix, XMMatrixIdentity());
}

void CHierarchyNode_Instance::Add_ChildNode(CHierarchyNode_Instance* _pNode)
{
	if (nullptr == _pNode)
		return;

	m_ChildNodes.push_back(_pNode);
}

HRESULT CHierarchyNode_Instance::NativeConstruct(CModel_Instance* _pModel, const HNODEDESC& _tDesc)
{
	m_strName = CMethod::ConvertWToS(_tDesc.strName);
	XMStoreFloat4x4(&m_TransformMatrix, XMLoadFloat4x4(&_tDesc.TransformMatrix));
	XMStoreFloat4x4(&m_OffsetMatrix, XMLoadFloat4x4(&_tDesc.OffsetMatrix));
	m_fDepth = _tDesc.fDepth;
	if (nullptr != _pModel)
	{
		string str = CMethod::ConvertWToS(_tDesc.strParentsName);
		m_pParents = _pModel->Get_HierarchyNodes_Proto(str.c_str());
		Safe_AddRef(m_pParents);
		m_strPatentsName = str;

		if (nullptr != m_pParents)
		{
			m_pParents->Add_ChildNode(this);
		}
	}
	return S_OK;
}

void CHierarchyNode_Instance::Update_CombinedTransformationMatrix()
{
	if (nullptr != m_pParents)
	{
		XMStoreFloat4x4(&m_CombinedTransformMatrix, XMLoadFloat4x4(&m_TransformMatrix) *
			m_pParents->Get_CombinedTransformationMatrix());
	}
	else
	{
		m_CombinedTransformMatrix = m_TransformMatrix;
	}
}

CHierarchyNode_Instance* CHierarchyNode_Instance::Clone_Node(const _uint& _iGroupID)
{
	CHierarchyNode_Instance* pNode = new CHierarchyNode_Instance();
	pNode->m_CombinedTransformMatrix = m_CombinedTransformMatrix;
	pNode->m_fDepth = m_fDepth;
	pNode->m_OffsetMatrix = m_OffsetMatrix;
	pNode->m_strName = m_strName;
	pNode->m_strPatentsName = m_strPatentsName;
	pNode->m_TransformMatrix = m_TransformMatrix;
	pNode->m_iGroupID = _iGroupID;
	return pNode;
}

void CHierarchyNode_Instance::Find_Parents(CModel_Instance* _pModel)
{
	if (nullptr != _pModel)
	{
		m_pParents = _pModel->Get_HierarchyNodes_Clone(m_iGroupID, m_strPatentsName.c_str());
		Safe_AddRef(m_pParents);
		if (nullptr != m_pParents)
		{
			m_pParents->Add_ChildNode(this);
		}
	}
}

CHierarchyNode_Instance* CHierarchyNode_Instance::Create(CModel_Instance* _pModel, const HNODEDESC& _tDesc)
{
	CHierarchyNode_Instance* pInstance = new CHierarchyNode_Instance();
	if (FAILED(pInstance->NativeConstruct(_pModel, _tDesc)))
	{
		Safe_Release(pInstance);
#ifdef _DEBUG
		MSG_BOX("CHierachyNode_Instance::Create To Failed");
#endif
	}
	return pInstance;
}

void CHierarchyNode_Instance::Free()
{
	Safe_Release(m_pParents);
	m_ChildNodes.clear();
}
