#include "CMainCamera.h"
#include "CMethod.h"
#include "CGameInstance.h"
#include "CTransform.h"
#include "CPipeLine.h"

shared_ptr<CActor> CMainCamera::Clone(const VOIDDATAS& _tDatas)
{
	shared_ptr<CMainCamera> pInstance = CSmartPointerFactory<CMainCamera>::CreateShared();
	if (FAILED(pInstance->NativeConstruct(_tDatas))) {
		Safe_Free(pInstance);
		DEBUG_MSG("CMainCamera::Failed To CloneActor");
	}
	return pInstance;
}

void CMainCamera::Free()
{
	__super::Free();
}

HRESULT CMainCamera::NativeConstruct()
{
	return __super::NativeConstruct();
}

HRESULT CMainCamera::NativeConstruct(VOIDDATAS _tDatas)
{
	return __super::NativeConstruct(_tDatas);
}

HRESULT CMainCamera::Tick(const _double& _dTimeDelta)
{
	if (FAILED(__super::Tick(_dTimeDelta)))
		return E_FAIL;
	return S_OK;
}

HRESULT CMainCamera::Late_Tick(const _double& _dTimeDelta)
{
	if (FAILED(__super::Late_Tick(_dTimeDelta)))
		return E_FAIL;


	ChaseToPosition(_dTimeDelta);
	return S_OK;
}

HRESULT CMainCamera::Render(const HDC& _hDC)
{
	return __super::Render(_hDC);
}

void CMainCamera::ChaseToPosition(const _double& _dTimeDelta)
{
	if (nullptr == m_pMoveTR.lock())
		return;

	//Get_Transform()->Translate_Pos(m_pMoveTR.lock()->Get_OriginPos(), _dTimeDelta, Get_CamSpeed(), 0.2f);
	Get_Transform()->Set_Pos(m_pMoveTR.lock()->Get_OriginPos());
}
