#include "EngineDefine.h"
#include "UDefaultCube.h"
#include "UTransform.h"
#include "UVIBufferCube.h"
#include "UGameInstance.h"
#include "UShaderConstantBuffer.h"
#include "UMethod.h"
#include "UShader.h"

UDefaultCube::UDefaultCube(CSHPTRREF<UDevice> _spDevice, const _wstring & _wstrLayer, const CLONETYPE & _eCloneType)
	: UPawn(_spDevice, _wstrLayer, _eCloneType, BACKINGTYPE::STATIC),
	m_spVIBufferSphere{ nullptr },
	m_spNormalCubeShaderConstnatBuffer{ nullptr }
{
}

UDefaultCube::UDefaultCube(const UDefaultCube& _rhs) :
	UPawn(_rhs), m_spVIBufferSphere{ nullptr }, m_spNormalCubeShaderConstnatBuffer{ nullptr }
{
}

void UDefaultCube::Free()
{
}

HRESULT UDefaultCube::NativeConstruct()
{
	if (FAILED(__super::NativeConstruct()))
		return E_FAIL;

	return S_OK;
}

HRESULT UDefaultCube::NativeConstructClone(const VOIDDATAS& _vecDatas)
{
	if (FAILED(__super::NativeConstructClone(_vecDatas)))
		return E_FAIL;

	if (_vecDatas.size() >= 1)
	{
		CUBEDESC tDesc = UMethod::ConvertTemplate_Index<CUBEDESC>(_vecDatas, 0);
		m_stNormalCubeParam.g_vNormalCubeColor = tDesc.vColor;
	}

	SHPTR<UGameInstance> spGameInstance = GET_INSTANCE(UGameInstance);
	m_spVIBufferSphere = static_pointer_cast<UVIBufferCube>(spGameInstance->CloneResource(PROTO_RES_VIBUFFERNORMALCUBE));
	// Add Shader 
	AddShader(PROTO_RES_3DNORMALCUBESHADER);

	m_spNormalCubeShaderConstnatBuffer = CreateNative<UShaderConstantBuffer>(GetDevice(), CBV_REGISTER::NORMALCUBEPARAM, CUBEPARAM_SIZE);
	return S_OK;
}

void UDefaultCube::TickActive(const _double& _dTimeDelta)
{
}

void UDefaultCube::LateTickActive(const _double& _dTimeDelta)
{
	AddRenderGroup(RI_NONALPHA_MIDDLE);
}

HRESULT UDefaultCube::RenderActive(CSHPTRREF<UCommand> _spCommand, CSHPTRREF<UTableDescriptor> _spTableDescriptor)
{
	// Settings 
	__super::RenderActive(_spCommand, _spTableDescriptor);
	GetTransform()->BindTransformData(GetShader());
	GetShader()->BindCBVBuffer(m_spNormalCubeShaderConstnatBuffer, &m_stNormalCubeParam, CUBEPARAM_SIZE);
	m_spVIBufferSphere->Render(GetShader(), _spCommand);
	return S_OK;
}

HRESULT UDefaultCube::RenderShadowActive(CSHPTRREF<UCommand> _spCommand, CSHPTRREF<UTableDescriptor> _spTableDescriptor)
{
	// Settings 
	__super::RenderShadowActive(_spCommand, _spTableDescriptor);

	return S_OK;
}

HRESULT UDefaultCube::RenderOutlineActive(CSHPTRREF<UCommand> _spCommand, CSHPTRREF<UTableDescriptor> _spTableDescriptor, _bool _pass)
{
	return __super::RenderOutlineActive(_spCommand, _spTableDescriptor,_pass);
	
}

void UDefaultCube::Collision(CSHPTRREF<UPawn> _pEnemy, const _double& _dTimeDelta)
{

}
