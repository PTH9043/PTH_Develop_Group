#include "EngineDefine.h"
#include "UMat.h"
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

UMat::UMat(CSHPTRREF<UDevice> _spDevice,
	const _wstring& _wstrLayer, const CLONETYPE& _eCloneType)
	: UPawn(_spDevice, _wstrLayer, _eCloneType, BACKINGTYPE::NON),
	m_spVIBufferRect{ nullptr }, m_spTimerBuffer{ nullptr }, _bloodTimer{}, ScaleValue{ _float3(1,1,1) }
{
}

UMat::UMat(const UMat& _rhs) :
	UPawn(_rhs), m_spVIBufferRect{ nullptr }, m_spTimerBuffer{ nullptr }, _bloodTimer{}, ScaleValue{ _float3(1,1,1) } {}


void UMat::Free()
{
}

void UMat::Collision(CSHPTRREF<UPawn> _pEnemy, const _double& _dTimeDelta)
{
}

HRESULT UMat::NativeConstruct()
{
	return __super::NativeConstruct();
}

HRESULT UMat::NativeConstructClone(const VOIDDATAS& _vecDatas)
{
	if (FAILED(__super::NativeConstructClone(_vecDatas)))
		return E_FAIL;

	SHPTR<UGameInstance> spGameInstance = GET_INSTANCE(UGameInstance);
	m_spVIBufferRect = static_pointer_cast<UVIBufferRect>(spGameInstance->CloneResource(PROTO_RES_VIBUFFERRECT));


	if (m_spDustTexGroup == nullptr)m_spDustTexGroup = static_pointer_cast<UTexGroup>(spGameInstance->CloneResource(PROTO_RES_MATTEXTUREGROUP));

	m_spTimerBuffer = CreateNative<UShaderConstantBuffer>(GetDevice(), CBV_REGISTER::BLOODTIMERBUFFER, sizeof(BLOODTIMER));

	AddShader(PROTO_RES_MATSHADER);

	ScaleValue = _float3(15, 15, 1);
	GetTransform()->SetScale(ScaleValue);
	GetTransform()->RotateFix(_float3(90, 0, 0));

	return S_OK;
}


void UMat::TickActive(const _double& _dTimeDelta)
{
	if (_bloodTimer.m_fLeftTime > 0.f) {
		_bloodTimer.m_fLeftTime -= _dTimeDelta;
		GetTransform()->RotateTurn(_float3(0, 1, 0), 30* _dTimeDelta);
		ScaleValue.x += 3.5 * _dTimeDelta;
		ScaleValue.y += 2.5 * _dTimeDelta;
	}

}



void UMat::LateTickActive(const _double& _dTimeDelta)
{
	
	AddRenderGroup(RENDERID::RI_NONALPHA_LAST);

}

HRESULT UMat::RenderActive(CSHPTRREF<UCommand> _spCommand, CSHPTRREF<UTableDescriptor> _spTableDescriptor)
{
	__super::RenderActive(_spCommand, _spTableDescriptor);
	SHPTR<UGameInstance> spGameInstance = GET_INSTANCE(UGameInstance);

	GetTransform()->BindTransformData(GetShader());
	GetShader()->BindSRVBuffer(SRV_REGISTER::T0, m_spDustTexGroup->GetTexture(ColorTextureIndex));
	GetShader()->BindCBVBuffer(m_spTimerBuffer, &_bloodTimer, sizeof(BLOODTIMER));


	m_spVIBufferRect->Render(GetShader(), _spCommand);


	return S_OK;
}

HRESULT UMat::RenderShadowActive(CSHPTRREF<UCommand> _spCommand, CSHPTRREF<UTableDescriptor> _spTableDescriptor)
{
	__super::RenderShadowActive(_spCommand, _spTableDescriptor);

	return S_OK;
}

HRESULT UMat::RenderOutlineActive(CSHPTRREF<UCommand> _spCommand, CSHPTRREF<UTableDescriptor> _spTableDescriptor, _bool _pass)
{
	return __super::RenderOutlineActive(_spCommand, _spTableDescriptor, _pass);
}

void UMat::SetColorTexture(const _wstring& TexName)
{
	ColorTextureIndex = m_spDustTexGroup->GetTextureIndex(TexName);
}

void UMat::SetColorTexture(_uint _index)
{
	ColorTextureIndex = _index;
}



void UMat::SetTimer(_float _setTime)
{
	ScaleValue = _float3(15, 15, 1);
	_bloodTimer.m_fFullTime = _setTime;
	_bloodTimer.m_fLeftTime = _setTime;
}

_bool UMat::CheckTimeOver()
{
	if (_bloodTimer.m_fLeftTime <= 0.00001f) {

		return true;
	}
	else {
		return false;
	}

}