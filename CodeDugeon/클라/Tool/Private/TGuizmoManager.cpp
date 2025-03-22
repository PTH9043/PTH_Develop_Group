#include "ToolDefines.h"
#include "UGameInstance.h"
#include "UActor.h"
#include "UTransform.h"
#include "UCamera.h"
#include "TGuizmoManager.h"
#include "UPawn.h"
#include "UCollider.h"

TGuizmoManager::TGuizmoManager() :
	m_stGuizmoDesc{}
{
}

void TGuizmoManager::Free()
{
}

HRESULT TGuizmoManager::SetSelectedActor(CSHPTRREF<UPawn> _spActor)
{
	RETURN_CHECK(_spActor == nullptr, E_FAIL)
	if (nullptr != m_stGuizmoDesc.spSelectedActor)
		m_stGuizmoDesc.spSelectedActor.reset();
	m_stGuizmoDesc.spSelectedActor = _spActor;
	return S_OK;
}

HRESULT TGuizmoManager::CalculateGuizmoDatas()
{
	RETURN_CHECK(m_stGuizmoDesc.spSelectedActor == nullptr, E_FAIL)

	SHPTR<UGameInstance> spGameInstance = GET_INSTANCE(UGameInstance);

	m_stGuizmoDesc.vPosition = m_stGuizmoDesc.spSelectedActor->GetTransform()->GetPos();

	m_stGuizmoDesc.vRotation = _float3(
		m_stGuizmoDesc.spSelectedActor->GetTransform()->GetRotationValue().x,
		m_stGuizmoDesc.spSelectedActor->GetTransform()->GetRotationValue().y,
		m_stGuizmoDesc.spSelectedActor->GetTransform()->GetRotationValue().z);

	m_stGuizmoDesc.vScale = m_stGuizmoDesc.spSelectedActor->GetTransform()->GetScale();

	_float4x4 worldmat = m_stGuizmoDesc.spSelectedActor->GetTransform()->GetWorldMatrix();
	_float4x4 viewmat = spGameInstance->GetMainCamViewMatrix();
	_float4x4 projmat = spGameInstance->GetMainCamProjMatrix();

	m_stGuizmoDesc.mtWorld = worldmat;
	m_stGuizmoDesc.mtView = viewmat;
	m_stGuizmoDesc.mtProj = projmat;

	return S_OK;
}

void TGuizmoManager::ConvertXMFLOAT4x4ToFloatArray(const _float4x4& matrix, float* floatArray)
{
	for (int i = 0; i < 4; ++i)
	{
		for (int j = 0; j < 4; ++j)
		{
			floatArray[i * 4 + j] = matrix.m[i][j];
		}
	}
}

void TGuizmoManager::ConvertFloatArrayToXMFLOAT4x4(const float* floatArray, _float4x4& matrix)
{
	for (int i = 0; i < 4; ++i)
	{
		for (int j = 0; j < 4; ++j)
		{
			matrix.m[i][j] = floatArray[i * 4 + j];
		}
	}
}

void TGuizmoManager::EditTransformViaGuizmo()
{
	ImGuizmo::SetOrthographic(false);
	ImGuizmo::BeginFrame();

	static ImGuizmo::MODE mCurrentGizmoMode(ImGuizmo::WORLD);

	CalculateGuizmoDatas();

	if (ImGui::RadioButton("Translate", mCurrentGizmoOperation == ImGuizmo::TRANSLATE))
		mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
	ImGui::SameLine();
	if (ImGui::RadioButton("Rotate", mCurrentGizmoOperation == ImGuizmo::ROTATE))
		mCurrentGizmoOperation = ImGuizmo::ROTATE;
	ImGui::SameLine();
	if (ImGui::RadioButton("Scale", mCurrentGizmoOperation == ImGuizmo::SCALE))
		mCurrentGizmoOperation = ImGuizmo::SCALE;

	if (ImGui::RadioButton("Universal", mCurrentGizmoOperation == ImGuizmo::UNIVERSAL))
		mCurrentGizmoOperation = ImGuizmo::UNIVERSAL;

	float translate[3] = { m_stGuizmoDesc.vPosition.x, m_stGuizmoDesc.vPosition.y , m_stGuizmoDesc.vPosition.z };
	float rotation[3] = { m_stGuizmoDesc.vRotation.x, m_stGuizmoDesc.vRotation.y , m_stGuizmoDesc.vRotation.z };
	float scale[3] = { m_stGuizmoDesc.vScale.x, m_stGuizmoDesc.vScale.y , m_stGuizmoDesc.vScale.z };

	ImGuizmo::DecomposeMatrixToComponents(*m_stGuizmoDesc.mtWorld.m, translate, rotation, scale);
	ImGui::InputFloat3("Tr", translate);
	ImGui::InputFloat3("Rt", rotation);
	ImGui::InputFloat3("Sc", scale);
	ImGuizmo::RecomposeMatrixFromComponents(translate, rotation, scale, *m_stGuizmoDesc.mtWorld.m);

	ImGuiIO& io = ImGui::GetIO();
	float viewManipulateRight = io.DisplaySize.x;
	float viewManipulateTop = 0;
	static ImGuiWindowFlags gizmoWindowFlags = 0;

	ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);

	float cameraView[16];
	float cameraProj[16];
	float World[16];

	ConvertXMFLOAT4x4ToFloatArray(m_stGuizmoDesc.mtView, cameraView);
	ConvertXMFLOAT4x4ToFloatArray(m_stGuizmoDesc.mtProj, cameraProj);
	ConvertXMFLOAT4x4ToFloatArray(m_stGuizmoDesc.mtWorld, World);

	ImGuizmo::Manipulate(cameraView, cameraProj, mCurrentGizmoOperation, mCurrentGizmoMode, World, NULL, NULL);
	ConvertFloatArrayToXMFLOAT4x4(World, m_stGuizmoDesc.mtWorld);
	m_stGuizmoDesc.spSelectedActor->GetTransform()->SetNewWorldMtx(m_stGuizmoDesc.mtWorld);
	for (auto& Collider : m_stGuizmoDesc.spSelectedActor->GetColliderContainer())
		Collider.second->SetTransform(m_stGuizmoDesc.spSelectedActor->GetTransform());

		
}
