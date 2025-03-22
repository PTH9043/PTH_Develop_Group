#include "AssimpFIleLoad_ Tool.h" 
#include "Animation_Tool.h"
#include "Channel_Tool.h"
#include "Method.h"

CAnimation_Tool::CAnimation_Tool()
{
}

HRESULT CAnimation_Tool::NativeConstruct(aiAnimation* _pAIAnim, class CModel_Tool* _pModel)
{
	strcpy_s(m_szName, _pAIAnim->mName.data);

	m_dDuration = _pAIAnim->mDuration;
	m_dTickPerSecond = _pAIAnim->mTicksPerSecond;

	m_iNumChannels = _pAIAnim->mNumChannels;

	for (_uint i = 0; i < m_iNumChannels; ++i)
	{
		CChannel_Tool* pChannel = CChannel_Tool::Create(_pAIAnim->mChannels[i], _pModel);
		if (nullptr == pChannel)
			return E_FAIL;

		m_Channels.push_back(pChannel);
	}

	return S_OK;
}

HRESULT CAnimation_Tool::NativeConstruct(aiAnimation* _pAIAnim, CModel_Tool* _pModel, const _wstring& _strName)
{
	string str = CMethod::ConvertWToS(_strName);
	strcpy_s(m_szName, str.c_str());

	m_dDuration = _pAIAnim->mDuration;
	m_dTickPerSecond = _pAIAnim->mTicksPerSecond;

	m_iNumChannels = _pAIAnim->mNumChannels;

	for (_uint i = 0; i < m_iNumChannels; ++i)
	{
		CChannel_Tool* pChannel = CChannel_Tool::Create(_pAIAnim->mChannels[i], _pModel);
		if (nullptr == pChannel)
			return E_FAIL;

		m_Channels.push_back(pChannel);
	}

	return S_OK;
}

void CAnimation_Tool::Update_TransformMatrices(const _double& _dTimeDelta)
{
	m_dTimeAcc += m_dTickPerSecond * _dTimeDelta;

	if (m_dTimeAcc >= m_dDuration)
	{
		m_dTimeAcc = 0.0;
		m_bFinished = true;
	}

	for (_uint i = 0; i < m_iNumChannels; ++i)
	{
		m_Channels[i]->Update_TransformationMatrices(m_dTimeAcc);
	}
}

CAnimation_Tool* CAnimation_Tool::Clone_Animation(CModel_Tool* _pModel)
{
	if (nullptr == _pModel)
		return nullptr;

	CAnimation_Tool* pAnimation = new CAnimation_Tool();
	for (auto& iter : m_Channels)
		pAnimation->m_Channels.push_back(iter->Clone_Channel(_pModel));

	pAnimation->m_dDuration = m_dTickPerSecond;
	pAnimation->m_dTickPerSecond = m_dTickPerSecond;
	strcpy_s(pAnimation->m_szName, m_szName);
	pAnimation->m_iNumChannels = m_iNumChannels;
	return pAnimation;
}

void CAnimation_Tool::Get_Data(ANIMDESC& _tDesc)
{
	_tDesc.iNumChannels = m_iNumChannels;
	_tDesc.dTickPerSeconds = m_dTickPerSecond;
	_tDesc.dDuration = m_dDuration;
	string str = m_szName;
	_tDesc.strName.assign(str.begin(), str.end());

	for (auto& iter : m_Channels)
	{
		CHANNELDESC tDesc;
		iter->Get_Data(tDesc);
		_tDesc.vecChannels.push_back(tDesc);
	}
}

CAnimation_Tool* CAnimation_Tool::Create(aiAnimation* _pAIAnim, class CModel_Tool* _pModel)
{
	CAnimation_Tool* pInstance = new CAnimation_Tool();

	if (FAILED(pInstance->NativeConstruct(_pAIAnim, _pModel)))
	{
		MSG_BOX("Failed to Created : CAnimation");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CAnimation_Tool* CAnimation_Tool::Create(aiAnimation* _pAIAnim, CModel_Tool* _pModel, const _wstring& _strName)
{
	CAnimation_Tool* pInstance = new CAnimation_Tool();

	if (FAILED(pInstance->NativeConstruct(_pAIAnim, _pModel, _strName)))
	{
		MSG_BOX("Failed to Created : CAnimation");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CAnimation_Tool::Free()
{
	for (auto& iter : m_Channels)
		Safe_Release(iter);
}
