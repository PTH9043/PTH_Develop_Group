#include "EngineDefine.h"
#include "UGrid.h"
#include "UGameInstance.h"
#include "UVIBufferRect.h"
#include "UVIBufferGrid.h"
#include "UTransform.h"
#include "UShaderConstantBuffer.h"
#include "UVIBufferPlane.h"
#include "UShader.h"
#include "UParticle.h"
#include "UCollider.h"

UGrid::UGrid(CSHPTRREF<UDevice> _spDevice,
	const _wstring& _wstrLayer, const CLONETYPE& _eCloneType)
	: UPawn(_spDevice, _wstrLayer, _eCloneType, BACKINGTYPE::STATIC),
	m_spVIBufferGrid{ nullptr }
{
}

UGrid::UGrid(const UGrid& _rhs) :
	UPawn(_rhs), m_spVIBufferGrid{ nullptr }
{
}

void UGrid::Free()
{
}

HRESULT UGrid::NativeConstruct()
{
	return __super::NativeConstruct();
}

HRESULT UGrid::NativeConstructClone(const VOIDDATAS& _vecDatas)
{
	if (FAILED(__super::NativeConstructClone(_vecDatas)))
		return E_FAIL;

	SHPTR<UGameInstance> spGameInstance = GET_INSTANCE(UGameInstance);
	m_spVIBufferGrid = static_pointer_cast<UVIBufferGrid>(spGameInstance->CloneResource(PROTO_RES_VIBUFFERGRID));
	// Add Shader 
	AddShader(PROTO_RES_GRIDSHADER);



	UCollider::COLLIDERDESC tDesc;

	tDesc.vTranslation = _float3(0.f, 0.f, 0.f);
	tDesc.vScale = _float3(300.f, 0.f, 300.f);

	m_spCollider = std::static_pointer_cast<UCollider>(spGameInstance->CloneComp(PROTO_COMP_ABBCOLLIDER, { &tDesc }));

	return S_OK;
}

void UGrid::TickActive(const _double& _dTimeDelta)
{

}

void UGrid::LateTickActive(const _double& _dTimeDelta)
{
	//SHPTR<UGameInstance> spGameInstance = GET_INSTANCE(UGameInstance);
	//spGameInstance->AddPickingObject(ThisShared<UActor>(), m_spVIBufferGrid);
	AddRenderGroup(RENDERID::RI_NONALPHA_MIDDLE);
}

HRESULT UGrid::RenderActive(CSHPTRREF<UCommand> _spCommand, CSHPTRREF<UTableDescriptor> _spTableDescriptor)
{
	__super::RenderActive(_spCommand, _spTableDescriptor);
	GetTransform()->BindTransformData(GetShader());
	
	m_spVIBufferGrid->Render(GetShader(), _spCommand);
	//m_spParticle->SetActive(true);
	return S_OK;
}

HRESULT UGrid::RenderShadowActive(CSHPTRREF<UCommand> _spCommand, CSHPTRREF<UTableDescriptor> _spTableDescriptor)
{
	__super::RenderShadowActive(_spCommand, _spTableDescriptor);
	
	return S_OK;
}

HRESULT UGrid::RenderOutlineActive(CSHPTRREF<UCommand> _spCommand, CSHPTRREF<UTableDescriptor> _spTableDescriptor, _bool _pass)
{
	return __super::RenderOutlineActive(_spCommand, _spTableDescriptor,_pass);
}

void UGrid::Collision(CSHPTRREF<UPawn> _pEnemy, const _double& _dTimeDelta)
{

}