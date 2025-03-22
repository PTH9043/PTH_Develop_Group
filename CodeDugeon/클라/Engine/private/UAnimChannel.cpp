#include "EngineDefine.h"
#include "UAnimChannel.h"
#include "UBoneNode.h"
#include "UAnimModel.h"
#include "UAnimation.h"
#include "UGameInstance.h"
#include "UMethod.h"

UAnimChannel::UAnimChannel() :
	UBase(),
	m_wstrBoneName{ L"" },
	m_iNumKeyMaxRealSize{ 0 },
	m_spBoneNode{ nullptr },
	m_vecKeyFrames{},
	m_iCurrentKeyFrames{ 0 },
	m_dCurTimeAcc{ 0 },
	m_vScale{},
	m_vRotation{},
	m_vPosition{}
{
}

UAnimChannel::UAnimChannel(const UAnimChannel& _rhs) :
	UBase(_rhs),
	m_wstrBoneName{ _rhs.m_wstrBoneName },
	m_iNumKeyMaxRealSize{ _rhs.m_iNumKeyMaxRealSize },
	m_spBoneNode{ _rhs.m_spBoneNode },
	m_iCurrentKeyFrames{ _rhs.m_iCurrentKeyFrames },
	m_vecKeyFrames{ _rhs.m_vecKeyFrames },
	m_dCurTimeAcc{ 0 },
	m_vScale{},
	m_vRotation{},
	m_vPosition{}
{
}

SHPTR<UAnimChannel> UAnimChannel::Clone(CSHPTRREF<UAnimModel> _spAnimModel)
{
	SHPTR<UAnimChannel> pChannel{ CloneThis<UAnimChannel>(*this) };
	pChannel->m_spBoneNode = _spAnimModel->FindBoneNode(m_wstrBoneName);
	return pChannel;
}

void UAnimChannel::Free()
{
}

HRESULT UAnimChannel::NativeConstruct(CSHPTRREF<UAnimModel> _spAnimModel, const CHANNELDESC& _stChannelDesc)
{
	RETURN_CHECK(nullptr == _spAnimModel, E_FAIL);
	m_iNumKeyMaxRealSize = _stChannelDesc.iNumMaxKeyFrames - 1;
	m_vecKeyFrames.resize(_stChannelDesc.iNumMaxKeyFrames);
	::memcpy(&m_vecKeyFrames[0], _stChannelDesc.pKeyFrames, sizeof(KEYFRAME) * _stChannelDesc.iNumMaxKeyFrames);
	m_wstrBoneName = _stChannelDesc.wstrBoneName;
	m_spBoneNode = _spAnimModel->FindBoneNode(m_wstrBoneName);
	KEYFRAME* pKeyFrame = _stChannelDesc.pKeyFrames;
	Make::ReleaseBuffer(pKeyFrame);
	return S_OK;
}

// Update Transform Matrix
void UAnimChannel::UpdateTransformMatrix(const _double& _dTimeAcc, UAnimation* _pAnimation)
{
	RETURN_CHECK(nullptr == _pAnimation, ;);

	if (_dTimeAcc >= m_vecKeyFrames[m_iNumKeyMaxRealSize].dTime)
	{
		m_vScale = m_vecKeyFrames[m_iNumKeyMaxRealSize].vScale;
		m_vRotation = m_vecKeyFrames[m_iNumKeyMaxRealSize].vRotation;
		m_vPosition = m_vecKeyFrames[m_iNumKeyMaxRealSize].vPosition;
	}
	else
	{
		ComputeCurKeyFrames(_dTimeAcc, m_iCurrentKeyFrames);

		if (m_iCurrentKeyFrames >= m_iNumKeyMaxRealSize)
		{
			m_vScale = m_vecKeyFrames[m_iNumKeyMaxRealSize].vScale;
			m_vRotation = m_vecKeyFrames[m_iNumKeyMaxRealSize].vRotation;
			m_vPosition = m_vecKeyFrames[m_iNumKeyMaxRealSize].vPosition;
		}
		else
		{
			// 현재 키프레임하고 다음 키프레임하고 결합하는 것이다.
			_float fCurrentRatio = static_cast<_float>((_dTimeAcc - m_vecKeyFrames[m_iCurrentKeyFrames].dTime)
				/ (m_vecKeyFrames[m_iCurrentKeyFrames + 1].dTime - m_vecKeyFrames[m_iCurrentKeyFrames].dTime));

			m_vScale = DirectX::XMVectorLerp(m_vecKeyFrames[m_iCurrentKeyFrames].vScale,
				m_vecKeyFrames[m_iCurrentKeyFrames + 1].vScale, fCurrentRatio);
			m_vRotation = DirectX::XMQuaternionSlerp(m_vecKeyFrames[m_iCurrentKeyFrames].vRotation,
				m_vecKeyFrames[m_iCurrentKeyFrames + 1].vRotation, fCurrentRatio);
			m_vPosition = DirectX::XMVectorLerp(m_vecKeyFrames[m_iCurrentKeyFrames].vPosition,
				m_vecKeyFrames[m_iCurrentKeyFrames + 1].vPosition, fCurrentRatio);
		}
	}

	if (nullptr != m_spBoneNode)
	{
		m_spBoneNode->SetTransformMatrix(DirectX::XMMatrixAffineTransformation(m_vScale, ZERO_VALUE,
			m_vRotation, m_vPosition));
	}
}

void UAnimChannel::UpdateSupplyToCurAndNextTransformMatrix(const _double& _dTimeAcc, const _float _fRatio,
	UAnimation* _pAnimation, CSHPTRREF<UAnimChannel> _spNextAnimChannel)
{
	assert(nullptr != _pAnimation && nullptr != _spNextAnimChannel);
	// 현재 애니메이션과 다음 애니메이션의 첫 번째 값들을 Lerp 시킨다. 
	_int NextAnimCurrFrames = _spNextAnimChannel->GetCurrentKeyFrame();
	// 값들을 Lerp 시킴
	m_vScale = _float3::Lerp(m_vecKeyFrames[m_iCurrentKeyFrames].vScale,
		_spNextAnimChannel->m_vecKeyFrames[NextAnimCurrFrames].vScale, _fRatio);
	m_vRotation = _float4::Lerp(m_vecKeyFrames[m_iCurrentKeyFrames].vRotation,
		_spNextAnimChannel->m_vecKeyFrames[NextAnimCurrFrames].vRotation, _fRatio);
	m_vPosition = _float4::Lerp(m_vecKeyFrames[m_iCurrentKeyFrames].vPosition,
		_spNextAnimChannel->m_vecKeyFrames[NextAnimCurrFrames].vPosition, _fRatio);

	if (nullptr != m_spBoneNode)
	{
		m_spBoneNode->SetTransformMatrix(DirectX::XMMatrixAffineTransformation(m_vScale, ZERO_VALUE,
			m_vRotation, m_vPosition));
	}
}

void UAnimChannel::ComputeCurKeyFrames(const _double& _dTimeAcc)
{
	ComputeCurKeyFrames(_dTimeAcc, m_iCurrentKeyFrames);
}

void UAnimChannel::ComputeCurKeyFrames(const _double& _dTimeAcc, _uint& _iCurKeyFrame)
{
	// 키 프레임이 1개 밖에 없을 경우.
	if (m_vecKeyFrames.size() <= 1)
	{
		_iCurKeyFrame = 0;
	}
	// 1번 키프레임보다 작을 경우
	else if (_dTimeAcc < m_vecKeyFrames[1].dTime)
	{
		_iCurKeyFrame = 0;
	}
	else if (!(_dTimeAcc >= m_vecKeyFrames[m_iNumKeyMaxRealSize].dTime))
	{
		while (_dTimeAcc > m_vecKeyFrames[_iCurKeyFrame + 1].dTime)
		{
			++_iCurKeyFrame;
		}

		// 애니메이션을 뒤로 돌리는 경우에 KeyFrame을 맞춰준다.
		while (_dTimeAcc < m_vecKeyFrames[_iCurKeyFrame].dTime)
		{
			if (0 == _iCurKeyFrame)
				break;

			--_iCurKeyFrame;
		}
	}
	m_dCurTimeAcc = _dTimeAcc;
}

void UAnimChannel::SaveServerData(std::ofstream& _Saves)
{
	UMethod::SaveString(_Saves, UMethod::ConvertWToS(m_wstrBoneName));

	size_t KeyFrameSize = m_vecKeyFrames.size();
	_Saves.write((_char*)&KeyFrameSize, sizeof(size_t));
	_Saves.write((_char*)&m_vecKeyFrames[0], sizeof(KEYFRAME) * m_vecKeyFrames.size());
}
