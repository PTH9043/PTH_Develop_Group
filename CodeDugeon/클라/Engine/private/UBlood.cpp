#include "EngineDefine.h"
#include "UBlood.h"
#include "UGameInstance.h"
#include "UVIBufferRect.h"
#include "UTransform.h"
#include "UShaderConstantBuffer.h"
#include "UShader.h"
#include "UCollider.h"
#include "UMethod.h"
#include "UTexGroup.h"
#include "UTexture.h"

UBlood::UBlood(CSHPTRREF<UDevice> _spDevice,
	const _wstring& _wstrLayer, const CLONETYPE& _eCloneType)
	: UPawn(_spDevice, _wstrLayer, _eCloneType, BACKINGTYPE::NON),
	m_spVIBufferRect{ nullptr }, m_spTransformConstantBuffer{nullptr}, m_spTimerBuffer{nullptr}, _bloodTimer{}
{
}

 UBlood::UBlood(const UBlood& _rhs) :
	UPawn(_rhs), m_spVIBufferRect{ nullptr }, m_spTransformConstantBuffer{ nullptr }, m_spTimerBuffer{ nullptr }, _bloodTimer{  } {}


void UBlood::Free()
{
}

void UBlood::Collision(CSHPTRREF<UPawn> _pEnemy, const _double& _dTimeDelta)
{
}

HRESULT UBlood::NativeConstruct()
{
	return __super::NativeConstruct();
}

HRESULT UBlood::NativeConstructClone(const VOIDDATAS& _vecDatas)
{
	if (FAILED(__super::NativeConstructClone(_vecDatas)))
		return E_FAIL;

	SHPTR<UGameInstance> spGameInstance = GET_INSTANCE(UGameInstance);
	m_spVIBufferRect = static_pointer_cast<UVIBufferRect>(spGameInstance->CloneResource(PROTO_RES_VIBUFFERRECT));


	if (m_spBloodTexGroup == nullptr)m_spBloodTexGroup = static_pointer_cast<UTexGroup>(spGameInstance->CloneResource(PROTO_RES_BLOODTEXTUREGROUP, _vecDatas));

	spGameInstance->GetPreAllocatedConstantBuffer(PREALLOCATED_TRANSFORM, m_spTransformConstantBuffer);
	// Add Shader 
	AddShader(PROTO_RES_BLOODSHADER);
	m_spTimerBuffer = CreateNative<UShaderConstantBuffer>(GetDevice(), CBV_REGISTER::BLOODTIMERBUFFER, sizeof(BLOODTIMER));

	//GetTransform()->SetScale(_float3(40, 35, 1));
	m_stFinalRenderTransformParam.mWorldMatrix._11 = spGameInstance->GetD3DViewport().Width;
	m_stFinalRenderTransformParam.mWorldMatrix._22 = spGameInstance->GetD3DViewport().Height;
	m_stFinalRenderTransformParam.mWorldMatrix = m_stFinalRenderTransformParam.mWorldMatrix.Transpose();
	m_stFinalRenderTransformParam.iCamIndex = 1;
	return S_OK;
}


void UBlood::TickActive(const _double& _dTimeDelta)
{
	if (_bloodTimer.m_fLeftTime > 0.f)_bloodTimer.m_fLeftTime -= _dTimeDelta;

}



void UBlood::LateTickActive(const _double& _dTimeDelta)
{

	AddRenderGroup(RENDERID::RI_2DUILAST);

}

HRESULT UBlood::RenderActive(CSHPTRREF<UCommand> _spCommand, CSHPTRREF<UTableDescriptor> _spTableDescriptor)
{
	__super::RenderActive(_spCommand, _spTableDescriptor);

	//GetTransform()->BindTransformData(GetShader(),1);
	GetShader()->BindCBVBuffer(m_spTransformConstantBuffer, &m_stFinalRenderTransformParam, GetTypeSize<TRANSFORMPARAM>());
	GetShader()->BindSRVBuffer(SRV_REGISTER::T0, m_spBloodTexGroup->GetTexture(L"BloodBackgroung"));
	


	GetShader()->BindCBVBuffer(m_spTimerBuffer, &_bloodTimer, sizeof(BLOODTIMER));


	m_spVIBufferRect->Render(GetShader(), _spCommand);


	return S_OK;
}

HRESULT UBlood::RenderShadowActive(CSHPTRREF<UCommand> _spCommand, CSHPTRREF<UTableDescriptor> _spTableDescriptor)
{
	__super::RenderShadowActive(_spCommand, _spTableDescriptor);

	return S_OK;
}

HRESULT UBlood::RenderOutlineActive(CSHPTRREF<UCommand> _spCommand, CSHPTRREF<UTableDescriptor> _spTableDescriptor, _bool _pass)
{
	return __super::RenderOutlineActive(_spCommand, _spTableDescriptor, _pass);
}

void UBlood::SetColorTexture(const _wstring& TexName)
{
	ColorTextureIndex = m_spBloodTexGroup->GetTextureIndex(TexName);
}

void UBlood::SetColorTexture(_uint _index)
{
	ColorTextureIndex = _index;
}

void UBlood::SetTimer(_float _setTime)
{
	_bloodTimer.m_fFullTime = _setTime;
	_bloodTimer.m_fLeftTime = _setTime;
}

_bool UBlood::CheckTimeOver()
{
	if (_bloodTimer.m_fLeftTime <= 0.00001f) {
		return true;
	}
	else {
		return false;
	}
	
}

