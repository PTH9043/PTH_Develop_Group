#include "EngineDefine.h"
#include "UDust.h"
#include "UGameInstance.h"
#include "UVIBufferRect.h"
#include "UTransform.h"
#include "UShaderConstantBuffer.h"
#include "UShader.h"
#include "UCollider.h"
#include "UMethod.h"
#include "UTexGroup.h"
#include "UTexture.h"
#include "URenderTargetGroup.h"

UDust::UDust(CSHPTRREF<UDevice> _spDevice,
	const _wstring& _wstrLayer, const CLONETYPE& _eCloneType)
	: UPawn(_spDevice, _wstrLayer, _eCloneType, BACKINGTYPE::NON),
	m_spVIBufferRect{ nullptr }, m_spTimerBuffer{ nullptr }, _bloodTimer{}, ScaleValue{_float3(1,1,1)}
{
}

UDust::UDust(const UDust& _rhs) :
	UPawn(_rhs), m_spVIBufferRect{ nullptr }, m_spTimerBuffer{ nullptr }, _bloodTimer{}, ScaleValue{ _float3(1,1,1) } {}


void UDust::Free()
{
}

void UDust::Collision(CSHPTRREF<UPawn> _pEnemy, const _double& _dTimeDelta)
{
}

HRESULT UDust::NativeConstruct()
{
	return __super::NativeConstruct();
}

HRESULT UDust::NativeConstructClone(const VOIDDATAS& _vecDatas)
{
	if (FAILED(__super::NativeConstructClone(_vecDatas)))
		return E_FAIL;

	SHPTR<UGameInstance> spGameInstance = GET_INSTANCE(UGameInstance);
	m_spVIBufferRect = static_pointer_cast<UVIBufferRect>(spGameInstance->CloneResource(PROTO_RES_VIBUFFERRECT));
	

	if (m_spDustTexGroup == nullptr)m_spDustTexGroup = static_pointer_cast<UTexGroup>(spGameInstance->CloneResource(PROTO_RES_PARTICLETEXTUREGROUP));

	m_spTimerBuffer = CreateNative<UShaderConstantBuffer>(GetDevice(), CBV_REGISTER::BLOODTIMERBUFFER, sizeof(BLOODTIMER));

	AddShader(PROTO_RES_DUSTSHADER);

	ScaleValue = _float3(2.5,1.25, 1);

	return S_OK;
}


void UDust::TickActive(const _double& _dTimeDelta)
{

	if (_bloodTimer.m_fLeftTime > 0.f) {
		_bloodTimer.m_fLeftTime -= _dTimeDelta;
		ScaleValue.x += 3.5 * _dTimeDelta;
		ScaleValue.y += 2.5 * _dTimeDelta;
	}
	{
		SHPTR<UGameInstance> spGameInstance = GET_INSTANCE(UGameInstance);
		_float3 MainCamPos = spGameInstance->GetMainCameraTransform()->GetPos();
		//GetTransform()->LookAtWithFixedUp(_float3(MainCamPos));
		GetTransform()->LookAtWithFixedUp(_float3(MainCamPos));
	}
}



void UDust::LateTickActive(const _double& _dTimeDelta)
{
	GetTransform()->SetScale(ScaleValue);
	AddRenderGroup(RENDERID::RI_NONALPHA_LAST);

}

HRESULT UDust::RenderActive(CSHPTRREF<UCommand> _spCommand, CSHPTRREF<UTableDescriptor> _spTableDescriptor)
{
	__super::RenderActive(_spCommand, _spTableDescriptor);
	SHPTR<UGameInstance> spGameInstance = GET_INSTANCE(UGameInstance);
	SHPTR<URenderTargetGroup> spShadowDepthGroup{ spGameInstance->FindRenderTargetGroup(RTGROUPID::NONALPHA_DEFFERED) };

	GetTransform()->BindTransformData(GetShader());
	GetShader()->BindSRVBuffer(SRV_REGISTER::T0, m_spDustTexGroup->GetTexture(L"Sand"));
	GetShader()->BindSRVBuffer(SRV_REGISTER::T1, spShadowDepthGroup->GetRenderTargetTexture(RTOBJID::NONALPHA_DEPTH_DEFFERED));
	GetShader()->BindCBVBuffer(m_spTimerBuffer, &_bloodTimer, sizeof(BLOODTIMER));


	m_spVIBufferRect->Render(GetShader(), _spCommand);


	return S_OK;
}

HRESULT UDust::RenderShadowActive(CSHPTRREF<UCommand> _spCommand, CSHPTRREF<UTableDescriptor> _spTableDescriptor)
{
	__super::RenderShadowActive(_spCommand, _spTableDescriptor);

	return S_OK;
}

HRESULT UDust::RenderOutlineActive(CSHPTRREF<UCommand> _spCommand, CSHPTRREF<UTableDescriptor> _spTableDescriptor, _bool _pass)
{
	return __super::RenderOutlineActive(_spCommand, _spTableDescriptor, _pass);
}

void UDust::SetColorTexture(const _wstring& TexName)
{
	ColorTextureIndex = m_spDustTexGroup->GetTextureIndex(TexName);
}

void UDust::SetColorTexture(_uint _index)
{
	ColorTextureIndex = _index;
}



void UDust::SetTimer(_float _setTime)
{
	ScaleValue = _float3(2.5, 1.25, 1);
	_bloodTimer.m_fFullTime = _setTime;
	_bloodTimer.m_fLeftTime = _setTime;
}

_bool UDust::CheckTimeOver()
{
	if (_bloodTimer.m_fLeftTime <= 0.00001f) {
		
		return true;
	}
	else {
		return false;
	}

}