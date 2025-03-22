#include "CChannel_FBX.h"
#include "CModel_FBX.h"
#include "CHierarchyNode_FBX.h"


CChannel_FBX::CChannel_FBX()
{
}

void CChannel_FBX::Get_Data(CHANNELDESC& _tDesc)
{
	_tDesc.iNumMaxKeyFrames = m_iNumMaxKeyFrames;
	_string str = m_szName;
	_tDesc.strName.assign(str.begin(), str.end());
	_tDesc.pKeyFrames = m_pKeyFrames;
}

HRESULT CChannel_FBX::NativeConstruct(aiNodeAnim* _pChannel, class CModel_FBX* _pModel)
{
	strcpy_s(m_szName, _pChannel->mNodeName.data);

	m_pHierachyNode = _pModel->Get_HierarchyNodes(m_szName);
	Safe_AddRef(m_pHierachyNode);

	m_iNumMaxKeyFrames = max(_pChannel->mNumScalingKeys, _pChannel->mNumRotationKeys);
	m_iNumMaxKeyFrames = max(m_iNumMaxKeyFrames, _pChannel->mNumPositionKeys);

	_float3		vScale;
	_float4		vRotation;
	_float4		vPosition;

	_double		Time = 0.0;

	m_pKeyFrames = new KEYFRAME[m_iNumMaxKeyFrames];
	for (_uint i = 0; i < m_iNumMaxKeyFrames; ++i)
	{
		KEYFRAME			KeyFrame;
		ZeroMemory(&KeyFrame, sizeof(KEYFRAME));

		if (i < _pChannel->mNumScalingKeys)
		{
			memcpy(&vScale, &_pChannel->mScalingKeys[i].mValue, sizeof(_float3));
			Time = _pChannel->mScalingKeys[i].mTime;
		}

		if (i < _pChannel->mNumRotationKeys)
		{
			vRotation.x = _pChannel->mRotationKeys[i].mValue.x;
			vRotation.y = _pChannel->mRotationKeys[i].mValue.y;
			vRotation.z = _pChannel->mRotationKeys[i].mValue.z;
			vRotation.w = _pChannel->mRotationKeys[i].mValue.w;
			Time = _pChannel->mRotationKeys[i].mTime;
		}

		if (i < _pChannel->mNumPositionKeys)
		{
			memcpy(&vPosition, &_pChannel->mPositionKeys[i].mValue, sizeof(_float3));
			vPosition.w = 1.f;
			Time = _pChannel->mPositionKeys[i].mTime;
		}

		KeyFrame.vScale = vScale;
		KeyFrame.vRotation = vRotation;
		KeyFrame.vPosition = vPosition;
		KeyFrame.Time = Time;

		m_pKeyFrames[i] = KeyFrame;
	}

	return S_OK;
}

void CChannel_FBX::Update_TransformationMatrices(const _double& _dTimeAcc)
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

	_matrix TransformationMatrix = XMMatrixAffineTransformation(vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRotation, vPosition);
	if(nullptr != m_pHierachyNode)
		m_pHierachyNode->Set_TransformationMatrix(TransformationMatrix);
}

CChannel_FBX* CChannel_FBX::Clone_Channel(CModel_FBX* _pModel)
{
	CChannel_FBX* pChannel = new CChannel_FBX();
	strcpy_s(pChannel->m_szName, m_szName);
	pChannel->m_pHierachyNode = _pModel->Get_HierarchyNodes(m_pHierachyNode->Get_Name());
	pChannel->m_iNumMaxKeyFrames = m_iNumMaxKeyFrames;
	pChannel->m_pKeyFrames = new KEYFRAME[m_iNumMaxKeyFrames];
	memcpy(pChannel->m_pKeyFrames, m_pKeyFrames, sizeof(KEYFRAME) * m_iNumMaxKeyFrames);
	return pChannel;
}

CChannel_FBX* CChannel_FBX::Create(aiNodeAnim* _pChannel, class CModel_FBX* _pModel)
{
	CChannel_FBX* pInstance = new CChannel_FBX();

	if (FAILED(pInstance->NativeConstruct(_pChannel, _pModel)))
	{
		MSG_BOX("Failed to Created : CChannel");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CChannel_FBX::Free()
{
	Safe_Release(m_pHierachyNode);

	Safe_Delete_Array(m_pKeyFrames);
}
