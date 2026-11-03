#include "CPosin.h"
#include "CCubeBuffer.h"
#include "CMethod.h"
#include "CTransform.h"
#include "CRenderer.h"
#include "COBBCollider.h"
#include "CGameInstance.h"

shared_ptr<CActor> CPosin::Clone(const VOIDDATAS& _tDatas)
{
	shared_ptr<CPosin> pActor = CSmartPointerFactory<CPosin>::CreateShared(*this);
	if (FAILED(pActor->NativeConstruct(_tDatas))) {
		Safe_Free(pActor);
		DEBUG_MSG("CPosin::Clone To Failed");
	}
	return pActor;
}

void CPosin::Free()
{
	__super::Free();
}

HRESULT CPosin::NativeConstruct()
{
	return S_OK;
}

HRESULT CPosin::NativeConstruct(VOIDDATAS _tDatas)
{
	if (FAILED(__super::NativeConstruct(_tDatas))) {
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CPosin::CreateComponent()
{
	if (FAILED(__super::CreateComponent())) {
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CPosin::Tick(const _double& _dTimeDelta) {

	if (FAILED(__super::Tick(_dTimeDelta)))
		return E_FAIL;

	shared_ptr<CGameInstance> pGameInstance = GET_INSTNACE(CGameInstance);

	if (true == pGameInstance->Is_Pressing(KEYBOARD_W)) {
		if(IsCheckOverAngleUp())
			Get_Transform()->Rotate_Turn(RIGHT_VECTOR, _dTimeDelta, -1.f);
	}
	else if (true == pGameInstance->Is_Pressing(KEYBOARD_S)) {

		if(IsCheckOverAngleDown())
			Get_Transform()->Rotate_Turn(RIGHT_VECTOR, _dTimeDelta, 1.f);
	}

	return S_OK;
}

HRESULT CPosin::Late_Tick(const _double& _dTimeDelta){
	return __super::Late_Tick(_dTimeDelta);
}

HRESULT CPosin::Render(const HDC& _hDC) {
	return __super::Render(_hDC);
}

_bool CPosin::IsCheckOverAngleUp()
{
	OUTMATRIX tOutMatrix = CMethod::Out_Matrix(Get_Transform()->Get_ParentsToWorldMatrix());
	m_fShootAngle = tOutMatrix.vRot.x;
	if (tOutMatrix.vRot.x <= 30.f && tOutMatrix.vRot.x >= -30.f) {
		return true;
	}
	return false;
}

_bool CPosin::IsCheckOverAngleDown()
{
	OUTMATRIX tOutMatrix = CMethod::Out_Matrix(Get_Transform()->Get_ParentsToWorldMatrix());
	m_fShootAngle = tOutMatrix.vRot.x;
	if (tOutMatrix.vRot.x <= 10.f) {
		return true;
	}
	return false;
}

