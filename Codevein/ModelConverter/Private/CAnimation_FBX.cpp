#include "CAnimation_FBX.h"
#include "CChannel_FBX.h"

CAnimation_FBX::CAnimation_FBX()
{
}

void CAnimation_FBX::Get_Data(ANIMDESC& _tDesc)
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

HRESULT CAnimation_FBX::NativeConstruct(aiAnimation* _pAIAnim, class CModel_FBX* _pModel)
{
	strcpy_s(m_szName, _pAIAnim->mName.data);

	m_dDuration = _pAIAnim->mDuration;
	m_dTickPerSecond = _pAIAnim->mTicksPerSecond;

	m_iNumChannels = _pAIAnim->mNumChannels;

	for (_uint i = 0; i < m_iNumChannels; ++i)
	{
		CChannel_FBX* pChannel = CChannel_FBX::Create(_pAIAnim->mChannels[i], _pModel);
		if (nullptr == pChannel)
			return E_FAIL;

		m_Channels.push_back(pChannel);
	}

	return S_OK;
}

HRESULT CAnimation_FBX::NativeConstruct(aiAnimation* _pAIAnim, CModel_FBX* _pModel, const _wstring& _strName)
{
	string str;
	str.assign(_strName.begin(), _strName.end());
	strcpy_s(m_szName, str.c_str());

	m_dDuration = _pAIAnim->mDuration;
	m_dTickPerSecond = _pAIAnim->mTicksPerSecond;

	m_iNumChannels = _pAIAnim->mNumChannels;

	for (_uint i = 0; i < m_iNumChannels; ++i)
	{
		CChannel_FBX* pChannel = CChannel_FBX::Create(_pAIAnim->mChannels[i], _pModel);
		if (nullptr == pChannel)
			return E_FAIL;

		m_Channels.push_back(pChannel);
	}

	return S_OK;
}

void CAnimation_FBX::Update_TransformMatrices(const _double& _dTimeDelta)
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

CAnimation_FBX* CAnimation_FBX::Clone_Animation(CModel_FBX* _pModel)
{
	if (nullptr == _pModel)
		return nullptr;

	CAnimation_FBX* pAnimation = new CAnimation_FBX();
	for (auto& iter : m_Channels)
		pAnimation->m_Channels.push_back(iter->Clone_Channel(_pModel));

	pAnimation->m_dDuration = m_dTickPerSecond;
	pAnimation->m_dTickPerSecond = m_dTickPerSecond;
	strcpy_s(pAnimation->m_szName, m_szName);
	pAnimation->m_iNumChannels = m_iNumChannels;
	return pAnimation;
}

CAnimation_FBX* CAnimation_FBX::Create(aiAnimation* _pAIAnim, class CModel_FBX* _pModel)
{
	CAnimation_FBX* pInstance = new CAnimation_FBX();

	if (FAILED(pInstance->NativeConstruct(_pAIAnim, _pModel)))
	{
		MSG_BOX("Failed to Created : CAnimation");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CAnimation_FBX* CAnimation_FBX::Create(aiAnimation* _pAIAnim, CModel_FBX* _pModel, const _wstring& _strName)
{
	CAnimation_FBX* pInstance = new CAnimation_FBX();

	if (FAILED(pInstance->NativeConstruct(_pAIAnim, _pModel, _strName)))
	{
		MSG_BOX("Failed to Created : CAnimation");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CAnimation_FBX::Free()
{
	for (auto& iter : m_Channels)
		Safe_Release(iter);
}
