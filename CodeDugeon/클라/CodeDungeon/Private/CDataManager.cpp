#include "ClientDefines.h"
#include "UGameInstance.h"
#include "CDataManager.h"
#include "CModelManager.h"
#include "UStageManager.h"
#include "UStage.h"
#include "URegion.h"
#include "UNavigation.h"

CDataManager::CDataManager() : m_spModelManager{ nullptr },
m_spStageManager{nullptr}
{
}

void CDataManager::Free()
{


}

HRESULT CDataManager::NativeConstruct()
{
	m_spModelManager = Create<CModelManager>();
	m_spStageManager = Create<UStageManager>();
	return S_OK;
}

HRESULT CDataManager::Load_Data()
{
	SHPTR<UGameInstance> spGameInstance = GET_INSTANCE(UGameInstance);
	m_spModelManager->CreateModelProtos(spGameInstance, spGameInstance->GetDevice());

	m_spStageManager->Load();
	spGameInstance->AddPrototype(PROTO_NAVI_CAMERA, m_spStageManager->GetStage()->GetRegion(0)->GetNavigation());
	spGameInstance->AddPrototype(PROTO_NAVI_INTERIOR, m_spStageManager->GetStage()->GetRegion(1)->GetNavigation());

	return S_OK;
}
