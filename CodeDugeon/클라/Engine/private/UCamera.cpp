#include "EngineDefine.h"
#include "UCamera.h"
#include "UGameInstance.h"
#include "UMethod.h"
#include "UTransform.h"

UCamera::UCamera(CSHPTRREF<UDevice> _spDevice, const _wstring& _wstrLayer, const CLONETYPE& _eCloneType) :
	UActor(_spDevice, _wstrLayer, _eCloneType, BACKINGTYPE::NON),
	m_mPrevViewMatrix{ _float4x4::Identity },
	m_mProjMatrix{ _float4x4::Identity },
	m_eProjType{ PROJECTION_TYPE::PROJ_END },
	m_stCamValue{},
	m_stCamProj{},
	m_iCamID{ 0 },
	m_isFrustomOn{ false }
{
}

UCamera::UCamera(const UCamera& _rhs) : UActor(_rhs),
m_mPrevViewMatrix{ _float4x4::Identity },
m_mProjMatrix{ _float4x4::Identity },
m_eProjType{ PROJECTION_TYPE::PROJ_END },
m_stCamValue{},
m_stCamProj{},
m_iCamID{ 0 },
m_isFrustomOn{ false }
{
}

void UCamera::Free()
{
}

HRESULT UCamera::NativeConstruct()
{
	return __super::NativeConstruct();
}

HRESULT UCamera::NativeConstructClone(const VOIDDATAS& _vecDatas)
{
	RETURN_CHECK(_vecDatas.size() <= 0, E_FAIL);
	RETURN_CHECK_FAILED(__super::NativeConstructClone(_vecDatas), E_FAIL);

	CAMDESC stCamDesc = UMethod::ConvertTemplate_Index<CAMDESC>(_vecDatas, VOIDDATAS_ID::CAMDESC_ID);
	m_stCamProj = stCamDesc.stCamProj;
	m_stCamValue = stCamDesc.stCamValue;

	SHPTR<UGameInstance> spGameInstance = GET_INSTANCE(UGameInstance);
	VIEWPROJMATRIX ViewProjMatrix;
	ViewProjMatrix.fCamFar = m_stCamProj.fFar;
	spGameInstance->RegisterCameraInPipeline(ThisShared<UCamera>(), m_iCamID, ViewProjMatrix, stCamDesc.eCamType);

	if (m_iCamID == MAIN_CAMERA_ID)
	{
		m_isFrustomOn = true;
	}

	return MakeProjMatrix(m_stCamProj);
}

HRESULT UCamera::BindMatrix(VIEWPROJMATRIX& _stViewProjMatrix)
{
	_stViewProjMatrix.mPrevViewMatrix = _stViewProjMatrix.mViewMatrix;
	_stViewProjMatrix.mViewMatrix = GetTransform()->GetWorldMatrixInv();
	_stViewProjMatrix.mViewMatrix = _stViewProjMatrix.mViewMatrix.Transpose();
	_stViewProjMatrix.mProjectionMatrix = m_mProjMatrix.Transpose();
	_stViewProjMatrix.vCamPosition = GetTransform()->GetPos();
	return S_OK;
}

void UCamera::TickActive(const _double& _dTimeDelta)
{
}

void UCamera::LateTickActive(const _double& _dTimeDelta)
{
}

HRESULT UCamera::RenderActive(CSHPTRREF<UCommand> _spCommand, CSHPTRREF<UTableDescriptor> _spTableDescriptor)
{
	return S_OK;
}
HRESULT UCamera::RenderShadowActive(CSHPTRREF<UCommand> _spCommand, CSHPTRREF<UTableDescriptor> _spTableDescriptor)
{
	return S_OK;
}
HRESULT UCamera::RenderOutlineActive(CSHPTRREF<UCommand> _spCommand, CSHPTRREF<UTableDescriptor> _spTableDescriptor, _bool _pass)
{
	return S_OK;
}
HRESULT UCamera::MakeProjMatrix(const CAMPROJ& _stCamProj)
{
	_float fWidth = static_cast<_float>(_stCamProj.fWidth);
	_float fHeight = static_cast<_float>(_stCamProj.fHeight);
	switch (_stCamProj.eProjType)
	{
	case PROJECTION_TYPE::PERSPECTIVE:
		m_mProjMatrix = DirectX::XMMatrixPerspectiveFovLH(_stCamProj.fFovy, fWidth / fHeight, _stCamProj.fNear, _stCamProj.fFar);
		break;
	case PROJECTION_TYPE::ORTHOGRAPHIC:
		m_mProjMatrix = DirectX::XMMatrixOrthographicLH(fWidth * _stCamProj.fScale, fHeight * _stCamProj.fScale, _stCamProj.fNear,
			_stCamProj.fFar);
		break;
	}


	return S_OK;
}

//typedef struct tagCameraProj
//{
//	tagCameraProj() = default;
//	tagCameraProj(const PROJECTION_TYPE& _eProjType, const _float3& _v3Eye, const _float3& _v3At,
//		const _float& _fForv, const _float& _fWidth, const _float& _fHeight, const _float& _fNear = 0.f, const _float& _fFar = 1000.f,
//		const _float& _fScale = 1.f)
//		: eProjType(_eProjType), v3Eye(_v3Eye), v3At(_v3At), fFovy(_fForv), fWidth(_fWidth), fHeight(_fHeight), fNear(_fNear), fFar(_fFar),
//		fScale(_fScale)
//	{}
//
//	PROJECTION_TYPE		eProjType = PROJECTION_TYPE::PROJ_END;
//	_float3 v3Eye = _float3(0.f, 0.f, 0.f), v3At = _float3(0.f, 0.f, 0.f);
//	_float fFovy = 0.f, fWidth = 0.f, fHeight = 0.f, fNear = 0.f, fFar = 1000.f;
//	_float fScale = 1.f;
//}CAMPROJ;
#ifdef _USE_IMGUI
void UCamera::ShowObjectInfo()
{
	__super::ShowObjectInfo();

	static const char* CamName{ "CameraInfo" };
	static const char* ORTHOGRAPHIC{ "OrthoGraphic" };
	static const char* PERSPECTIVE{ "Perspective" };
	static const char* CAM_SPEED{ "CamSpeed" };
	static const char* CAM_ROTSPEED{ "CamRotSpeed" };
	static const char* CAMERA_ID{ "CameraID: %d" };
	static const char* CAM_FOV{ "CameraFov" };
	static const char* CAM_NEAR{ "CamNear" };
	static const char* CAM_FAR{ "CamFar" };
	static const char* PROJECTION_INFO{ "ProjectionInfo" };
	static const char* FRUSTOM_ON{ "FrustomOn" };

	if (ImGui::TreeNode(CamName))
	{
		if (m_eProjType == PROJECTION_TYPE::PERSPECTIVE)
		{
			ImGui::Text(PERSPECTIVE);
		}
		else
		{
			ImGui::Text(ORTHOGRAPHIC);
		}
		ImGui::DragFloat(CAM_SPEED, &m_stCamValue.fCamMoveSpeed, 0.1f);
		ImGui::DragFloat(CAM_ROTSPEED, &m_stCamValue.fCamRotSpeed, 0.1f);
		ImGui::Text(CAMERA_ID, m_iCamID);
		if (ImGui::TreeNode(PROJECTION_INFO))
		{
			ImGui::DragFloat(CAM_FOV, &m_stCamProj.fFovy);
			ImGui::DragFloat(CAM_NEAR, &m_stCamProj.fNear);
			ImGui::DragFloat(CAM_FAR, &m_stCamProj.fFar);

			if (ImGui::Button("ChangeProj"))
			{
				MakeProjMatrix(m_stCamProj);
			}
			ImGui::TreePop();
		}
		// Frustom 
		ImGui::Checkbox(FRUSTOM_ON, &m_isFrustomOn);
		ImGui::TreePop();
	}
}
#endif