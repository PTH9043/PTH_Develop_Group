#include "EngineDefine.h"
#include "UFire.h"
#include "UShader.h"
#include "UTexGroup.h"
#include "UTexture.h"
#include "UMethod.h"
#include "UShaderConstantBuffer.h"
#include "UGameInstance.h"
#include "UTransform.h"
#include "UVIBufferRect.h"

UFire::UFire(CSHPTRREF<UDevice> _spDevice, const _wstring& _wstrLayer,
	const CLONETYPE& _eCloneType)
	: UPawn(_spDevice, _wstrLayer, _eCloneType, BACKINGTYPE::NON),
	m_spFireColorTexGroup{ nullptr },
	m_spFireNoiseTexGroup{ nullptr },
	m_spFireAlphaTexGroup{ nullptr },
	m_spVIBufferRect{ nullptr }
{
}

UFire::UFire(const UFire& _rhs) :
	UPawn(_rhs),
	m_spFireColorTexGroup{ nullptr },
	m_spFireNoiseTexGroup{ nullptr },
	m_spFireAlphaTexGroup{ nullptr },
	m_spVIBufferRect{ nullptr }
{
}

void UFire::Free()
{
}

HRESULT UFire::NativeConstruct()
{
	return __super::NativeConstruct();
}

HRESULT UFire::NativeConstructClone(const VOIDDATAS& _convecDatas)
{
	RETURN_CHECK_FAILED(__super::NativeConstructClone(_convecDatas), E_FAIL);

	SHPTR<UGameInstance> spGameInstance = GET_INSTANCE(UGameInstance);
	if (_convecDatas.size() > 0)
	{
		FIREDESC stParticleDesc = UMethod::ConvertTemplate_Index<FIREDESC>(_convecDatas, 0);
		m_spShaderFireNoiseBuffer = CreateNative<UShaderConstantBuffer>(GetDevice(), CBV_REGISTER::FIRENOISEBUFFER,
			static_cast<_uint>(sizeof(FIRENOISEBUFFER)));
		m_spShaderDistortionBuffer = CreateNative<UShaderConstantBuffer>(GetDevice(), CBV_REGISTER::FIREDISTORTION,
			static_cast<_uint>(sizeof(FIREDISTORTIONBUFFER)));

		if (m_spFireColorTexGroup == nullptr)m_spFireColorTexGroup = static_pointer_cast<UTexGroup>(spGameInstance->CloneResource(PROTO_RES_FIRECOLORTEXTUREGROUP));
		if (m_spFireNoiseTexGroup == nullptr)m_spFireNoiseTexGroup = static_pointer_cast<UTexGroup>(spGameInstance->CloneResource(PROTO_RES_FIRENOISETEXTUREGROUP));
		if (m_spFireAlphaTexGroup == nullptr)m_spFireAlphaTexGroup = static_pointer_cast<UTexGroup>(spGameInstance->CloneResource(PROTO_RES_FIREALPHATEXTUREGROUP));
		m_spVIBufferRect = static_pointer_cast<UVIBufferRect>(spGameInstance->CloneResource(PROTO_RES_VIBUFFERRECT));

		AddShader(stParticleDesc.wstrFireShader);
		AddShadowShader(PROTO_RES_2DFIRESHADOWSHADER);

		SetActive(false);
	}
	{
		m_stFireNoiseBuffer.fScales = scales;
		m_stFireNoiseBuffer.fScrollSpeeds = scrollSpeeds;
		m_stFireDistortionBuffer.fDistortion1 = distortion1;
		m_stFireDistortionBuffer.fDistortion2 = distortion2;
		m_stFireDistortionBuffer.fDistortion3 = distortion3;

		m_stFireDistortionBuffer.fDistortionScale = distortionScale;
		m_stFireDistortionBuffer.fDistortionBias = distortionBias;
	}
	SetColorTexture(L"Sun");
	SetNoiseTexture(L"noise01");

	return S_OK;
}

void UFire::Update(const _double& _dTimeDelta)
{
	static float frameTime = 0.0f;
	frameTime += 0.01f;
	if (frameTime > 1000.0f)
	{
		frameTime = 0.0f;
	}
	m_stFireNoiseBuffer.fFrameTime = frameTime;

}


void UFire::TickActive(const _double& _dTimeDelta)
{
	if (IsActive()) {
		Update(_dTimeDelta);

		{
			SHPTR<UGameInstance> spGameInstance = GET_INSTANCE(UGameInstance);
			_float3 MainCamPos = spGameInstance->GetMainCameraTransform()->GetPos();
			//GetTransform()->LookAtWithFixedUp(_float3(MainCamPos));
			GetTransform()->LookAtWithFixedUp(_float3(MainCamPos), static_cast<_float>(_dTimeDelta), 1.5f);
		}
	}
	
}

void UFire::LateTickActive(const _double& _dTimeDelta)
{
	if (IsActive()) {
		AddRenderGroup(RENDERID::RI_NONALPHA_LAST);
		//AddRenderGroup(RENDERID::RI_SHADOW);
	
	}
}

HRESULT UFire::RenderActive(CSHPTRREF<UCommand> _spCommand, CSHPTRREF<UTableDescriptor> _spTableDescriptor)
{
	__super::RenderActive(_spCommand, _spTableDescriptor);

	BindShaderBuffer();
	// Bind Transform
	GetTransform()->BindTransformData(GetShader());


	GetShader()->BindSRVBuffer(SRV_REGISTER::T0, m_spFireColorTexGroup->GetTexture(ColorTextureIndex));
	GetShader()->BindSRVBuffer(SRV_REGISTER::T1, m_spFireNoiseTexGroup->GetTexture(NoiseTextureIndex));
	GetShader()->BindSRVBuffer(SRV_REGISTER::T2, m_spFireAlphaTexGroup->GetTexture(AlphaTextureIndex));


	m_spVIBufferRect->Render(GetShader(), _spCommand);
	return S_OK;
}
HRESULT UFire::RenderShadowActive(CSHPTRREF<UCommand> _spCommand, CSHPTRREF<UTableDescriptor> _spTableDescriptor)
{
	__super::RenderShadowActive(_spCommand, _spTableDescriptor);
	//GetShadowShader()->BindCBVBuffer(m_spShaderFireNoiseBuffer, &m_stFireNoiseBuffer, sizeof(FIRENOISEBUFFER));
	//GetShadowShader()->BindCBVBuffer(m_spShaderDistortionBuffer, &m_stFireDistortionBuffer, sizeof(FIREDISTORTIONBUFFER));

	//// Bind Transform
	//GetTransform()->BindTransformData(GetShadowShader());


	//GetShadowShader()->BindSRVBuffer(SRV_REGISTER::T0, m_spFireColorTexGroup->GetTexture(ColorTextureIndex));
	//GetShadowShader()->BindSRVBuffer(SRV_REGISTER::T1, m_spFireNoiseTexGroup->GetTexture(NoiseTextureIndex));
	//GetShadowShader()->BindSRVBuffer(SRV_REGISTER::T2, m_spFireAlphaTexGroup->GetTexture(AlphaTextureIndex));


	//m_spVIBufferRect->Render(GetShadowShader(), _spCommand);
	return S_OK;
}
HRESULT UFire::RenderOutlineActive(CSHPTRREF<UCommand> _spCommand, CSHPTRREF<UTableDescriptor> _spTableDescriptor, _bool _pass)
{
	return __super::RenderOutlineActive(_spCommand, _spTableDescriptor,_pass);
}
void UFire::BindShaderBuffer()
{
	GetShader()->BindCBVBuffer(m_spShaderFireNoiseBuffer, &m_stFireNoiseBuffer, sizeof(FIRENOISEBUFFER));
	GetShader()->BindCBVBuffer(m_spShaderDistortionBuffer, &m_stFireDistortionBuffer, sizeof(FIREDISTORTIONBUFFER));

}

void UFire::Collision(CSHPTRREF<UPawn> _pEnemy, const _double& _dTimeDelta)
{
}

_bool UFire::Save(const _wstring& _wstrPath)
{
	return _bool();
}

_bool UFire::Load(const _wstring& _wstrPath)
{
	return _bool();
}


void UFire::SetColorTexture(const _wstring& TexName)
{
	ColorTextureIndex = m_spFireColorTexGroup->GetTextureIndex(TexName);
}

void UFire::SetColorTexture(_uint _index)
{
	ColorTextureIndex = _index;
}

void UFire::SetNoiseTexture(const _wstring& TexName)
{
	NoiseTextureIndex = m_spFireNoiseTexGroup->GetTextureIndex(TexName);
}

void UFire::SetNoiseTexture(_uint _index)
{
	NoiseTextureIndex = _index;
}

void UFire::SetAlphaTexture(const _wstring& TexName)
{
	AlphaTextureIndex = m_spFireAlphaTexGroup->GetTextureIndex(TexName);
}

void UFire::SetAlphaTexture(_uint _index)
{
	AlphaTextureIndex = _index;
}

#ifdef _USE_IMGUI


void UFire::ShowObjectInfo()
{

}

#endif 