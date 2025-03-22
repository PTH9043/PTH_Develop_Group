#include "EngineDefine.h"
#include "UAudioSystem.h"
#include "UGameInstance.h"
namespace fs = std::filesystem;

UAudioSystem::UAudioSystem(CSHPTRREF<UDevice> _spDevice) : 
	UResource(_spDevice), 
	m_pSoundSystem{nullptr},m_pBGmChannel{nullptr}, 
	m_SoundContainer{},	m_SoundOrders{}, 
	m_spSound{nullptr}, m_spBgmSound{nullptr}
{
}

UAudioSystem::UAudioSystem(const UAudioSystem& _rhs) 
	: UResource(_rhs),
	m_pSoundSystem{ nullptr }, m_pBGmChannel{ nullptr },
	m_SoundContainer{}, m_SoundOrders{}, 
	m_spSound{ nullptr }, m_spBgmSound{nullptr}
{
}

void UAudioSystem::Free()
{
}

HRESULT UAudioSystem::NativeConstruct()
{
	return __super::NativeConstruct();
}

HRESULT UAudioSystem::NativeConstruct(FMOD::System* _pSystem, const _wstring& _wstrSoudFolderPath)
{
	RETURN_CHECK(true == _wstrSoudFolderPath.empty()  || nullptr == _pSystem, E_FAIL);
	RETURN_CHECK_FAILED(NativeConstruct(), E_FAIL);
	m_pSoundSystem = _pSystem;
	LoadSound(_wstrSoudFolderPath);
	return S_OK;
}

HRESULT UAudioSystem::NativeConstruct(FMOD::System* _pSystem, CSHPTRREF<FILEGROUP> _spSoundFileGroup)
{
	RETURN_CHECK(nullptr == _spSoundFileGroup || nullptr == _pSystem, E_FAIL);
	RETURN_CHECK_FAILED(NativeConstruct(), E_FAIL);
	m_pSoundSystem = _pSystem;
	for (auto& iter : _spSoundFileGroup->FileDataList) {
		CreateSound(iter.second->wstrfilePath);
	}
	return S_OK;
}


HRESULT UAudioSystem::NativeConstructClone(const VOIDDATAS& _Datas)
{
	return __super::NativeConstructClone(_Datas);
}


void UAudioSystem::Tick()
{
	for (auto& iter : m_SoundContainer)
	{
		iter->Tick();
}

	}
void UAudioSystem::TickWithManyChannels()
{
	for (auto& iter : m_SoundContainer)
	{
		iter->TickWithManyChannels();
	}
}
_bool UAudioSystem::Play(const _wstring& _wstrSoundName)
{
	m_spSound = BringSound(_wstrSoundName);
	RETURN_CHECK(nullptr == m_spSound, false);
	m_spSound->Play();
	return true;
}

_bool UAudioSystem::PlayWithManyChannels(const _wstring& _wstrSoundName)
{
	m_spSound = BringSound(_wstrSoundName);
	RETURN_CHECK(nullptr == m_spSound, false);
	m_spSound->PlayWithManyChannels();
	return true;
}

_bool UAudioSystem::Play(const _wstring& _wstrSoundName, const _float _fVolumeUpdate)
{
	m_spSound = BringSound(_wstrSoundName);
	RETURN_CHECK(nullptr == m_spSound, false);
	m_spSound->Play();
	m_spSound->UpdateVolume(_fVolumeUpdate);
	return true;
}

_bool UAudioSystem::PlayOnce(const _wstring& _wstrSoundName)
{
	m_spSound = BringSound(_wstrSoundName);
	RETURN_CHECK(nullptr == m_spSound, false);
	m_spSound->PlayOnce();
	return true;
}

_bool UAudioSystem::PlayOnceWithManyChannels(const _wstring& _wstrSoundName)
{
	m_spSound = BringSound(_wstrSoundName);
	RETURN_CHECK(nullptr == m_spSound, false);
	m_spSound->PlayOnceWithManyChannels();
	return true;
}

_bool UAudioSystem::PlayBGM(const _wstring& _wstrSoundName)
{
	m_spBgmSound = BringSound(_wstrSoundName);
	RETURN_CHECK(nullptr == m_spBgmSound, false);
	m_spBgmSound->PlayBGM(&m_pBGmChannel);
	return true;
}

_bool UAudioSystem::StopBGM(const _wstring& _wstrSoundName)
{
	m_spBgmSound = BringSound(_wstrSoundName);
	RETURN_CHECK(nullptr == m_spBgmSound, false);
	m_spBgmSound->StopBGM(&m_pBGmChannel);
	return true;
}

_bool UAudioSystem::PauseBGM(const _wstring& _wstrSoundName)
{
	m_spBgmSound = BringSound(_wstrSoundName);
	RETURN_CHECK(nullptr == m_spBgmSound, false);
	m_spBgmSound->PauseBGM(&m_pBGmChannel);
	return true;
}

_bool UAudioSystem::ResumeBGM(const _wstring& _wstrSoundName)
{
	m_spBgmSound = BringSound(_wstrSoundName);
	RETURN_CHECK(nullptr == m_spBgmSound, false);
	m_spBgmSound->ResumeBGM(&m_pBGmChannel);
	return true;
}

_bool UAudioSystem::PlayBGM(const _wstring& _wstrSoundName, const _float _fVolumeUpdate)
{
	m_spBgmSound = BringSound(_wstrSoundName);
	RETURN_CHECK(nullptr == m_spBgmSound, false);
	m_spBgmSound->PlayBGM(&m_pBGmChannel);
	m_spBgmSound->UpdateVolume(_fVolumeUpdate);
	return true;
}

_bool UAudioSystem::Stop(const _wstring& _wstrSoundName)
{
	m_spSound = BringSound(_wstrSoundName);
	RETURN_CHECK(nullptr == m_spSound, false);
	m_spSound->Stop();
	return true;
}

_bool UAudioSystem::StopWithManyChannels(const _wstring& _wstrSoundName)
{
	m_spSound = BringSound(_wstrSoundName);
	RETURN_CHECK(nullptr == m_spSound, false);
	m_spSound->StopWithManyChannels();
	return true;
}

_bool UAudioSystem::VolumeUpdate(const _wstring& _wstrSoundName, const _float& _fVolumeUpdate)
{
	m_spSound = BringSound(_wstrSoundName);
	RETURN_CHECK(nullptr == m_spSound, false);
	m_spSound->UpdateVolume(_fVolumeUpdate);
	return true;
}

_bool UAudioSystem::BGMVolumeUpdate(const _wstring& _wstrSoundName, const _float& _fVolumeUpdate)
{
	m_spSound = BringSound(_wstrSoundName);
	RETURN_CHECK(nullptr == m_spSound, false);
	m_spSound->UpdateVolume(&m_pBGmChannel,_fVolumeUpdate);
	return true;
}

_bool UAudioSystem::Restart(const _wstring& _wstrSoundName)
{
	m_spSound = BringSound(_wstrSoundName);
	RETURN_CHECK(nullptr == m_spSound, false);
	m_spSound->Restart();
	return true;
}

_bool UAudioSystem::RestartOnce(const _wstring& _wstrSoundName)
{
	m_spSound = BringSound(_wstrSoundName);
	RETURN_CHECK(nullptr == m_spSound, false);
	m_spSound->RestartOnce();
	return true;
}

_bool UAudioSystem::Pause(const _wstring& _wstrSoundName)
{
	m_spSound = BringSound(_wstrSoundName);
	RETURN_CHECK(nullptr == m_spSound, false);
	m_spSound->Pause();
	return true;
}

_bool UAudioSystem::Resume(const _wstring& _wstrSoundName)
{
	m_spSound = BringSound(_wstrSoundName);
	RETURN_CHECK(nullptr == m_spSound, false);
	m_spSound->Resume();
	return true;
}

_bool UAudioSystem::SetPlaybackPosition(const _wstring& _wstrSoundName, unsigned int position)
{
	m_spSound = BringSound(_wstrSoundName);
	RETURN_CHECK(nullptr == m_spSound, false);
	m_spSound->SetPlaybackPosition(position);
	return true;
}

//unsigned int UAudioSystem::GetPlaybackPosition(const _wstring& _wstrSoundName)
//{
//	m_spSound = BringSound(_wstrSoundName);
//	RETURN_CHECK(nullptr == m_spSound, ;);
//	unsigned int pos = m_spSound->GetPlaybackPosition();
//	return pos;
//}

_bool UAudioSystem::SetLooping(const _wstring& _wstrSoundName, bool loop)
{
	m_spSound = BringSound(_wstrSoundName);
	RETURN_CHECK(nullptr == m_spSound, false);
	m_spSound->SetLooping(loop);
	return true;
}

_bool UAudioSystem::SetPitch(const _wstring& _wstrSoundName, float pitch)
{
	m_spSound = BringSound(_wstrSoundName);
	RETURN_CHECK(nullptr == m_spSound, false);
	m_spSound->SetPitch(pitch);
	return true;
}

_bool UAudioSystem::SetPan(const _wstring& _wstrSoundName, float pan)
{
	m_spSound = BringSound(_wstrSoundName);
	RETURN_CHECK(nullptr == m_spSound, false);
	m_spSound->SetPan(pan);
	return true;
}

_bool UAudioSystem::Mute(const _wstring& _wstrSoundName)
{
	m_spSound = BringSound(_wstrSoundName);
	RETURN_CHECK(nullptr == m_spSound, false);
	m_spSound->Mute();
	return true;
}

_bool UAudioSystem::Unmute(const _wstring& _wstrSoundName)
{
	m_spSound = BringSound(_wstrSoundName);
	RETURN_CHECK(nullptr == m_spSound, false);
	m_spSound->Unmute();
	return true;
}

_bool UAudioSystem::FadeIn(const _wstring& _wstrSoundName, float fadeDuration)
{
	m_spSound = BringSound(_wstrSoundName);
	RETURN_CHECK(nullptr == m_spSound, false);
	m_spSound->FadeIn(fadeDuration);
	return true;
}

_bool UAudioSystem::FadeOut(const _wstring& _wstrSoundName, float fadeDuration)
{
	m_spSound = BringSound(_wstrSoundName);
	RETURN_CHECK(nullptr == m_spSound, false);
	m_spSound->FadeOut(fadeDuration);
	return true;
}

_bool UAudioSystem::UpdateVolume(const _wstring& _wstrSoundName, const _float _fVolume)
{
	m_spSound = BringSound(_wstrSoundName);
	RETURN_CHECK(nullptr == m_spSound, false);
	m_spSound->FadeOut(_fVolume);
	return true;
}

_bool UAudioSystem::DisableOncePlay(const _wstring& _wstrSoundName)
{
	m_spSound = BringSound(_wstrSoundName);
	RETURN_CHECK(nullptr == m_spSound, false);
	m_spSound->DisableOncePlay();
	return true;
}

_bool UAudioSystem::UpdateSound3D(const _wstring& _wstrSoundName, const _float3& _vSoudPos,
	const _float3& _vSoundVelocity, CSHPTRREF<UTransform> _spTargetTransform_CanNullptr)
{
	CSHPTRREF<USound> spSound = BringSound(_wstrSoundName);
	RETURN_CHECK(nullptr == m_spSound, false);
	spSound->UpdateSound3D(_vSoudPos, _vSoundVelocity, _spTargetTransform_CanNullptr);
	return true;
}

_bool UAudioSystem::UpdateSound3D(const _wstring& _wstrSoundName, CSHPTRREF<UTransform> _spSelfTransform, const _float3& _vSoundVelocity,
	CSHPTRREF<UTransform> _spTargetTransform_CanNullptr)
{
	CSHPTRREF<USound> spSound = BringSound(_wstrSoundName);
	RETURN_CHECK(nullptr == m_spSound, false);
	spSound->UpdateSound3D(_spSelfTransform, _vSoundVelocity, _spTargetTransform_CanNullptr);
	return true;
}

_bool UAudioSystem::ChangeMinMaxDistance3D(const _wstring& _wstrSoundName, const _float _fMinDistance, const _float _fMaxDistance)
{
	CSHPTRREF<USound> spSound = BringSound(_wstrSoundName);
	RETURN_CHECK(nullptr == m_spSound, false);
	spSound->ChangeMinMaxDistance3D(_fMinDistance, _fMaxDistance);
	return true;
}

SHPTR<USound> UAudioSystem::BringSound(const _int _Index)
{
	RETURN_CHECK(_Index < 0 || _Index >= m_SoundContainer.size(), nullptr)
	return m_SoundContainer[_Index];
}

SHPTR<USound> UAudioSystem::BringSound(const _wstring& _wstrSoundName)
{
	const auto& FindIter = m_SoundOrders.find(_wstrSoundName);
	RETURN_CHECK(m_SoundOrders.end() == FindIter, nullptr);
	return m_SoundContainer[FindIter->second];
}

void UAudioSystem::Release()
{
	for (auto& iter : m_SoundContainer)
		iter->Free();
}
void UAudioSystem::LoadSound(const _wstring& _wstrSoundFolderPath)
{
	fs::directory_iterator end;
	for (fs::directory_iterator iter(_wstrSoundFolderPath); iter != end; ++iter)
	{
		// RegularFile
		if (fs::is_regular_file(iter->status()))
		{
			CreateSound(iter->path());
		}
		else if (fs::is_directory(iter->status()))
		{
			LoadSound(iter->path());
		}
	}
}

void UAudioSystem::CreateSound(const _wstring& _wstrSoundPath)
{
	_wstring Name = _wstrSoundPath.substr(_wstrSoundPath.find_last_of(L"\\") + 1, _wstrSoundPath.find_last_of(L"."));
	Name = Name.substr(0, Name.find_last_of(L"."));
	_int SoundIndex = static_cast<_int>(m_SoundContainer.size());
	m_SoundOrders.emplace(MakePair(Name, SoundIndex));
	m_SoundContainer.push_back(Create<USound>(m_pSoundSystem, _wstrSoundPath, SoundIndex));
}
