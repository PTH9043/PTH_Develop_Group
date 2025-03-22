#include "ClientDefines.h"
#include "CShuriken.h"


CShuriken::CShuriken(CSHPTRREF<UDevice> _spDevice, const _wstring& _wstrLayer, const CLONETYPE& _eCloneType)
	: UWeapon(_spDevice, _wstrLayer, _eCloneType, WEAPONTYPE::WEAPON_SWORD)
{
}

CShuriken::CShuriken(const CShuriken& _rhs)
	:UWeapon(_rhs)
{
}

void CShuriken::Free()
{
}

HRESULT CShuriken::NativeConstruct()
{
	return __super::NativeConstruct();
}

HRESULT CShuriken::NativeConstructClone(const VOIDDATAS& _Datas)
{
	if (FAILED(__super::NativeConstructClone(_Datas)))
		return E_FAIL;

	return S_OK;
}

void CShuriken::TickActive(const _double& _dTimeDelta)
{
	__super::TickActive(_dTimeDelta);
}

void CShuriken::LateTickActive(const _double& _dTimeDelta)
{
	__super::LateTickActive(_dTimeDelta);
}

HRESULT CShuriken::RenderActive(CSHPTRREF<UCommand> _spCommand, CSHPTRREF<UTableDescriptor> _spTableDescriptor)
{
	return __super::RenderActive(_spCommand, _spTableDescriptor);
}

HRESULT CShuriken::RenderShadowActive(CSHPTRREF<UCommand> _spCommand, CSHPTRREF<UTableDescriptor> _spTableDescriptor)
{
	return __super::RenderShadowActive(_spCommand, _spTableDescriptor);
}

HRESULT CShuriken::RenderOutlineActive(CSHPTRREF<UCommand> _spCommand, CSHPTRREF<UTableDescriptor> _spTableDescriptor, _bool _pass)
{
	return __super::RenderOutlineActive(_spCommand, _spTableDescriptor, _pass);
}

void CShuriken::Collision(CSHPTRREF<UPawn> _pEnemy, const _double& _dTimeDelta)
{
}
