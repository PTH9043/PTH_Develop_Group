#include "Select_Item.h"
#include "Renderer.h"

CSelect_Item::CSelect_Item(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext)
	: CItem(_pDevice, _pContext)
{
}

CSelect_Item::CSelect_Item(const CSelect_Item& _rhs)
	: CItem(_rhs)
{
}

HRESULT CSelect_Item::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;
	return S_OK;
}

HRESULT CSelect_Item::NativeConstruct_Clone(void* pArg)
{
	RETURN_IFNOT(nullptr == pArg, E_FAIL);

	if (FAILED(__super::NativeConstruct_Clone(pArg)))
		return E_FAIL;

	m_fDepthValue = -1.f;

	return S_OK;
}

void CSelect_Item::Tick(const _double& _dTimeDelta)
{
	__super::Tick(_dTimeDelta);
	
	RenderItem += (_float)_dTimeDelta;
}

void CSelect_Item::Late_Tick(const _double& _dTimeDelta)
{
	//__super::Late_Tick(_dTimeDelta);
	
	if (nullptr != m_pRendererCom)
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
	}
}

HRESULT CSelect_Item::Render()
{
	if (RenderItem >= 4.5f && RenderModel == true)//////////
	{
		if (FAILED(__super::Render()))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CSelect_Item::SetUp_ConstantTable()
{
	if (FAILED(__super::SetUp_ConstantTable()))
		return E_FAIL;

	return S_OK;
}

HRESULT CSelect_Item::SetUp_Components()
{
	if (FAILED(__super::SetUp_Components()))
		return E_FAIL;
	return S_OK;
}

CSelect_Item* CSelect_Item::Create(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext)
{
	CSelect_Item* pInstance = new CSelect_Item(_pDevice, _pContext);
	CHECK_CREATE_PROTOTYPE_NOTVALUE(pInstance, "CSelect_Item::Create To Failed");
	return pInstance;
}

CGameObject* CSelect_Item::Clone(void* pArg)
{
	CSelect_Item* pInstance = new CSelect_Item(*this);
	CHECK_CREATE_CLONE(pInstance, pArg, "CSelect_Item::Clone To Failed");
	return pInstance;
}

void CSelect_Item::Free()
{
	__super::Free();
	
	//Safe_Release(m_pRendererCom);
}
