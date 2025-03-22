#include "CHierarchyNode.h"
#include "CModel.h"

CHierarchyNode::CHierarchyNode()
{
	XMStoreFloat4x4(&m_OffsetMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_TransformMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_CombinedTransformMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_MoveTransformMatrix, XMMatrixIdentity());
}

void CHierarchyNode::Add_ChildNode(CHierarchyNode* _pNode)
{
	if (nullptr == _pNode)
		return;

	m_ChildNodes.push_back(_pNode);
}

void CHierarchyNode::Restrict_Position(const _vector& _vPos)
{
	XMStoreFloat3(&m_vRestricPos, _vPos);
	m_bRestricPos = true;
}

HRESULT CHierarchyNode::NativeConstruct(class CModel* _pModel, const HNODEDESC& _tDesc)
{
	m_strName.assign(_tDesc.strName.begin(), _tDesc.strName.end());
	XMStoreFloat4x4(&m_TransformMatrix, XMLoadFloat4x4(&_tDesc.TransformMatrix));
	XMStoreFloat4x4(&m_OffsetMatrix, XMLoadFloat4x4(&_tDesc.OffsetMatrix));
	m_fDepth = _tDesc.fDepth;
	if (nullptr != _pModel)
	{
		string str;
		str.assign(_tDesc.strParentsName.begin(), _tDesc.strParentsName.end());
		m_pParents = _pModel->Get_HierarchyNodes(str.c_str());
		Safe_AddRef(m_pParents);
		m_strPatentsName = str;

		if (nullptr != m_pParents)
		{
			m_pParents->Add_ChildNode(this);
		}
	}
	return S_OK;
}

void CHierarchyNode::Update_CombinedTransformationMatrix()
{
	if (true == m_bNotCalCombine)
	{
		m_bNotCalCombine = false;
		return;
	}

	if (true == m_bRestricPos)
	{
		XMStoreFloat3((_float3*)&m_CombinedTransformMatrix.m[3][0], XMLoadFloat3(&m_vRestricPos));
		m_bRestricPos = false;
	}
	else
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
}

CHierarchyNode* CHierarchyNode::Clone_Node()
{
	CHierarchyNode* pNode = new CHierarchyNode();
	pNode->m_CombinedTransformMatrix = m_CombinedTransformMatrix;
	pNode->m_MoveTransformMatrix = m_MoveTransformMatrix;
	pNode->m_fDepth = m_fDepth;
	pNode->m_OffsetMatrix = m_OffsetMatrix;
	pNode->m_strName = m_strName;
	pNode->m_strPatentsName = m_strPatentsName;
	pNode->m_TransformMatrix = m_TransformMatrix;
	return pNode;
}

void CHierarchyNode::Find_Parents(CModel* _pModel)
{
	if (nullptr != _pModel)
	{
		m_pParents = _pModel->Get_HierarchyNodes(m_strPatentsName.c_str());
		Safe_AddRef(m_pParents);
	}
}

CHierarchyNode* CHierarchyNode::Create(CModel* _pModel, const HNODEDESC& _tDesc)
{
	CHierarchyNode* pInstance = new CHierarchyNode();
	if (FAILED(pInstance->NativeConstruct(_pModel, _tDesc)))
	{
		Safe_Release(pInstance);
		MSG_BOX("CHierachyNode::Create To Failed");
	}
	return pInstance;
}

void CHierarchyNode::Free()
{
	Safe_Release(m_pParents);
	m_ChildNodes.clear();
}
