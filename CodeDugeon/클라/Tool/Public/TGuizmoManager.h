#pragma once
#include "UBase.h"

BEGIN(Engine)
class UGameInstance;
class UActor;
class UPawn;
END


BEGIN(Tool)


/*
@ Date: 2024-04-01, Writer: 이성현
@ Explain
- Guizmo를 보여주고 모델과 상호작용을 위한 클래스
*/

static ImGuizmo::OPERATION mCurrentGizmoOperation(ImGuizmo::TRANSLATE);

struct GUIZMODESC {
	GUIZMODESC() = default;
	GUIZMODESC(CSHPTRREF<UPawn> _spActor, const _float3& _vPosition, const _float3& _vRotation, const _float3& _vScale
	, const _float4x4& _mtWorld, const _float4x4& _mtView, const _float4x4& _mtProj)
		: spSelectedActor(_spActor), vPosition(_vPosition), vRotation(_vRotation), vScale(_vScale),
		mtWorld(_mtWorld), mtView(_mtView), mtProj(_mtProj)
	{}
	SHPTR<UPawn>	spSelectedActor{ nullptr };
	_float3			vPosition{ 0, 0, 0 };
	_float3			vRotation{ 0, 0, 0 };
	_float3			vScale{ 0, 0, 0 };
	
	_float4x4		mtWorld{};
	_float4x4		mtView{};
	_float4x4		mtProj{};
};

class TGuizmoManager : public UBase
{
public:
	TGuizmoManager();
	NO_COPY(TGuizmoManager)
	DESTRUCTOR(TGuizmoManager)
public:
	virtual void Free() override;
	HRESULT SetSelectedActor(CSHPTRREF<UPawn> _spActor);
	HRESULT CalculateGuizmoDatas();

	void ConvertXMFLOAT4x4ToFloatArray(const _float4x4& matrix, float* floatArray);
	void ConvertFloatArrayToXMFLOAT4x4(const float* floatArray, _float4x4& matrix);

	void EditTransformViaGuizmo();

private:
	GUIZMODESC			m_stGuizmoDesc;

};

END