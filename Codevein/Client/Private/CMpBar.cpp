#include "CMpBar.h"
#include "CGameInstance.h"

CMpBar::CMpBar(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext)
	: CBar(_pDevice, _pContext)
{
}

CMpBar::CMpBar(const CMpBar& _rhs)
	: CBar(_rhs)
{
}

HRESULT CMpBar::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return S_OK;
	return E_FAIL;
}

HRESULT CMpBar::NativeConstruct_Clone(void* pArg)
{
	if (FAILED(__super::NativeConstruct_Clone(pArg)))
		return E_FAIL;

	return S_OK;
}

void CMpBar::Tick(const _double& _dTimeDelta)
{
	__super::Tick(_dTimeDelta);
}

void CMpBar::Late_Tick(const _double& _dTimeDelta)
{
	__super::Late_Tick(_dTimeDelta);
}
_float f = 10.f;
HRESULT CMpBar::Render()
{
	if (FAILED(SetUp_ConstantTable(m_tBarDesc.strBarBack.c_str(), m_pBackRectTransform, m_pBackTransform )))
		return E_FAIL;

	m_pShaderCom->Begin(0);
	m_pVIBackRectCom->Render();

	if (FAILED(SetUp_ConstantTable(m_tUIDesc.strImgName.c_str(), m_pRectTransformCom, m_pTransformCom)))
		return E_FAIL;


	m_pShaderCom->Begin(1);
	m_pVIRectCom->Render();
	return S_OK;
}

CMpBar* CMpBar::Create(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext)
{
	CMpBar* pInstance = new CMpBar(_pDevice, _pContext);
	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		Safe_Release(pInstance);
		MSG_BOX("CMpBar::Create To Failed");
	}
	return pInstance;
}

CGameObject* CMpBar::Clone(void* _pArg)
{
	CMpBar* pInstance = new CMpBar(*this);
	if (FAILED(pInstance->NativeConstruct_Clone(_pArg)))
	{
		Safe_Release(pInstance);
		MSG_BOX("CMpBar::Clone To Failed");
	}
	return pInstance;
}

void CMpBar::Free()
{
	__super::Free();
}
