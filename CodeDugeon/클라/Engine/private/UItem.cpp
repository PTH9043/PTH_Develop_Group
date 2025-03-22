#include "EngineDefine.h"
#include "UItem.h"
#include "UMethod.h"

BEGIN(Engine)

UItem::UItem(CSHPTRREF<UDevice> _spDevice, const _wstring& _wstrLayer, 
	const CLONETYPE& _eCloneType, const BACKINGTYPE _eBackingType, const ITEMTYPE _eItemType) : 
	UPawn(_spDevice, _wstrLayer, _eCloneType, _eBackingType), m_eItemType{_eItemType}
{
}

UItem::UItem(const UItem& _rhs) : 
	UPawn(_rhs), m_eItemType{_rhs.m_eItemType}
{
}


void UItem::Free()
{
}

HRESULT UItem::NativeConstruct()
{
	return __super::NativeConstruct();
}

HRESULT UItem::NativeConstructClone(const VOIDDATAS& _vecDatas)
{
	if (FAILED(__super::NativeConstructClone(_vecDatas)))
		return E_FAIL;

	return S_OK;
}

void UItem::TickActive(const _double& _dTimeDelta)
{
}

void UItem::LateTickActive(const _double& _dTimeDelta)
{
}

HRESULT UItem::RenderActive(CSHPTRREF<UCommand> _spCommand, CSHPTRREF<UTableDescriptor> _spTableDescriptor)
{
	return __super::RenderActive(_spCommand, _spTableDescriptor);
}

HRESULT UItem::RenderShadowActive(CSHPTRREF<UCommand> _spCommand, CSHPTRREF<UTableDescriptor> _spTableDescriptor)
{
	return __super::RenderShadowActive(_spCommand, _spTableDescriptor);
}

HRESULT UItem::RenderOutlineActive(CSHPTRREF<UCommand> _spCommand, CSHPTRREF<UTableDescriptor> _spTableDescriptor, _bool _pass)
{
	return __super::RenderOutlineActive(_spCommand, _spTableDescriptor,_pass);
}


void UItem::Collision(CSHPTRREF<UPawn> _pEnemy, const _double& _dTimeDelta)
{
}


END