#include "Sheath.h"
#include "Level_Story.h"

CSheath::CSheath(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext)
	: CItem(_pDevice, _pContext)
{
}

CSheath::CSheath(const CSheath& _rhs)
	: CItem(_rhs)
{
}

HRESULT CSheath::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	m_eItemType = ITEM_SHEATH;
	return S_OK;
}

HRESULT CSheath::NativeConstruct_Clone(void* pArg)
{
	RETURN_IFNOT(nullptr == pArg, E_FAIL);

	if (FAILED(__super::NativeConstruct_Clone(pArg)))
		return E_FAIL;

	return S_OK;
}

void CSheath::Tick(const _double& _dTimeDelta)
{
	__super::Tick(_dTimeDelta);
}

void CSheath::Late_Tick(const _double& _dTimeDelta)
{
	__super::Late_Tick(_dTimeDelta);
}

HRESULT CSheath::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CSheath::RenderLight()
{
	return S_OK;
}

HRESULT CSheath::SetUp_ConstantTable()
{
	if (FAILED(__super::SetUp_ConstantTable()))
		return E_FAIL;

	return S_OK;
}

HRESULT CSheath::SetUp_Components()
{
	if (FAILED(__super::SetUp_Components()))
		return E_FAIL;
	return S_OK;
}

CSheath* CSheath::Create(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext)
{
	CSheath* pInstance = new CSheath(_pDevice, _pContext);
	CHECK_CREATE_PROTOTYPE_NOTVALUE(pInstance, "CSheath::Create To Failed");
	return pInstance;
}

CGameObject* CSheath::Clone(void* pArg)
{
	CSheath* pInstance = new CSheath(*this);
	CHECK_CREATE_CLONE(pInstance, pArg, "CSword::Clone To Failed");
	return pInstance;
}

void CSheath::Free()
{
	__super::Free();
}
