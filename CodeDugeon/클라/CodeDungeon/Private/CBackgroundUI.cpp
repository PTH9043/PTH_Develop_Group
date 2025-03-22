#include "ClientDefines.h"
#include "CBackgroundUI.h"

CBackgroundUI::CBackgroundUI(CSHPTRREF<UDevice> _spDevice, const _wstring& _wstrLayer, const CLONETYPE& _eCloneType) : 
    UUserInterface(_spDevice, _wstrLayer, _eCloneType)
{
}

CBackgroundUI::CBackgroundUI(const CBackgroundUI& _rhs) :
    UUserInterface(_rhs)
{
}

HRESULT CBackgroundUI::NativeConstruct()
{
    return E_NOTIMPL;
}

HRESULT CBackgroundUI::NativeConstructClone(const VOIDDATAS& _vecDatas)
{
    return E_NOTIMPL;
}

void CBackgroundUI::TickActive(const _double& _dTimeDelta)
{
}

void CBackgroundUI::LateTickActive(const _double& _dTimeDelta)
{
}

HRESULT CBackgroundUI::RenderActive(CSHPTRREF<UCommand> _spCommand, CSHPTRREF<UTableDescriptor> _spTableDescriptor)
{
    return E_NOTIMPL;
}

HRESULT CBackgroundUI::ReadyRectTransform(const VOIDDATAS& _vecDatas)
{
    return E_NOTIMPL;
}

HRESULT CBackgroundUI::RenderShadowActive(CSHPTRREF<UCommand> _spCommand, CSHPTRREF<UTableDescriptor> _spTableDescriptor)
{
    return E_NOTIMPL;
}

HRESULT CBackgroundUI::RenderOutlineActive(CSHPTRREF<UCommand> _spCommand, CSHPTRREF<UTableDescriptor> _spTableDescriptor, _bool _pass)
{
    return E_NOTIMPL;
}

void CBackgroundUI::Collision(CSHPTRREF<UPawn> _pEnemy, const _double& _dTimeDelta)
{
}

void CBackgroundUI::Free()
{
}
