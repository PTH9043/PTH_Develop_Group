#include "EngineDefine.h"
#include "UDefaultDebugging.h"
#include "UVIBuffer.h"
#include "UTransform.h"
#include "UGameInstance.h"
#include "UMethod.h"
#include "UShaderConstantBuffer.h"
#include "UShader.h"

UDefaultDebugging::UDefaultDebugging(CSHPTRREF<UDevice> _spDevice, const _wstring& _wstrLayer, const CLONETYPE& _eCloneType)
	: UPawn(_spDevice, _wstrLayer, _eCloneType, BACKINGTYPE::NON),
	m_spVIBuffer{ nullptr },
	m_eDebugType{ DEBUGTYPE::DEBUG_END }
{
}

UDefaultDebugging::UDefaultDebugging(const UDefaultDebugging& _rhs)
	: UPawn(_rhs),
	m_spVIBuffer{ nullptr }, m_eDebugType{ DEBUGTYPE::DEBUG_END }
{
}


void UDefaultDebugging::Free()
{
}

HRESULT UDefaultDebugging::NativeConstruct()
{
	return __super::NativeConstruct();
}

HRESULT UDefaultDebugging::NativeConstructClone(const VOIDDATAS& _vecDatas)
{
	RETURN_CHECK(_vecDatas.size() <= 0, E_FAIL);

	if (FAILED(__super::NativeConstructClone(_vecDatas)))
		return E_FAIL;

	// Debug Type
	m_eDebugType = UMethod::ConvertTemplate_Index<DEBUGTYPE>(_vecDatas, 0);
	SHPTR<UGameInstance> spGameInstnace = GET_INSTANCE(UGameInstance);
	if (DEBUGTYPE::DEBUG_CUBE == m_eDebugType)
	{
		m_spVIBuffer = static_pointer_cast<UVIBuffer>(spGameInstnace->CloneResource(PROTO_RES_VIBUFFERCUBE));
		AddShader(PROTO_RES_DEBUGGINGDEFAULTSHADER);
	}
	else if(DEBUGTYPE::DEBUG_SPHERE == m_eDebugType)
	{
		m_spVIBuffer = static_pointer_cast<UVIBuffer>(spGameInstnace->CloneResource(PROTO_RES_VIBUFFERSPHERE));
		AddShader(PROTO_RES_DEBUGGINGDEFAULTSHADER);
	}
	else if (DEBUGTYPE::DEBUG_CUBE_WHITE == m_eDebugType)
	{
		m_spVIBuffer = static_pointer_cast<UVIBuffer>(spGameInstnace->CloneResource(PROTO_RES_VIBUFFERCUBE));
		AddShader(PROTO_RES_DEBUGGINGDEFAULTWHITESHADER);
	}
	

	m_stDebuggParam.vDebugging.w = 0.5f;
	return S_OK;
}

void UDefaultDebugging::AddRenderer(RENDERID _eRenderID)
{
	AddRenderGroup(_eRenderID);
}

void UDefaultDebugging::SetColor(const _float3& _vChangeColor)
{
	m_stDebuggParam.vDebugging = _vChangeColor;
}

void UDefaultDebugging::SetColor(const _float4& _vChangeColor)
{
	m_stDebuggParam.vDebugging = _vChangeColor;
}

void UDefaultDebugging::TickActive(const _double& _dTimeDelta)
{
}

void UDefaultDebugging::LateTickActive(const _double& _dTimeDelta)
{
#ifdef _USE_DEBUGGING
	AddDebugRenderGroup(DEBUG_RI_ETC);
#endif 
}

HRESULT UDefaultDebugging::RenderActive(CSHPTRREF<UCommand> _spCommand, CSHPTRREF<UTableDescriptor> _spTableDescriptor)
{
	// Settings 
	__super::RenderActive(_spCommand, _spTableDescriptor);

	GetTransform()->BindTransformData(GetShader());
	m_spVIBuffer->Render(GetShader(), _spCommand);
	return S_OK;
}
HRESULT UDefaultDebugging::RenderShadowActive(CSHPTRREF<UCommand> _spCommand, CSHPTRREF<UTableDescriptor> _spTableDescriptor)
{
	// Settings 
	__super::RenderShadowActive(_spCommand, _spTableDescriptor);
	
	return S_OK;
}
HRESULT UDefaultDebugging::RenderOutlineActive(CSHPTRREF<UCommand> _spCommand, CSHPTRREF<UTableDescriptor> _spTableDescriptor, _bool _pass)
{
	return __super::RenderOutlineActive(_spCommand, _spTableDescriptor,_pass);
}
void UDefaultDebugging::Collision(CSHPTRREF<UPawn> _pEnemy, const _double& _dTimeDelta)
{
}