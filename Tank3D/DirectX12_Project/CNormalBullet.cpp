#include "CNormalBullet.h"

shared_ptr<CActor> CNormalBullet::Clone(const VOIDDATAS& _tDatas)
{
	shared_ptr<CNormalBullet> pTankShooter = CSmartPointerFactory<CNormalBullet>::CreateShared(*this);
	if (FAILED(pTankShooter->NativeConstruct(_tDatas))) {
		Safe_Free(pTankShooter);
		DEBUG_MSG("CNormalBullet::Clone To Failed");
	}
	return pTankShooter;
}

void CNormalBullet::Free()
{
	__super::Free();
}

HRESULT CNormalBullet::NativeConstruct()
{
	RETURN_CHECK_FAILED(__super::NativeConstruct(), E_FAIL);
	return S_OK;
}

HRESULT CNormalBullet::NativeConstruct(VOIDDATAS _tDatas)
{
	RETURN_CHECK_FAILED(__super::NativeConstruct(_tDatas), E_FAIL);
	return S_OK;
}

HRESULT CNormalBullet::CreateComponent()
{
	RETURN_CHECK_FAILED(__super::CreateComponent(), E_FAIL);
	return S_OK;
}

HRESULT CNormalBullet::Tick(const _double& _dTimeDelta)
{
	return __super::Tick(_dTimeDelta);
}

HRESULT CNormalBullet::Late_Tick(const _double& _dTimeDelta)
{
	return __super::Late_Tick(_dTimeDelta);
}

HRESULT CNormalBullet::Render(const HDC& _hDC)
{
	return __super::Render(_hDC);
}

_float CNormalBullet::GiveDamage()
{
	return __super::GiveDamage();
}
