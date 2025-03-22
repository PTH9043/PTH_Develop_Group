#include "EngineDefine.h"
#include "UPawn.h"
#include "UShader.h"
#include "URenderer.h"
#include "UGameInstance.h"
#include "UCollider.h"
#include "UTransform.h"

UPawn::UPawn(CSHPTRREF<UDevice> _spDevice, const _wstring& _wstrLayer, const CLONETYPE& _eCloneType, const BACKINGTYPE _eBackingType, PAWNTYPE _ePawnType) :
	UActor(_spDevice, _wstrLayer, _eCloneType, _eBackingType, USECOLLISIONTYPE::ACTIVE), m_ePawnType{_ePawnType},
	m_spRenderer{ nullptr },
	m_spShader{ nullptr },
	m_spShadowShader{ nullptr },
	m_spOutlineShader{ nullptr },
	m_spNorPosShader{ nullptr }
#ifdef _USE_DEBUGGING
	, m_isDebugRenderingType{ false }
#endif 
{
}

UPawn::UPawn(const UPawn& _rhs) :
	UActor(_rhs),
	m_spRenderer{ nullptr },
	m_spShader{ nullptr },
	m_spShadowShader{ nullptr },
	m_spOutlineShader{ nullptr },
	m_spNorPosShader{ nullptr },
	m_ePawnType{ _rhs.m_ePawnType }
#ifdef _USE_DEBUGGING
	, m_isDebugRenderingType{ false }
#endif 
{
}

CSHPTRREF<UShader> UPawn::GetShader() const {
	return m_spShader;
}

CSHPTRREF<UShader> UPawn::GetShadowShader() const {
	return m_spShadowShader;
}
CSHPTRREF<UShader> UPawn::GetOutlineShader() const {
	return m_spOutlineShader;
}

CSHPTRREF<UShader> UPawn::GetNorPosShader() const {
	return m_spNorPosShader;
}
void UPawn::Free()
{
}

HRESULT UPawn::NativeConstruct()
{
	RETURN_CHECK_FAILED(__super::NativeConstruct(), E_FAIL);

	return S_OK;
}

HRESULT UPawn::NativeConstructClone(const VOIDDATAS& _vecDatas)
{
	RETURN_CHECK_FAILED(__super::NativeConstructClone(_vecDatas), E_FAIL);
	SHPTR<UGameInstance> spGameInstance = GET_INSTANCE(UGameInstance);
	// Renderer
	m_spRenderer = AddComponent<URenderer>(PROTO_COMP_RENDERER, COM_RENDERER);


	return S_OK;
}

bool UPawn::IsHit(CSHPTRREF<UPawn> _spEnemy, const _double& _dTimeDelta)
{
	assert(nullptr != _spEnemy);
	RETURN_CHECK(0 >= m_ColliderContainer.size(), false);

	_bool isTrue{ true };
	for (auto& iter : _spEnemy->m_ColliderContainer)
	{
		const auto& Collider = m_ColliderContainer.find(iter.first);
		if (Collider == m_ColliderContainer.end())
		{
			continue;
		}

		isTrue |= Collider->second->IsCollision(iter.second);
	}
	if (true == isTrue)
		Collision(_spEnemy, _dTimeDelta);

	return isTrue;
}

void UPawn::AddRenderGroup(const RENDERID _iRenderID)
{
	m_spRenderer->AddRenderGroup(_iRenderID, GetShader(), ThisShared<UPawn>());
}

void UPawn::AddShadowRenderGroup(const RENDERID _iRenderID)
{
	m_spRenderer->AddRenderGroup(_iRenderID, GetShadowShader(), ThisShared<UPawn>());
}

void UPawn::AddOutlineRenderGroup(const RENDERID _iRenderID)
{
	m_spRenderer->AddRenderGroup(_iRenderID, GetOutlineShader(), ThisShared<UPawn>());
}

void UPawn::AddNorPosRenderGroup(const RENDERID _iRenderID)
{
	m_spRenderer->AddRenderGroup(_iRenderID, GetNorPosShader(), ThisShared<UPawn>());
}

void UPawn::TickActive(const _double& _dTimeDelta)
{

}

void UPawn::LateTickActive(const _double& _dTimeDelta)
{
}

HRESULT UPawn::RenderActive(CSHPTRREF<UCommand> _spCommand, CSHPTRREF<UTableDescriptor> _spTableDescriptor)
{
	// Settings 
	GetShader()->SetTableDescriptor(_spTableDescriptor);
	return S_OK;
}

HRESULT UPawn::RenderShadowActive(CSHPTRREF<UCommand> _spCommand, CSHPTRREF<UTableDescriptor> _spTableDescriptor)
{
	// Settings 
	GetShadowShader()->SetTableDescriptor(_spTableDescriptor);
	return S_OK;
}

HRESULT UPawn::RenderOutlineActive(CSHPTRREF<UCommand> _spCommand, CSHPTRREF<UTableDescriptor> _spTableDescriptor,_bool pass)
{
	// Settings 
	if (pass) {
		GetOutlineShader()->SetTableDescriptor(_spTableDescriptor);
	}
	else {
		GetNorPosShader()->SetTableDescriptor(_spTableDescriptor);
	}
	
	return S_OK;
}


void UPawn::Collision(CSHPTRREF<UPawn> _pEnemy, const _double& _dTimeDelta)
{

}

void UPawn::AddColliderInContainer(const _wstring& _wstrTag, const _wstring& _wstrColliderProto, const _float3& _vColliderPos, const _float3& _vColliderScale)
{
	UCollider::COLLIDERDESC ColliderDesc{ _vColliderPos, _vColliderScale };
	SHPTR<UGameInstance> spGameInstance = GET_INSTANCE(UGameInstance);
	m_ColliderContainer.emplace(MakePair(_wstrTag, std::static_pointer_cast<UCollider>(spGameInstance->CloneComp(_wstrColliderProto,
		VOIDDATAS{ &ColliderDesc }))));
}

void UPawn::AddColliderInContainer(const _wstring& _wstrTag, CSHPTRREF<UCollider> _spCollider)
{
	assert(nullptr != _spCollider);
	m_ColliderContainer.emplace(MakePair(_wstrTag, _spCollider));
}

void UPawn::AddShader(const _wstring& _wstrProtoTag, const _wstring& _wstrTag, const VOIDDATAS& _vecDatas)
{
	m_spShader = AddResource<UShader>(_wstrProtoTag, _wstrTag, _vecDatas);
}
void UPawn::AddShadowShader(const _wstring& _wstrProtoTag, const _wstring& _wstrTag, const VOIDDATAS& _vecDatas)
{
	m_spShadowShader = AddResource<UShader>(_wstrProtoTag, _wstrTag, _vecDatas);
}

void UPawn::AddOutlineShader(const _wstring& _wstrProtoTag, const _wstring& _wstrTag, const VOIDDATAS& _vecDatas)
{
	m_spOutlineShader = AddResource<UShader>(_wstrProtoTag, _wstrTag, _vecDatas);
}

void UPawn::AddNorPosShader(const _wstring& _wstrProtoTag, const _wstring& _wstrTag, const VOIDDATAS& _vecDatas)
{
	m_spNorPosShader = AddResource<UShader>(_wstrProtoTag, _wstrTag, _vecDatas);
}


void UPawn::UpdateCollision()
{
	for (auto& iter : m_ColliderContainer)
	{
		iter.second->SetTransform(GetTransform()->GetWorldMatrix());
	}
}

#ifdef _USE_DEBUGGING
void UPawn::AddDebugRenderGroup(const DEBUGRENDERID _iRenderID)
{
	m_vecRenderingDatas.push_back(_iRenderID);
	m_spRenderer->AddDebugRenderGroup(_iRenderID, GetShader(), ThisShared<UPawn>());
	m_isDebugRenderingType = true;
}
#endif
#ifdef _USE_IMGUI

void UPawn::ShowObjectInfo()
{
	__super::ShowObjectInfo();
}

#endif 
