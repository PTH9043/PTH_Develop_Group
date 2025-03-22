#include "ClientDefines.h"
#include "CSword.h"

CSword::CSword(CSHPTRREF<UDevice> _spDevice, const _wstring& _wstrLayer, const CLONETYPE& _eCloneType) :
	UWeapon(_spDevice, _wstrLayer, _eCloneType, WEAPONTYPE::WEAPON_SWORD)
{
}

CSword::CSword(const CSword& _rhs) :
	UWeapon(_rhs)
{
}

void CSword::Free()
{
}

HRESULT CSword::NativeConstruct()
{
	return __super::NativeConstruct();
}

HRESULT CSword::NativeConstructClone(const VOIDDATAS& _Datas)
{
	if (FAILED(__super::NativeConstructClone(_Datas)))
		return E_FAIL;

	return S_OK;
}

void CSword::TickActive(const _double& _dTimeDelta)
{
	__super::TickActive(_dTimeDelta);
}

void CSword::LateTickActive(const _double& _dTimeDelta)
{
	__super::LateTickActive(_dTimeDelta);
}

HRESULT CSword::RenderActive(CSHPTRREF<UCommand> _spCommand, CSHPTRREF<UTableDescriptor> _spTableDescriptor)
{
	return __super::RenderActive(_spCommand, _spTableDescriptor);
}

HRESULT CSword::RenderShadowActive(CSHPTRREF<UCommand> _spCommand, CSHPTRREF<UTableDescriptor> _spTableDescriptor)
{
	return __super::RenderShadowActive(_spCommand, _spTableDescriptor);
}

HRESULT CSword::RenderOutlineActive(CSHPTRREF<UCommand> _spCommand, CSHPTRREF<UTableDescriptor> _spTableDescriptor, _bool _pass)
{
	return __super::RenderOutlineActive(_spCommand, _spTableDescriptor,_pass);
}

void CSword::Collision(CSHPTRREF<UPawn> _pEnemy, const _double& _dTimeDelta)
{
}
