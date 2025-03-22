#pragma once
#include "fmod.hpp"
#include "fmod_errors.h"
#include "CBase.h"
#include "Engine_Defines.h"

#define SOUND_MAX 1.0f
#define SOUND_MIN 0.0f
#define SOUND_DEFAULT 0.5f
#define SOUND_WEIGHT 0.1f

using namespace FMOD;

class CSound_Manager : public CBase
{
	DECLARE_SINGLETON(CSound_Manager)
public:
	enum CHANNELID { BGM, PLAYER, EFFECT, UI, SYSTEM_EFFECT, SYSTEM_EFFECT2, SYSTEM_EFFECT3, MAXCHANNEL };
protected:
	explicit CSound_Manager();
	virtual ~CSound_Manager() = default;
public:
	map<const _tchar*, Sound*>* Get_SoundMap() { return &m_mapSound; }
public:
	HRESULT Initialize();
public:
	_int  Volume_Up(const CHANNELID& eID, const _float& _fVol);
	_int  Volume_Down(const CHANNELID& eID, const _float& _fVol);
	_int  BGM_Volume_Up(const _float& _fVol);
	_int  BGM_Volume_Down(const _float& _fVol);
	_int  Pause(const CHANNELID& eID);
	void Play_SoundW(const _tchar* pSoundKey, const CHANNELID& eID, const _float& _fVol);
	void Play_BGM(const _tchar* pSoundKey);
	void Stop_Sound(const CHANNELID& eID);
	void Stop_All();

	HRESULT LoadSoundFile(PATHS* _pFolderList);
private:
	float m_volume = SOUND_DEFAULT;
	float m_BGMvolume = SOUND_DEFAULT;
	_bool m_bool;
private:
	// 사운드 리소스 정보를 갖는 객체 
	map<const _tchar*, Sound*> m_mapSound;
	// FMOD_CHANNEL : 재생하고 있는 사운드를 관리할 객체 
	Channel*		m_pChannelArr[MAXCHANNEL];
	// 사운드 ,채널 객체 및 장치를 관리하는 객체 
	System*		m_pSystem;
	_bool				m_bPause = false;
public:
	virtual void Free() override;
};

