#include "ClientDefines.h"
#include "CAnubisStaff.h"


CAnubisStaff::CAnubisStaff(CSHPTRREF<UDevice> _spDevice, const _wstring& _wstrLayer, const CLONETYPE& _eCloneType)
	: UWeapon(_spDevice, _wstrLayer, _eCloneType, WEAPONTYPE::WEAPON_SWORD)
{
}

CAnubisStaff::CAnubisStaff(const CAnubisStaff& _rhs)
	:UWeapon(_rhs)
{
}

void CAnubisStaff::Free()
{
}

HRESULT CAnubisStaff::NativeConstruct()
{
	return __super::NativeConstruct();
}

HRESULT CAnubisStaff::NativeConstructClone(const VOIDDATAS& _Datas)
{
	if (FAILED(__super::NativeConstructClone(_Datas)))
		return E_FAIL;

	return S_OK;
}

void CAnubisStaff::TickActive(const _double& _dTimeDelta)
{
	__super::TickActive(_dTimeDelta);
}

void CAnubisStaff::LateTickActive(const _double& _dTimeDelta)
{
	__super::LateTickActive(_dTimeDelta);
}

HRESULT CAnubisStaff::RenderActive(CSHPTRREF<UCommand> _spCommand, CSHPTRREF<UTableDescriptor> _spTableDescriptor)
{
	return __super::RenderActive(_spCommand, _spTableDescriptor);
}

HRESULT CAnubisStaff::RenderShadowActive(CSHPTRREF<UCommand> _spCommand, CSHPTRREF<UTableDescriptor> _spTableDescriptor)
{
	return __super::RenderShadowActive(_spCommand, _spTableDescriptor);
}

HRESULT CAnubisStaff::RenderOutlineActive(CSHPTRREF<UCommand> _spCommand, CSHPTRREF<UTableDescriptor> _spTableDescriptor, _bool _pass)
{
	return __super::RenderOutlineActive(_spCommand, _spTableDescriptor, _pass);
}

void CAnubisStaff::Collision(CSHPTRREF<UPawn> _pEnemy, const _double& _dTimeDelta)
{
}
