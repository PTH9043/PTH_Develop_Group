#pragma once
#include "FMode\\fmod.hpp"
#include "FMode\\fmod_errors.h"
#include "Base.h"
#include "Engine_Defines.h"

#define SOUND_MAX 1.0f
#define SOUND_MIN 0.0f
#define SOUND_DEFAULT 0.5f
#define SOUND_WEIGHT 0.1f

using namespace FMOD;

BEGIN(Engine)

class CSound_Manager : public CBase
{
	DECLARE_SINGLETON(CSound_Manager)
public:
	enum CHANNELID { CH0, CH1, CH2, CH3, CH4, CH5, CH6, CH7, CH8, CH9, CH10, CH11, CH12, CH13, CH14, CH15, CH16, BGM, CH_S, CH_END };
protected:
	explicit CSound_Manager();
	virtual ~CSound_Manager() = default;
public:
	map<const _tchar*, FMOD_SOUND*>* Get_SoundMap() { return &m_mapSound; }
public:
	HRESULT Initialize();
public:
	_int  Volume_Up(const CHANNELID& eID, const _float& _fVol);
	_int  Volume_Down(const CHANNELID& eID, const _float& _fVol);
	_int  BGM_Volume_Up(const _float& _fVol);
	_int  BGM_Volume_Down(const _float& _fVol);
	_int  Pause(const CHANNELID& eID);
	_int  Play_Channel(const CHANNELID& _eID);
	void Play_In_Silence(const _tchar* pSoundKey, const _float& _fVol, const _float& _fVolDown);
	void Play_Sound_ByCondition(const _tchar* pSoundKey, const _float& _fVol, class CTransform* pTransform = nullptr);
	void Play_SoundW(const _tchar* pSoundKey, const _float& _fVol, class CTransform* pTransform = nullptr);
	void Play_BGM(const _tchar* pSoundKey, const _float& _fVol = 0.5f);
	void Stop_Sound(const CHANNELID& eID);
	void Stop_All();

	HRESULT LoadSoundFile(PATHS* _pFolderList);
private:
	float m_volume = SOUND_DEFAULT;
	float m_BGMvolume = SOUND_DEFAULT;
	float m_volumeDown = 0.f;
	_bool m_bool;
private:
	// 사운드 리소스 정보를 갖는 객체 
	map<const _tchar*, FMOD_SOUND*> m_mapSound;
	// FMOD_CHANNEL : 재생하고 있는 사운드를 관리할 객체 
	FMOD_CHANNEL*		m_pChannelArr[CH_END];
	// 현재 채널
	CHANNELID					m_eChannel = CHANNELID::CH0;
	// 사운드 ,채널 객체 및 장치를 관리하는 객체 
	FMOD_SYSTEM*			m_pSystem;
	_bool								m_bPause = false;
	FMOD_SOUND*			m_pSoundBGM = nullptr;
	_wstring						m_strSaveBGMName = L"";
public:
	virtual void Free() override;
};

END