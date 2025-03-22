#include "EngineDefine.h"
#include "USceneManager.h"
#include "UGraphicDevice.h"
#include "ULight.h"
#include "UScene.h"
#include "UGameInstance.h"
#include "URenderer.h"
#include "UShader.h"
#include "UCommand.h"
#include "UGlobalConstantBuffer.h"
#include "UPipeLine.h"
#include "URenderTargetManager.h"
#include "URenderTargetGroup.h"
#include "UGpuCommand.h"

USceneManager::USceneManager() :
	m_c_iLightParamSize{ (_uint)sizeof(LIGHTPARAM) },
	m_iLightCamIndex{ 0 },
	m_stLightParam{},
	m_spLightGlobalBuffer{ nullptr },
	m_spCurScene{ nullptr }
{
}

void USceneManager::Free()
{

}

HRESULT USceneManager::ReadySceneManager(UGameInstance* _pGameInstance)
{
	_pGameInstance->GetGlobalConstantBuffer(GLOBAL_LIGHTSTATIC, m_spLightGlobalBuffer);
	return S_OK;
}

void USceneManager::RegisterScene(CSHPTRREF<UScene> _spScene)
{
	RETURN_CHECK(nullptr == _spScene, ;);

	m_spCurScene.reset();

	SHPTR<UGameInstance> spGameInstance = GET_INSTANCE(UGameInstance);
	spGameInstance->ClearOnceTypeData();
	spGameInstance->SetSceneIDToNetController(_spScene->GetSceneID());

	m_spCurScene = _spScene;
	m_spCurScene->LoadSceneData();
}

void USceneManager::RegisterSceneWithOutLoadingData(CSHPTRREF<UScene> _spScene)
{
	RETURN_CHECK(nullptr == _spScene, ;);

	m_spCurScene.reset();

	SHPTR<UGameInstance> spGameInstance = GET_INSTANCE(UGameInstance);
	spGameInstance->ClearOnceTypeData();
	spGameInstance->SetSceneIDToNetController(_spScene->GetSceneID());

	m_spCurScene = _spScene;
	
}

void USceneManager::Tick(const _double& _dTimeDelta)
{
	RETURN_CHECK(nullptr == m_spCurScene, ;);
	m_spCurScene->Tick(_dTimeDelta);
}

void USceneManager::LateTick(const _double& _dTimeDelta)
{
	RETURN_CHECK(nullptr == m_spCurScene, ;);
	m_spCurScene->LateTick(_dTimeDelta);
}

void USceneManager::OutLightControlInfo(LIGHTPARAM& _stLightControl)
{
	_stLightControl = m_stLightParam;
}

void USceneManager::ChangeLightCamIndex(const CAMID& _iID)
{
	m_iLightCamIndex = _iID;
}

void USceneManager::RenderLight(CSHPTRREF<UPipeLine> _spPipeLine, CSHPTRREF<URenderTargetManager> _spRenderTargetManager,
	const TRANSFORMPARAM& _stDefferendParam)
{
	SHPTR<UGameInstance> spGameInstance = GET_INSTANCE(UGameInstance);

	SHPTR<UCommand> spCmdList = static_pointer_cast<UCommand>(spGameInstance->GetGpuCommand());
	{
		SHPTR<URenderTargetGroup> spRenderTargetGroup{ _spRenderTargetManager->FindRenderTargetGroup(RTGROUPID::LIGHTSHADE_DEFFERED) };
		spRenderTargetGroup->WaitResourceToTarget(spCmdList);
		spRenderTargetGroup->ClearRenderTargetView(spCmdList);
		spRenderTargetGroup->OmSetRenderTargets(spCmdList);


		if (nullptr != m_spCurScene)
		{
			//수정사항 2024-04-09
			m_stLightParam.mScreenWorldMatrix = _stDefferendParam.mWorldMatrix;
				//_spPipeLine->GetMainCamWorldMatrix();
			{
				// View Proj
				m_stLightParam.mScreenViewmatrix = _spPipeLine->GetCamViewMatrix(_stDefferendParam.iCamIndex);
				m_stLightParam.mScreenProjMatrix = _spPipeLine->GetCamProjMatrix(_stDefferendParam.iCamIndex);
				// Far
				m_stLightParam.fCamFar = _spPipeLine->GetCamFar(m_iLightCamIndex);
				// View Proj
				m_stLightParam.mViewMatrix = _spPipeLine->GetCamViewMatrix(m_iLightCamIndex);
				m_stLightParam.mProjMatrix = _spPipeLine->GetCamProjMatrix(m_iLightCamIndex);
				//CamPos
				m_stLightParam.vCamPos = _spPipeLine->GetCameraPosition(m_iLightCamIndex);
			}

			m_stLightParam.mScreenViewmatrix = m_stLightParam.mScreenViewmatrix.Transpose();
			m_stLightParam.mScreenProjMatrix = m_stLightParam.mScreenProjMatrix.Transpose();

			m_stLightParam.mViewMatrixInv = m_stLightParam.mViewMatrix;
			m_stLightParam.mProjMatrixInv = m_stLightParam.mProjMatrix;

			m_stLightParam.mViewMatrix = m_stLightParam.mViewMatrix.Transpose();
			m_stLightParam.mProjMatrix = m_stLightParam.mProjMatrix.Transpose();
			// View Proj Inverse
			m_stLightParam.mViewMatrixInv = m_stLightParam.mViewMatrixInv.Invert();
			m_stLightParam.mViewMatrixInv = m_stLightParam.mViewMatrixInv.Transpose();

			m_stLightParam.mProjMatrixInv = m_stLightParam.mProjMatrixInv.Invert();
			m_stLightParam.mProjMatrixInv = m_stLightParam.mProjMatrixInv.Transpose();
			// Setting Grobal 
			{
				m_spLightGlobalBuffer->SettingGlobalData(spCmdList,
					&m_stLightParam, m_c_iLightParamSize); //원래 다른 커맨드 리스트로 처리했음
			}

			m_spCurScene->RenderLights();
		}
		spRenderTargetGroup->WaitTargetToResource(spCmdList);
		
	}
}

HRESULT USceneManager::AddLight(const LIGHTINFO& _stInfo)
{
	RETURN_CHECK(nullptr == m_spCurScene, E_FAIL);
	return m_spCurScene->AddLight(_stInfo);
}

void USceneManager::OutLight(const LIGHTTYPE& _eLightType, const _uint& _iIndex, SHPTR<ULight>& _spLight)
{
	RETURN_CHECK(nullptr == m_spCurScene, ;);
	m_spCurScene->OutLight(_eLightType, _iIndex, _spLight);
}

HRESULT USceneManager::ActiveLIght(const LIGHTTYPE& _eLightType, const _uint& _iIndex, const _bool& _isActive)
{
	RETURN_CHECK(nullptr == m_spCurScene, E_FAIL);
	return m_spCurScene->ActiveLIght(_eLightType, _iIndex, _isActive);
}

HRESULT USceneManager::DeleteLight(const LIGHTTYPE& _eLightType, const _uint& _iIndex)
{
	RETURN_CHECK(nullptr == m_spCurScene, E_FAIL);
	return m_spCurScene->DeleteLight(_eLightType, _iIndex);
}

HRESULT USceneManager::ClearLight()
{
	RETURN_CHECK(nullptr == m_spCurScene, E_FAIL);
	return m_spCurScene->ClearLight();
}