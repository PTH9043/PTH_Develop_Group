#pragma once
#include "CManager.h"

class CSoundMgr : public CManager
{
public:
	explicit CSoundMgr();
	explicit CSoundMgr(CGameMgr*& pGameMgr);
	virtual ~CSoundMgr();
protected:
	// 사운드 리소스 정보를 갖는 객체 
	map<TCHAR*, FMOD_SOUND*> m_mapSound;
	// FMOD_CHANNEL : 재생하고 있는 사운드를 관리할 객체 
	FMOD_CHANNEL* m_pChannelArr[MAXCHANNEL];
	// 사운드 ,채널 객체 및 장치를 관리하는 객체 
	FMOD_SYSTEM* m_pSystem;
public:
	virtual void Initialize() override;
	virtual void Release() override;
	// CManager을(를) 통해 상속됨
	virtual int Update() override { return 0; }
public:
	void PlaySound(TCHAR* pSoundKey, CHANNELID eID, float fVolume);
	void PlayBGM(TCHAR* pSoundKey, float fVolume);
	void StopSound(CHANNELID eID);
	void StopAll();
	void SetChannelVolume(CHANNELID eID, float fVolume);

private:
	void LoadSoundFile(const char* szPath);
};

