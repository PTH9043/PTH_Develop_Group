#include "EngineDefine.h"
#include "ULight.h"
#include "UGameInstance.h"
#include "UShaderConstantBuffer.h"
#include "UShader.h"
#include "UVIBufferRect.h"

#include "URenderTargetManager.h"
//#include "URenderObject.h"

ULight::ULight(CSHPTRREF<UDevice> _spDevice, const LIGHTINFO& _stLightInfo)
	: UObject(_spDevice),
	m_stLightParam{ _stLightInfo }
{
}

void ULight::Free()
{
}

HRESULT ULight::NativeConstruct()
{
	m_isActive = true;
return S_OK;
}

void ULight::Render(SHPTR<UCommand> spCmdList,CSHPTRREF<UVIBufferRect> _spVIBufferRect, CSHPTRREF<UShader> _spShader)
{
	RETURN_CHECK(nullptr == _spShader || nullptr == _spVIBufferRect, ;);
	//_spShader->BindCBVBuffer(m_spLightConstantBuffer, &m_stLightParam, GetTypeSize<LIGHTCONTROLPARAM>());
	//_spVIBufferRect->Render(_spShader, spCmdList);

	if (m_isActive == true)m_stLightParam.tLightInfo.eLightActive = LIGHTACTIVE::ISACTIVE;
	else m_stLightParam.tLightInfo.eLightActive = LIGHTACTIVE::NONACTIVE;
}


void ULight::OutLightInfo(LIGHTINFO& _stLightInfo)
{
	
}


