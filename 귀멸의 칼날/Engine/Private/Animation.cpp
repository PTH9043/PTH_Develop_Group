#include "Animation.h"
#include "Channel.h"
#include "Method.h"
#include<direct.h>

CAnimation::CAnimation()
{
}

void CAnimation::Set_RootBoneMatrix(_fmatrix _RootBoneMatrix)
{
	m_RootBoneMatrix = CMethod::XMMatrixStoreFloat4x4_P(_RootBoneMatrix);
}

_bool CAnimation::Get_AnimTimeSituation(const _float& _divineValue)
{
	return m_dTimeAcc > ((m_dDuration) / _divineValue);
}

_bool CAnimation::Is_BetweenTime(const _double& _dTime, const _double& _dEndTime)
{
	if (m_dTimeAcc >= _dTime && m_dTimeAcc <= _dEndTime)
		return true;

	return false;
}

_bool CAnimation::Is_BetweenTime_FastTime(const _uint& _iIndex)
{
	if (m_AnimFastSection.size() > _iIndex)
	{
		if (true == m_AnimFastSection[_iIndex].Pass(m_dTimeAcc))
			return true;
	}

	return false;
}

_bool CAnimation::Is_BetweenTime_Clip(const _char* _pName)
{
	for (auto& iter : m_AnimClipSection)
	{
		if (iter.Is_Between(_pName, m_dTimeAcc))
			return true;
	}
	return false;
}

_bool CAnimation::Is_OverTime(const _double& _dTime)
{
	if (m_dTimeAcc >= _dTime)
		return true;
	return false;
}

_bool CAnimation::Is_OverTime_AnimClip(const _char* _pName, const _double& _dSupTime)
{
	for (auto& iter : m_AnimClipSection)
	{
		if (iter.Pass(_pName, m_dTimeAcc, _dSupTime))
			return true;
	}
	return false;
}

void CAnimation::Reset_Data()
{
	m_dTimeAcc = 0.0;
	m_fSupplementValue = 0.001f;
	m_bSupplementSituation = false;
	m_bStopAnimation = false;
	m_fFastValue = 1.f;
	m_fSuppleFastValue = 1.f;
	for (_uint i = 0; i < m_iNumChannels; ++i)
		m_Channels[i]->Reset_KeyFrames(0);
}

void CAnimation::Reset_Data_ClipAnimation()
{
	m_fSupplementValue = 0.001f;
	m_bSupplementSituation = false;
	m_bStopAnimation = false;
	//m_fFastValue = 1.f;
	m_fSuppleFastValue = 1.f;
}

void CAnimation::ReMove_FastSection(const ANIMFASTSECTION& _tAnimFastSection)
{
	for (vector<ANIMFASTSECTION>::iterator it = m_AnimFastSection.begin(); it != m_AnimFastSection.end(); ++it)
	{
		if ((*it).fStartSpot == _tAnimFastSection.fStartSpot && (*it).fEndSpot == _tAnimFastSection.fEndSpot
			&& (*it).fFastValue == _tAnimFastSection.fFastValue)
		{
			m_AnimFastSection.erase(it);
			break;
		}
	}
}

void CAnimation::Remove_AnimClipSection(const ANIMCLIPSECTION& _tAnimClipSection)
{
	for (vector<ANIMCLIPSECTION>::iterator it = m_AnimClipSection.begin(); it != m_AnimClipSection.end(); ++it)
	{
		if (!strcmp(_tAnimClipSection.strName.c_str(), (*it).strName.c_str()) && _tAnimClipSection.fChange == (*it).fChange)
		{
			m_AnimClipSection.erase(it);
			break;
		}
	}
}

HRESULT CAnimation::NativeConstruct(CModel* _pModel, const ANIMDESC& _tAnimDesc)
{
	m_strName = CMethod::ConvertWToS(_tAnimDesc.strName);
	m_iNumChannels = _tAnimDesc.iNumChannels;
	m_dDuration = _tAnimDesc.dDuration;
	m_dTickPerSeconds = _tAnimDesc.dTickPerSeconds;
	for (auto& iter : _tAnimDesc.vecChannels)
	{
		CChannel* pChannel = CChannel::Create(_pModel, iter);
		if (nullptr == pChannel)
			return E_FAIL;

		m_Channels.push_back(pChannel);
	}

	return S_OK;
}

void CAnimation::Update_TransformMatrices(const _double& _dTimeDelta)
{
	if (false == m_bStopAnimation)
	{
		_double fValue = 0.;
		fValue = m_dTickPerSeconds * _dTimeDelta * m_fFastValue;
		for (auto& iter : m_AnimFastSection)
		{
			fValue = iter.Convert(m_dTimeAcc, fValue);
		}
		m_dTimeAcc += fValue;
	}
	if (m_dTimeAcc >= m_dDuration)
	{
		m_dTimeAcc = 0.0;
		m_fSupplementValue = 0.001f;
		m_bFinished = true;
		m_bSupplementSituation = true;		
		m_fFastValue = 1.f;
	}
	else
	{
		m_bFinished = false;
		for (_uint i = 0; i < m_iNumChannels; ++i)
		{
			m_Channels[i]->Update_TransformationMatrices(m_dTimeAcc, this);
		}
	}
}

void CAnimation::Update_TransformMatrices(const _double& _dTimeDelta, const _bool& bIsLoop)
{
	if (false == m_bStopAnimation)
	{
		_double fValue = 0.;
		fValue = m_dTickPerSeconds * _dTimeDelta * m_fFastValue;
		for (auto& iter : m_AnimFastSection)
		{
			fValue = iter.Convert(m_dTimeAcc, fValue);
		}
		m_dTimeAcc += fValue;
	}
	if (m_dTimeAcc >= m_dDuration)
	{
		m_fSupplementValue = 0.001f;
		m_bFinished = true;
		m_bSupplementSituation = true;
		m_dTimeAcc = 0.0;
		//if(false == bIsLoop)
		//	m_fFastValue = 1.f;
	}
	else
	{
		m_bFinished = false;
		for (_uint i = 0; i < m_iNumChannels; ++i)
		{
			m_Channels[i]->Update_TransformationMatrices(m_dTimeAcc, this);
		}
	}
}

void CAnimation::Update_TransformMatrices()
{
	for (_uint i = 0; i < m_iNumChannels; ++i)
	{
		m_Channels[i]->Set_KeyFrame(m_Channels[i]->Compute_CurKeyFrame(m_dTimeAcc));
		m_Channels[i]->Reset_TransformMatrix();
	}
}

void CAnimation::Update_SubTransformMatrices(const _double& _dTimeDelta)
{
	if (false == m_bStopAnimation)
	{
		_double fValue = 0.;
		fValue = m_dTickPerSeconds * _dTimeDelta * m_fFastValue;
		for (auto& iter : m_AnimFastSection)
		{
			fValue = iter.Convert(m_dTimeAcc, fValue);
		}
		m_dTimeAcc += fValue;
	}
	if (m_dTimeAcc >= m_dDuration)
	{
		m_dTimeAcc = 0.0;
		m_fSupplementValue = 0.001f;
		m_bFinished = true;
		m_bSupplementSituation = true;
		m_fFastValue = 1.f;
	}
	else
	{
		m_bFinished = false;
		for (_uint i = 0; i < m_iNumChannels; ++i)
		{
			if (2 < m_Channels[i]->Get_NumMaxKeyFrames())
				m_Channels[i]->Update_TransformationMatrices(m_dTimeAcc, this);
		}
	}
}


void CAnimation::Updatet_NextAnim_TransformMatrix(const _double& _dTimeDelta, CAnimation* _pNextAnim, const _bool& _bIsSupplement)
{
	if (nullptr == _pNextAnim)
		return;

	if (m_iNumChannels != (_uint)_pNextAnim->Get_Channels().size())
	{
		Reset_Data();
		return;
	}

	m_fSupplementValue += (_float)((_dTimeDelta)) * 5.f * m_fSuppleFastValue;

	if (m_fSupplementValue >= 1.f)
	{
		m_bSupplementSituation = false;
		Reset_Data();
	}
	else
	{
		for (_uint i = 0; i < m_iNumChannels; ++i)
			m_Channels[i]->Supplement_NextAnim_TransformMatrix(m_dTimeAcc, _pNextAnim->Get_Channel(i),
				m_fSupplementValue, this, _bIsSupplement);

		m_bSupplementSituation = true;
	}
}

void CAnimation::Update_NextAnimClip_TransformMatrix(const _double& _dTimeDelta, const _double& _dNextAnimTime, const _double& _dLerpSpeed, 
	CAnimation* _pNextAnim, const _bool& _bIsSupplement)
{
	if (nullptr == _pNextAnim)
		return;

	if (m_iNumChannels != (_uint)_pNextAnim->Get_Channels().size())
	{
		Reset_Data();
		return;
	}

	m_fSupplementValue += (_float)((_dTimeDelta)) * 5.f * (_float)(_dLerpSpeed);

	if (m_fSupplementValue >= 1.f)
	{
		m_bSupplementSituation = false;
		Reset_Data_ClipAnimation();
	}
	else
	{
		// 다음 애니메이션의 보간할 부분부터 보간한다.
		for (_uint i = 0; i < m_iNumChannels; ++i)
			m_Channels[i]->Supplement_NextAnim_TransformMatrix(_dNextAnimTime, _pNextAnim->Get_Channel(i),
				m_fSupplementValue, this, _bIsSupplement);

		m_bSupplementSituation = true;
	}
}

CAnimation* CAnimation::Clone_Animation(CModel* _pModel)
{
	if (nullptr == _pModel)
		return nullptr;

	CAnimation* pAnimation = new CAnimation();
	for (auto& iter : m_Channels)
		pAnimation->m_Channels.push_back(iter->Clone_Channel(_pModel));

	pAnimation->m_dDuration = m_dDuration;
	pAnimation->m_dTickPerSeconds = m_dTickPerSeconds;
	pAnimation->m_strName = m_strName;
	pAnimation->m_iNumChannels = m_iNumChannels;
	pAnimation->m_AnimFastSection = m_AnimFastSection;
	pAnimation->m_AnimClipSection = m_AnimClipSection;
	return pAnimation;
}

void CAnimation::Save_AnimationFastSection(FILEGROUP* _pFileGroup)
{
	if (0 == (_uint)m_AnimFastSection.size())
		return;

	if (nullptr == _pFileGroup)
		return;

	FILEGROUP* pFileGroup = _pFileGroup->Find_Group(L"Animations_FBX");
	if (nullptr == pFileGroup)
		return;

	_wstring strFilePath = pFileGroup->strPath;
	_wstring strName;
	strName.assign(m_strName.begin(), m_strName.end());
	strFilePath.append(L"\\");
	strFilePath.append(L"FastSection");

	_wmkdir(strFilePath.c_str());

	strFilePath.append(L"\\");
	strFilePath.append(strName);
	strFilePath.append(L".bin");

	HANDLE hFile = CreateFile(strFilePath.c_str(), GENERIC_WRITE, NULL,
		NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	if (INVALID_HANDLE_VALUE == hFile)
	{
		CloseHandle(hFile);
		return;
	}

	DWORD dwByte = 0;
	_uint iLength = (_uint)m_AnimFastSection.size();
	WriteFile(hFile, &iLength, sizeof(_uint), &dwByte, 0);
	for (auto& iter : m_AnimFastSection)
	{
		WriteFile(hFile, &iter, sizeof(ANIMFASTSECTION), &dwByte, 0);
	}
	CloseHandle(hFile);
}

void CAnimation::Load_AnimationFastSection(FILEGROUP* _pFileGroup)
{
	if (nullptr == _pFileGroup)
		return;

	_wstring strFilePath = _pFileGroup->strPath;
	_wstring strName;
	strName.assign(m_strName.begin(), m_strName.end());
	strFilePath.append(L"\\");
	strFilePath.append(L"FastSection");
	strFilePath.append(L"\\");
	strFilePath.append(strName);
	strFilePath.append(L".bin");
	HANDLE hFile = CreateFile(strFilePath.c_str(), GENERIC_READ, NULL,
		NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if (INVALID_HANDLE_VALUE == hFile)
	{
		CloseHandle(hFile);
		return;
	}
	DWORD dwByte = 0;
	_uint iLength = 0;
	ReadFile(hFile, &iLength, sizeof(_uint), &dwByte, nullptr);
	for (_uint i = 0; i < iLength; ++i)
	{
		ANIMFASTSECTION tAnimFastSetion;
		ReadFile(hFile, &tAnimFastSetion, sizeof(ANIMFASTSECTION), &dwByte, 0);
		m_AnimFastSection.push_back(tAnimFastSetion);
	}
	CloseHandle(hFile);
}

void CAnimation::Save_AnimationClipSection(FILEGROUP* _pFileGroup)
{
	if (0 == (_uint)m_AnimClipSection.size())
		return;

	if (nullptr == _pFileGroup)
		return;

	FILEGROUP* pFileGroup = _pFileGroup->Find_Group(L"Animations_FBX");
	if (nullptr == pFileGroup)
		return;

	_wstring strFilePath = pFileGroup->strPath;
	_wstring strName;
	strName.assign(m_strName.begin(), m_strName.end());
	strFilePath.append(L"\\");
	strFilePath.append(L"ClipSection");

	_int iResult = _wmkdir(strFilePath.c_str());


	strFilePath.append(L"\\");
	strFilePath.append(strName);
	strFilePath.append(L".bin");

	HANDLE hFile = CreateFile(strFilePath.c_str(), GENERIC_WRITE, NULL,
		NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	if (INVALID_HANDLE_VALUE == hFile)
	{
		CloseHandle(hFile);
		return;
	}

	DWORD dwByte = 0;
	_uint iLength = (_uint)m_AnimClipSection.size();
	WriteFile(hFile, &iLength, sizeof(_uint), &dwByte, 0);
	for (auto& iter : m_AnimClipSection)
	{
		WriteFile(hFile, &iter.fChange, sizeof(_float), &dwByte, 0);
		WriteFile(hFile, &iter.fEndValue, sizeof(_float), &dwByte, 0);
		{
			_uint iLength = (_uint)iter.strName.size() + 1;
			WriteFile(hFile, &iLength, sizeof(_uint), &dwByte, 0);
			WriteFile(hFile, iter.strName.c_str(), sizeof(_char) * iLength, &dwByte, 0);
		}
	}
	CloseHandle(hFile);
}

void CAnimation::Load_AnimationClipSection(FILEGROUP* _pFileGroup)
{
	if (nullptr == _pFileGroup)
		return;

	_wstring strFilePath = _pFileGroup->strPath;
	_wstring strName;
	strName.assign(m_strName.begin(), m_strName.end());
	strFilePath.append(L"\\");
	strFilePath.append(L"ClipSection");
	strFilePath.append(L"\\");
	strFilePath.append(strName);
	strFilePath.append(L".bin");
	HANDLE hFile = CreateFile(strFilePath.c_str(), GENERIC_READ, NULL,
		NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if (INVALID_HANDLE_VALUE == hFile)
	{
		CloseHandle(hFile);
		return;
	}
	DWORD dwByte = 0;
	_uint iLength = 0;
	ReadFile(hFile, &iLength, sizeof(_uint), &dwByte, nullptr);
	for (_uint i = 0; i < iLength; ++i)
	{
		ANIMCLIPSECTION tAnimClipSection;
		ReadFile(hFile, &tAnimClipSection.fChange, sizeof(_float), &dwByte, 0);
		ReadFile(hFile, &tAnimClipSection.fEndValue, sizeof(_float), &dwByte, 0);
		{
			_uint iLength = 0;
			ReadFile(hFile, &iLength, sizeof(_uint), &dwByte, 0);
			_char* pName = new _char[iLength];
			ReadFile(hFile, pName, sizeof(_char) * iLength, &dwByte, 0);
			tAnimClipSection.strName = pName;
			Safe_Delete_Array(pName);
		}
		m_AnimClipSection.push_back(tAnimClipSection);
	}
	CloseHandle(hFile);
}

void CAnimation::Save_Data(const HANDLE& _hFile, DWORD& _dwByte)
{
	WriteFile(_hFile, &m_iNumChannels, sizeof(_uint), &_dwByte, 0);
	WriteFile(_hFile, &m_dTickPerSeconds, sizeof(_double), &_dwByte, 0);
	WriteFile(_hFile, &m_dDuration, sizeof(_double), &_dwByte, 0);
	_uint Length = (_uint)strlen(m_strName.c_str()) + 1;
	WriteFile(_hFile, &Length, sizeof(_uint), &_dwByte, 0);
	WriteFile(_hFile, m_strName.c_str(), sizeof(_char) * Length, &_dwByte, 0);
	for (auto& iter : m_Channels) {
		iter->Save_Data(_hFile, _dwByte);
	}
}

CAnimation* CAnimation::Create(CModel* _pModel, const ANIMDESC& _tAnimDesc)
{
	CAnimation* pInstance = new CAnimation();

	if (FAILED(pInstance->NativeConstruct(_pModel, _tAnimDesc)))
	{
		MSG_BOX("Failed to Created : CAnimation");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CAnimation::Free()
{
	for (auto& iter : m_Channels)
		Safe_Release(iter);
}
