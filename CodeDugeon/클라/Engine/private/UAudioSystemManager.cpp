#include "EngineDefine.h"
#include "UAudioSystemManager.h"
#include "UGameInstance.h"
#include "UAudioSystem.h"
#include "UTransform.h"
#include "UCharacter.h"
#include "UAnimModel.h"
#include "UAnimation.h"

UAudioSystemManager::UAudioSystemManager() : m_AudioSystemContainer{}, m_pSystem { nullptr }
{
	for (auto& iter : m_AudioSystemContainer)
		iter = nullptr;
}

SHPTR<UAudioSystem> UAudioSystemManager::GetAudioSystem(const SOUNDTYPE _SoundType)
{
	RETURN_CHECK(_SoundType >= SOUND_END, nullptr);
	const auto& iter = m_AudioSystemContainer[_SoundType];
	return iter;
}

HRESULT UAudioSystemManager::ReadyAudioSystemManager(UGameInstance* _pGameInstance)
{
	// Create System 
	FMOD_RESULT Result = FMOD::System_Create(&m_pSystem);
	RETURN_CHECK(FMOD_OK != Result, E_FAIL);
	// Init 
	Result = m_pSystem->init(MAX_CHANNEL_CNT, FMOD_INIT_NORMAL, 0);
	RETURN_CHECK(FMOD_OK != Result, E_FAIL);
	return S_OK;
}

HRESULT UAudioSystemManager::CreateAudioSystemAndRegister(UGameInstance* _pGameInstance, SOUNDTYPE _SoundType,
	const _wstring& _wstrSoundFolderPath)
{
	RETURN_CHECK(_SoundType >= SOUND_END, E_FAIL);
	assert(nullptr == m_AudioSystemContainer[_SoundType]);
	SHPTR<UAudioSystem> spAudioSystem{ CreateConstructorNative< UAudioSystem>(_pGameInstance->GetDevice(), m_pSystem, _wstrSoundFolderPath) };
	m_AudioSystemContainer[_SoundType] = spAudioSystem;
	return S_OK;
}

HRESULT UAudioSystemManager::CreateAudioSystemAndRegister(UGameInstance* _pGameInstance, SOUNDTYPE _SoundType,
	CSHPTRREF<FILEGROUP> _spSoundFileGroup)
{
	RETURN_CHECK(_SoundType >= SOUND_END, E_FAIL);
	assert(nullptr == m_AudioSystemContainer[_SoundType]);
	SHPTR<UAudioSystem> spAudioSystem{ CreateConstructorNative< UAudioSystem>(_pGameInstance->GetDevice(), m_pSystem, _spSoundFileGroup) };
	m_AudioSystemContainer[_SoundType] = spAudioSystem;
	return S_OK;
}

HRESULT UAudioSystemManager::CreateAudioSystemToFolderNameAndRegister(UGameInstance* _pGameInstance, SOUNDTYPE _SoundType,
	const _wstring& _wstrSoundFolderName)
{
	SHPTR<FILEGROUP> spAudioFolder = _pGameInstance->FindFolder(_wstrSoundFolderName);
	return CreateAudioSystemAndRegister(_pGameInstance, _SoundType, spAudioFolder);
}

void UAudioSystemManager::Play(const _wstring& _wstrSoundName)
{
	for (auto& iter : m_AudioSystemContainer)
	{
		if (nullptr != iter)
		{
			if (true == iter->Play(_wstrSoundName))
				return;
		}
	}
}

void UAudioSystemManager::PlayWithManyChannels(const _wstring& _wstrSoundName)
{
	for (auto& iter : m_AudioSystemContainer)
	{
		if (nullptr != iter)
		{
			if (true == iter->PlayWithManyChannels(_wstrSoundName))
				return;
		}
	}
}

void UAudioSystemManager::Play(const _wstring& _wstrSoundName, const _float& _fVolumeUpdate)
{
	for (auto& iter : m_AudioSystemContainer)
	{
		if (nullptr != iter)
		{
			if (true == iter->Play(_wstrSoundName, _fVolumeUpdate))
				return;
		}
	}
}

void UAudioSystemManager::PlayOnce(const _wstring& _wstrSoundName)
{
	for (auto& iter : m_AudioSystemContainer)
	{
		if (nullptr != iter)
		{
			if (true == iter->PlayOnce(_wstrSoundName))
				return;
		}
	}
}

void UAudioSystemManager::PlayOnceWithManyChannels(const _wstring& _wstrSoundName)
{
	for (auto& iter : m_AudioSystemContainer)
	{
		if (nullptr != iter)
		{
			if (true == iter->PlayOnceWithManyChannels(_wstrSoundName))
				return;
		}
	}

}

void UAudioSystemManager::PlayBGM(const _wstring& _wstrSoundName)
{
	for (auto& iter : m_AudioSystemContainer)
	{
		if (nullptr != iter)
		{
			if (true == iter->PlayBGM(_wstrSoundName))
				return;
		}
	}
}

void UAudioSystemManager::StopBGM(const _wstring& _wstrSoundName)
{
	for (auto& iter : m_AudioSystemContainer)
	{
		if (nullptr != iter)
		{
			if (true == iter->StopBGM(_wstrSoundName))
				return;
		}
	}
}

void UAudioSystemManager::PauseBGM(const _wstring& _wstrSoundName)
{
	for (auto& iter : m_AudioSystemContainer)
	{
		if (nullptr != iter)
		{
			if (true == iter->PauseBGM(_wstrSoundName))
				return;
		}
	}
}

void UAudioSystemManager::ResumeBGM(const _wstring& _wstrSoundName)
{
	for (auto& iter : m_AudioSystemContainer)
	{
		if (nullptr != iter)
		{
			if (true == iter->ResumeBGM(_wstrSoundName))
				return;
		}
	}
}

void UAudioSystemManager::PlayBGM(const _wstring& _wstrSoundName, const _float& _fVolumeUpdate)
{
	for (auto& iter : m_AudioSystemContainer)
	{
		if (nullptr != iter)
		{
			if (true == iter->PlayBGM(_wstrSoundName, _fVolumeUpdate))
				return;
		}
	}
}

void UAudioSystemManager::Stop(const _wstring& _wstrSoundName)
{
	for (auto& iter : m_AudioSystemContainer)
	{
		if (nullptr != iter)
		{
			if (true == iter->Stop(_wstrSoundName))
				return;
		}
	}
}

void UAudioSystemManager::StopWithManyChannels(const _wstring& _wstrSoundName)
{
	for (auto& iter : m_AudioSystemContainer)
	{
		if (nullptr != iter)
		{
			if (true == iter->Stop(_wstrSoundName))
				return;
		}
	}
}

void UAudioSystemManager::VolumeUpdate(const _wstring& _wstrSoundName, const _float& _fVolumeUpdate)
{
	for (auto& iter : m_AudioSystemContainer)
	{
		if (nullptr != iter)
		{
			if (true == iter->VolumeUpdate(_wstrSoundName, _fVolumeUpdate))
				return;
		}
	}
}

void UAudioSystemManager::DisableOncePlay(const _wstring& _wstrSoundName)
{
	for (auto& iter : m_AudioSystemContainer)
	{
		if (nullptr != iter)
		{
			if (true == iter->DisableOncePlay(_wstrSoundName))
				return;
		}
	}
}

void UAudioSystemManager::Restart(const _wstring& _wstrSoundName)
{
	for (auto& iter : m_AudioSystemContainer)
	{
		if (nullptr != iter)
		{
			if (true == iter->Restart(_wstrSoundName))
				return;
		}
	}
}

void UAudioSystemManager::RestartOnce(const _wstring& _wstrSoundName)
{
	for (auto& iter : m_AudioSystemContainer)
	{
		if (nullptr != iter)
		{
			if (true == iter->RestartOnce(_wstrSoundName))
				return;
		}
	}
}

void UAudioSystemManager::Pause(const _wstring& _wstrSoundName)
{
	for (auto& iter : m_AudioSystemContainer)
	{
		if (nullptr != iter)
		{
			if (true == iter->Pause(_wstrSoundName))
				return;
		}
	}
}

void UAudioSystemManager::Resume(const _wstring& _wstrSoundName)
{
	for (auto& iter : m_AudioSystemContainer)
	{
		if (nullptr != iter)
		{
			if (true == iter->Resume(_wstrSoundName))
				return;
		}
	}
}

void UAudioSystemManager::SetPlaybackPosition(const _wstring& _wstrSoundName, unsigned int position)
{
	for (auto& iter : m_AudioSystemContainer)
	{
		if (nullptr != iter)
		{
			if (true == iter->SetPlaybackPosition(_wstrSoundName, position))
				return;
		}
	}
}

//unsigned int UAudioSystemManager::GetPlaybackPosition(const _wstring& _wstrSoundName)
//{
//	for (auto& iter : m_AudioSystemContainer)
//	{
//		if (nullptr != iter)
//			return iter->GetPlaybackPosition(_wstrSoundName);
//	}
//}

void UAudioSystemManager::SetLooping(const _wstring& _wstrSoundName, bool loop)
{
	for (auto& iter : m_AudioSystemContainer)
	{
		if (nullptr != iter)
		{
			if (true == iter->SetLooping(_wstrSoundName, loop))
				return;
		}
	}
}

void UAudioSystemManager::SetPitch(const _wstring& _wstrSoundName, float pitch)
{
	for (auto& iter : m_AudioSystemContainer)
	{
		if (nullptr != iter)
		{
			if (true == iter->SetPitch(_wstrSoundName, pitch))
				return;
		}
	}
}

void UAudioSystemManager::SetPan(const _wstring& _wstrSoundName, float pan)
{
	for (auto& iter : m_AudioSystemContainer)
	{
		if (nullptr != iter)
		{
			if (true == iter->SetPan(_wstrSoundName, pan))
				return;
		}
	}
}

void UAudioSystemManager::Mute(const _wstring& _wstrSoundName)
{
	for (auto& iter : m_AudioSystemContainer)
	{
		if (nullptr != iter)
		{
			if (true == iter->Mute(_wstrSoundName))
				return;
		}
	}
}

void UAudioSystemManager::Unmute(const _wstring& _wstrSoundName)
{
	for (auto& iter : m_AudioSystemContainer)
	{
		if (nullptr != iter)
		{
			if (true == iter->Unmute(_wstrSoundName))
				return;
		}
	}
}

void UAudioSystemManager::FadeIn(const _wstring& _wstrSoundName, float fadeDuration)
{
	for (auto& iter : m_AudioSystemContainer)
	{
		if (nullptr != iter)
		{
			if (true == iter->FadeIn(_wstrSoundName, fadeDuration))
				return;
		}
	}
}

void UAudioSystemManager::FadeOut(const _wstring& _wstrSoundName, float fadeDuration)
{
	for (auto& iter : m_AudioSystemContainer)
	{
		if (nullptr != iter)
		{
			if (true == iter->FadeOut(_wstrSoundName, fadeDuration))
				return;
		}
	}
}

void UAudioSystemManager::UpdateVolume(const _wstring& _wstrSoundName, const _float _fVolume)
{
	for (auto& iter : m_AudioSystemContainer)
	{
		if (nullptr != iter)
		{
			if (true == iter->UpdateVolume(_wstrSoundName, _fVolume))
				return;
		}
	}
}

void UAudioSystemManager::BGMUpdateVolume(const _wstring& _wstrSoundName, const _float _fVolume)
{
	for (auto& iter : m_AudioSystemContainer)
	{
		if (nullptr != iter)
		{
			if (true == iter->BGMVolumeUpdate(_wstrSoundName, _fVolume))
				return;
		}
	}
}

void UAudioSystemManager::UpdateSound3D(const _wstring& _wstrSoundName, const _float3& _vSoudPos, const _float3& _vSoundVelocity, CSHPTRREF<UTransform> _spTargetTransform)
{
	for (auto& iter : m_AudioSystemContainer)
	{
		if (nullptr != iter)
		{
			if (true == iter->UpdateSound3D(_wstrSoundName, _vSoudPos, _vSoundVelocity, _spTargetTransform))
				return;
		}
	}
}

void UAudioSystemManager::UpdateSound3D(const _wstring& _wstrSoundName, CSHPTRREF<UTransform> _spSoundTransform, const _float3& _vSoundVelocity, CSHPTRREF<UTransform> _spTargetTransform)
{
	for (auto& iter : m_AudioSystemContainer)
	{
		if (nullptr != iter)
		{
			if (true == iter->UpdateSound3D(_wstrSoundName, _spSoundTransform, _vSoundVelocity, _spTargetTransform))
				return;
		}
	}
}

void UAudioSystemManager::ChangeMinMaxDistance3D(const _wstring& _wstrSoundName, const _float _fMinDistance, const _float _fMaxDistance)
{
	for (auto& iter : m_AudioSystemContainer)
	{
		if (nullptr != iter)
		{
			if (true == iter->ChangeMinMaxDistance3D(_wstrSoundName, _fMinDistance, _fMaxDistance))
				return;
		}
	}
}

SHPTR<USound> UAudioSystemManager::BringSound(const _int _Index)
{
	SHPTR<USound> spSound{ nullptr };
	for (auto& iter : m_AudioSystemContainer)
	{
		if (nullptr != iter)
		{
			spSound = iter->BringSound(_Index);
			if (nullptr != spSound)
				return spSound;
		}
	}
	return nullptr;
}

SHPTR<USound> UAudioSystemManager::BringSound(const _wstring& _wstrSoundName)
{
	SHPTR<USound> spSound{ nullptr };
	for (auto& iter : m_AudioSystemContainer)
	{
		if (nullptr != iter)
		{
			spSound = iter->BringSound(_wstrSoundName);
			if (nullptr != spSound)
				return spSound;
		}
	}
	return nullptr;
}

void UAudioSystemManager::Free()
{
	for (auto& iter : m_AudioSystemContainer)
	{
		if (nullptr != iter)
			iter->Release();

		iter.reset();
	}

	if (m_pSystem) {
		m_pSystem->release();
	}
}


void UAudioSystemManager::HandleSoundsForAnimation(CSHPTRREF<UCharacter> _Owner, const _wstring& animName, const _wstring& SoundName, _float startThreshold, _float endThreshold)
{
	SHPTR<UGameInstance> spGameInstance = GET_INSTANCE(UGameInstance);
	const _wstring& CurAnimName = _Owner->GetAnimModel()->GetCurrentAnimation()->GetAnimName();
	SHPTR<USound> Sound = spGameInstance->BringSound(SoundName);
	if (CurAnimName == animName)
	{
		SHPTR<USound> Sound = spGameInstance->BringSound(SoundName);

		_float progressRate = _Owner->GetAnimModel()->GetCurrentAnimation()->GetAnimationProgressRate();

		if (progressRate >= endThreshold)
		{
			Sound->Stop();
		}
		else if (progressRate >= startThreshold && progressRate < (startThreshold + 0.01))
		{
			Sound->PlayOnce();
		}
	}
}



void UAudioSystemManager::HandleSoundsForAnimationWithManyChannels(CSHPTRREF<UCharacter> _Owner, const _wstring& animName, const _wstring& SoundName, _float startThreshold, _float endThreshold)
{
	SHPTR<UGameInstance> spGameInstance = GET_INSTANCE(UGameInstance);
	const _wstring& CurAnimName = _Owner->GetAnimModel()->GetCurrentAnimation()->GetAnimName();

	if (CurAnimName == animName)
	{
		SHPTR<USound> Sound = spGameInstance->BringSound(SoundName);
		_float progressRate = _Owner->GetAnimModel()->GetCurrentAnimation()->GetAnimationProgressRate();

		if (progressRate >= endThreshold)
		{
			Sound->StopWithManyChannels();
		}
		else if (progressRate >= startThreshold && progressRate < (startThreshold + 0.01))
		{
			Sound->PlayWithManyChannels();
		}
	}
}
