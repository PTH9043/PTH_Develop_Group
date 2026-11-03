#include "CControlHead.h"
#include "CGameInstance.h"
#include "CTransform.h"
#include "COBBCollider.h"
#include "CMethod.h"

shared_ptr<CActor> CControlHead::Clone(const VOIDDATAS& _tDatas)
{
	shared_ptr<CControlHead> pActor = CSmartPointerFactory<CControlHead>::CreateShared(*this);
	if (FAILED(pActor->NativeConstruct(_tDatas))) {
		Safe_Free(pActor);
		DEBUG_MSG("CControlHead::Clone To Failed");
	}
	return pActor;
}

void CControlHead::Free() {
	__super::Free();
}

HRESULT CControlHead::NativeConstruct()
{
	RETURN_CHECK_FAILED(__super::NativeConstruct(), E_FAIL);
	return S_OK;
}

HRESULT CControlHead::NativeConstruct(VOIDDATAS _tDatas)
{
	RETURN_CHECK_FAILED(__super::NativeConstruct(_tDatas), E_FAIL);

	return S_OK;
}

HRESULT CControlHead::CreateComponent()
{
	RETURN_CHECK_FAILED(__super::CreateComponent(), E_FAIL);
	return S_OK;
}

HRESULT CControlHead::Tick(const _double& _dTimeDelta)
{
	if (FAILED(__super::Tick(_dTimeDelta)))
		return E_FAIL;

	shared_ptr<CGameInstance> pGameInstance = GET_INSTNACE(CGameInstance);
	if (true == pGameInstance->Is_Pressing(KEYBOARD_A)) {
		Get_Transform()->Rotate_Turn(UP_VECTOR, _dTimeDelta, -m_fPosinRotateSpeed);
	}
	else if (true == pGameInstance->Is_Pressing(KEYBOARD_D)) {
		Get_Transform()->Rotate_Turn(UP_VECTOR, _dTimeDelta, m_fPosinRotateSpeed);
	}
	return S_OK;
}

HRESULT CControlHead::Late_Tick(const _double& _dTimeDelta)
{
	return __super::Late_Tick(_dTimeDelta);
}

HRESULT CControlHead::Render(const HDC& _hDC)
{
	return __super::Render(_hDC);
}
