#include "Channel.h"
#include "Model.h"
#include "HierarchyNode.h"
#include "Animation.h"
#include "Method.h"

CChannel::CChannel()
{
}

HRESULT CChannel::NativeConstruct(class CModel* _pModel, const CHANNELDESC& _tChannel)
{
	m_strName = CMethod::ConvertWToS(_tChannel.strName);
	m_iNumMaxKeyFrames = _tChannel.iNumMaxKeyFrames;

	m_pHierachyNode = _pModel->Get_HierarchyNodes(m_strName.c_str());
	Safe_AddRef(m_pHierachyNode);

	m_pKeyFrames = _tChannel.pKeyFrames;
	return S_OK;
}

void CChannel::Update_TransformationMatrices(const _double& _dTimeAcc, CAnimation* _pAnimation)
{
	_vector			vScale;
	_vector			vRotation;
	_vector			vPosition;

	m_iCurrentKeyFrame = Compute_CurKeyFrame(_dTimeAcc);

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

	//if (true == _pAnimation->Is_RootBone())
	//{
	//	// Root와 이름이 같다면 포지션을 없앤다.
	//	if (this->Get_Name() == "Root")
	//	{
	//		// Position값을 0으로 만들기전에 BoneDeltaPos에 넣어준다.
	//		_pAnimation->Set_RootBoneMatrix(m_pHierachyNode->Get_RootTransformationMatrix());
	//	}
	//}

	_matrix TransformationMatrix = XMMatrixAffineTransformation(vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRotation, vPosition);
	if (nullptr != m_pHierachyNode)
		m_pHierachyNode->Set_TransformationMatrix(TransformationMatrix);
}

void CChannel::Supplement_NextAnim_TransformMatrix(const _double& _dTimeAcc, CChannel* _pNextChannel, const _float& _fRatio,
	CAnimation* _pAnimation, const _bool& _bSupplement)
{
	if (false == _bSupplement)
		return;

	if (nullptr == _pNextChannel)
		return;

	_uint iKeyFrame = m_iCurrentKeyFrame;

	_vector			vScale;
	_vector			vRotation;
	_vector			vPosition;

	_vector		vSourScale, vDestScale;
	_vector		vSourRotation, vDestRotation;
	_vector		vSourPosition, vDestPosition;

	_uint iNextChannelKeyFrame = _pNextChannel->Compute_CurKeyFrame(_dTimeAcc);	

	vSourScale = XMLoadFloat3(&m_pKeyFrames[iKeyFrame].vScale);
	vDestScale = XMLoadFloat3(&_pNextChannel->Get_KeyFrame(iNextChannelKeyFrame).vScale);

	vSourRotation = XMLoadFloat4(&m_pKeyFrames[iKeyFrame].vRotation);
	vDestRotation = XMLoadFloat4(&_pNextChannel->Get_KeyFrame(iNextChannelKeyFrame).vRotation);

	vSourPosition = XMLoadFloat4(&m_pKeyFrames[iKeyFrame].vPosition);
	vDestPosition = XMLoadFloat4(&_pNextChannel->Get_KeyFrame(iNextChannelKeyFrame).vPosition);

	vScale = XMVectorLerp(vSourScale, vDestScale, _fRatio);
	vRotation = XMQuaternionSlerp(vSourRotation, vDestRotation, _fRatio);
	vPosition = XMVectorLerp(vSourPosition, vDestPosition, _fRatio);

	if (nullptr != m_pHierachyNode)
	{
		if (!strcmp(m_pHierachyNode->Get_Name().c_str(), "Root"))
		{
			vPosition = vDestPosition;
		}

		_matrix 	TransformationMatrix = XMMatrixAffineTransformation(vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRotation, vPosition);
		m_pHierachyNode->Set_TransformationMatrix(TransformationMatrix);
	}
}

void CChannel::Reset_TransformMatrix(const _double& _dTimeAcc)
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

void CChannel::Reset_TransformMatrix()
{
	_vector			vScale = XMLoadFloat3(&m_pKeyFrames[m_iCurrentKeyFrame].vScale);
	_vector			vRotation = XMLoadFloat4(&m_pKeyFrames[m_iCurrentKeyFrame].vRotation);
	_vector			vPosition = XMLoadFloat4(&m_pKeyFrames[m_iCurrentKeyFrame].vPosition);

	_matrix TransformationMatrix = XMMatrixAffineTransformation(vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRotation, vPosition);
	if (nullptr != m_pHierachyNode)
		m_pHierachyNode->Set_TransformationMatrix(TransformationMatrix);
}

CChannel* CChannel::Clone_Channel(CModel* _pModel)
{
	CChannel* pChannel = new CChannel();
	pChannel->m_strName = m_strName;
	if (nullptr != m_pHierachyNode)
	{
		pChannel->m_pHierachyNode = _pModel->Get_HierarchyNodes(m_pHierachyNode->Get_Name().c_str());
		Safe_AddRef(pChannel->m_pHierachyNode);
	}
	pChannel->m_iNumMaxKeyFrames = m_iNumMaxKeyFrames;
	pChannel->m_pKeyFrames = new KEYFRAME[m_iNumMaxKeyFrames];
	for (_uint i = 0; i < m_iNumMaxKeyFrames; ++i)
		pChannel->m_pKeyFrames[i] = m_pKeyFrames[i];
	return pChannel;
}

_uint CChannel::Compute_CurKeyFrame(const _double& _dTimeAcc)
{
	_uint iKeyFrame = 0;

	if (_dTimeAcc < m_pKeyFrames[1].Time)
		iKeyFrame = 0;

	if (_dTimeAcc >= m_pKeyFrames[m_iNumMaxKeyFrames - 1].Time)
	{
		iKeyFrame = iKeyFrame;
	}
	else
	{
		while (_dTimeAcc > m_pKeyFrames[iKeyFrame + 1].Time)
		{
			++iKeyFrame;
		}

		// 애니메이션을 뒤로 돌리는 경우에 KeyFrame을 맞춰준다.
		while (_dTimeAcc < m_pKeyFrames[iKeyFrame].Time)
		{
			if (0 == m_iCurrentKeyFrame)
				break;

			--iKeyFrame;
		}
	}
	return iKeyFrame;
}

void CChannel::Save_Data(const HANDLE& _hFile, DWORD& _dwByte)
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

CChannel* CChannel::Create(class CModel* _pModel, const CHANNELDESC& _tChannel)
{
	CChannel* pInstance = new CChannel();

	if (FAILED(pInstance->NativeConstruct(_pModel, _tChannel)))
	{
		MSG_BOX("Failed to Created : CChannel");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CChannel::Free()
{
	Safe_Release(m_pHierachyNode);
	Safe_Delete_Array(m_pKeyFrames);
}
