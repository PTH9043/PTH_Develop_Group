#include "EngineDefine.h"
#include "UFog.h"
#include "UShader.h"
#include "UTexGroup.h"
#include "UTexture.h"
#include "UMethod.h"
#include "UShaderConstantBuffer.h"
#include "UGameInstance.h"
#include "UTransform.h"
#include "UVIBufferCube.h"

UFog::UFog(CSHPTRREF<UDevice> _spDevice, const _wstring& _wstrLayer,
	const CLONETYPE& _eCloneType)
	: UPawn(_spDevice, _wstrLayer, _eCloneType, BACKINGTYPE::NON),
	m_spVIBufferCube{ nullptr }
{
}

UFog::UFog(const UFog& _rhs) :
	UPawn(_rhs),
	m_spVIBufferCube{ nullptr }
{
}

void UFog::Free()
{
}

HRESULT UFog::NativeConstruct()
{
	return __super::NativeConstruct();
}

HRESULT UFog::NativeConstructClone(const VOIDDATAS& _convecDatas)
{
	RETURN_CHECK_FAILED(__super::NativeConstructClone(_convecDatas), E_FAIL);

	SHPTR<UGameInstance> spGameInstance = GET_INSTANCE(UGameInstance);
	if (_convecDatas.size() > 0)
	{
		FOGDESC stParticleDesc = UMethod::ConvertTemplate_Index<FOGDESC>(_convecDatas, 0);
		m_spShaderFogBuffer = CreateNative<UShaderConstantBuffer>(GetDevice(), CBV_REGISTER::FOGBUFFER, static_cast<_int>(sizeof(FOGBUFFER)));


		if (m_spFogTexGroup == nullptr)m_spFogTexGroup = static_pointer_cast<UTexGroup>(spGameInstance->CloneResource(PROTO_RES_FOGTEXTUREGROUP));
		
		m_spVIBufferCube = static_pointer_cast<UVIBufferCube>(spGameInstance->CloneResource(PROTO_RES_VIBUFFERCUBE));

		m_stFogBuffer.fFogEnd = fogEnd;
		m_stFogBuffer.fFogStart = fogStart;
		m_stFogBuffer.fFogDensity = fogDensity;
		m_stFogBuffer.fType = 1;
		m_stFogBuffer.fFogColor = fogColor;

		AddShader(stParticleDesc.wstrFogShader);
		AddShadowShader(PROTO_RES_SHADOWSHADER);

		SetActive(false);
	}
	return S_OK;
}

void UFog::Update(const _double& _dTimeDelta)
{
		

}


void UFog::TickActive(const _double& _dTimeDelta)
{

	Update(_dTimeDelta);

}

void UFog::LateTickActive(const _double& _dTimeDelta)
{
	AddRenderGroup(RENDERID::RI_NONALPHA_LAST);
	AddShadowRenderGroup(RENDERID::RI_SHADOW);
}

HRESULT UFog::RenderActive(CSHPTRREF<UCommand> _spCommand, CSHPTRREF<UTableDescriptor> _spTableDescriptor)
{
	__super::RenderActive(_spCommand, _spTableDescriptor);

	BindShaderBuffer();
	// Bind Transform
	GetTransform()->BindTransformData(GetShader());


	GetShader()->BindSRVBuffer(SRV_REGISTER::T0, m_spFogTexGroup->GetTexture(ColorTextureIndex));
	


	m_spVIBufferCube->Render(GetShader(), _spCommand);
	return S_OK;
}

HRESULT UFog::RenderShadowActive(CSHPTRREF<UCommand> _spCommand, CSHPTRREF<UTableDescriptor> _spTableDescriptor)
{
	__super::RenderShadowActive(_spCommand, _spTableDescriptor);

	GetTransform()->BindTransformData(GetShadowShader());



	m_spVIBufferCube->Render(GetShadowShader(), _spCommand);
	return S_OK;
}

HRESULT UFog::RenderOutlineActive(CSHPTRREF<UCommand> _spCommand, CSHPTRREF<UTableDescriptor> _spTableDescriptor, _bool _pass)
{
	return __super::RenderOutlineActive(_spCommand, _spTableDescriptor,_pass);
}

void UFog::BindShaderBuffer()
{
	GetShader()->BindCBVBuffer(m_spShaderFogBuffer, &m_stFogBuffer, sizeof(FOGBUFFER));


}

void UFog::Collision(CSHPTRREF<UPawn> _pEnemy, const _double& _dTimeDelta)
{
}

_bool UFog::Save(const _wstring& _wstrPath)
{
	return _bool();
}

_bool UFog::Load(const _wstring& _wstrPath)
{
	return _bool();
}



#ifdef _USE_IMGUI


void UFog::ShowObjectInfo()
{

}

void UFog::SetColorTexture(const _wstring& TexName)
{
	ColorTextureIndex = m_spFogTexGroup->GetTextureIndex(TexName);
}

void UFog::SetColorTexture(_uint _index)
{
	ColorTextureIndex = _index;
}
#endif 