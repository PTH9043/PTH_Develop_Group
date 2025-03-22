#include "ToolDefines.h"
#include "TAssimpChannel.h"
#include "TAssimpBoneNode.h"
#include "TAssimpModel.h"
#include "UMethod.h"

TAssimpChannel::TAssimpChannel() :
	m_vecKeyFrames{},
	m_spBoneNode{ nullptr },
	m_wstrName{ L"" },
	m_iNumMaxKeyFrames{ 0 },
	m_iCurKeyFrames{ 0 }
{
}

TAssimpChannel::TAssimpChannel(const TAssimpChannel& _rhs) :
	m_vecKeyFrames{ _rhs.m_vecKeyFrames },
	m_spBoneNode{ _rhs.m_spBoneNode },
	m_wstrName{ _rhs.m_wstrName },
	m_iNumMaxKeyFrames{ _rhs.m_iNumMaxKeyFrames },
	m_iCurKeyFrames{ _rhs.m_iCurKeyFrames }
{
}

void TAssimpChannel::Free()
{
}

HRESULT TAssimpChannel::NativeConstruct(aiNodeAnim* _pChannel, CSHPTRREF<TAssimpModel> _pModel)
{
	m_wstrName = UMethod::ConvertSToW(_pChannel->mNodeName.data);
	m_spBoneNode = _pModel->FindBoneNode(m_wstrName);

	m_iNumMaxKeyFrames = max(_pChannel->mNumScalingKeys, _pChannel->mNumRotationKeys);
	m_iNumMaxKeyFrames = max(m_iNumMaxKeyFrames, _pChannel->mNumPositionKeys);
	_float3		vScale;
	_float4		vRotation, vPosition;
	_double		Time = 0.0;

	m_vecKeyFrames.resize(m_iNumMaxKeyFrames);
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
		KeyFrame.dTime = Time;

		m_vecKeyFrames[i] = KeyFrame;
	}
	return S_OK;
}

HRESULT TAssimpChannel::NativeConstruct(aiNodeAnim* _pChannel, CSHPTRREF<TAssimpModel> _pModel, const _wstring& _wstrRootBoneName)
{
	m_wstrName = _wstrRootBoneName;
	m_spBoneNode = _pModel->FindBoneNode(_wstrRootBoneName);

	m_iNumMaxKeyFrames = max(_pChannel->mNumScalingKeys, _pChannel->mNumRotationKeys);
	m_iNumMaxKeyFrames = max(m_iNumMaxKeyFrames, _pChannel->mNumPositionKeys);
	_float3		vScale;
	_float4		vRotation, vPosition;
	_double		Time = 0.0;

	m_vecKeyFrames.resize(m_iNumMaxKeyFrames);
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
		KeyFrame.dTime = Time;

		m_vecKeyFrames[i] = KeyFrame;
	}
	return S_OK;
}

HRESULT TAssimpChannel::NativeConstruct(CSHPTRREF<TAssimpModel> _spAnimModel, const CHANNELDESC& _stChannelDesc)
{
	RETURN_CHECK(nullptr == _spAnimModel, E_FAIL);
	m_iNumMaxKeyFrames = _stChannelDesc.iNumMaxKeyFrames ;
	m_vecKeyFrames.resize(_stChannelDesc.iNumMaxKeyFrames);
	::memcpy(&m_vecKeyFrames[0], _stChannelDesc.pKeyFrames, sizeof(KEYFRAME) * _stChannelDesc.iNumMaxKeyFrames);
	m_wstrName = _stChannelDesc.wstrBoneName;
	m_spBoneNode = _spAnimModel->FindBoneNode(m_wstrName);
	KEYFRAME* pKeyFrame = _stChannelDesc.pKeyFrames;
	Make::ReleaseBuffer(pKeyFrame);
	return S_OK;
}

void TAssimpChannel::GetData(CHANNELDESC& _stChannelDesc)
{
	_stChannelDesc.iNumMaxKeyFrames = m_iNumMaxKeyFrames;
	_stChannelDesc.wstrBoneName = m_wstrName;
	_stChannelDesc.pKeyFrames = m_vecKeyFrames.data();
}
