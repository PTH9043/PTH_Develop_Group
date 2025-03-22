#pragma once
#include <fmod/fmod.hpp>
#include <fmod/fmod_errors.h>
#include "UBase.h"

BEGIN(Engine)
class UAudioSystem;
class UTransform;
/*
@ Date: 2024-02-25, Writer: 박태현
@ Explain
- Sound를 파일에서 로드하고 담당하는 클래스
*/
class USound final : public UBase {
public:
	USound(FMOD::System* _pSystem, const _wstring& _wstrSoundPath, _int _SoundIndex);
	DESTRUCTOR(USound)
public: /* get set */
	SOUNDTYPE GetSoundType() const { return m_SoundDesc.SoundType; }
	_float GetVolume() const { return m_SoundDesc.fVolume; }
	FMOD::Sound* GetSound() const { return m_pSound; }
	const _bool IsSoundPlay() const { return m_isSoundPlay; }

	void SetVolume(const _float _fVolume) { m_SoundDesc.fVolume = _fVolume; }
public:
	virtual void Free() override;
	void Tick();
	void TickWithManyChannels();
	void TickWithInputChannel(IN FMOD::Channel** _ppChannel);
	void Play();
	void PlayWithInputChannel(IN FMOD::Channel** _ppChannel);
	void PlayWithManyChannels();
	void PlayBGM(IN FMOD::Channel** _ppChannel);
	void PlayOnce();
	void PlayOnceWithManyChannels();
	void Stop();
	void StopWithInputChannel(IN FMOD::Channel** _ppChannel);
	void StopWithManyChannels();
	void StopBGM(IN FMOD::Channel** _ppChannel);
	void PauseBGM(IN FMOD::Channel** _ppChannel);
	void ResumeBGM(IN FMOD::Channel** _ppChannel);
	void Restart();
	void RestartOnce();
	void Pause();
	void Resume();
	void SetPlaybackPosition(unsigned int position);
	//unsigned int GetPlaybackPosition();
	void SetLooping(bool loop);
	void SetPitch(float pitch);
	void SetPan(float pan);
	void Mute();
	void Unmute();
	void FadeIn(float fadeDuration);
	void FadeOut(float fadeDuration);
	void UpdateVolume(const _float _fVolume) ;
	void UpdateVolume(IN FMOD::Channel** _ppChannel,const _float _fVolume) ;
	void UpdateSound3D(const _float3& _vSoudPos, const _float3& _vSoundVelocity, CSHPTRREF<UTransform> _spTargetTransform_CanNullptr = nullptr);
	void UpdateSound3D(CSHPTRREF<UTransform> _spSelfTransform, const _float3& _vSoundVelocity,
		CSHPTRREF<UTransform> _spTargetTransform_CanNullptr = nullptr);
	void ChangeMinMaxDistance3D(const _float _fMinDistance, const _float _fMaxDistance);
	void DisableOncePlay() { m_isOncePlay = false; }
private:
	_int							m_iIndex;
	SOUNDDESC			m_SoundDesc;
	_bool						m_isSoundPlay;

	FMOD_VECTOR		m_SoundPos;
	FMOD_VECTOR		m_SoundVelocity;

	FMOD_VECTOR		m_ListenerPos;
	FMOD_VECTOR		m_ListenerLook;
	FMOD_VECTOR		m_ListenerUp;

	FMOD::System*	m_pSystem;
	FMOD::Sound*		m_pSound;
	FMOD::Channel*  m_pChannel;
	std::vector<FMOD::Channel*> m_Channels;
	_bool						m_isOncePlay;
};

/* 
=============================================
Sound
=============================================
*/

END
