#include "ToolDefines.h"
#include "TAssimpAnimation.h"
#include "TAssimpModel.h"
#include "TAssimpChannel.h"
#include "TAssimpBoneNode.h"
#include "UMethod.h"

TAssimpAnimation::TAssimpAnimation() :
	m_wstrName{ L"" },
	m_iNumChannels{ 0 },
	m_dTickPerSeconds{ 0 },
	m_dDuration{ 0 },
	m_vecAssimpChannels{},
	m_dTimeAcc{ 0 },
	m_isFinished{ false }
{
}

TAssimpAnimation::TAssimpAnimation(const TAssimpAnimation& _rhs) :
	m_wstrName{ _rhs.m_wstrName },
	m_iNumChannels{ _rhs.m_iNumChannels },
	m_dTickPerSeconds{ _rhs.m_dTickPerSeconds },
	m_dDuration{ _rhs.m_dDuration },
	m_vecAssimpChannels{ _rhs.m_vecAssimpChannels },
	m_dTimeAcc{ _rhs.m_dTimeAcc },
	m_isFinished{ false }
{
}

void TAssimpAnimation::Free()
{
	m_vecAssimpChannels.clear();
}

HRESULT TAssimpAnimation::NativeConstruct(aiAnimation* _pAIAnim, CSHPTRREF<TAssimpModel> _spModel)
{
	m_wstrName = UMethod::ConvertSToW(_pAIAnim->mName.data);
	size_t index = m_wstrName.find(L"|");
	m_wstrName = m_wstrName.substr(index + 1, m_wstrName.length());

	AnimationStringNameSplit(m_wstrName);
	m_dDuration = _pAIAnim->mDuration;
	m_dTickPerSeconds = _pAIAnim->mTicksPerSecond;
	m_iNumChannels = _pAIAnim->mNumChannels;
	m_vecAssimpChannels.reserve(m_iNumChannels);
	for (_uint i = 0; i < m_iNumChannels; ++i) {
		SHPTR<TAssimpChannel> spChannel = CreateNative<TAssimpChannel>(_pAIAnim->mChannels[i], _spModel);
		m_vecAssimpChannels.push_back(spChannel);
	}

	return S_OK;
}

HRESULT TAssimpAnimation::NativeConstruct(aiAnimation* _pAIAnim, CSHPTRREF<TAssimpModel> _spModel,
	const _wstring& _strName)
{
	m_wstrName = _strName;
	m_dDuration = _pAIAnim->mDuration;
	m_dTickPerSeconds = _pAIAnim->mTicksPerSecond;
	m_iNumChannels = _pAIAnim->mNumChannels;
	m_vecAssimpChannels.reserve(m_iNumChannels);
	for (_uint i = 0; i < m_iNumChannels; ++i) {
		SHPTR<TAssimpChannel> spChannel = CreateNative<TAssimpChannel>(_pAIAnim->mChannels[i], _spModel);
		m_vecAssimpChannels.push_back(spChannel);
	}
	return S_OK;
}

HRESULT TAssimpAnimation::NativeConstruct(aiAnimation* _pAIAnim, CSHPTRREF<TAssimpModel> _spModel, const CHANNELS& _Channeles)
{
	m_wstrName = UMethod::ConvertSToW(_pAIAnim->mName.data);
	AnimationStringNameSplit(m_wstrName);
	m_dDuration = _pAIAnim->mDuration;
	m_dTickPerSeconds = _pAIAnim->mTicksPerSecond;
	m_iNumChannels = _pAIAnim->mNumChannels;
	m_vecAssimpChannels.reserve(m_iNumChannels);
	for (_uint i = 0; i < m_iNumChannels; ++i) {
		SHPTR<TAssimpChannel> spChannel = CreateNative<TAssimpChannel>(_pAIAnim->mChannels[i], _spModel, _Channeles[i]->GetName());
		m_vecAssimpChannels.push_back(spChannel);
	}

	return S_OK;
}

HRESULT TAssimpAnimation::NativeConstruct(CSHPTRREF<TAssimpModel> _spModel, const ANIMDESC& _AnimDesc)
{
	m_iNumChannels = static_cast<_uint>(_AnimDesc.Channels.size());
	m_dDuration = _AnimDesc.stExtraData.dDuration;
	m_dTickPerSeconds = _AnimDesc.stExtraData.dTickPerSeconds;
	m_wstrName = _AnimDesc.wstrName;
	m_vecAssimpChannels.reserve(m_iNumChannels);
	for (auto& iter : _AnimDesc.Channels)
	{
		SHPTR<TAssimpChannel> pChannel{ CreateNative<TAssimpChannel>(_spModel, iter) };
		m_vecAssimpChannels.push_back(pChannel);
	}

	return S_OK;
}

void TAssimpAnimation::UpdateBoneMatrices(const _double& _dTimeDelta)
{
}

void TAssimpAnimation::GetData(ANIMDESC& _stAnimDesc)
{
	_stAnimDesc.wstrName = m_wstrName;
	_stAnimDesc.Channels.resize(m_iNumChannels);
	for (_uint i = 0; i < m_iNumChannels; ++i) {
		m_vecAssimpChannels[i]->GetData(_stAnimDesc.Channels[i]);
	}
	_stAnimDesc.stExtraData.dDuration = m_dDuration;
	_stAnimDesc.stExtraData.dTickPerSeconds = m_dTickPerSeconds;
	_stAnimDesc.stExtraData.iNumChannels = m_iNumChannels;
}

void TAssimpAnimation::AnimationStringNameSplit(_wstring& _wstrAnimName)
{
	if (_wstrAnimName.length() >= 20)
	{
		_int FirstIndex = static_cast<_int>(_wstrAnimName.find(L"_"));
		_int Lastndex = static_cast<_int>(_wstrAnimName.find_last_of(L"_"));
		if (FirstIndex != Lastndex)
		{
			_wstrAnimName = _wstrAnimName.substr(FirstIndex + 1, _wstrAnimName.length());
		}
		else
		{
			_wstrAnimName = _wstrAnimName.substr(_wstrAnimName.length() - 20, _wstrAnimName.length());
		}
	}
}
