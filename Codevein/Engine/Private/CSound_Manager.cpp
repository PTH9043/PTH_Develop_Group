#include "CSound_Manager.h"
#include <io.h>
#include <tchar.h>
#include"CFile_Manager.h"

IMPLEMENT_SINGLETON(CSound_Manager)

CSound_Manager::CSound_Manager()
{
}

HRESULT CSound_Manager::Initialize()
{
	System_Create(&m_pSystem);

	m_pSystem->init(32, FMOD_INIT_NORMAL, nullptr);

	for (_uint i = 0; i < MAXCHANNEL; ++i)
		m_pChannelArr[i] = nullptr;

	return S_OK;
}

int CSound_Manager::Volume_Up(const CHANNELID& eID, const _float& _fVol)
{
	if (m_volume < SOUND_MAX)
		m_volume += _fVol;
	else
		m_volume = SOUND_MAX;

	m_pChannelArr[eID]->setVolume(m_volume);

	return 0;
}

_int CSound_Manager::Volume_Down(const CHANNELID& eID, const _float& _fVol)
{
	if (m_volume > SOUND_MIN)
		m_volume -= _fVol;
	else
		m_volume = SOUND_MIN;

	m_pChannelArr[eID]->setVolume(m_volume);

	return 0;
}

_int CSound_Manager::BGM_Volume_Up(const _float& _fVol)
{
	if (m_BGMvolume < SOUND_MAX)
		m_BGMvolume += _fVol;
	else
		m_BGMvolume = SOUND_MAX;

	m_pChannelArr[BGM]->setVolume(m_BGMvolume);

	return 0;
}

_int CSound_Manager::BGM_Volume_Down(const _float& _fVol)
{
	if (m_BGMvolume > SOUND_MIN)
		m_BGMvolume -= _fVol;
	else
		m_BGMvolume = SOUND_MIN;

	m_pChannelArr[BGM]->setVolume(m_BGMvolume);

	return 0;
}

_int CSound_Manager::Pause(const CHANNELID& eID)
{
	m_bPause = !m_bPause;
	m_pChannelArr[eID]->setPaused(m_bPause);
	return 0;
}

void CSound_Manager::Play_SoundW(const _tchar* pSoundKey, const CHANNELID& eID, const _float& _fVol)
{
	map<const _tchar*, Sound*>::iterator iter;

	iter = find_if(m_mapSound.begin(), m_mapSound.end(), CTag_Finder(pSoundKey));

	if (iter == m_mapSound.end())
		return;

	_bool bPlay = FALSE;
	if (m_pChannelArr[eID]->isPlaying(&bPlay))
	{
		m_pSystem->playSound(iter->second, 0, false, &m_pChannelArr[eID]);
		if (_fVol >= SOUND_MAX)
			m_pChannelArr[eID]->setVolume(1.f);
		else if (_fVol <= SOUND_MIN)
			m_pChannelArr[eID]->setVolume(0.f);
	}
	m_pSystem->update();
}

void CSound_Manager::Play_BGM(const _tchar* pSoundKey)
{
	map<const _tchar*, Sound*>::iterator iter;

	iter = find_if(m_mapSound.begin(), m_mapSound.end(), CTag_Finder(pSoundKey));

	if (iter == m_mapSound.end())
		return;

	m_pSystem->playSound(iter->second, 0, false, &m_pChannelArr[BGM]);
	m_pChannelArr[BGM]->setMode(FMOD_LOOP_NORMAL);
	m_pSystem->update();
}

void CSound_Manager::Stop_Sound(const CHANNELID& eID)
{
	m_pChannelArr[eID]->stop();
}

void CSound_Manager::Stop_All()
{
	for (int i = 0; i < MAXCHANNEL; ++i)
		m_pChannelArr[i]->stop();
}

HRESULT CSound_Manager::LoadSoundFile(PATHS* _pFolderList)
{
	FILEGROUP* pDesc = nullptr;
	BEGIN_SINGLETON(CFile_Manager, pFileManager);
	{
		pDesc = pFileManager->Find_Folder(_pFolderList);
		if (nullptr == pDesc)
		{
			END_SINGLETON(pFileManager);
			return E_FAIL;
		}
	}
	END_SINGLETON(pFileManager);

	for(auto& iter : pDesc->vecFileDataList)
	{
		Sound* pSound = nullptr;
		string strPath;
		strPath.assign(iter.strFilePath.begin(), iter.strFilePath.end());
		FMOD_RESULT eRes = m_pSystem->createSound(strPath.c_str(), FMOD_DEFAULT, 0, &pSound);
		if (eRes == FMOD_OK)
		{
			m_mapSound.emplace(iter.strFileName.c_str(), pSound);
		}
	}
	m_pSystem->update();
	return S_OK;
}

void CSound_Manager::Free()
{
	for (auto& Mypair : m_mapSound)
	{
		Mypair.second->release();
	}
	m_mapSound.clear();

	m_pSystem->close();
	m_pSystem->release();
}
