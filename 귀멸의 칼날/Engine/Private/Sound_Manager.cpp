#include "Sound_Manager.h"
#include <io.h>
#include <tchar.h>
#include "FilePath_Manager.h"
#include "Method.h"
#include "Transform.h"

IMPLEMENT_SINGLETON(CSound_Manager)

CSound_Manager::CSound_Manager() 
{
}

HRESULT CSound_Manager::Initialize()
{
	FMOD_System_Create(&m_pSystem, FMOD_VERSION);

	FMOD_System_Init(m_pSystem, 32, FMOD_INIT_NORMAL, NULL);

	for (_uint i = 0; i < CH_END; ++i)
		m_pChannelArr[i] = nullptr;

	return S_OK;
}

int CSound_Manager::Volume_Up(const CHANNELID& eID, const _float& _fVol)
{
	if (m_volume < SOUND_MAX)
		m_volume += _fVol;
	else
		m_volume = SOUND_MAX;

	FMOD_Channel_SetVolume(m_pChannelArr[eID], _fVol);
	FMOD_System_Update(m_pSystem);
	return 0;
}

_int CSound_Manager::Volume_Down(const CHANNELID& eID, const _float& _fVol)
{
	_float fCurVol = 0.f;
	if (FMOD_OK == FMOD_Channel_GetVolume(m_pChannelArr[eID], &fCurVol))
	{
		fCurVol -= _fVol;

		if (fCurVol < SOUND_MIN)
			fCurVol = SOUND_MIN;

		FMOD_Channel_SetVolume(m_pChannelArr[eID], fCurVol);
		FMOD_System_Update(m_pSystem);
	}
	return 0;
}

_int CSound_Manager::BGM_Volume_Up(const _float& _fVol)
{
	if (m_BGMvolume < SOUND_MAX)
		m_BGMvolume += _fVol;
	else
		m_BGMvolume = SOUND_MAX;

	FMOD_Channel_SetVolume(m_pChannelArr[BGM], _fVol);
	FMOD_System_Update(m_pSystem);
	m_BGMvolume = _fVol;
	return 0;
}

_int CSound_Manager::BGM_Volume_Down(const _float& _fVol)
{
	/*if (m_BGMvolume > SOUND_MIN)
		m_BGMvolume -= _fVol;
	else
		m_BGMvolume = SOUND_MIN;

	FMOD_Channel_SetVolume(m_pChannelArr[BGM], _fVol);
	FMOD_System_Update(m_pSystem);*/
	Volume_Down(BGM, _fVol);

	return 0;
}

_int CSound_Manager::Pause(const CHANNELID& eID)
{
	FMOD_Channel_SetPaused(m_pChannelArr[eID], TRUE);
	FMOD_System_Update(m_pSystem);
	return 0;
}

_int CSound_Manager::Play_Channel(const CHANNELID& _eID)
{
	FMOD_Channel_SetPaused(m_pChannelArr[_eID], FALSE);
	FMOD_System_Update(m_pSystem);
	return 0;
}

void CSound_Manager::Play_In_Silence(const _tchar* pSoundKey, const _float& _fVol, const _float& _fVolDown)
{
	map<const _tchar*, FMOD_SOUND*>::iterator iter;

	iter = find_if(m_mapSound.begin(), m_mapSound.end(), CTag_Finder(pSoundKey));

	if (iter == m_mapSound.end())
		return;

	FMOD_BOOL bPlay = FALSE;
	//if (FMOD_Channel_IsPlaying(m_pChannelArr[CH_S], &bPlay))
	{
		Stop_Sound(CH_S);
		FMOD_System_PlaySound(m_pSystem, iter->second, nullptr, FALSE, &m_pChannelArr[CH_S]);
		FMOD_Channel_SetVolume(m_pChannelArr[CH_S], _fVol);


		for (_uint iCh = (_uint)CH0; iCh < (_uint)BGM; iCh++)
		{
			Volume_Down((CHANNELID)iCh, _fVolDown);
		}
		m_volumeDown = _fVolDown;
	}
}

void CSound_Manager::Play_Sound_ByCondition(const _tchar* pSoundKey, const _float& _fVol,  CTransform* pTransform)
{
	m_eChannel = static_cast<CHANNELID>(m_eChannel < CHANNELID::CH16 ? static_cast<_ubyte>(m_eChannel) + 1 : static_cast<_ubyte>(CHANNELID::CH0));

	map<const _tchar*, FMOD_SOUND*>::iterator iter;

	iter = find_if(m_mapSound.begin(), m_mapSound.end(), CTag_Finder(pSoundKey));

	if (iter == m_mapSound.end())
		return;

	// CH_S이 플레이 도중에는 볼륨을 낮춰서 실행한다.
	_float fVol = _fVol;
	FMOD_BOOL bCH_SPlay = FALSE;
	FMOD_Channel_IsPlaying(m_pChannelArr[CH_S], &bCH_SPlay);
	if (bCH_SPlay)
	{
		fVol -= m_volumeDown;
		if (fVol < SOUND_MIN)
			fVol = SOUND_MIN;
	}

	for (auto& pChannel : m_pChannelArr)
	{
		FMOD_SOUND* pSound = nullptr;

		FMOD_Channel_GetCurrentSound(pChannel, &pSound);

		if (nullptr != pSound)
		{
			// 만약에 지금 등록하려는 사운드가 이미 등록이 되어있다면 실행중인지 판단 후 실행중이 아닐때만 진행한다.
			if (pSound == iter->second)
			{				
				FMOD_BOOL bPlay = FALSE;
				FMOD_Channel_IsPlaying(pChannel, &bPlay);

				if (TRUE == bPlay)
					return;
				else
				{
					FMOD_System_PlaySound(m_pSystem, iter->second, nullptr, FALSE, &m_pChannelArr[m_eChannel]);
					FMOD_Channel_SetVolume(m_pChannelArr[m_eChannel], fVol);

					// Transform 없으면 그냥 2D 사운드로 실행
					if (nullptr == pTransform)
					{
						FMOD_Channel_SetMode(m_pChannelArr[m_eChannel], FMOD_2D);
					}
					// Transform 있으면 3D사운드로
					else
					{
						FMOD_Channel_SetMode(m_pChannelArr[m_eChannel], FMOD_3D);
						_float4x4 WorldMatrix = pTransform->Get_WorldFloat4x4();
						FMOD_3D_ATTRIBUTES attribute;
						attribute.position = *(FMOD_VECTOR*)&WorldMatrix.m[3][0];
						attribute.forward = *(FMOD_VECTOR*)&WorldMatrix.m[2][0];
						attribute.up = *(FMOD_VECTOR*)&WorldMatrix.m[1][0];
						attribute.velocity = { 0.0f, 0.0f, 0.0f };
						FMOD_Channel_Set3DAttributes(m_pChannelArr[m_eChannel], &attribute.position, &attribute.velocity);
					}
					FMOD_System_Update(m_pSystem);

					return;
				}
			}
		}
	}

	FMOD_BOOL bPlay = FALSE;
	if (FMOD_Channel_IsPlaying(m_pChannelArr[m_eChannel], &bPlay))
	{
		FMOD_System_PlaySound(m_pSystem, iter->second, nullptr, FALSE, &m_pChannelArr[m_eChannel]);
		FMOD_Channel_SetVolume(m_pChannelArr[m_eChannel], fVol);
	}
	// Transform 없으면 그냥 2D 사운드로 실행
	if (nullptr == pTransform)
	{
		FMOD_Channel_SetMode(m_pChannelArr[m_eChannel], FMOD_2D);
	}
	// Transform 있으면 3D사운드로
	else
	{
		FMOD_Channel_SetMode(m_pChannelArr[m_eChannel], FMOD_3D);
		_float4x4 WorldMatrix = pTransform->Get_WorldFloat4x4();
		FMOD_3D_ATTRIBUTES attribute;
		attribute.position = *(FMOD_VECTOR*)&WorldMatrix.m[3][0];
		attribute.forward = *(FMOD_VECTOR*)&WorldMatrix.m[2][0];
		attribute.up = *(FMOD_VECTOR*)&WorldMatrix.m[1][0];
		attribute.velocity = { 0.0f, 0.0f, 0.0f };
		FMOD_Channel_Set3DAttributes(m_pChannelArr[m_eChannel], &attribute.position, &attribute.velocity);
	}
	FMOD_System_Update(m_pSystem);
}

void CSound_Manager::Play_SoundW(const _tchar* pSoundKey, const _float& _fVol, CTransform* pTransform)
{
	m_eChannel = static_cast<CHANNELID>(m_eChannel < CHANNELID::CH16 ? static_cast<_ubyte>(m_eChannel) + 1 : static_cast<_ubyte>(CHANNELID::CH0));

	map<const _tchar*, FMOD_SOUND*>::iterator iter;

	iter = find_if(m_mapSound.begin(), m_mapSound.end(), CTag_Finder(pSoundKey));

	if (iter == m_mapSound.end())
		return;

	// CH_S이 플레이 도중에는 볼륨을 낮춰서 실행한다.
	_float fVol = _fVol;
	FMOD_BOOL bCH_SPlay = FALSE;
	FMOD_Channel_IsPlaying(m_pChannelArr[CH_S], &bCH_SPlay);
	if (bCH_SPlay)
	{
		fVol -= m_volumeDown;
		if (fVol < SOUND_MIN)
			fVol = SOUND_MIN;
	}

	FMOD_BOOL bPlay = FALSE;
	if (FMOD_Channel_IsPlaying(m_pChannelArr[m_eChannel], &bPlay))
	{
		FMOD_System_PlaySound(m_pSystem, iter->second, nullptr, FALSE, &m_pChannelArr[m_eChannel]);
		FMOD_Channel_SetVolume(m_pChannelArr[m_eChannel], fVol);
	}
	// Transform 없으면 그냥 2D 사운드로 실행
	if (nullptr == pTransform)
	{
		FMOD_Channel_SetMode(m_pChannelArr[m_eChannel], FMOD_2D);
	}
	// Transform 있으면 3D사운드로
	else
	{
		FMOD_Channel_SetMode(m_pChannelArr[m_eChannel], FMOD_3D);
		_float4x4 WorldMatrix = pTransform->Get_WorldFloat4x4();
		FMOD_3D_ATTRIBUTES attribute;
		attribute.position = *(FMOD_VECTOR*)&WorldMatrix.m[3][0];
		attribute.forward = *(FMOD_VECTOR*)&WorldMatrix.m[2][0];
		attribute.up = *(FMOD_VECTOR*)&WorldMatrix.m[1][0];
		attribute.velocity = { 0.0f, 0.0f, 0.0f };
		FMOD_Channel_Set3DAttributes(m_pChannelArr[m_eChannel], &attribute.position, &attribute.velocity);
	}
	FMOD_System_Update(m_pSystem);
}

void CSound_Manager::Play_BGM(const _tchar* pSoundKey, const _float& _fVol)
{
	return;

	map<const _tchar*, FMOD_SOUND*>::iterator iter;

	iter = find_if(m_mapSound.begin(), m_mapSound.end(), CTag_Finder(pSoundKey));

	if (iter == m_mapSound.end())
		return;

	if (m_strSaveBGMName == pSoundKey)
		return;

	m_strSaveBGMName = pSoundKey;
	m_pSoundBGM = (*iter).second;
	m_BGMvolume = _fVol;
	FMOD_System_PlaySound(m_pSystem, iter->second, nullptr, FALSE, &m_pChannelArr[CHANNELID::BGM]);
	FMOD_Channel_SetVolume(m_pChannelArr[CHANNELID::BGM], _fVol);
	FMOD_Channel_SetMode(m_pChannelArr[CHANNELID::BGM], FMOD_LOOP_NORMAL);
	FMOD_System_Update(m_pSystem);
}

void CSound_Manager::Stop_Sound(const CHANNELID& eID)
{
	if (BGM == eID)
		m_strSaveBGMName = L"";

	FMOD_Channel_Stop(m_pChannelArr[eID]);
}

void CSound_Manager::Stop_All()
{
	FMOD_Channel_SetMode(m_pChannelArr[CHANNELID::BGM], FMOD_LOOP_OFF);

	for (_ubyte i = 0; i < CH_END; ++i)
	{
		FMOD_Channel_SetVolume(m_pChannelArr[i], 0.f);
		FMOD_Channel_SetPaused(m_pChannelArr[i], TRUE);
		FMOD_Channel_Stop(m_pChannelArr[i]);
	}

	m_strSaveBGMName = L"";
}

HRESULT CSound_Manager::LoadSoundFile(PATHS* _pFolderList)
{
	// 파일 경로를 찾음
	FILEGROUP* pDesc = nullptr;
	CFilePath_Manager* pInstance = GET_INSTANCE(CFilePath_Manager);
	{
		pDesc = pInstance->Find_Folder(_pFolderList);
		if (nullptr == pDesc)
		{
			// 경로 없으면 리턴
			RELEASE_INSTANCE(CFilePath_Manager);
			return E_FAIL;
		}
	}
	RELEASE_INSTANCE(CFilePath_Manager);
	// 파일 경로에 따라 오디오 파일 받아온다.
 	for (auto& iter : pDesc->vecFileDataList)
	{
		FMOD_SOUND* pSound = nullptr;
		string strPath = CMethod::ConvertWToS(iter.strFilePath);		
		FMOD_RESULT eRes = FMOD_System_CreateSound(m_pSystem, strPath.c_str(), FMOD_3D, 0, &pSound);
		if (eRes == FMOD_OK)
		{
			_tchar* strSoundName = new _tchar[MAX_PATH];
			lstrcpy(strSoundName, iter.strFileName.c_str());
			m_mapSound.emplace(strSoundName, pSound);
		}
	}
	FMOD_System_Update(m_pSystem);
	return S_OK;
}

void CSound_Manager::Free()
{
	for (auto& Pair : m_mapSound)
	{
		delete[](Pair.first);
		FMOD_Sound_Release(Pair.second);
	}
	m_mapSound.clear();
	FMOD_System_Release(m_pSystem);
	FMOD_System_Close(m_pSystem);
}
