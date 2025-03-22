#include "CoreDefines.h"
#include "AChannel.h"
#include "AAnimator.h"
#include "ABoneNode.h"

namespace Core {
	AChannel::AChannel() : 
		m_vScale{}, m_vRotation{},
	m_vPosition{}, m_dCurTimeAcc{0}, m_iCurrentKeyFrames{0}, m_iNumKeyFrameRealSize{0},
	m_KeyFrameContainer{}, m_strBoneNodeName{""}, m_spBoneNode{nullptr}
	{
	}

	AChannel::AChannel(const AChannel& _rhs) :
		m_vScale{ _rhs.m_vScale}, m_vRotation{ _rhs.m_vRotation}, m_vPosition{},
		m_dCurTimeAcc{ 0 },	m_iCurrentKeyFrames{ 0 }, m_iNumKeyFrameRealSize{ 0 },
		m_KeyFrameContainer{}, m_strBoneNodeName{ "" }, m_spBoneNode{ nullptr }
	{
	}

	_bool AChannel::NativeConstruct(AAnimator* _pAnimator, std::ifstream& _read)
	{
		ASSERT_CRASH(nullptr != _pAnimator);
		size_t KeyFrameSize = 0;
		ReadString(_read, m_strBoneNodeName);
		_read.read((_char*)&KeyFrameSize, sizeof(size_t));
		m_KeyFrameContainer.resize(KeyFrameSize);
		m_iNumKeyFrameRealSize = static_cast<_int>(KeyFrameSize) - 1;
		_read.read((_char*)&m_KeyFrameContainer[0], sizeof(KEYFRAME) * KeyFrameSize);

		m_spBoneNode = _pAnimator->FindBoneNode(m_strBoneNodeName);
		return true;
	}

	void AChannel::UpdateTransformMatrix(const _double& _dTimeAcc, AAnimation* _pAnimation)
	{
		RETURN_CHECK(nullptr == _pAnimation, ;);

		if (_dTimeAcc >= m_KeyFrameContainer[m_iNumKeyFrameRealSize].dTime)
		{
			m_vScale = m_KeyFrameContainer[m_iNumKeyFrameRealSize].vScale;
			m_vRotation = m_KeyFrameContainer[m_iNumKeyFrameRealSize].vRotation;
			m_vPosition = m_KeyFrameContainer[m_iNumKeyFrameRealSize].vPosition;
		}
		else
		{
			ComputeCurKeyFrames(_dTimeAcc, m_iCurrentKeyFrames);

			if (m_iCurrentKeyFrames >= m_iNumKeyFrameRealSize)
			{
				m_vScale = m_KeyFrameContainer[m_iNumKeyFrameRealSize].vScale;
				m_vRotation = m_KeyFrameContainer[m_iNumKeyFrameRealSize].vRotation;
				m_vPosition = m_KeyFrameContainer[m_iNumKeyFrameRealSize].vPosition;
			}
			else
			{
				// 현재 키프레임하고 다음 키프레임하고 결합하는 것이다.
				_float fCurrentRatio = static_cast<_float>((_dTimeAcc - m_KeyFrameContainer[m_iCurrentKeyFrames].dTime)
					/ (m_KeyFrameContainer[m_iCurrentKeyFrames + 1].dTime - m_KeyFrameContainer[m_iCurrentKeyFrames].dTime));

				m_vScale = DirectX::XMVectorLerp(m_KeyFrameContainer[m_iCurrentKeyFrames].vScale,
					m_KeyFrameContainer[m_iCurrentKeyFrames + 1].vScale, fCurrentRatio);
				m_vRotation = DirectX::XMQuaternionSlerp(m_KeyFrameContainer[m_iCurrentKeyFrames].vRotation,
					m_KeyFrameContainer[m_iCurrentKeyFrames + 1].vRotation, fCurrentRatio);
				m_vPosition = DirectX::XMVectorLerp(m_KeyFrameContainer[m_iCurrentKeyFrames].vPosition,
					m_KeyFrameContainer[m_iCurrentKeyFrames + 1].vPosition, fCurrentRatio);
			}
		}

		if (nullptr != m_spBoneNode)
		{
			m_spBoneNode->SetTransformMatrix(DirectX::XMMatrixAffineTransformation(m_vScale, ZERO_VALUE,
				m_vRotation, m_vPosition));
		}
	}

	void AChannel::UpdateSupplyToCurAndNextTransformMatrix(const _double& _dTimeAcc, const _float _fRatio, 
		AAnimation* _pAnimation, CSHPTRREF<AChannel> _spNextAnimChannel)
	{
		assert(nullptr != _pAnimation && nullptr != _spNextAnimChannel);
		// 현재 애니메이션과 다음 애니메이션의 첫 번째 값들을 Lerp 시킨다. 
		_int NextAnimCurrFrames = _spNextAnimChannel->GetCurrentKeyFrame();
		// 값들을 Lerp 시킴
		m_vScale = Vector3::Lerp(m_KeyFrameContainer[m_iCurrentKeyFrames].vScale,
			_spNextAnimChannel->m_KeyFrameContainer[NextAnimCurrFrames].vScale, _fRatio);
		m_vRotation = Vector4::Lerp(m_KeyFrameContainer[m_iCurrentKeyFrames].vRotation,
			_spNextAnimChannel->m_KeyFrameContainer[NextAnimCurrFrames].vRotation, _fRatio);
		m_vPosition = Vector4::Lerp(m_KeyFrameContainer[m_iCurrentKeyFrames].vPosition,
			_spNextAnimChannel->m_KeyFrameContainer[NextAnimCurrFrames].vPosition, _fRatio);

		if (nullptr != m_spBoneNode)
		{
			m_spBoneNode->SetTransformMatrix(DirectX::XMMatrixAffineTransformation(m_vScale, ZERO_VALUE,
				m_vRotation, m_vPosition));
		}
	}

	void AChannel::ComputeCurKeyFrames(const _double& _dTimeAcc)
	{
		ComputeCurKeyFrames(_dTimeAcc, REF_OUT m_iCurrentKeyFrames);
	}

	void AChannel::ComputeCurKeyFrames(const _double& _dTimeAcc, REF_OUT _int& _iCurKeyFrame)
	{	
		// 키 프레임이 1개 밖에 없을 경우.
		if (m_KeyFrameContainer.size() <= 1)
		{
			_iCurKeyFrame = 0;
		}
		// 1번 키프레임보다 작을 경우
		else if (_dTimeAcc < m_KeyFrameContainer[1].dTime)
		{
			_iCurKeyFrame = 0;
		}
		else if (!(_dTimeAcc >= m_KeyFrameContainer[m_iNumKeyFrameRealSize].dTime))
		{
			while (_dTimeAcc > m_KeyFrameContainer[_iCurKeyFrame + 1].dTime)
			{
				++_iCurKeyFrame;
			}

			// 애니메이션을 뒤로 돌리는 경우에 KeyFrame을 맞춰준다.
			while (_dTimeAcc < m_KeyFrameContainer[_iCurKeyFrame].dTime)
			{
				if (0 == _iCurKeyFrame)
					break;

				--_iCurKeyFrame;
			}
		}
		m_dCurTimeAcc = _dTimeAcc;
	}
	void AChannel::Free()
	{
	}
}