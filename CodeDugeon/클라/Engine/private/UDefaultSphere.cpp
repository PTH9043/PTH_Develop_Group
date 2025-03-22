#include "EngineDefine.h"
#include "UDefaultSphere.h"
#include "UTransform.h"
#include "UVIBufferSphere.h"
#include "UGameInstance.h"

UDefaultSphere::UDefaultSphere(CSHPTRREF<UDevice> _spDevice, const _wstring& _wstrLayer, const CLONETYPE& _eCloneType)
	: UPawn(_spDevice, _wstrLayer, _eCloneType, BACKINGTYPE::STATIC),
	m_spVIBufferSphere{ nullptr }
{
}

UDefaultSphere::UDefaultSphere(const UDefaultSphere& _rhs) :
	UPawn(_rhs), m_spVIBufferSphere{ nullptr }
{
}

void UDefaultSphere::Free()
{
}

HRESULT UDefaultSphere::NativeConstruct()
{
	return __super::NativeConstruct();
}

HRESULT UDefaultSphere::NativeConstructClone(const VOIDDATAS& _vecDatas)
{
	if (FAILED(__super::NativeConstructClone(_vecDatas)))
		return E_FAIL;
	SHPTR<UGameInstance> spGameInstance = GET_INSTANCE(UGameInstance);
	m_spVIBufferSphere = static_pointer_cast<UVIBufferSphere>(spGameInstance->CloneResource(PROTO_RES_VIBUFFERNORMALSPHERE));
	// Add Shader 
	AddShader(PROTO_RES_NORMALOBJECTSHADER);
	return S_OK;
}

void UDefaultSphere::TickActive(const _double& _dTimeDelta)
{
}

void UDefaultSphere::LateTickActive(const _double& _dTimeDelta)
{
#ifdef _USE_DEBUGGING
	AddDebugRenderGroup(DEBUG_RI_ETC);
#endif 
}

HRESULT UDefaultSphere::RenderActive(CSHPTRREF<UCommand> _spCommand, CSHPTRREF<UTableDescriptor> _spTableDescriptor)
{
	__super::RenderActive(_spCommand, _spTableDescriptor);
	GetTransform()->BindTransformData(GetShader());
	m_spVIBufferSphere->Render(GetShader(), _spCommand);
	return S_OK;
}

HRESULT UDefaultSphere::RenderShadowActive(CSHPTRREF<UCommand> _spCommand, CSHPTRREF<UTableDescriptor> _spTableDescriptor)
{
	__super::RenderShadowActive(_spCommand, _spTableDescriptor);
	return S_OK;
}

HRESULT UDefaultSphere::RenderOutlineActive(CSHPTRREF<UCommand> _spCommand, CSHPTRREF<UTableDescriptor> _spTableDescriptor, _bool _pass)
{
	return __super::RenderOutlineActive(_spCommand, _spTableDescriptor,_pass);
}

void UDefaultSphere::Collision(CSHPTRREF<UPawn> _pEnemy, const _double& _dTimeDelta)
{
}
