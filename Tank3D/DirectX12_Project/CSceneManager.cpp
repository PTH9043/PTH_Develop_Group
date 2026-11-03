#include "CSceneManager.h"
#include "CScene.h"
#include "CGameInstance.h"

void CSceneManager::Free(){
	Safe_Free(m_pCurScene);
}

void CSceneManager::RegisterScene(const shared_ptr<CScene>& _pScene){

	RETURN_CHECK(nullptr == _pScene, ;);

	shared_ptr<CGameInstance> pGameInstance = GET_INSTNACE(CGameInstance);
	pGameInstance->Clear();

	Safe_Free(m_pCurScene);
	m_pCurScene = _pScene;
	m_pCurScene->LoadSceneData();
}

void CSceneManager::Tick(const _double& _dTimeDelta){
	RETURN_CHECK(nullptr == m_pCurScene, ;);
	m_pCurScene->Tick(_dTimeDelta);
}

void CSceneManager::Late_Tick(const _double& _dTimeDelta){
	RETURN_CHECK(nullptr == m_pCurScene, ;);
	m_pCurScene->Late_Tick(_dTimeDelta);
}
