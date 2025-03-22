#include "EngineDefine.h"
#include "UParticle.h"
#include "UParticleSystem.h"
#include "UShader.h"
#include "UTexGroup.h"
#include "UTexture.h"
#include "UMethod.h"
#include "UGameInstance.h"
#include "UTransform.h"
#include "UVIBufferPoint.h"
#include "URenderTargetGroup.h"

UParticle::UParticle(CSHPTRREF<UDevice> _spDevice, const _wstring& _wstrLayer,
	const CLONETYPE& _eCloneType)
	: UPawn(_spDevice, _wstrLayer, _eCloneType, BACKINGTYPE::NON),
	m_spTexGroup{ nullptr }, m_spParticleSystem{ nullptr }, m_spVIBufferPoint{ nullptr },TextureIndex{0}, m_LifeTime0{0}, m_LifeTime1{ 0 }
{
}

UParticle::UParticle(const UParticle& _rhs) :
	UPawn(_rhs),
	m_spTexGroup{ nullptr }, m_spParticleSystem{ nullptr }, m_spVIBufferPoint{ nullptr }, TextureIndex{ 0 }, m_LifeTime0{0}, m_LifeTime1{ 0 }
{
}

void UParticle::Free()
{
}

HRESULT UParticle::NativeConstruct()
{
	return __super::NativeConstruct();
}

HRESULT UParticle::NativeConstructClone(const VOIDDATAS& _convecDatas)
{
	RETURN_CHECK_FAILED(__super::NativeConstructClone(_convecDatas), E_FAIL);

	SHPTR<UGameInstance> spGameInstance = GET_INSTANCE(UGameInstance);
	if (_convecDatas.size() > 0)
	{
		PARTICLEDESC stParticleDesc = UMethod::ConvertTemplate_Index<PARTICLEDESC>(_convecDatas, 0);

		m_spParticleSystem = static_pointer_cast<UParticleSystem>(spGameInstance->CloneResource(PROTO_RES_PARTICLESYSTEM, { &stParticleDesc.ParticleParam }));
		if (m_spTexGroup == nullptr) {
			if (stParticleDesc.ParticleParam.stGlobalParticleInfo.fParticleKind == PARTICLE_ANIM) {
				m_spTexGroup = static_pointer_cast<UTexGroup>(spGameInstance->CloneResource(PROTO_RES_ANIMPARTICLETEXTUREGROUP));
			}
			else {
				m_spTexGroup = static_pointer_cast<UTexGroup>(spGameInstance->CloneResource(PROTO_RES_PARTICLETEXTUREGROUP));
			}
		}
		
			m_spVIBufferPoint = static_pointer_cast<UVIBufferPoint>(spGameInstance->CloneResource(PROTO_RES_VIBUFFERPOINT));

		m_spParticleSystem->SettingComputeShader(stParticleDesc.wstrParticleComputeShader);
		AddShader(stParticleDesc.wstrParticleShader);

		/*for (int i = 0; i < 2; i++) {
			_uint iIndex=m_spTexGroup->GetTextureIndex(stParticleDesc.wstrParticleTextureName[i]);
			m_TextureIndexContainer.insert(std::pair<_uint, SRV_REGISTER>(iIndex, SRV_REGISTER::T0));
		}*/
		
		m_LifeTimer = CUSTIMER{ stParticleDesc.ParticleParam.stGlobalParticleInfo.fMaxLifeTime };

		SetActive(false);
	}
	return S_OK;
}

void UParticle::TickActive(const _double& _dTimeDelta)
{
	/*if (true == m_LifeTimer.IsOver(_dTimeDelta))
	{
		SetActive(false);
		m_LifeTimer.ResetTimer();
	}*/switch (m_spParticleSystem->GetParticleType()) {
	case PARTICLE_ORIGINAL:
		break;
	case PARTICLE_ROTATION:
		break;
	case PARTICLE_FLARE:
		break;
	case PARTICLE_BLOOD:
		//if (m_spParticleSystem->GetAddParticleAmount() != 0) {
		//	//m_LifeTime0 += _dTimeDelta;
		//	if (m_spParticleSystem->GetParticleParam()->stGlobalParticleInfo.fAccTime >= *m_spParticleSystem->GetCreateInterval()) {
		//		m_spParticleSystem->GetParticleParam()->stGlobalParticleInfo.fAccTime = 0;
		//	//	*m_spParticleSystem->GetAddParticleAmount() = 0;
		//	}
		//}
		/*if (m_spParticleSystem->GetAddParticleAmount() == 0) {
			
			m_LifeTime1 += _dTimeDelta;
		
			_float maxLifeTime = m_spParticleSystem->GetParticleParam()->stGlobalParticleInfo.fMaxLifeTime;
			_float minLifeTime = m_spParticleSystem->GetParticleParam()->stGlobalParticleInfo.fMinLifeTime;
			if (m_LifeTime1 > (maxLifeTime - minLifeTime) + minLifeTime - 0.5) {
				m_LifeTime1 = 0;
				SetActive(false);
			}
		}*/
		break;
	case PARTICLE_ATTACK:
	case PARTICLE_OPENCHEST:
	case PARTICLE_HEAL:

		break;
	case PARTICLE_SLASH:

		break;
	case PARTICLE_FOOTPRINT:
		
		break;
	case PARTICLE_ANIM:
		
		break;
	default:
		break;

	}
	
	m_spParticleSystem->Update(_dTimeDelta);
}

void UParticle::LateTickActive(const _double& _dTimeDelta)
{
	switch (m_spParticleSystem->GetParticleType()) {
	case PARTICLE_ORIGINAL:
		AddRenderGroup(RENDERID::RI_NONALPHA_LAST);
	break;
	case PARTICLE_ROTATION:
		AddRenderGroup(RENDERID::RI_NONALPHA_LAST);
	break;
	case PARTICLE_FLARE:
		AddRenderGroup(RENDERID::RI_NONALPHA_LAST);
		break;
	case PARTICLE_BLOOD:
		AddRenderGroup(RENDERID::RI_NONALPHA_LAST);
		break;
	case PARTICLE_ATTACK:
	case PARTICLE_OPENCHEST:
	case PARTICLE_HEAL:
		AddRenderGroup(RENDERID::RI_NONALPHA_LAST);
		break;
	case PARTICLE_SLASH:
		AddRenderGroup(RENDERID::RI_NONALPHA_LAST);
		break;
	case PARTICLE_FOOTPRINT:
		AddRenderGroup(RENDERID::RI_NONALPHA_LAST);
		break;
	case PARTICLE_ANIM:
		AddRenderGroup(RENDERID::RI_NONALPHA_LAST);
		break;
	default:
		break;

	}
	
}

HRESULT UParticle::RenderActive(CSHPTRREF<UCommand> _spCommand, CSHPTRREF<UTableDescriptor> _spTableDescriptor)
{
	SHPTR<UGameInstance> spGameInstance = GET_INSTANCE(UGameInstance);
SHPTR<URenderTargetGroup> spShadowDepthGroup{ spGameInstance->FindRenderTargetGroup(RTGROUPID::NONALPHA_DEFFERED) };

	__super::RenderActive(_spCommand, _spTableDescriptor);
	m_spParticleSystem->Render(_spCommand, _spTableDescriptor);
	BindShaderBuffer();
	// Bind Transform
	GetTransform()->BindTransformData(GetShader());
	// Bind Particle System Buffer
	m_spParticleSystem->BindShaderParams(GetShader());
	// bind Srv Buffer 
	switch (m_spParticleSystem->GetParticleType()) {
	case PARTICLE_ORIGINAL:
		GetShader()->BindSRVBuffer(SRV_REGISTER::T0, m_spTexGroup->GetTexture(TextureIndex)); 
		GetShader()->BindSRVBuffer(SRV_REGISTER::T1, spShadowDepthGroup->GetRenderTargetTexture(RTOBJID::NONALPHA_DEPTH_DEFFERED));
		break;
	case PARTICLE_ROTATION:
		GetShader()->BindSRVBuffer(SRV_REGISTER::T0, m_spTexGroup->GetTexture(TextureIndex)); 
		GetShader()->BindSRVBuffer(SRV_REGISTER::T1, spShadowDepthGroup->GetRenderTargetTexture(RTOBJID::NONALPHA_DEPTH_DEFFERED));
		break;

	case PARTICLE_FLARE:
		GetShader()->BindSRVBuffer(SRV_REGISTER::T0, m_spTexGroup->GetTexture(TextureIndex));
		break;
	case PARTICLE_BLOOD:
		GetShader()->BindSRVBuffer(SRV_REGISTER::T0, m_spTexGroup->GetTexture(BloodTextureIndices[0]));
		GetShader()->BindSRVBuffer(SRV_REGISTER::T1, m_spTexGroup->GetTexture(BloodTextureIndices[1]));
		GetShader()->BindSRVBuffer(SRV_REGISTER::T2, m_spTexGroup->GetTexture(BloodTextureIndices[2]));
		GetShader()->BindSRVBuffer(SRV_REGISTER::T3, m_spTexGroup->GetTexture(BloodTextureIndices[3]));
		GetShader()->BindSRVBuffer(SRV_REGISTER::T4, m_spTexGroup->GetTexture(BloodTextureIndices[4]));
		GetShader()->BindSRVBuffer(SRV_REGISTER::T5, m_spTexGroup->GetTexture(BloodTextureIndices[5]));
		GetShader()->BindSRVBuffer(SRV_REGISTER::T6, m_spTexGroup->GetTexture(BloodTextureIndices[6]));
		GetShader()->BindSRVBuffer(SRV_REGISTER::T7, m_spTexGroup->GetTexture(BloodTextureIndices[7]));
		break;
	case PARTICLE_ATTACK:
	case PARTICLE_OPENCHEST:
	case PARTICLE_HEAL:
		GetShader()->BindSRVBuffer(SRV_REGISTER::T0, m_spTexGroup->GetTexture(TextureIndex));
		GetShader()->BindSRVBuffer(SRV_REGISTER::T1, spShadowDepthGroup->GetRenderTargetTexture(RTOBJID::NONALPHA_DEPTH_DEFFERED));
		break;
	case PARTICLE_SLASH:
		GetShader()->BindSRVBuffer(SRV_REGISTER::T0, m_spTexGroup->GetTexture(L"Slash3"));
		GetShader()->BindSRVBuffer(SRV_REGISTER::T2, m_spTexGroup->GetTexture(L"Slash2"));
		GetShader()->BindSRVBuffer(SRV_REGISTER::T1, spShadowDepthGroup->GetRenderTargetTexture(RTOBJID::NONALPHA_DEPTH_DEFFERED));
		break;
	case PARTICLE_FOOTPRINT:
		GetShader()->BindSRVBuffer(SRV_REGISTER::T0, m_spTexGroup->GetTexture(TextureIndex));
		GetShader()->BindSRVBuffer(SRV_REGISTER::T1, spShadowDepthGroup->GetRenderTargetTexture(RTOBJID::NONALPHA_DEPTH_DEFFERED));
		break;
	case PARTICLE_ANIM:
		GetShader()->BindSRVBuffer(SRV_REGISTER::T0, m_spTexGroup->GetTexture(TextureIndex));
		GetShader()->BindSRVBuffer(SRV_REGISTER::T1, spShadowDepthGroup->GetRenderTargetTexture(RTOBJID::NONALPHA_DEPTH_DEFFERED));
		break;
	default:
		break;

	}
	
	
	m_spVIBufferPoint->Render(GetShader(), _spCommand, m_spParticleSystem->GetMaxParticleCnt());
	return S_OK;
}

HRESULT UParticle::RenderShadowActive(CSHPTRREF<UCommand> _spCommand, CSHPTRREF<UTableDescriptor> _spTableDescriptor)
{
	__super::RenderShadowActive(_spCommand, _spTableDescriptor);
	return S_OK;
}

HRESULT UParticle::RenderOutlineActive(CSHPTRREF<UCommand> _spCommand, CSHPTRREF<UTableDescriptor> _spTableDescriptor, _bool _pass)
{
	return __super::RenderOutlineActive(_spCommand, _spTableDescriptor,_pass);
}


void UParticle::BindShaderBuffer()
{
}

void UParticle::Collision(CSHPTRREF<UPawn> _pEnemy, const _double& _dTimeDelta)
{
}

_bool UParticle::Save(const _wstring& _wstrPath)
{
	return _bool();
}

_bool UParticle::Load(const _wstring& _wstrPath)
{
	return _bool();
}


void UParticle::SetTexture(const _wstring& TexName)
{
	TextureIndex = m_spTexGroup->GetTextureIndex(TexName);
}

void UParticle::SetPosition(_float3 Pos)
{
	if (m_spParticleSystem != nullptr)
		m_spParticleSystem->GetParticleParam()->stGlobalParticleInfo.fParticlePosition = Pos;
}

void UParticle::SetDirection(_float3 Dir)
{
	if (m_spParticleSystem != nullptr)
		m_spParticleSystem->GetParticleParam()->stGlobalParticleInfo.fParticleDirection = Dir;
}

void UParticle::SetBloodTexture(_uint Index, const _wstring& TexName)
{
	if(Index<BloodTextureIndices.size())
	BloodTextureIndices[Index] =m_spTexGroup->GetTextureIndex(TexName);
}

void UParticle::SetTexture(_uint _index)
{
	TextureIndex = _index;
}

void UParticle::SetParticleType(PARTICLE_KIND _type) {
	if (m_spParticleSystem != nullptr)
		m_spParticleSystem->GetParticleParam()->stGlobalParticleInfo.fParticleKind = _type;

}

void UParticle::SetNextAnimTimer(_float _fSetTime)
{
	if (m_spParticleSystem != nullptr)
		m_spParticleSystem->GetParticleParam()->stGlobalParticleInfo.fNextAnimTime = _fSetTime;
}

void UParticle::SetTextureRowsAndCols(_float _fRows, _float _fCols)
{
	if (m_spParticleSystem != nullptr) {
		m_spParticleSystem->GetParticleParam()->stGlobalParticleInfo.fAnimSizeX = _fRows;
		m_spParticleSystem->GetParticleParam()->stGlobalParticleInfo.fAnimSizeY = _fCols;
	}
		
}

#ifdef _USE_IMGUI

void UParticle::ShowObjectInfo()
{

}
#endif 