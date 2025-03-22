#include "EngineDefine.h"
#include "UActor.h"
#include "UMethod.h"
#include "UGameInstance.h"
#include "UTransform.h"

UActor::UActor(CSHPTRREF<UDevice> _spDevice, const _wstring& _wstrLayer,
	const CLONETYPE& _eCloneType, const BACKINGTYPE _eBackingType,
	const USECOLLISIONTYPE _eCollType)
	: UCloneObject(_spDevice),
	m_Components{},
	m_Resources{},
	m_wpParentsActor{},
	m_spTranfsorm{ nullptr },
	m_spCloneData{ nullptr },
	m_isActive{ true },
	m_isTickActive{ true },
	m_isRenderActive{ true },
	m_isOutlineRenderActive{ true },
	m_isShadowRenderActive{ false },
	m_pAwakeTick{ &UActor::AwakeTickActive },
	m_pTick{ &UActor::TickActive },
	m_pLateTick{ &UActor::LateTickActive },
	m_pNetworkTick{&UActor::NetworkTickActive},
	m_pRender{ &UActor::RenderActive },
	m_pShadowRender{ &UActor::RenderShadowActive },
	m_pOutlineRender{ &UActor::RenderOutlineActive },
	m_eBackingType{ _eBackingType },
	m_eUseCollType{ _eCollType },
	m_iNetworkID{0}
{
	ReadyCloneType(_wstrLayer, _eCloneType);
}

UActor::UActor(const UActor& _rhs)
	: UCloneObject(_rhs), m_spCloneData(_rhs.m_spCloneData),
	m_spTranfsorm{ nullptr },
	m_isActive{ true },
	m_isTickActive{ true },
	m_isRenderActive{ true },
	m_isShadowRenderActive{ false },
	m_isOutlineRenderActive{ true },
	m_pAwakeTick{ &UActor::AwakeTickActive },
	m_pTick{ &UActor::TickActive },
	m_pLateTick{ &UActor::LateTickActive },
	m_pNetworkTick{ &UActor::NetworkTickActive },
	m_pRender{ &UActor::RenderActive },
	m_pShadowRender{ &UActor::RenderShadowActive },
	m_pOutlineRender{ &UActor::RenderOutlineActive },
	m_Components{},
	m_Resources{},
	m_wpParentsActor{},
	m_eBackingType{ _rhs.m_eBackingType },
	m_eUseCollType{ _rhs.m_eUseCollType },
	m_iNetworkID{ 0 }
{
}

void UActor::SetActive(const _bool _isActvie)
{
	if (true == _isActvie)
	{
		m_pAwakeTick = &UActor::AwakeTickActive;
		m_pTick = &UActor::TickActive;
		m_pLateTick = &UActor::LateTickActive;
		m_pNetworkTick = &UActor::NetworkTickActive;
		m_pRender = &UActor::RenderActive;
	}
	else
	{
		m_pAwakeTick = &UActor::AwakeTickNonActive;
		m_pTick = &UActor::TickNonActive;
		m_pLateTick = &UActor::LateTickNonActive;
		m_pNetworkTick = &UActor::NetowkrTickNonActive;
		m_pRender = &UActor::RenderNonActive;
	}
	m_isActive = _isActvie;
}

void UActor::SetTickActive(const _bool _isActvie)
{
	if (true == _isActvie)
	{
		m_pAwakeTick = &UActor::AwakeTickActive;
		m_pTick = &UActor::TickActive;
		m_pLateTick = &UActor::LateTickActive;
		m_pNetworkTick = &UActor::NetworkTickActive;
	}
	else
	{
		m_pAwakeTick = &UActor::AwakeTickNonActive;
		m_pTick = &UActor::TickNonActive;
		m_pLateTick = &UActor::LateTickNonActive;
		m_pNetworkTick = &UActor::NetowkrTickNonActive;
	}
	m_isTickActive = _isActvie;
}

void UActor::SetRenderActive(const _bool _isActvie)
{
	if (true == _isActvie)
	{
		m_pRender = &UActor::RenderActive;
	}
	else
	{
		m_pRender = &UActor::RenderNonActive;
	}
	m_isRenderActive = _isActvie;
}

void UActor::SetShadowRenderActive(const _bool _isActvie)
{
	if (true == _isActvie)
	{
		
		m_pShadowRender = &UActor::RenderShadowActive;
	}
	else
	{
		
		m_pShadowRender = &UActor::RenderShadowNonActive;
	}
	m_isShadowRenderActive = _isActvie;
}

void UActor::SetOutlineRenderActive(const _bool _isActvie)
{
	if (true == _isActvie)
	{

		m_pOutlineRender = &UActor::RenderOutlineActive;
	}
	else
	{

		m_pOutlineRender = &UActor::RenderOutlineNonActive;
	}
	m_isOutlineRenderActive = _isActvie;
}

void UActor::SetParentsActor(CSHPTRREF<UActor> _spActor)
{
	RETURN_CHECK(nullptr == _spActor, ;);
	m_wpParentsActor = _spActor;
}

SHPTR<UComponent> UActor::GetComponent(const _wstring& _wstrTag)
{
	const auto& iter = m_Components.find(_wstrTag);
	RETURN_CHECK(iter == m_Components.end(), nullptr;);
	return iter->second;
}

SHPTR<UResource> UActor::GetResource(const _wstring& _wstrTag)
{
	const auto& iter = m_Resources.find(_wstrTag);
	RETURN_CHECK(iter == m_Resources.end(), nullptr;);
	return iter->second;
}

void UActor::Free()
{
	m_spTranfsorm.reset();

	m_Components.clear();
	m_Resources.clear();
}

HRESULT UActor::NativeConstruct()
{
	RETURN_CHECK_FAILED(__super::NativeConstruct(), E_FAIL);
	return S_OK;
}

HRESULT UActor::NativeConstructClone(const VOIDDATAS& _stDatas)
{
	RETURN_CHECK_FAILED(__super::NativeConstructClone(_stDatas), E_FAIL);
	return ReadyTransform();
}

void UActor::ReceiveNetworkProcessData(const UProcessedData& _ProcessData)
{

}

void UActor::AwakeTickActive(const _double& _dTimeDelta)
{

}

void UActor::TickActive(const _double& _dTimeDelta)
{
}

void UActor::LateTickActive(const _double& _dTimeDelta)
{
}

void UActor::NetworkTickActive(const _double&)
{
}

HRESULT UActor::RenderActive(CSHPTRREF<UCommand> _spCommand, CSHPTRREF<UTableDescriptor> _spTableDescriptor)
{
	return S_OK;
}
HRESULT UActor::RenderShadowActive(CSHPTRREF<UCommand> _spCommand, CSHPTRREF<UTableDescriptor> _spTableDescriptor)
{
	return S_OK;
}
HRESULT UActor::RenderOutlineActive(CSHPTRREF<UCommand> _spCommand, CSHPTRREF<UTableDescriptor> _spTableDescriptor, _bool _pass)
{
	return S_OK;
}

HRESULT UActor::ReadyTransform()
{
	m_spTranfsorm = CreateConstructorToNative<UTransform>(GetDevice());
	m_Components.insert(std::pair<_wstring, SHPTR<UComponent>>(COM_TRANFSORM, m_spTranfsorm));
	RETURN_CHECK(nullptr == m_spTranfsorm, E_FAIL);
	return S_OK;
}

HRESULT UActor::CreateResource(const _wstring& _wstrProtoTag, const _wstring& _strResTag, SHPTR<UResource>& _pResource, const VOIDDATAS& _vecDatas)
{
	SHPTR<UGameInstance> pGameInstance = GET_INSTANCE(UGameInstance);
	_pResource = pGameInstance->CloneResource(_wstrProtoTag, _vecDatas);
	m_Resources.insert(MakePair(_strResTag, _pResource));
	return S_OK;
}

HRESULT UActor::CreateComponent(const _wstring& _strProtoTag, const _wstring& _wstrTag, SHPTR<UComponent>& _pComponent, const VOIDDATAS& _vecDatas)
{
	SHPTR<UGameInstance> pGameInstance = GET_INSTANCE(UGameInstance);
	_pComponent = pGameInstance->CloneComp(_strProtoTag, _vecDatas);
	m_Components.insert(MakePair(_wstrTag, _pComponent));
	return S_OK;
}

HRESULT UActor::ReadyCloneType(const _wstring& _strCloneLayer, const CLONETYPE _eCloneType)
{
	if (nullptr != m_spCloneData) {
		return E_FAIL;
	}
	m_spCloneData = std::make_shared<const CLONEDATA>(CLONEDATA{ _strCloneLayer, static_cast<_ubyte>(_eCloneType) });
	return S_OK;
}

#ifdef _USE_IMGUI
void UActor::ShowObjectInfo()
{
	SHPTR<UActor> spParentsActor = m_wpParentsActor.lock();
	if (nullptr != spParentsActor)
	{
		static const char* PARENTS{ "ParentsActor" };
		ImGui::Text(PARENTS);
		spParentsActor->ShowObjectInfo();
	}
	const char* ACTORINFO{ "ActorInfo" };
	ImGui::Text(ACTORINFO);
	if (ImGui::TreeNode(ACTORINFO))
	{
		const char* ACTIVE_ALL{ "ActiveAll" };
		const char* TICK_ACTIVE{ "TickActive" };
		const char* RENDER_ACTIVE{ "RenderActive" };
		const char* BACKINGTYPENAME{ "BackingType" };
		// CloneType
		ImGui::Text(CLONETYPE_STRING);
		if ((_ubyte)CLONETYPE::CLONE_ONCE == m_spCloneData->bCloneType)
		{
			ImGui::Text(CLONETYPE_ONCE_STRING);
		}
		else
		{
			ImGui::Text(CLONETYPE_STATIC_STRING);
		}
		// Actives 
		{
			if (true == ImGui::Checkbox(ACTIVE_ALL, &m_isActive))
			{
				SetActive(m_isActive);
			}
			ImGui::SameLine();
			if (true == ImGui::Checkbox(TICK_ACTIVE, &m_isTickActive))
			{
				SetTickActive(m_isTickActive);
			}
			if (true == ImGui::Checkbox(RENDER_ACTIVE, &m_isRenderActive))
			{
				SetRenderActive(m_isRenderActive);
			}
		}
		static const char* const BACKINGS[]{ "Static",  "Dynamic", "Non" };
		static const _uint ITEM_SIZE = 3;
		static _int iSelect{ 0 };
		if (true == ImGui::ListBox(BACKINGTYPENAME, &iSelect, BACKINGS, ITEM_SIZE))
		{
			m_eBackingType = static_cast<BACKINGTYPE>(iSelect);
		}
		m_spTranfsorm->ShowObjectInfo();

		ImGui::TreePop();
	}
}
#endif