
#include "EngineDefine.h"
#include "UGuard.h"
#include "UGameInstance.h"
#include "UVIBufferRect.h"
#include "UTransform.h"
#include "UShaderConstantBuffer.h"
#include "UShader.h"
#include "UCollider.h"
#include "UMethod.h"
#include "UTexGroup.h"
#include "UTexture.h"
#include "UVIBufferSphere.h"


UGuard::UGuard(CSHPTRREF<UDevice> _spDevice,
	const _wstring& _wstrLayer, const CLONETYPE& _eCloneType)
	: UPawn(_spDevice, _wstrLayer, _eCloneType, BACKINGTYPE::NON, PAWNTYPE::PAWN_ECT),
	m_spVIBufferRect{ nullptr }
{
}

UGuard::UGuard(const UGuard& _rhs) :
	UPawn(_rhs), m_spVIBufferRect{ nullptr } {}


void UGuard::Free()
{
}

void UGuard::Collision(CSHPTRREF<UPawn> _pEnemy, const _double& _dTimeDelta)
{
}

HRESULT UGuard::NativeConstruct()
{
	return __super::NativeConstruct();
}

HRESULT UGuard::NativeConstructClone(const VOIDDATAS& _vecDatas)
{
	if (FAILED(__super::NativeConstructClone(_vecDatas)))
		return E_FAIL;
	GUARDDESC guardDesc = UMethod::ConvertTemplate_Index<GUARDDESC>(_vecDatas, 0);

	m_GuardType = guardDesc.GuardType;

	SHPTR<UGameInstance> spGameInstance = GET_INSTANCE(UGameInstance);

	if(m_GuardType == TYPE_RECT)
		m_spVIBufferRect = static_pointer_cast<UVIBufferRect>(spGameInstance->CloneResource(PROTO_RES_VIBUFFERRECT));
	else
		m_spVIBufferSphere = static_pointer_cast<UVIBufferSphere>(spGameInstance->CloneResource(PROTO_RES_VIBUFFERSPHERE));

	if (m_spGuardTexGroup == nullptr)m_spGuardTexGroup = static_pointer_cast<UTexGroup>(spGameInstance->CloneResource(PROTO_RES_GUARDTEXTUREGROUP, _vecDatas));

	if (m_GuardType == TYPE_RECT)
	{
		UCollider::COLLIDERDESC tDesc;
		tDesc.vTranslation = _float3(0.f, 0.f, 0.f);
		tDesc.vScale = _float3(40, 40, 0);
		SHPTR<UCollider> Collider1 = static_pointer_cast<UCollider>(spGameInstance->CloneComp(PROTO_COMP_OBBCOLLIDER, { &tDesc }));
		_wstring mainColliderTag = L"Main";
		AddColliderInContainer(mainColliderTag, Collider1);

		UCollider::COLLIDERDESC tDesc2;
		tDesc2.vTranslation = _float3(0.f, 0.f, 0.f);
		tDesc2.vScale = _float3(40, 40, 10);
		SHPTR<UCollider> Collider2 = static_pointer_cast<UCollider>(spGameInstance->CloneComp(PROTO_COMP_OBBCOLLIDER, { &tDesc2 }));
		_wstring subColliderTag = L"ForInteractionGuard";
		AddColliderInContainer(subColliderTag, Collider2);
	}


	/*SetPawnType(PAWNTYPE::PAWN_STATICOBJ);*/

	// Add Shader 
	AddShader(PROTO_RES_GUARDSHADER);
	return S_OK;
}


void UGuard::TickActive(const _double& _dTimeDelta)
{
	for (auto& Containers : GetColliderContainer())
	{
		if (m_GuardType == TYPE_RECT)
			Containers.second->SetTransform(GetTransform()->GetPos(), GetTransform()->GetQuaternion());
	}
}



void UGuard::LateTickActive(const _double& _dTimeDelta)
{
	AddRenderGroup(RENDERID::RI_NONALPHA_LAST);
}

HRESULT UGuard::RenderActive(CSHPTRREF<UCommand> _spCommand, CSHPTRREF<UTableDescriptor> _spTableDescriptor)
{
	__super::RenderActive(_spCommand, _spTableDescriptor);

	GetTransform()->BindTransformData(GetShader());
	GetShader()->BindSRVBuffer(SRV_REGISTER::T0, m_spGuardTexGroup->GetTexture(ColorTextureIndex));

	GetShader()->BindSRVBuffer(SRV_REGISTER::T1, m_spGuardTexGroup->GetTexture(L"FireNoise"));

	GetShader()->BindSRVBuffer(SRV_REGISTER::T2, m_spGuardTexGroup->GetTexture(L"alpha01"));

	if (m_GuardType == TYPE_RECT)
		m_spVIBufferRect->Render(GetShader(), _spCommand);
	else
		m_spVIBufferSphere->Render(GetShader(), _spCommand);

	return S_OK;
}

HRESULT UGuard::RenderShadowActive(CSHPTRREF<UCommand> _spCommand, CSHPTRREF<UTableDescriptor> _spTableDescriptor)
{
	__super::RenderShadowActive(_spCommand, _spTableDescriptor);

	return S_OK;
}

HRESULT UGuard::RenderOutlineActive(CSHPTRREF<UCommand> _spCommand, CSHPTRREF<UTableDescriptor> _spTableDescriptor, _bool _pass)
{
	return __super::RenderOutlineActive(_spCommand, _spTableDescriptor, _pass);
}

void UGuard::SetColorTexture(const _wstring& TexName)
{
	ColorTextureIndex = m_spGuardTexGroup->GetTextureIndex(TexName);
}

void UGuard::SetColorTexture(_uint _index)
{
	ColorTextureIndex = _index;
}

