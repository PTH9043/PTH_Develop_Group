#include "ClientDefines.h"
#include "UGameInstance.h"
#include "UCollider.h"
#include "UTransform.h"
#include "UMethod.h"
#include "CTorch.h"
#include "UFire.h"
#include "UPawn.h"
#include "UParticle.h"
#include "UParticleSystem.h"

CTorch::CTorch(CSHPTRREF<UDevice> _spDevice, const _wstring& _wstrLayer, const CLONETYPE& _eCloneType)
	: CModelObjects(_spDevice, _wstrLayer, _eCloneType),
	m_spFire{nullptr},
	m_stFireNoiseBuffer{},
	m_stFireDistortionBuffer{}
{
}

CTorch::CTorch(const CTorch& _rhs)
	: CModelObjects(_rhs),
	m_spFire{ nullptr },
	m_stFireNoiseBuffer{},
	m_stFireDistortionBuffer{}
{
}

void CTorch::Free()
{
}

HRESULT CTorch::NativeConstruct()
{
	return __super::NativeConstruct();
}

HRESULT CTorch::NativeConstructClone(const VOIDDATAS& _vecDatas)
{
	RETURN_CHECK_FAILED(__super::NativeConstructClone(_vecDatas), E_FAIL);
	SHPTR<UGameInstance> spGameInstance = GET_INSTANCE(UGameInstance);

	SetModel(L"Proto_Res_Model_Torch_FBX.bin");

	TORCHDESC tTorchDesc = UMethod::ConvertTemplate_Index<TORCHDESC>(_vecDatas, 0);
	GetTransform()->SetNewWorldMtx(tTorchDesc._Worldm);

	UFire::FIREDESC tFireDesc;
	tFireDesc.wstrFireShader = PROTO_RES_2DFIRESHADER;
	m_spFire = std::static_pointer_cast<UFire>(spGameInstance->CloneActorAdd(PROTO_ACTOR_FIRE, { &tFireDesc }));
	
	static float ScaleX = 3.f;
	static float ScaleY = 6.8f;
	_float3 ScaleFloat3 = _float3(ScaleX, ScaleY, 1);

	m_spFire->GetTransform()->SetScale(ScaleFloat3);
	m_spFire->GetTransform()->SetPos(_float3(GetTransform()->GetPos().x, GetTransform()->GetPos().y + 7.5f, GetTransform()->GetPos().z));

	m_stFireNoiseBuffer = m_spFire->GetFireNoiseBuffer();
	m_stFireDistortionBuffer = m_spFire->GetFireDistortionBuffer();
	
	{
		UParticle::PARTICLEDESC tDesc;
		tDesc.wstrParticleComputeShader = PROTO_RES_COMPUTEEMITPARTICLE2DSHADER;
		tDesc.wstrParticleShader = PROTO_RES_PARTICLEFLARE2DSHADER;

		tDesc.ParticleParam.stGlobalParticleInfo.fAccTime = 0.f;
		//tDesc.ParticleParam.stGlobalParticleInfo.fDeltaTime = 2.f;
		tDesc.ParticleParam.stGlobalParticleInfo.fEndScaleParticle = 0.25f;
		tDesc.ParticleParam.stGlobalParticleInfo.fStartScaleParticle = 0.5f;
		tDesc.ParticleParam.stGlobalParticleInfo.fMaxLifeTime = 1.8f;
		tDesc.ParticleParam.stGlobalParticleInfo.fMinLifeTime = 0.9f;
		tDesc.ParticleParam.stGlobalParticleInfo.fMaxSpeed = 1.f;
		tDesc.ParticleParam.stGlobalParticleInfo.fMinSpeed = 0.5f;
		tDesc.ParticleParam.stGlobalParticleInfo.iMaxCount = 200;
		tDesc.ParticleParam.stGlobalParticleInfo.fParticleThickness = 0.5f;
		tDesc.ParticleParam.stGlobalParticleInfo.fParticleDirection = _float3(0.f, 0.1f, 0.f);
		tDesc.ParticleParam.stGlobalParticleInfo.fParticleKind = PARTICLE_FLARE;
		m_spParticle = std::static_pointer_cast<UParticle>(spGameInstance->CloneActorAdd(PROTO_ACTOR_PARTICLE, { &tDesc }));
	}
	{
		m_stParticleParam = m_spParticle->GetParticleSystem()->GetParticleParam();
		m_stParticleType =  m_spParticle->GetParticleSystem()->GetParticleTypeParam();
		m_stParticleType->fParticleType = PARTICLE_TYPE_DEFAULT;
		m_stParticleType->fParticleLifeTimeType = PARTICLE_LIFETIME_TYPE_DEFAULT;
		m_spParticle->SetTexture(L"Particle"); // y값 증가 x 원
	}
	{
		*m_spParticle->GetParticleSystem()->GetCreateInterval() = 0.15f;
		*m_spParticle->GetParticleSystem()->GetAddParticleAmount() = 1;
		m_spParticle->SetParticleType(PARTICLE_FLARE);

	}
	m_spParticle->GetTransform()->SetPos(_float3(GetTransform()->GetPos().x, GetTransform()->GetPos().y + 3.f, GetTransform()->GetPos().z));
	m_spParticle->SetActive(false);

	m_spFire->SetActive(false);
	/*SetOutline(true);*/
	return S_OK;
}
