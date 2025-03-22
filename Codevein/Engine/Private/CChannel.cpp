#include "CChannel.h"
#include "CModel.h"
#include "CHierarchyNode.h"
#include "CAnimation.h"

CChannel::CChannel()
{
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

HRESULT CChannel::NativeConstruct(class CModel* _pModel, const CHANNELDESC& _tChannel)
{
	m_strName.assign(_tChannel.strName.begin(), _tChannel.strName.end());
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

	if (_dTimeAcc < m_pKeyFrames[1].Time)
		m_iCurrentKeyFrame = 0;

	if (true == _pAnimation->Is_NotRatio())
	{
		vScale = XMLoadFloat3(&m_pKeyFrames[0].vScale);
		vRotation = XMLoadFloat4(&m_pKeyFrames[0].vRotation);
		vPosition = XMLoadFloat4(&m_pKeyFrames[0].vPosition);
		_pAnimation->Set_NotRatio(false);
	}
	else
	{
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
	}

	_matrix TransformationMatrix = XMMatrixAffineTransformation(vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRotation, vPosition);
	if (nullptr != m_pHierachyNode)
		m_pHierachyNode->Set_TransformationMatrix(TransformationMatrix);
}

void CChannel::Supplement_NextAnim_TransformMatrix(const _double& _dTimeAcc, CChannel* _pNextChannel, const _float& _fRatio,
	CAnimation* _pAnimation)
{
	if (nullptr == _pNextChannel)
		return;

	_uint iKeyFrame = m_iCurrentKeyFrame;

	_vector			vScale;
	_vector			vRotation;
	_vector			vPosition;

	_vector		vSourScale, vDestScale;
	_vector		vSourRotation, vDestRotation;
	_vector		vSourPosition, vDestPosition;

	if (CAnimation::SUPINDEX_INIT == _pAnimation->Get_AnimSupIndex())
		iKeyFrame = 0;
	else if (CAnimation::SUPINDEX_LAST == _pAnimation->Get_AnimSupIndex())
		iKeyFrame = m_iNumMaxKeyFrames - 1;

	if (true == _pAnimation->Is_NotRatio())
	{
		vSourScale = XMLoadFloat3(&m_pKeyFrames[m_iNumMaxKeyFrames - 1].vScale);
		vDestScale = XMLoadFloat3(&_pNextChannel->Get_KeyFrame(0).vScale);

		vSourRotation = XMLoadFloat4(&m_pKeyFrames[m_iNumMaxKeyFrames - 1].vRotation);
		vDestRotation = XMLoadFloat4(&_pNextChannel->Get_KeyFrame(0).vRotation);

		vSourPosition = XMLoadFloat4(&m_pKeyFrames[m_iNumMaxKeyFrames - 1].vPosition);
		vDestPosition = XMLoadFloat4(&_pNextChannel->Get_KeyFrame(0).vPosition);

		if (CAnimation::SUPCONTROL_SOUR == _pAnimation->Get_AnimSupControl())
		{
			_vector vSupPos = vSourPosition - vDestPosition;
			_float fValue = XMVectorGetX(XMVector3Normalize(vSupPos));
			vSupPos = XMVector3Normalize(vSourPosition) * fValue;
			vSourPosition -= vSupPos;
		}
		else if (CAnimation::SUPCONTROL_DEST == _pAnimation->Get_AnimSupControl())
		{
			_vector vSupPos = vSourPosition - vDestPosition;
			_float fValue = XMVectorGetX(XMVector3Normalize(vSupPos));
			vSupPos = XMVector3Normalize(vDestPosition) * fValue;
			vDestPosition += vSupPos;
		}

		vScale = XMVectorLerp(vSourScale, vDestScale, _fRatio);
		vRotation = XMQuaternionSlerp(vSourRotation, vDestRotation, _fRatio);
		vPosition = XMVectorLerp(vSourPosition, vDestPosition, _fRatio);
	}
	else
	{
		vSourScale = XMLoadFloat3(&m_pKeyFrames[iKeyFrame].vScale);
		vDestScale = XMLoadFloat3(&_pNextChannel->Get_KeyFrame(0).vScale);

		vSourRotation = XMLoadFloat4(&m_pKeyFrames[iKeyFrame].vRotation);
		vDestRotation = XMLoadFloat4(&_pNextChannel->Get_KeyFrame(0).vRotation);

		vSourPosition = XMLoadFloat4(&m_pKeyFrames[iKeyFrame].vPosition);
		vDestPosition = XMLoadFloat4(&_pNextChannel->Get_KeyFrame(0).vPosition);

		if (CAnimation::SUPCONTROL_SOUR == _pAnimation->Get_AnimSupControl())
		{
			_vector vSupPos = vSourPosition - vDestPosition;
			_float fValue = XMVectorGetX(XMVector3Normalize(vSupPos));
			vSupPos = XMVector3Normalize(vSourPosition) * fValue;
			vSourPosition -= vSupPos;
		}
		else if (CAnimation::SUPCONTROL_DEST == _pAnimation->Get_AnimSupControl())
		{
			_vector vSupPos = vSourPosition - vDestPosition;
			_float fValue = XMVectorGetX(XMVector3Normalize(vSupPos));
			vSupPos = XMVector3Normalize(vDestPosition) * fValue;
			vDestPosition += vSupPos;
		}

		vScale = XMVectorLerp(vSourScale, vDestScale, _fRatio);
		vRotation = XMQuaternionSlerp(vSourRotation, vDestRotation, _fRatio);
		vPosition = XMVectorLerp(vSourPosition, vDestPosition, _fRatio);
	}

	if (nullptr != m_pHierachyNode)
	{
		if (CAnimation::ROT_SOUR == _pAnimation->Get_SelectAnimRot())
			vRotation = vSourRotation;
		else if (CAnimation::ROT_DEST == _pAnimation->Get_SelectAnimRot())
			vRotation = vDestRotation;

		if (CAnimation::ROT_SOUR == _pAnimation->Get_SelectAnimRot())
			vRotation = vSourRotation;
		else if (CAnimation::ROT_DEST == _pAnimation->Get_SelectAnimRot())
			vRotation = vDestRotation;

		if (!strcmp(m_pHierachyNode->Get_Name().c_str(), "CHARA_OFFSET"))
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
	_vector			vScale = XMLoadFloat3(&m_pKeyFrames[0].vScale);
	_vector			vRotation = XMLoadFloat4(&m_pKeyFrames[0].vRotation);
	_vector			vPosition = XMLoadFloat4(&m_pKeyFrames[0].vPosition);

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
