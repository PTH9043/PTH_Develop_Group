#include "ToolDefines.h"
#include "TTestObject.h"
#include "UGameInstance.h"
#include "UVIBufferRect.h"
#include "UVIBufferGrid.h"
#include "UTransform.h"
#include "UShaderConstantBuffer.h"
#include "UVIBufferPlane.h"
#include "UShader.h"
#include "UParticle.h"
#include "UCollider.h"

TTestObject::TTestObject(CSHPTRREF<UDevice> _spDevice, 
	const _wstring& _wstrLayer, const CLONETYPE& _eCloneType)
	: UPawn(_spDevice, _wstrLayer, _eCloneType, BACKINGTYPE::STATIC),
	m_spVIBufferGrid{ nullptr }, m_spRectColorBuffer{nullptr}
{
}

TTestObject::TTestObject(const TTestObject& _rhs) : 
	UPawn(_rhs), m_spVIBufferGrid{ nullptr }
{
}

void TTestObject::Free()
{
}

HRESULT TTestObject::NativeConstruct()
{
	return __super::NativeConstruct();
}

HRESULT TTestObject::NativeConstructClone(const VOIDDATAS& _vecDatas)
{
	if (FAILED(__super::NativeConstructClone(_vecDatas)))
		return E_FAIL;

	SHPTR<UGameInstance> spGameInstance = GET_INSTANCE(UGameInstance);
	m_spVIBufferGrid = static_pointer_cast<UVIBufferGrid>(spGameInstance->CloneResource(PROTO_RES_VIBUFFERGRID));
	// Add Shader 
	AddShader(PROTO_RES_GRIDSHADER);

	m_spRectColorBuffer = CreateNative< UShaderConstantBuffer>(GetDevice(), CBV_REGISTER::B3, RECTCOLOR_SIZE, 1);
	m_RectColor.g_RectColor = { 0.f, 1.f, 0.f, 1.f };

	UCollider::COLLIDERDESC tDesc;

	tDesc.vTranslation = _float3(0.f, 0.f, 0.f);
	tDesc.vScale = _float3(300.f, 0.f, 300.f);

	m_spCollider = std::static_pointer_cast<UCollider>(spGameInstance->CloneComp(PROTO_COMP_ABBCOLLIDER, { &tDesc }));

	return S_OK;
}

void TTestObject::TickActive(const _double& _dTimeDelta)
{

}

void TTestObject::LateTickActive(const _double& _dTimeDelta)
{
	//SHPTR<UGameInstance> spGameInstance = GET_INSTANCE(UGameInstance);
	//spGameInstance->AddPickingObject(ThisShared<UActor>(), m_spVIBufferGrid);
	AddRenderGroup(RENDERID::RI_NONALPHA_MIDDLE);
}

HRESULT TTestObject::RenderActive(CSHPTRREF<UCommand> _spCommand, CSHPTRREF<UTableDescriptor> _spTableDescriptor)
{
	__super::RenderActive(_spCommand, _spTableDescriptor);
	GetTransform()->BindTransformData(GetShader());
	GetShader()->BindCBVBuffer(m_spRectColorBuffer, &m_RectColor, RECTCOLOR_SIZE);
	m_spVIBufferGrid->Render(GetShader(), _spCommand);
	//m_spParticle->SetActive(true);
	return S_OK;
}

HRESULT TTestObject::RenderShadowActive(CSHPTRREF<UCommand> _spCommand, CSHPTRREF<UTableDescriptor> _spTableDescriptor)
{
	__super::RenderShadowActive(_spCommand, _spTableDescriptor);

	return S_OK;
}
HRESULT TTestObject::RenderOutlineActive(CSHPTRREF<UCommand> _spCommand, CSHPTRREF<UTableDescriptor> _spTableDescriptor, _bool _pass)
{
	__super::RenderOutlineActive(_spCommand, _spTableDescriptor,_pass);

	return S_OK;
}
void TTestObject::Collision(CSHPTRREF<UPawn> _pEnemy, const _double& _dTimeDelta)
{

}