#include "EngineDefine.h"
#include "USound.h"
#include "UAudioSystem.h"
#include "UTransform.h"
#include "UMethod.h"

USound::USound(FMOD::System* _pSystem, const _wstring& _wstrSoundPath, _int _SoundIndex) :
	m_iIndex{ _SoundIndex }, m_SoundDesc{ SOUND_END, 0.5f }, m_isSoundPlay{ false },
	m_SoundPos{}, m_SoundVelocity{}, m_ListenerPos{}, m_ListenerLook{}, m_ListenerUp{}, 
	m_pSystem{_pSystem}, m_pSound{nullptr}, m_pChannel{nullptr}, m_isOncePlay{false}
{
	assert(nullptr != m_pSystem);
	FMOD_RESULT Result = m_pSystem->createSound(UMethod::ConvertWToS(_wstrSoundPath).c_str(), FMOD_DEFAULT, nullptr, &m_pSound);
	assert(FMOD_OK == Result);
}

void USound::Tick()
{
	RETURN_CHECK(nullptr == m_pChannel, ;);
	m_pChannel->isPlaying(&m_isSoundPlay);
	if (false == m_isSoundPlay){
		m_pChannel = nullptr;
		m_isSoundPlay = false;
	}
}

void USound::TickWithManyChannels()
{
	for (auto it = m_Channels.begin(); it != m_Channels.end(); ) {
		bool isPlaying = false;
		(*it)->isPlaying(&isPlaying);
		if (!isPlaying) {
			it = m_Channels.erase(it);
		}
		else {
			++it;
		}
	}
}

void USound::TickWithInputChannel(IN FMOD::Channel** _ppChannel)
{
	_bool isPlaying;
	(*_ppChannel)->isPlaying(&isPlaying);
	if (false == isPlaying) {
		*_ppChannel = nullptr;
	}
}

void USound::Play()
{
	Stop();
	m_pSystem->playSound(m_pSound, nullptr, false, &m_pChannel);
	m_pChannel->setVolume(m_SoundDesc.fVolume);
}

void USound::PlayWithInputChannel(IN FMOD::Channel** _ppChannel)
{
	TickWithInputChannel(_ppChannel);
	m_pSystem->playSound(m_pSound, nullptr, false, _ppChannel);
}

void USound::PlayWithManyChannels()
{
	TickWithManyChannels();
	FMOD::Channel* channel = nullptr;
	m_pSystem->playSound(m_pSound, nullptr, false, &channel);
	m_Channels.push_back(channel);
}

void USound::PlayBGM(IN FMOD::Channel** _ppChannel)
{
	assert(nullptr != _ppChannel);
	m_pSystem->playSound(m_pSound, nullptr, false, _ppChannel);
	(*_ppChannel)->setMode(FMOD_LOOP_NORMAL);
	(*_ppChannel)->setVolume(m_SoundDesc.fVolume);
	m_pSystem->update();
}

void USound::PlayOnce()
{
	Tick();
	if (false == IsSoundPlay() || false == m_isOncePlay) {
		Play();
		m_isOncePlay = true;
	}
}

void USound::PlayOnceWithManyChannels()
{
	//TickWithManyChannels();
	if (!m_isOncePlay) {
		PlayWithManyChannels();
		m_isOncePlay = true;
	}
}

void USound::Restart()
{
	// Stop the sound if it's playing
	if (m_pChannel) {
		bool isPlaying = false;
		m_pChannel->isPlaying(&isPlaying);
		if (isPlaying) {
			m_pChannel->stop();
		}
	}

	// Play the sound from the beginning
	m_pSystem->playSound(m_pSound, nullptr, false, &m_pChannel);
	m_pChannel->setVolume(m_SoundDesc.fVolume);
}

void USound::RestartOnce()
{
	Tick();
	if (false == IsSoundPlay() || false == m_isOncePlay) {
		Restart();
		m_isOncePlay = true;
	}
}
void USound::Stop()
{
	RETURN_CHECK(nullptr == m_pChannel, ;);
	m_pChannel->stop();
	m_pChannel = nullptr;
}

void USound::StopWithInputChannel(IN FMOD::Channel** _ppChannel)
{
	if (_ppChannel && *_ppChannel)
	{
		(*_ppChannel)->stop();
		*_ppChannel = nullptr;  // 채널 포인터를 해제
	}
}

void USound::StopWithManyChannels() {
	for (auto& channel : m_Channels) {
		if (channel) {
			channel->stop();
		}
	}
	m_Channels.clear();
	m_isOncePlay = false;
}

void USound::StopBGM(IN FMOD::Channel** _ppChannel)
{
	if (_ppChannel && *_ppChannel)
	{
		(*_ppChannel)->stop();
		*_ppChannel = nullptr;  // 채널 포인터를 해제
	}
}

void USound::PauseBGM(IN FMOD::Channel** _ppChannel)
{
	if (_ppChannel && *_ppChannel)
	{
		(*_ppChannel)->setPaused(true);
	}
}

void USound::ResumeBGM(IN FMOD::Channel** _ppChannel)
{
	if (_ppChannel && *_ppChannel)
	{
		(*_ppChannel)->setPaused(false);
	}
}

void USound::Pause()
{
	RETURN_CHECK(nullptr == m_pChannel, ;);
	m_pChannel->setPaused(true);
}

void USound::Resume()
{
	RETURN_CHECK(nullptr == m_pChannel, ;);
	m_pChannel->setPaused(false);
}

void USound::SetPlaybackPosition(unsigned int position)
{
	RETURN_CHECK(nullptr == m_pChannel, ;);
	m_pChannel->setPosition(position, FMOD_TIMEUNIT_MS);
}

//unsigned int USound::GetPlaybackPosition()
//{
//	RETURN_CHECK(nullptr == m_pChannel, 0);
//	unsigned int position = 0;
//	m_pChannel->getPosition(&position, FMOD_TIMEUNIT_MS);
//	return position;
//}

void USound::SetLooping(bool loop)
{
	RETURN_CHECK(nullptr == m_pChannel, ;);
	m_pChannel->setMode(loop ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF);
}

void USound::SetPitch(float pitch)
{
	RETURN_CHECK(nullptr == m_pChannel, ;);
	m_pChannel->setPitch(pitch);
}

void USound::SetPan(float pan)
{
	RETURN_CHECK(nullptr == m_pChannel, ;);
	m_pChannel->setPan(pan);
}

void USound::Mute()
{
	RETURN_CHECK(nullptr == m_pChannel, ;);
	m_pChannel->setMute(true);
}

void USound::Unmute()
{
	RETURN_CHECK(nullptr == m_pChannel, ;);
	m_pChannel->setMute(false);
}

void USound::FadeIn(float fadeDuration)// 볼륨 서서히 증가
{
	RETURN_CHECK(nullptr == m_pChannel, ;);
	m_pChannel->setVolume(0.0f);
	m_pChannel->setPaused(false);
	for (float volume = 0.0f; volume <= m_SoundDesc.fVolume; volume += (m_SoundDesc.fVolume / fadeDuration))
	{
		m_pChannel->setVolume(volume);
		Sleep(10); // Sleep for 10 milliseconds
	}
	m_pChannel->setVolume(m_SoundDesc.fVolume);
}

void USound::FadeOut(float fadeDuration)
{
	RETURN_CHECK(nullptr == m_pChannel, ;);
	for (float volume = m_SoundDesc.fVolume; volume >= 0.0f; volume -= (m_SoundDesc.fVolume / fadeDuration))
	{
		m_pChannel->setVolume(volume);
		Sleep(10); // Sleep for 10 milliseconds
	}
	m_pChannel->setVolume(0.0f);
	Stop();
}


void USound::UpdateVolume(const _float _fVolume)
{
	RETURN_CHECK(nullptr == m_pChannel, ;);
	m_SoundDesc.fVolume = _fVolume;
	m_pChannel->setVolume(_fVolume);
}


void USound::UpdateVolume(IN FMOD::Channel** _ppChannel,const _float _fVolume)
{
	RETURN_CHECK(nullptr == *_ppChannel, ;);
	m_SoundDesc.fVolume = _fVolume;
	(*_ppChannel)->setVolume(_fVolume);
}

void USound::UpdateSound3D(const _float3& _vSoudPos, const _float3& _vSoundVelocity, 
	CSHPTRREF<UTransform> _spTargetTransform_CanNullptr)
{
	assert(SOUND_GAME != m_SoundDesc.SoundType);
	RETURN_CHECK(nullptr == m_pChannel, ;);
	// Sound Pos, Velocity Update
	{
		::memcpy(&m_SoundPos, &_vSoudPos, GetTypeSize<_float3>());
		::memcpy(&m_SoundVelocity, &_vSoundVelocity, GetTypeSize<_float3>());
		m_pChannel->set3DAttributes(&m_SoundPos, &m_SoundVelocity);
	}
	if (nullptr == _spTargetTransform_CanNullptr)
	{
		::memcpy(&m_ListenerPos, &_float3::Zero, GetTypeSize<_float3>());
		::memcpy(&m_ListenerLook, &_float3::Forward, GetTypeSize<_float3>());
		::memcpy(&m_ListenerUp, &_float3::Up, GetTypeSize<_float3>());
	}
	else
	{
		::memcpy(&m_ListenerPos, &_spTargetTransform_CanNullptr->GetPos(), GetTypeSize<_float3>());
		::memcpy(&m_ListenerLook, &_spTargetTransform_CanNullptr->GetLook(), GetTypeSize<_float3>());
		::memcpy(&m_ListenerUp, &_spTargetTransform_CanNullptr->GetUp(), GetTypeSize<_float3>());
	}
	m_pSystem->set3DListenerAttributes(0, &m_ListenerPos, nullptr, &m_ListenerLook, &m_ListenerUp);
	m_pSystem->update();
}

void USound::UpdateSound3D(CSHPTRREF<UTransform> _spSelfTransform, const _float3& _vSoundVelocity, CSHPTRREF<UTransform> _spTargetTransform_CanNullptr)
{
	assert(nullptr != _spSelfTransform);
	UpdateSound3D(_spSelfTransform->GetPos(), _vSoundVelocity, _spTargetTransform_CanNullptr);
}

void USound::ChangeMinMaxDistance3D(const _float _fMinDistance, const _float _fMaxDistance)
{
	m_pSound->set3DMinMaxDistance(_fMinDistance, _fMaxDistance);
}

void USound::Free()
{
	if (nullptr != m_pSound)
		m_pSound->release();
}

/*
=============================================
Sound
=============================================
Channel
=============================================
*/
