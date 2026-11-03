#include "CScene.h"
#include "CCubeMonster.h"
#include "CGameInstance.h"

void CScene::Free()
{
}

HRESULT CScene::NativeConstruct()
{
	return S_OK;
}

HRESULT CScene::LoadSceneData()
{
	return S_OK;
}

void CScene::Tick(const _double& _dTimeDelta){

}

void CScene::Late_Tick(const _double& _dTimeDelta){

}