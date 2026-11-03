#include "CCamera.h"
#include "CMethod.h"
#include "CGameInstance.h"
#include "CTransform.h"
#include "CPipeLine.h"

shared_ptr<CActor> CCamera::Clone(const VOIDDATAS& _tDatas){
	return nullptr;
}

void CCamera::Free(){

}

HRESULT CCamera::NativeConstruct()
{
	return S_OK;
}

HRESULT CCamera::NativeConstruct(VOIDDATAS _tDatas)
{
	if (_tDatas.size() <= 0)
		return E_FAIL;

	if (FAILED(__super::NativeConstruct(_tDatas))) {
		return E_FAIL;
	}
	// Set active
	Set_Active(false);

	CAMDESC tCamDesc = CMethod::ConvertTemplate_PopBack<CAMDESC>(&_tDatas);

	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixPerspectiveFovLH(tCamDesc.fForv, 
		tCamDesc.fWinWidth / tCamDesc.fWinHeight, 	tCamDesc.fNear, tCamDesc.fFar));

	Get_Transform()->Set_PosF3(tCamDesc.vPos);
	Get_Transform()->LookAt(XMLoadFloat3(&tCamDesc.vLook));
	m_fCamSpeed = tCamDesc.fCamSpeed;
	return S_OK;
}

void CCamera::BindMatrix()
{
	shared_ptr<CGameInstance> pInstance = GET_INSTNACE(CGameInstance);
	pInstance->Set_CamPosition(Get_Transform()->Get_Pos());
	pInstance->Bind_Matrix4x4(MAT_VIEW ,Get_Transform()->Get_ParentsToWorldFloat4x4Inv());
	pInstance->Bind_Matrix4x4(MAT_PROJ, m_ProjMatrix);
}

void CCamera::BindMatrix(CPipeLine* _pPipeLine)
{
	if (false == Is_Active())
		return;

	_pPipeLine->Set_CamPosition(Get_Transform()->Get_Pos());
	_pPipeLine->Bind_Matrix4x4(MAT_VIEW, Get_Transform()->Get_ParentsToWorldFloat4x4Inv());
	_pPipeLine->Bind_Matrix4x4(MAT_PROJ, m_ProjMatrix);
}

HRESULT CCamera::Tick(const _double& _dTimeDelta)
{
	if (FAILED(__super::Tick(_dTimeDelta)))
		return E_FAIL;

	return S_OK;
}

HRESULT CCamera::Late_Tick(const _double& _dTimeDelta)
{
	if (FAILED(__super::Late_Tick(_dTimeDelta))) {
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CCamera::Render(const HDC& _hDC)
{
	return __super::Render(_hDC);
}