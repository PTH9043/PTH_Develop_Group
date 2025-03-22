#include "EngineDefine.h"
#include "UPipeLine.h"
#include "UGameInstance.h"
#include "UTransform.h"
#include "UCamera.h"
#include "UGlobalConstantBuffer.h"
#include "UGraphicDevice.h"
#include "UFrustomCulling.h"
#include "UCommand.h"
#include "URenderer.h"

UPipeLine::UPipeLine() :
	m_arrCameras{},
	m_spMainCamera{ nullptr },
	m_spViewProjGlobalBuffer{ nullptr },
	m_stViewProjParam{},
	m_RenderCamID{ 0 }
{
}

void UPipeLine::Free()
{
	for (auto& iter : m_arrCameras)
	{
		iter.reset();
	}
}

HRESULT UPipeLine::ReadyPipeLine(UGameInstance* _pGameInstance)
{
	_pGameInstance->GetGlobalConstantBuffer(GLOBAL_VIEWPROJ, m_spViewProjGlobalBuffer);
	return S_OK;
}

void UPipeLine::RegisterCameraInPipeline(CSHPTRREF<UCamera> _spCamera, CAMID& _iCamID,
	const VIEWPROJMATRIX& _stViewProjMatrix, const CAMERATYPE _eCamType)
{
	RETURN_CHECK(nullptr == _spCamera, ;);
	_uint ID = GetID(_eCamType);
	RETURN_CHECK(m_arrCameras.size() <= ID, ;);

	if (MAIN_CAMERA_ID == ID || DEFFERED_CAMERA_ID == ID || SMALL_DEFFERED_CAMERA_ID == ID || SHADOWLIGHT_CAMERA_ID == ID)
	{
		// Main 카메라가 존재하면 바꾼다. 
		if (nullptr != m_arrCameras[MAIN_CAMERA_ID])
		{
			_uint OtherCamID = GetID(CAMERATYPE::OTHER);
			std::swap(m_arrCameras[OtherCamID], m_arrCameras[ID]);
		}
		if (MAIN_CAMERA_ID == ID)
			m_spMainCamera = _spCamera;
	}
	_iCamID = ID;
	m_arrCameras[ID] = _spCamera;

	::memcpy(&m_stViewProjParam.arrViewProjParams[ID], &_stViewProjMatrix, sizeof(VIEWPROJMATRIX));

	m_arrFrustoms[ID] = CreateNative<UFrustomCulling>(m_arrCameras[ID]);
}

void UPipeLine::RemoveCameraInPipeLine(CAMID _CamID)
{
	RETURN_CHECK(nullptr == m_arrCameras[_CamID], ;);
	m_arrFrustoms[_CamID] = nullptr;

	SHPTR<UGameInstance> spGameInstance = GET_INSTANCE(UGameInstance);
	spGameInstance->RemoveActor(m_arrCameras[_CamID]);

	m_arrCameras[_CamID] = nullptr;
	ArrangeToCameraArr();
}

void UPipeLine::FrustomTick()
{
	//m_RenderCamID = MAIN_CAMERA_ID;
	//m_RenderCamList.clear();
	//for (_uint i = 0; i < m_arrFrustoms.size(); ++i)
	//{
	//	if (nullptr != m_arrFrustoms[i])
	//	{
	//		if (true == m_arrCameras[i]->IsFrustomOn())
	//		{
	//			m_arrFrustoms[i]->Tick();
	//		}
	//	}
	//}
}

_bool UPipeLine::IsFrustomContains(const _float3& _vPos, const _float _fRadius, const CAMID& _iCamID)
{
	RETURN_CHECK(nullptr == m_arrFrustoms[_iCamID], false);
	return m_arrFrustoms[_iCamID]->IsContains(_vPos, _fRadius);
}

void UPipeLine::UpdateViewProjMatrix(const _float _fGrobalDeltaTime)
{
	// 글로벌 버퍼인 ViewProj에 값을 모아서 전달한다. 
	for (_uint i = 0; i < m_arrCameras.size(); ++i)
	{
		if (nullptr != m_arrCameras[i])
		{
			m_arrCameras[i]->BindMatrix(m_stViewProjParam.arrViewProjParams[i]);
		}
	}

	m_stViewProjParam.fGrobalDeltaTime = _fGrobalDeltaTime;
}

void UPipeLine::BindViewProjMatrix(CSHPTRREF<UCommand> _spCommand)
{
	m_stViewProjParam.isObjectMotionBlur = UTransform::IsMotionBlurOn();
	m_spViewProjGlobalBuffer->SettingGlobalData(_spCommand, &m_stViewProjParam, VIEWPARAM_SIZE);
}

const _float4x4 UPipeLine::GetMainCamViewMatrix() const
{
	RETURN_CHECK(nullptr == m_spMainCamera, _float4x4::Identity);
	return m_spMainCamera->GetTransform()->GetWorldMatrixInv();
}

//2024-04-09 수정
const _float4x4 UPipeLine::GetMainCamWorldMatrix() const
{
	RETURN_CHECK(nullptr == m_spMainCamera, _float4x4::Identity);
	return m_spMainCamera->GetTransform()->GetWorldMatrix();
}

const _float4x4& UPipeLine::GetMainCamProjMatrix() const
{
	RETURN_CHECK(nullptr == m_spMainCamera, _float4x4::Identity);
	return m_spMainCamera->GetProjMatrix();
}

const _float3& UPipeLine::GetMainCamPosition()
{
	RETURN_CHECK(nullptr == m_spMainCamera, _float3::Zero);
	return m_spMainCamera->GetTransform()->GetPos();
}

 SHPTR<UTransform>UPipeLine::GetMainCamTransform()
{
	RETURN_CHECK(nullptr == m_spMainCamera, nullptr);
	return m_spMainCamera->GetTransform();
}


const _float4x4 UPipeLine::GetCamViewMatrix(const CAMID& _iID) const
{
	RETURN_CHECK(m_arrCameras.size() <= _iID, _float4x4::Identity);
	RETURN_CHECK(nullptr == m_arrCameras[_iID], _float4x4::Identity);
	return m_arrCameras[_iID]->GetTransform()->GetWorldMatrixInv();
}

const _float4x4& UPipeLine::GetCamProjMatrix(const CAMID& _iID) const
{
	RETURN_CHECK(m_arrCameras.size() <= _iID, _float4x4::Identity);
	RETURN_CHECK(nullptr == m_arrCameras[_iID], _float4x4::Identity);
	return m_arrCameras[_iID]->GetProjMatrix();
}

const _float3& UPipeLine::GetCameraPosition(const CAMID& _iID)
{
	RETURN_CHECK(m_arrCameras.size() <= _iID, _float3::Zero);
	RETURN_CHECK(nullptr == m_arrCameras[_iID], _float3::Zero);
	return m_arrCameras[_iID]->GetTransform()->GetPos();
}

const _float UPipeLine::GetCamFar(const CAMID& _iID)
{
	RETURN_CHECK(m_arrCameras.size() <= _iID, 0.f);
	RETURN_CHECK(nullptr == m_arrCameras[_iID], 0.f);
	return m_arrCameras[_iID]->GetCamFar();
}

void UPipeLine::ClearOneTypeCamera()
{
	// Find Once Clone Type
	_ubyte bCloneType = static_cast<_ubyte>(CLONETYPE::CLONE_ONCE);
	for (auto& iter : m_arrCameras)
	{
		if (nullptr != iter)
		{
			if (iter->GetCloneData()->bCloneType == bCloneType)
			{
				iter = nullptr;
			}
		}
	}
	for (auto& iter : m_arrFrustoms)
	{
		if (m_arrFrustoms[DEFFERED_CAMERA_ID] == iter)
			continue;

		iter = nullptr;
	}
}

void UPipeLine::ChangeRenderCamera(CSHPTRREF<UCamera> _spCamera)
{
	RETURN_CHECK(nullptr == _spCamera, ;);
	m_RenderCamID = _spCamera->GetCamID();
}

void UPipeLine::ChangeRenderCamera(CAMID _iID)
{
	m_RenderCamID = _iID;
}

void UPipeLine::AddRenderCamList(CSHPTRREF<UCamera> _spCamera)
{
	RETURN_CHECK(nullptr == _spCamera, ;);
	m_RenderCamList.push_back(_spCamera->GetCamID());
}

void UPipeLine::AddRenderCamList(CAMID _iID)
{
	m_RenderCamList.push_back(_iID);
}

void UPipeLine::ArrangeToCameraArr()
{
	// Set New Array
	CAMERAARR arrNewCamera;
	for (_uint i = 3; i < arrNewCamera.size(); ++i)
	{
		for (auto& iter : m_arrCameras)
		{
			if (nullptr != iter)
			{
				arrNewCamera[i] = iter;
				arrNewCamera[i]->SetCamID(i);
				iter = nullptr;
				break;
			}
		}
	}
	// ChangeNew
	m_arrCameras = arrNewCamera;
}

_uint UPipeLine::GetID(const CAMERATYPE _eCamerType)
{
	_uint iIndex{ MAIN_CAMERA_ID };
	if (_eCamerType != CAMERATYPE::MAIN && _eCamerType != CAMERATYPE::DEFFERED && _eCamerType != CAMERATYPE::SHADOWLIGHT)
	{
		for (_uint i = (_uint)CAMERATYPE::OTHER; i < MAX_CAM_SIZE; ++i)
		{
			if (nullptr == m_arrCameras[i])
				return i;
		}
	}
	else if (_eCamerType == CAMERATYPE::DEFFERED)
	{
		iIndex = DEFFERED_CAMERA_ID;
	}
	else if (_eCamerType == CAMERATYPE::SHADOWLIGHT)
	{
		iIndex = SHADOWLIGHT_CAMERA_ID;
	}
	else
	{
		iIndex = MAIN_CAMERA_ID;
	}
	return iIndex;
}
