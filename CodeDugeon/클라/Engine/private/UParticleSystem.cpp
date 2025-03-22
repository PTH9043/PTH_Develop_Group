#include "EngineDefine.h"
#include "UParticleSystem.h"
#include "UGameInstance.h"
#include "UMethod.h"
#include "UShaderStructedBuffer.h"
#include "UShaderConstantBuffer.h"
#include <fstream>
#include "UComputeShader.h"
#include "UTableDescriptor.h"
#include "UComputeCommand.h"
#include "UDevice.h"




UParticleSystem::UParticleSystem(CSHPTRREF<UDevice> _spDevice) :
	UResource(_spDevice),
	m_iMaxParitcleCnt{ 400 },
	m_stParticleParam{},
	m_spComputeShaderParticleConstantBuffer{ nullptr },
	m_spParticleStructedBuffer{ nullptr },
	m_spComputeShaderStructedBuffer{ nullptr },
	m_spComputeShader{ nullptr },
	m_spComputeCommand{ nullptr },
	m_sComputeIndex{ 0 }, m_iParticleAddAmount{10}, m_fCreateInterval{2.5f}
{
}

UParticleSystem::UParticleSystem(const UParticleSystem& _rhs) :
	UResource(_rhs),
	m_iMaxParitcleCnt{ 400 },
	m_stParticleParam{},
	m_spComputeShaderParticleConstantBuffer{ nullptr },
	m_spParticleStructedBuffer{ nullptr },
	m_spComputeShaderStructedBuffer{ nullptr },
	m_spComputeShader{ nullptr },
	m_spComputeCommand{ nullptr },
	m_sComputeIndex{ 0 }, m_iParticleAddAmount{ 10 }, m_fCreateInterval{ 2.5f }
{
}

void UParticleSystem::Free()
{
}

HRESULT UParticleSystem::NativeConstruct()
{
	RETURN_CHECK_FAILED(__super::NativeConstruct(), E_FAIL);
	return S_OK;
}

HRESULT UParticleSystem::NativeConstructClone(const VOIDDATAS& _vecDatas)
{
	RETURN_CHECK_FAILED(__super::NativeConstructClone(_vecDatas), E_FAIL);
	// Particle Param Temporary Define
	{
		if (_vecDatas.size() > 0)
		{
			m_stParticleParam = UMethod::ConvertTemplate_Index<PARTICLEPARAM>(_vecDatas, 0);
		}
	}
	
	switch (m_stParticleParam.stGlobalParticleInfo.fParticleKind) {
	case PARTICLE_ORIGINAL:
		m_iMaxParitcleCnt = 1024;
		m_spComputeShaderTypeConstantBuffer = CreateNative<UShaderConstantBuffer>(GetDevice(), CBV_REGISTER::PARTICLETYPEBUFFER, PARTICLETYPEPARAM_SIZE);
		m_spParticleStructedBuffer = CreateNative<UShaderStructedBuffer>(GetDevice(), sizeof(PARTICLE), m_iMaxParitcleCnt);
		break;
	case PARTICLE_ROTATION:
		m_iMaxParitcleCnt = 102;
		m_spComputeShaderTypeConstantBuffer = CreateNative<UShaderConstantBuffer>(GetDevice(), CBV_REGISTER::PARTICLETYPEBUFFER, PARTICLETYPEPARAM_SIZE);
		m_spParticleStructedBuffer = CreateNative<UShaderStructedBuffer>(GetDevice(), sizeof(PARTICLE), m_iMaxParitcleCnt);
		break;
	case PARTICLE_FLARE:
		m_iMaxParitcleCnt = 116;
		m_spParticleStructedBufferPlus = CreateNative<UShaderStructedBuffer>(GetDevice(), sizeof(PARTICLEPLUS), m_iMaxParitcleCnt);
		break;
	case PARTICLE_BLOOD:
		m_iMaxParitcleCnt = 412;
		m_spComputeShaderTypeConstantBuffer = CreateNative<UShaderConstantBuffer>(GetDevice(), CBV_REGISTER::PARTICLETYPEBUFFER, PARTICLETYPEPARAM_SIZE);
		m_spParticleStructedBuffer = CreateNative<UShaderStructedBuffer>(GetDevice(), sizeof(PARTICLE), m_iMaxParitcleCnt);

		break;
	case PARTICLE_ATTACK:
		m_iMaxParitcleCnt = 512;
		m_spComputeShaderTypeConstantBuffer = CreateNative<UShaderConstantBuffer>(GetDevice(), CBV_REGISTER::PARTICLETYPEBUFFER, PARTICLETYPEPARAM_SIZE);
		m_spParticleStructedBuffer = CreateNative<UShaderStructedBuffer>(GetDevice(), sizeof(PARTICLE), m_iMaxParitcleCnt);

		break;
	case PARTICLE_OPENCHEST:
		m_iMaxParitcleCnt = 212;
		m_spComputeShaderTypeConstantBuffer = CreateNative<UShaderConstantBuffer>(GetDevice(), CBV_REGISTER::PARTICLETYPEBUFFER, PARTICLETYPEPARAM_SIZE);
		m_spParticleStructedBuffer = CreateNative<UShaderStructedBuffer>(GetDevice(), sizeof(PARTICLE), m_iMaxParitcleCnt);

		break;
	case PARTICLE_HEAL:
		m_iMaxParitcleCnt = 512;
		m_spComputeShaderTypeConstantBuffer = CreateNative<UShaderConstantBuffer>(GetDevice(), CBV_REGISTER::PARTICLETYPEBUFFER, PARTICLETYPEPARAM_SIZE);
		m_spParticleStructedBuffer = CreateNative<UShaderStructedBuffer>(GetDevice(), sizeof(PARTICLE), m_iMaxParitcleCnt);

		break;
	case PARTICLE_SLASH:
		m_iMaxParitcleCnt = 3;
		m_spComputeShaderTypeConstantBuffer = CreateNative<UShaderConstantBuffer>(GetDevice(), CBV_REGISTER::PARTICLETYPEBUFFER, PARTICLETYPEPARAM_SIZE);
		m_spParticleStructedBuffer = CreateNative<UShaderStructedBuffer>(GetDevice(), sizeof(PARTICLE), m_iMaxParitcleCnt);

		break;
	case PARTICLE_FOOTPRINT:
		m_iMaxParitcleCnt = 50;
		m_spComputeShaderTypeConstantBuffer = CreateNative<UShaderConstantBuffer>(GetDevice(), CBV_REGISTER::PARTICLETYPEBUFFER, PARTICLETYPEPARAM_SIZE);
		m_spParticleStructedBuffer = CreateNative<UShaderStructedBuffer>(GetDevice(), sizeof(PARTICLE), m_iMaxParitcleCnt);
		break;
	case PARTICLE_ANIM:
		m_iMaxParitcleCnt = 1024;
		m_spComputeShaderTypeConstantBuffer = CreateNative<UShaderConstantBuffer>(GetDevice(), CBV_REGISTER::PARTICLETYPEBUFFER, PARTICLETYPEPARAM_SIZE);
		m_spParticleStructedBuffer = CreateNative<UShaderStructedBuffer>(GetDevice(), sizeof(PARTICLE), m_iMaxParitcleCnt);
		break;
	default:
		break;
	}
	m_spComputeShaderParticleConstantBuffer = CreateNative<UShaderConstantBuffer>(GetDevice(), CBV_REGISTER::ALLPARTICLEBUFFER, PARTICLEPARAM_SIZE);
	m_spGraphicsShaderParticleConstantBuffer = CreateNative<UShaderConstantBuffer>(GetDevice(), CBV_REGISTER::ALLPARTICLEBUFFER, PARTICLEPARAM_SIZE);
	m_spComputeShaderStructedBuffer = CreateNative<UShaderStructedBuffer>(GetDevice(), sizeof(COMPUTESHADERINFO), 1);

	SHPTR<UGameInstance> spGameInstance = GET_INSTANCE(UGameInstance);
	{
		m_spComputeCommand = spGameInstance->GetComputeCommand();
		//m_spComputeTableDescriptor = CreateNative<UTableDescriptor>(GetDevice(), 1);
		m_spComputeTableDescriptor = spGameInstance->GetComputeTableDescriptor();
	}
	return S_OK;
}

void UParticleSystem::Update(const _double& _dTimeDelta)
{
	RETURN_CHECK(nullptr == m_spComputeShader, ;)
	{
		m_stParticleParam.stGlobalParticleInfo.fDeltaTime = static_cast<_float>(_dTimeDelta);
		m_stParticleParam.stGlobalParticleInfo.fAccTime += static_cast<_float>(_dTimeDelta);
		_int add = 0;
	
		switch (m_stParticleParam.stGlobalParticleInfo.fParticleKind) {
		case PARTICLE_ORIGINAL:
		case PARTICLE_ROTATION:
		case PARTICLE_FLARE:				
		case PARTICLE_ANIM:
			if (m_fCreateInterval < m_stParticleParam.stGlobalParticleInfo.fAccTime)
			{
				m_stParticleParam.stGlobalParticleInfo.fAccTime = m_stParticleParam.stGlobalParticleInfo.fAccTime - m_fCreateInterval;
				add = m_iParticleAddAmount;
			}
			m_stParticleParam.stGlobalParticleInfo.iAddCount = add;
			break;
		case PARTICLE_BLOOD:
			add = m_iParticleAddAmount;
			if (m_fCreateInterval < m_stParticleParam.stGlobalParticleInfo.fAccTime)
			{
				add = 0;
			}
			m_stParticleParam.stGlobalParticleInfo.iAddCount = add;
			break;
		case PARTICLE_ATTACK:
			/*if (m_fCreateInterval < m_stParticleParam.stGlobalParticleInfo.fAccTime)
			{
				m_stParticleParam.stGlobalParticleInfo.fAccTime = m_stParticleParam.stGlobalParticleInfo.fAccTime - m_fCreateInterval;
				add = m_iParticleAddAmount;
			}
			m_stParticleParam.stGlobalParticleInfo.iAddCount = add;*/
			add = m_iParticleAddAmount;

			if (m_fCreateInterval < m_stParticleParam.stGlobalParticleInfo.fAccTime)
			{
				add = 0;
			}

			m_stParticleParam.stGlobalParticleInfo.iAddCount = add;
			break;
		case PARTICLE_OPENCHEST:
		case PARTICLE_HEAL:
			add = m_iParticleAddAmount;
			
			if (m_fCreateInterval < m_stParticleParam.stGlobalParticleInfo.fAccTime)
			{
				add = 0;
			}
			
			m_stParticleParam.stGlobalParticleInfo.iAddCount = add;
			break;
		case PARTICLE_SLASH:
			
			add = m_iParticleAddAmount;
			if (m_fCreateInterval < m_stParticleParam.stGlobalParticleInfo.fAccTime)
			{
				add = 0;
			}
			m_stParticleParam.stGlobalParticleInfo.iAddCount = add;
			
			break;

		case PARTICLE_FOOTPRINT:
			
			if (m_fOffsetFactor)m_stParticleParam.stGlobalParticleInfo.fParticleThickness = -1;
			else m_stParticleParam.stGlobalParticleInfo.fParticleThickness = 2;//오른쪽 발 보폭 계산

			if (m_iParticleAddAmount == 0)m_stParticleParam.stGlobalParticleInfo.fAccTime = 0;

			if (m_fCreateInterval < m_stParticleParam.stGlobalParticleInfo.fAccTime)
			{
				m_stParticleParam.stGlobalParticleInfo.fAccTime = m_stParticleParam.stGlobalParticleInfo.fAccTime - m_fCreateInterval;
				
				add = m_iParticleAddAmount;
				m_fOffsetFactor = !m_fOffsetFactor;
			}
			
			m_stParticleParam.stGlobalParticleInfo.iAddCount = add;
			break;
		default:
			break;

		}
	

		
	}
}

void UParticleSystem::Render(CSHPTRREF<UCommand> _spCommand, CSHPTRREF<UTableDescriptor> _spTableDescriptor)
{
	m_spComputeShader->SetTableDescriptor(m_spComputeTableDescriptor);
	m_spComputeShader->BeginSettingDatas(m_spComputeCommand);


	switch (m_stParticleParam.stGlobalParticleInfo.fParticleKind) {
	case PARTICLE_ORIGINAL:
		m_spComputeShader->BindUAVBuffer(UAV_REGISTER::PARTICLEWRITEDATA, m_spParticleStructedBuffer);
		m_spComputeShader->BindCBVBuffer(m_spComputeShaderTypeConstantBuffer, &m_stParticleType, PARTICLETYPEPARAM_SIZE);
		break;
	case PARTICLE_ROTATION:
		m_spComputeShader->BindUAVBuffer(UAV_REGISTER::PARTICLEWRITEDATA, m_spParticleStructedBuffer);
		m_spComputeShader->BindCBVBuffer(m_spComputeShaderTypeConstantBuffer, &m_stParticleType, PARTICLETYPEPARAM_SIZE);
		break;
	case PARTICLE_FLARE:
		m_spComputeShader->BindUAVBuffer(UAV_REGISTER::PARTICLEWRITEDATA, m_spParticleStructedBufferPlus);
		break;
	case PARTICLE_BLOOD:
		m_spComputeShader->BindUAVBuffer(UAV_REGISTER::PARTICLEWRITEDATA, m_spParticleStructedBuffer);
		m_spComputeShader->BindCBVBuffer(m_spComputeShaderTypeConstantBuffer, &m_stParticleType, PARTICLETYPEPARAM_SIZE);

		break;
	case PARTICLE_ATTACK:
	case PARTICLE_HEAL:
	case PARTICLE_OPENCHEST:
		m_spComputeShader->BindUAVBuffer(UAV_REGISTER::PARTICLEWRITEDATA, m_spParticleStructedBuffer);
		m_spComputeShader->BindCBVBuffer(m_spComputeShaderTypeConstantBuffer, &m_stParticleType, PARTICLETYPEPARAM_SIZE);

		break;
	case PARTICLE_SLASH:
		m_spComputeShader->BindUAVBuffer(UAV_REGISTER::PARTICLEWRITEDATA, m_spParticleStructedBuffer);
		m_spComputeShader->BindCBVBuffer(m_spComputeShaderTypeConstantBuffer, &m_stParticleType, PARTICLETYPEPARAM_SIZE);

		break;
	case PARTICLE_FOOTPRINT:
		m_spComputeShader->BindUAVBuffer(UAV_REGISTER::PARTICLEWRITEDATA, m_spParticleStructedBuffer);
		m_spComputeShader->BindCBVBuffer(m_spComputeShaderTypeConstantBuffer, &m_stParticleType, PARTICLETYPEPARAM_SIZE);
		break;
	case PARTICLE_ANIM:
		m_spComputeShader->BindUAVBuffer(UAV_REGISTER::PARTICLEWRITEDATA, m_spParticleStructedBuffer);
		m_spComputeShader->BindCBVBuffer(m_spComputeShaderTypeConstantBuffer, &m_stParticleType, PARTICLETYPEPARAM_SIZE);
		break;
	default :
		break;

	}	
	m_spComputeShader->BindCBVBuffer(m_spComputeShaderParticleConstantBuffer, &m_stParticleParam, PARTICLEPARAM_SIZE);

	m_spComputeShader->BindUAVBuffer(UAV_REGISTER::SHRAEDDATA, m_spComputeShaderStructedBuffer);
	m_spComputeShader->CommitLocalShaderDatas(m_spComputeCommand, 1, 1, 1);
	//m_sComputeIndex = (m_sComputeIndex + 1) % m_arrComputeCommands.size();
}

void UParticleSystem::BindShaderParams(CSHPTRREF<UShader> _spShader)
{
	RETURN_CHECK(nullptr == _spShader, ;);
	switch (m_stParticleParam.stGlobalParticleInfo.fParticleKind) {
	case PARTICLE_ORIGINAL:
		_spShader->BindSRVBuffer(SRV_REGISTER::T14, m_spParticleStructedBuffer);
		_spShader->BindCBVBuffer(m_spGraphicsShaderParticleConstantBuffer, &m_stParticleParam, PARTICLEPARAM_SIZE);
		break;
	case PARTICLE_ROTATION:
		_spShader->BindSRVBuffer(SRV_REGISTER::T14, m_spParticleStructedBuffer);
		_spShader->BindCBVBuffer(m_spGraphicsShaderParticleConstantBuffer, &m_stParticleParam, PARTICLEPARAM_SIZE);
		break;
	case PARTICLE_FLARE:
		_spShader->BindSRVBuffer(SRV_REGISTER::T14, m_spParticleStructedBufferPlus);
		break;
	case PARTICLE_BLOOD:
		_spShader->BindSRVBuffer(SRV_REGISTER::T14, m_spParticleStructedBuffer);
		_spShader->BindCBVBuffer(m_spGraphicsShaderParticleConstantBuffer, &m_stParticleParam, PARTICLEPARAM_SIZE);

		break;
	case PARTICLE_ATTACK:
	case PARTICLE_HEAL:
	case PARTICLE_OPENCHEST:
		_spShader->BindSRVBuffer(SRV_REGISTER::T14, m_spParticleStructedBuffer);
		_spShader->BindCBVBuffer(m_spGraphicsShaderParticleConstantBuffer, &m_stParticleParam, PARTICLEPARAM_SIZE);

		break;
	case PARTICLE_SLASH:
		_spShader->BindSRVBuffer(SRV_REGISTER::T14, m_spParticleStructedBuffer);
		_spShader->BindCBVBuffer(m_spGraphicsShaderParticleConstantBuffer, &m_stParticleParam, PARTICLEPARAM_SIZE);

		break;
	case PARTICLE_FOOTPRINT:
		_spShader->BindSRVBuffer(SRV_REGISTER::T14, m_spParticleStructedBuffer);
		_spShader->BindCBVBuffer(m_spGraphicsShaderParticleConstantBuffer, &m_stParticleParam, PARTICLEPARAM_SIZE);
		break;
	case PARTICLE_ANIM:
		_spShader->BindSRVBuffer(SRV_REGISTER::T14, m_spParticleStructedBuffer);
		_spShader->BindCBVBuffer(m_spGraphicsShaderParticleConstantBuffer, &m_stParticleParam, PARTICLEPARAM_SIZE);
		break;
	default:
		break;

	}
		
	//그래픽스 쪽에서 이걸 scale팩터만 써서 그걸 PARTICLEPLUS의 padding을 scaleFactor로 사용
}

void UParticleSystem::SettingComputeShader(const _wstring& _wstrProtoName)
{
	SHPTR<UGameInstance> spGameInstance = GET_INSTANCE(UGameInstance);
	m_spComputeShader = static_pointer_cast<UComputeShader>(spGameInstance->CloneResource(_wstrProtoName));

	if (nullptr == m_spComputeShader)
	{
#ifdef _USE_DEBUGGING
		assert("Get Compute Shader Failed");
#endif 
	}
}

