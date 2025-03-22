#pragma once
#include "USound.h"
#include "UResource.h"

BEGIN(Engine)
using SOUNDCONTAINER = VECTOR<SHPTR<USound>>;
using SOUNDORDERS = UNORMAP<_wstring, _int>;
using ACTIVESOUND = UNORSET<SHPTR<USound>>;
using REMOVESOUND = LIST<SHPTR<USound>>;
/*
@ Date: 2024-02-25, Writer: 박태현
@ Explain
- Sound 폴더를 받아와서 Sound 전체를 로드해서 관리하는 클래스이다. 
*/
class UAudioSystem final : public UResource {
public:
	UAudioSystem(CSHPTRREF<UDevice> _spDevice);
	UAudioSystem(const UAudioSystem& _rhs);
	DESTRUCTOR(UAudioSystem)
public: /* get set*/
	const SOUNDCONTAINER& GetSoundContainer() const { return m_SoundContainer; }
	const SOUNDORDERS& GetSoundOrders() const { return m_SoundOrders; }

	CSHPTRREF<USound> GetSound() const { return m_spSound; }
	CSHPTRREF<USound> GetBGMSound() const { return m_spBgmSound; }
public:
	CLONE_THIS(UAudioSystem)
	virtual void Free() override;
public:
	virtual HRESULT NativeConstruct() override;
	HRESULT NativeConstruct(FMOD::System* _pSystem, const _wstring& _wstrSoudFolderPath);
	HRESULT NativeConstruct(FMOD::System* _pSystem, CSHPTRREF<FILEGROUP> _spSoundFileGroup);
	virtual HRESULT NativeConstructClone(const VOIDDATAS& _Datas) override;

	void Tick();
	void TickWithManyChannels();
	_bool Play(const _wstring& _wstrSoundName);
	_bool PlayWithManyChannels(const _wstring& _wstrSoundName);
	_bool Play(const _wstring& _wstrSoundName, const _float _fVolumeUpdate);
	_bool PlayOnce(const _wstring& _wstrSoundName);
	_bool PlayOnceWithManyChannels(const _wstring& _wstrSoundName);
	_bool PlayBGM(const _wstring& _wstrSoundName);
	_bool StopBGM(const _wstring& _wstrSoundName);
	_bool PauseBGM(const _wstring& _wstrSoundName);
	_bool ResumeBGM(const _wstring& _wstrSoundName);
	_bool PlayBGM(const _wstring& _wstrSoundName, const _float  _fVolumeUpdate);
	_bool Stop(const _wstring& _wstrSoundName);
	_bool StopWithManyChannels(const _wstring& _wstrSoundName);
	_bool VolumeUpdate(const _wstring& _wstrSoundName, const _float& _fVolumeUpdate);
	_bool BGMVolumeUpdate(const _wstring& _wstrSoundName, const _float& _fVolumeUpdate);
	_bool Restart(const _wstring& _wstrSoundName);
	_bool RestartOnce(const _wstring& _wstrSoundName);
	_bool Pause(const _wstring& _wstrSoundName);
	_bool Resume(const _wstring& _wstrSoundName);
	_bool SetPlaybackPosition(const _wstring& _wstrSoundName,unsigned int position);
	//unsigned int GetPlaybackPosition(const _wstring& _wstrSoundName);
	_bool SetLooping(const _wstring& _wstrSoundName,bool loop);
	_bool SetPitch(const _wstring& _wstrSoundName,float pitch);
	_bool SetPan(const _wstring& _wstrSoundName,float pan);
	_bool Mute(const _wstring& _wstrSoundName);
	_bool Unmute(const _wstring& _wstrSoundName);
	_bool FadeIn(const _wstring& _wstrSoundName,float fadeDuration);
	_bool FadeOut(const _wstring& _wstrSoundName,float fadeDuration);
	_bool UpdateVolume(const _wstring& _wstrSoundName,const _float _fVolume);
	_bool DisableOncePlay(const _wstring& _wstrSoundName);

	_bool UpdateSound3D(const _wstring& _wstrSoundName ,const _float3& _vSoudPos, const _float3& _vSoundVelocity,
		CSHPTRREF<UTransform> _spTargetTransform_CanNullptr = nullptr);
	_bool UpdateSound3D(const _wstring& _wstrSoundName, CSHPTRREF<UTransform> _spSelfTransform, const _float3& _vSoundVelocity,
		CSHPTRREF<UTransform> _spTargetTransform_CanNullptr = nullptr);
	_bool ChangeMinMaxDistance3D(const _wstring& _wstrSoundName, const _float _fMinDistance, const _float _fMaxDistance);

	SHPTR<USound> BringSound(const _int _Index);
	SHPTR<USound> BringSound(const _wstring& _wstrSoundName);

	void Release();
private:
	void LoadSound(const _wstring& _wstrSoundFolderPath);
	void CreateSound(const _wstring& _wstrSoundPath);
private:
	FMOD::System*			m_pSoundSystem;
	FMOD::Channel*			m_pBGmChannel;
	SOUNDCONTAINER		m_SoundContainer;
	SOUNDORDERS			m_SoundOrders;

	SHPTR<USound>			m_spSound;
	SHPTR<USound>			m_spBgmSound;
};

END