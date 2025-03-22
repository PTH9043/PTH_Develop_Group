#include "CutScene_Manager.h"
#include "CutSceneModul.h"
#include "GameInstance.h"

IMPLEMENT_SINGLETON(CCutScene_Manager);

CCutScene_Manager::CCutScene_Manager()
{
}

const _bool CCutScene_Manager::Is_PlayCutScene()
{
	if (nullptr == m_pSelectCutSceneModul && false == m_bJudgeMotionBlur)
		return false;

	if ("COMMON_INIT_BATTLE" == m_strCurrentTrigger)
		return false;

	if (true == m_bEndCutScene)
		return false;

	return !m_pSelectCutSceneModul->Is_End() || m_bJudgeMotionBlur;
}

HRESULT CCutScene_Manager::NativeConstruct(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	m_pDevice = _pDevice;
	m_pContext = _pContext;

	Safe_AddRef_DV(m_pDevice);
	Safe_AddRef_DV(m_pContext);

	{
		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
		{
			PATHS Path = { L"CutScene",  L"Data" };
			FILEGROUP* pFileGroup = pGameInstance->Find_Folder(&Path);
			if (nullptr != pFileGroup)
			{
				Add_CutScene(pFileGroup->FindData(L"Common_Init_Battle"));
			}
		}
		RELEASE_INSTANCE(CGameInstance);
	}

	if (nullptr != m_CutSceneModulMap["COMMON_INIT_BATTLE"])
	{
		m_CutSceneModulMap["COMMON_INIT_BATTLE"]->Set_NotReceivePosition(true);
	}
#ifdef _DEBUG 
	m_tCutTimer = CUSTIMER(10.f);
#else
	m_tCutTimer = CUSTIMER(0.1f);
#endif
	return S_OK;
}

void CCutScene_Manager::Tick(const _double& _dTimeDelta)
{
	if (nullptr != m_pSelectCutSceneModul)
		m_pSelectCutSceneModul->Tick(_dTimeDelta);
}

void CCutScene_Manager::Late_Tick(const _double& _dTimeDelta)
{
	if (nullptr != m_pSelectCutSceneModul)
	{
		m_pSelectCutSceneModul->Late_Tick(_dTimeDelta);
		if (true == m_pSelectCutSceneModul->Is_End())
		{
			if (false == m_bEndCutScene)
				m_pSelectCutSceneModul = nullptr;
		}
	}

	if (LEVEL_BATTLE == g_iCurrentLevel)
	{
		if (false == m_bInit)
		{
			CCutScene_Manager* pInstance = GET_INSTANCE(CCutScene_Manager);
			{
				if (m_tCutTimer.Is_Over(_dTimeDelta))
				{
					pInstance->Active_CutScene("COMMON_INIT_BATTLE");
					m_bInit = true;
				}
			}
			RELEASE_INSTANCE(CCutScene_Manager);
		}
//		m_bInit = true;
	}
}

void CCutScene_Manager::Render()
{
	if (nullptr != m_pSelectCutSceneModul)
	{
		m_pSelectCutSceneModul->Render_Texture();
	}
}

void CCutScene_Manager::Set_CutSceneModul(CCutSceneModul* _pModul)
{
	if (nullptr != m_pSelectCutSceneModul) return;

	if (nullptr != _pModul)
	{
		if ("BATTLE_END" == _pModul->Get_InputTrigger() || "TANJIRO_TUTORIAL_END" == _pModul->Get_InputTrigger())
		{
			m_bEndCutScene = true;
			_pModul->Set_NotChangeCutScene(true);
		}
	}

	this->m_pSelectCutSceneModul = _pModul;
	if(nullptr != m_pSelectCutSceneModul)
		m_pSelectCutSceneModul->Active_CutScene();
}

void CCutScene_Manager::Texture_Load(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	for (auto& iter : m_TextureMap)
		Safe_Release(iter.second);

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	{
		PATHS Path = { L"CutScene", L"TexGroup" };
		FILEGROUP* pFileGroup = pGameInstance->Find_Folder(&Path);
		if (nullptr != pFileGroup)
		{
			for (auto& iter : pFileGroup->lsUnderList)
			{
				m_TextureMap.emplace(pair<_wstring, CTexture*>(iter->strUppderName, CTexture::Create(_pDevice, _pContext, iter)));
			}
		}
	}
	RELEASE_INSTANCE(CGameInstance);
}

CTexture* CCutScene_Manager::Get_TextureToMap(const _wstring& _strName)
{
	CTexture* pTexture = nullptr;
	for (auto& iter : m_TextureMap)
	{
		if (iter.first == _strName)
			pTexture = iter.second;
	}
	return pTexture;
}

void CCutScene_Manager::Active_CutScene(const _string& _strInput)
{
	auto& iter = m_CutSceneModulMap.find(_strInput);
	if (iter == m_CutSceneModulMap.end())
		return;

	Set_CutSceneModul((*iter).second);

	m_strCurrentTrigger = iter->first;
}

void CCutScene_Manager::Update_Battle(CCharacter* _p1, CCharacter* _p2)
{
	for (auto& iter : m_CutSceneModulMap)
		iter.second->Start_CutScene(_p1, _p2);
}
HRESULT CCutScene_Manager::Add_CutScene(FILEDATA* _pFileData)
{
	CCutSceneModul* pInstance = CCutSceneModul::Create(m_pDevice, m_pContext);

	if (nullptr != pInstance)
	{
		if (FAILED(pInstance->Load_Data(_pFileData)))
			return E_FAIL;

		_string str = pInstance->Get_InputTrigger();
		m_CutSceneModulMap.emplace(pair<_string, CCutSceneModul*>(str, pInstance));
	}

	return S_OK;
}

void CCutScene_Manager::Free()
{
	for (auto& iter : m_TextureMap)
		Safe_Release(iter.second);

	for (auto& iter : m_CutSceneModulMap)
		Safe_Release(iter.second);

	m_pSelectCutSceneModul = nullptr;

	Safe_Release_DV(m_pDevice);
	Safe_Release_DV(m_pContext);
}
