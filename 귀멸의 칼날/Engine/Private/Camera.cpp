#include "Camera.h"
#include "PipeLine.h"
#include "Transform.h"
#include "Method.h"

CCamera::CCamera(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CGameObject(_pDevice, _pContext), m_pPipeLine(CPipeLine::Get_Instance())
{
	Safe_AddRef(m_pPipeLine);
}

CCamera::CCamera(const CCamera& _rhs)
	: CGameObject(_rhs), m_pPipeLine(_rhs.m_pPipeLine), m_strCamName(_rhs.m_strCamName)
{
	Safe_AddRef(m_pPipeLine);

	m_PrevViewMatrix = CMethod::XMMatrixIdentity_P();
	m_ProjMatrix = CMethod::XMMatrixIdentity_P();
}

void CCamera::Out_CameraDesc(CAMDESC*& _pOut)
{
	_pOut = &m_tCameraDesc;
}

void CCamera::Update_ProjMatrix()
{
	_matrix		ProjMatrix;
	ProjMatrix = XMMatrixPerspectiveFovLH(m_tCameraDesc.fFovy, m_tCameraDesc.fAspect, m_tCameraDesc.fNear, m_tCameraDesc.fFar);
	m_pPipeLine->Set_Transform(CPipeLine::D3DTS_PROJ, ProjMatrix);
	m_pPipeLine->Set_CamFar(m_tCameraDesc.fFar);

	m_ProjMatrix = CMethod::XMMatrixStoreFloat4x4_P(ProjMatrix);
}

HRESULT CCamera::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	_string str = typeid(*this).name();

	size_t iFind = str.find("CCamera");
	str = str.substr(iFind, str.length());

	m_strCamName = str;
	return S_OK;
}

HRESULT CCamera::NativeConstruct_Clone(void* pArg)
{
	if (FAILED(__super::NativeConstruct_Clone(pArg)))
		return E_FAIL;

	if (nullptr != pArg)
	{
		m_tCameraDesc = *static_cast<CAMDESC*>(pArg);
	}

	if (FAILED(__super::Ready_Transform(COM_TRANSFORM)))
		return E_FAIL;

	m_pTransformCom->Set_Pos(XMLoadFloat3(&m_tCameraDesc.v3Eye));
	m_pTransformCom->LookAt(XMLoadFloat3(&m_tCameraDesc.v3At));

	_matrix		ProjMatrix;
	ProjMatrix = XMMatrixPerspectiveFovLH(m_tCameraDesc.fFovy, m_tCameraDesc.fAspect, m_tCameraDesc.fNear, m_tCameraDesc.fFar);
	m_pPipeLine->Set_Transform(CPipeLine::D3DTS_PROJ, ProjMatrix);
	m_pPipeLine->Set_CamFar(m_tCameraDesc.fFar);
	m_ProjMatrix = CMethod::XMMatrixStoreFloat4x4_P(ProjMatrix);
	m_bActive = true;
	return S_OK;
}

void CCamera::Tick(const _double& _dTimeDelta)
{
	if (false == m_bActive)
		return;

	__super::Tick(_dTimeDelta);
	if (FAILED(Bind_Matrices()))
		return;
}

void CCamera::Late_Tick(const _double& _dTimeDelta)
{
	__super::Late_Tick(_dTimeDelta);
}

HRESULT CCamera::Render()
{
	return S_OK;
}

HRESULT CCamera::Bind_Matrices()
{
	if (false == m_bActive)
		return E_FAIL;

	if (nullptr == m_pPipeLine)
		return E_FAIL;

	/*카메라 월드행렬의 역행렬을 구한다. == 뷰행렬 */
	/* 장치에 바인드한다. */

	m_pPipeLine->Set_Transform(CPipeLine::D3DTS_PREVVIEW, CMethod::XMMatrixLoadFloat4x4_P(m_PrevViewMatrix));
	_matrix		ViewMatrix = XMMatrixInverse(nullptr, m_pTransformCom->Get_WorldMatrix());
	m_pPipeLine->Set_Transform(CPipeLine::D3DTS_VIEW, ViewMatrix);
	m_PrevViewMatrix = CMethod::XMMatrixStoreFloat4x4_P(ViewMatrix);

	m_pPipeLine->Set_Transform(CPipeLine::D3DTS_PROJ, CMethod::XMMatrixLoadFloat4x4_P(m_ProjMatrix));
	m_pPipeLine->Set_CamFar(m_tCameraDesc.fFar);

	return S_OK;
}

CGameObject* CCamera::Clone(void* _pArg)
{
	return nullptr;
}

void CCamera::Free()
{
	__super::Free();

	Safe_Release(m_pPipeLine);
}
