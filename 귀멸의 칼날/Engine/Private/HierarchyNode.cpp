#include "HierarchyNode.h"
#include "Model.h"
#include "Method.h"

CHierarchyNode::CHierarchyNode()
{
	XMStoreFloat4x4(&m_OffsetMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_TransformMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_CombinedTransformMatrix, XMMatrixIdentity());
}

void CHierarchyNode::Add_ChildNode(CHierarchyNode* _pNode)
{
	if (nullptr == _pNode)
		return;

	m_ChildNodes.push_back(_pNode);
}

HRESULT CHierarchyNode::NativeConstruct(class CModel* _pModel, const HNODEDESC& _tDesc)
{
	m_strName = CMethod::ConvertWToS(_tDesc.strName);
	XMStoreFloat4x4(&m_TransformMatrix, XMLoadFloat4x4(&_tDesc.TransformMatrix));
	XMStoreFloat4x4(&m_OffsetMatrix, XMLoadFloat4x4(&_tDesc.OffsetMatrix));
	m_fDepth = _tDesc.fDepth;
	if (nullptr != _pModel)
	{
		string str = CMethod::ConvertWToS(_tDesc.strParentsName);
		m_pParents = _pModel->Get_HierarchyNodes(str.c_str());
		Safe_AddRef(m_pParents);
		m_strPatentsName = str;

		if (nullptr != m_pParents)
		{
			m_pParents->Add_ChildNode(this);
		}
	}
	m_pModelCom = _pModel;
	return S_OK;
}

void CHierarchyNode::Update_CombinedTransformationMatrix()
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

	//// 이름에 헤어가 포함되어있다면 회전값을 없앤다.
	//if (true == CMethod::Is_Same_Text(this->Get_Name(), "Hair"))
	//{
	//	OUTMATRIX CombinedMatrix = CMethod::Out_Matrix(m_CombinedTransformMatrix);
	//	
	//	CombinedMatrix.vRot.z = 0.f;

	//	m_CombinedTransformMatrix = CMethod::

	//	//CMethod::Combine_Matrix(CombinedMatrix);

	//	int a = 10;
	//	//m_CombinedTransformMatrix = CMethod::XMMatrix4x4SetRotationFix(_float3(0.f, 0.f, 0.f), m_CombinedTransformMatrix);
	//}

	// Root와 이름이 같다면 포지션을 없앤다.
	if (this->Get_Name() == "Root")
	{
		// CombinedTransformMatrix를 담고
		m_RootTransformMatrix = m_CombinedTransformMatrix;

		// CombinedTransformMatrix의 위치값을 0으로 해준다.
		m_CombinedTransformMatrix = CMethod::XMMatrix4x4SetFloat3(_float3(0.f, 0.f, 0.f), MATROW_POS, m_CombinedTransformMatrix);
	}
}

CHierarchyNode* CHierarchyNode::Clone_Node()
{
	CHierarchyNode* pNode = new CHierarchyNode();
	pNode->m_CombinedTransformMatrix = m_CombinedTransformMatrix;
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
		if(m_pParents != this )
			Safe_AddRef(m_pParents);
		m_pModelCom = _pModel;
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
