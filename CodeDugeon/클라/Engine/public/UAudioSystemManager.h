#pragma once

#include <fmod/fmod.hpp>
#include <fmod/fmod_errors.h>
#include "UBase.h"

BEGIN(Engine)
class UGameInstance;
class UAudioSystem;
class USound;
class UTransform;
class UCharacter;


using AUDIOSYSTEMCONTAINER = ARRAY <SHPTR< UAudioSystem>, SOUND_END>;
/*
@ Date: 2024-02-25, Writer: 박태현
@ Explain
- UAudioSystem을 관리하는 매니저 클래스
*/
class UAudioSystemManager final : public UBase {
public:
	UAudioSystemManager();
	NO_COPY(UAudioSystemManager)
	DESTRUCTOR(UAudioSystemManager)
public:
	const AUDIOSYSTEMCONTAINER& GetAudioSystemContainer() const { return m_AudioSystemContainer; }
	SHPTR<UAudioSystem> GetAudioSystem(const SOUNDTYPE _SoundType);
public:
	HRESULT ReadyAudioSystemManager(UGameInstance* _pGameInstance);

	HRESULT CreateAudioSystemAndRegister(UGameInstance* _pGameInstance,  SOUNDTYPE _SoundType, const _wstring& _wstrSoundFolderPath);
	HRESULT CreateAudioSystemAndRegister(UGameInstance* _pGameInstance, SOUNDTYPE _SoundType, CSHPTRREF<FILEGROUP> _spSoundFileGroup);
	HRESULT CreateAudioSystemToFolderNameAndRegister(UGameInstance* _pGameInstance, SOUNDTYPE _SoundType, const _wstring& _wstrSoundFolderName);

	void Play(const _wstring& _wstrSoundName);
	void PlayWithManyChannels(const _wstring& _wstrSoundName);
	void Play(const _wstring& _wstrSoundName, const _float& _fVolumeUpdate);
	void PlayOnce(const _wstring& _wstrSoundName);
	void PlayOnceWithManyChannels(const _wstring& _wstrSoundName);
	void PlayBGM(const _wstring& _wstrSoundName);
	void StopBGM(const _wstring& _wstrSoundName);
	void PauseBGM(const _wstring& _wstrSoundName);
	void ResumeBGM(const _wstring& _wstrSoundName);
	void PlayBGM(const _wstring& _wstrSoundName, const _float& _fVolumeUpdate);
	void Stop(const _wstring& _wstrSoundName);
	void StopWithManyChannels(const _wstring& _wstrSoundName);
	void VolumeUpdate(const _wstring& _wstrSoundName, const _float& _fVolumeUpdate);
	void DisableOncePlay(const _wstring& _wstrSoundName);
	void Restart(const _wstring& _wstrSoundName);
	void RestartOnce(const _wstring& _wstrSoundName);
	void Pause(const _wstring& _wstrSoundName);
	void Resume(const _wstring& _wstrSoundName);
	void SetPlaybackPosition(const _wstring& _wstrSoundName, unsigned int position);
	//unsigned int GetPlaybackPosition(const _wstring& _wstrSoundName);
	void SetLooping(const _wstring& _wstrSoundName, bool loop);
	void SetPitch(const _wstring& _wstrSoundName, float pitch);
	void SetPan(const _wstring& _wstrSoundName, float pan);
	void Mute(const _wstring& _wstrSoundName);
	void Unmute(const _wstring& _wstrSoundName);
	void FadeIn(const _wstring& _wstrSoundName, float fadeDuration);
	void FadeOut(const _wstring& _wstrSoundName, float fadeDuration);
	void UpdateVolume(const _wstring& _wstrSoundName, const _float _fVolume);
	void BGMUpdateVolume(const _wstring& _wstrSoundName, const _float _fVolume);
	void UpdateSound3D(const _wstring& _wstrSoundName, const _float3& _vSoudPos, const _float3& _vSoundVelocity, 
		CSHPTRREF<UTransform> _spTargetTransform = nullptr);
	void UpdateSound3D(const _wstring& _wstrSoundName, CSHPTRREF<UTransform> _spSoundTransform, const _float3& _vSoundVelocity,
		CSHPTRREF<UTransform> _spTargetTransform = nullptr);
	void ChangeMinMaxDistance3D(const _wstring& _wstrSoundName, const _float _fMinDistance, const _float _fMaxDistance);
	void HandleSoundsForAnimation(CSHPTRREF<UCharacter> _Owner, const _wstring& animName, const _wstring& SoundName, _float startThreshold, _float endThreshold);
	void HandleSoundsForAnimationWithManyChannels(CSHPTRREF<UCharacter> _Owner, const _wstring& animName, const _wstring& SoundName, _float startThreshold, _float endThreshold);

	SHPTR<USound> BringSound(const _int _Index);
	SHPTR<USound> BringSound(const _wstring& _wstrSoundName);
private:
	virtual void Free() override;
	
private:
	FMOD::System*						m_pSystem;
	AUDIOSYSTEMCONTAINER	m_AudioSystemContainer;
};


END