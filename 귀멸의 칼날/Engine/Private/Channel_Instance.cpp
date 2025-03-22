#include "Channel_Instance.h"
#include "Model_Instance.h"
#include "Method.h"
#include "HierarchyNode_Instance.h"
#include "Animation_Instance.h"

CChannel_Instance::CChannel_Instance()
{
}

HRESULT CChannel_Instance::NativeConstruct(CModel_Instance* _pModel, const CHANNELDESC& _tChannel)
{
	m_strName = CMethod::ConvertWToS(_tChannel.strName);
	m_iNumMaxKeyFrames = _tChannel.iNumMaxKeyFrames;

	m_pHierachyNode = _pModel->Get_HierarchyNodes_Proto(m_strName.c_str());
	Safe_AddRef(m_pHierachyNode);

	m_pKeyFrames = new KEYFRAME[m_iNumMaxKeyFrames];
	memcpy(m_pKeyFrames, _tChannel.pKeyFrames, sizeof(KEYFRAME) * m_iNumMaxKeyFrames);

	KEYFRAME* pKeyFrame = _tChannel.pKeyFrames;
	Safe_Delete_Array(pKeyFrame);
	return S_OK;
}

void CChannel_Instance::Update_TransformationMatrices(const _double& _dTimeAcc, CAnimation_Instance* _pAnimation)
{
	_vector			vScale;
	_vector			vRotation;
	_vector			vPosition;

	if (_dTimeAcc < m_pKeyFrames[1].Time)
		m_iCurrentKeyFrame = 0;

	if (_dTimeAcc >= m_pKeyFrames[m_iNumMaxKeyFrames - 1].Time)
	{
		vScale = XMLoadFloat3(&m_pKeyFrames[m_iNumMaxKeyFrames - 1].vScale);
		vRotation = XMLoadFloat4(&m_pKeyFrames[m_iNumMaxKeyFrames - 1].vRotation);
		vPosition = XMLoadFloat4(&m_pKeyFrames[m_iNumMaxKeyFrames - 1].vPosition);
	}
	else
	{
		while (_dTimeAcc > m_pKeyFrames[m_iCurrentKeyFrame + 1].Time)
		{
			++m_iCurrentKeyFrame;
		}

		// 애니메이션을 뒤로 돌리는 경우에 KeyFrame을 맞춰준다.
		while (_dTimeAcc < m_pKeyFrames[m_iCurrentKeyFrame].Time)
		{
			if (0 == m_iCurrentKeyFrame)
				break;

			--m_iCurrentKeyFrame;
		}

		if (m_iCurrentKeyFrame + 1 >= m_iNumMaxKeyFrames)
		{
			vScale = XMLoadFloat3(&m_pKeyFrames[m_iNumMaxKeyFrames - 1].vScale);
			vRotation = XMLoadFloat4(&m_pKeyFrames[m_iNumMaxKeyFrames - 1].vRotation);
			vPosition = XMLoadFloat4(&m_pKeyFrames[m_iNumMaxKeyFrames - 1].vPosition);
		}
		else
		{
			_float		fRatio = (_float)((_dTimeAcc - m_pKeyFrames[m_iCurrentKeyFrame].Time)
				/ (m_pKeyFrames[m_iCurrentKeyFrame + 1].Time - m_pKeyFrames[m_iCurrentKeyFrame].Time));

			_vector		vSourScale, vDestScale;
			_vector		vSourRotation, vDestRotation;
			_vector		vSourPosition, vDestPosition;

			vSourScale = XMLoadFloat3(&m_pKeyFrames[m_iCurrentKeyFrame].vScale);
			vDestScale = XMLoadFloat3(&m_pKeyFrames[m_iCurrentKeyFrame + 1].vScale);

			vSourRotation = XMLoadFloat4(&m_pKeyFrames[m_iCurrentKeyFrame].vRotation);
			vDestRotation = XMLoadFloat4(&m_pKeyFrames[m_iCurrentKeyFrame + 1].vRotation);

			vSourPosition = XMLoadFloat4(&m_pKeyFrames[m_iCurrentKeyFrame].vPosition);
			vDestPosition = XMLoadFloat4(&m_pKeyFrames[m_iCurrentKeyFrame + 1].vPosition);

			vScale = XMVectorLerp(vSourScale, vDestScale, fRatio);
			vRotation = XMQuaternionSlerp(vSourRotation, vDestRotation, fRatio);
			vPosition = XMVectorLerp(vSourPosition, vDestPosition, fRatio);
		}
	}

	_matrix TransformationMatrix = XMMatrixAffineTransformation(vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRotation, vPosition);
	if (nullptr != m_pHierachyNode)
		m_pHierachyNode->Set_TransformationMatrix(TransformationMatrix);
}

void CChannel_Instance::Supplement_NextAnim_TransformMatrix(const _double& _dTimeAcc, CChannel_Instance* _pNextChannel, const _float& _fRatio, CAnimation_Instance* _pAnimation)
{
	_vector			vScale;
	_vector			vRotation;
	_vector			vPosition;

	_vector		vSourScale, vDestScale;
	_vector		vSourRotation, vDestRotation;
	_vector		vSourPosition, vDestPosition;

	_uint iKeyFrame = m_iCurrentKeyFrame;

	vSourScale = XMLoadFloat3(&m_pKeyFrames[iKeyFrame].vScale);
	vDestScale = XMLoadFloat3(&_pNextChannel->Get_KeyFrame(0).vScale);

	vSourRotation = XMLoadFloat4(&m_pKeyFrames[iKeyFrame].vRotation);
	vDestRotation = XMLoadFloat4(&_pNextChannel->Get_KeyFrame(0).vRotation);

	vSourPosition = XMLoadFloat4(&m_pKeyFrames[iKeyFrame].vPosition);
	vDestPosition = XMLoadFloat4(&_pNextChannel->Get_KeyFrame(0).vPosition);


	vScale = XMVectorLerp(vSourScale, vDestScale, _fRatio);
	vRotation = XMQuaternionSlerp(vSourRotation, vDestRotation, _fRatio);
	vPosition = XMVectorLerp(vSourPosition, vDestPosition, _fRatio);

	if (!strcmp(m_pHierachyNode->Get_Name().c_str(), "Root"))
	{
		vPosition = vDestPosition;
	}

	_matrix TransformationMatrix = XMMatrixAffineTransformation(vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRotation, vPosition);
	if (nullptr != m_pHierachyNode)
		m_pHierachyNode->Set_TransformationMatrix(TransformationMatrix);
}

void CChannel_Instance::Reset_TransformMatrix(const _double& _dTimeAcc)
{
	_vector			vScale;
	_vector			vRotation;
	_vector			vPosition;

	_vector		vSourScale, vDestScale;
	_vector		vSourRotation, vDestRotation;
	_vector		vSourPosition, vDestPosition;

	vSourScale = XMLoadFloat3(&m_pKeyFrames[m_iCurrentKeyFrame].vScale);
	vDestScale = XMLoadFloat3(&m_pKeyFrames[m_iCurrentKeyFrame + 1].vScale);

	vSourRotation = XMLoadFloat4(&m_pKeyFrames[m_iCurrentKeyFrame].vRotation);
	vDestRotation = XMLoadFloat4(&m_pKeyFrames[m_iCurrentKeyFrame + 1].vRotation);

	vSourPosition = XMLoadFloat4(&m_pKeyFrames[m_iCurrentKeyFrame].vPosition);
	vDestPosition = XMLoadFloat4(&m_pKeyFrames[m_iCurrentKeyFrame + 1].vPosition);

	_float		fRatio = (_float)((_dTimeAcc - m_pKeyFrames[m_iNumMaxKeyFrames - 2].Time)
		/ (m_pKeyFrames[m_iNumMaxKeyFrames - 1].Time - m_pKeyFrames[m_iNumMaxKeyFrames - 2].Time));

	vScale = XMVectorLerp(vSourScale, vDestScale, fRatio);
	vRotation = XMQuaternionSlerp(vSourRotation, vDestRotation, fRatio);
	vPosition = XMVectorLerp(vSourPosition, vDestPosition, fRatio);

	_matrix TransformationMatrix = XMMatrixAffineTransformation(vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRotation, vPosition);
	if (nullptr != m_pHierachyNode)
		m_pHierachyNode->Set_TransformationMatrix(TransformationMatrix);
}

void CChannel_Instance::Reset_TransformMatrix()
{
	_vector			vScale = XMLoadFloat3(&m_pKeyFrames[0].vScale);
	_vector			vRotation = XMLoadFloat4(&m_pKeyFrames[0].vRotation);
	_vector			vPosition = XMLoadFloat4(&m_pKeyFrames[0].vPosition);

	_matrix TransformationMatrix = XMMatrixAffineTransformation(vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRotation, vPosition);
	if (nullptr != m_pHierachyNode)
		m_pHierachyNode->Set_TransformationMatrix(TransformationMatrix);
}

CChannel_Instance* CChannel_Instance::Clone_Channel(CModel_Instance* _pModel, const _uint& _iGroupID)
{
	CChannel_Instance* pChannel = new CChannel_Instance();
	pChannel->m_strName = m_strName;
	pChannel->m_iGroupID = _iGroupID;
	if (nullptr != m_pHierachyNode)
	{
		pChannel->m_pHierachyNode = _pModel->Get_HierarchyNodes_Clone(_iGroupID, m_pHierachyNode->Get_Name().c_str());
		Safe_AddRef(pChannel->m_pHierachyNode);
	}
	pChannel->m_iNumMaxKeyFrames = m_iNumMaxKeyFrames;
	pChannel->m_pKeyFrames = new KEYFRAME[m_iNumMaxKeyFrames];
	for (_uint i = 0; i < m_iNumMaxKeyFrames; ++i)
		pChannel->m_pKeyFrames[i] = m_pKeyFrames[i];
	return pChannel;
}

void CChannel_Instance::Save_Data(const HANDLE& _hFile, DWORD& _dwByte)
{
	_wstring strName;
	strName.assign(m_strName.begin(), m_strName.end());
	WriteFile(_hFile, &m_iNumMaxKeyFrames, sizeof(_uint), &_dwByte, 0);
	_uint Length = (_uint)lstrlen(strName.c_str()) + 1;
	WriteFile(_hFile, &Length, sizeof(_uint), &_dwByte, 0);
	WriteFile(_hFile, strName.c_str(), sizeof(_tchar) * Length, &_dwByte, 0);

	for (_uint i = 0; i < m_iNumMaxKeyFrames; ++i)
		WriteFile(_hFile, &m_pKeyFrames[i], sizeof(KEYFRAME), &_dwByte, 0);
}

CChannel_Instance* CChannel_Instance::Create(CModel_Instance* _pModel, const CHANNELDESC& _tChannel)
{
	CChannel_Instance* pInstance = new CChannel_Instance();

	if (FAILED(pInstance->NativeConstruct(_pModel, _tChannel)))
	{
		MSG_BOX("Failed to Created : CChannel_Instance");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CChannel_Instance::Free()
{
	Safe_Release(m_pHierachyNode);
	Safe_Delete_Array(m_pKeyFrames);
}
