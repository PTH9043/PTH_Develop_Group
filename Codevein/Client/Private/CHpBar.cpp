#include "CHpBar.h"
#include "CGameInstance.h"

CHpBar::CHpBar(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext)
	: CBar(_pDevice, _pContext)
{
}

CHpBar::CHpBar(const CHpBar& _rhs)
	: CBar(_rhs)
{
}

HRESULT CHpBar::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return S_OK;
	return E_FAIL;
}

HRESULT CHpBar::NativeConstruct_Clone(void* pArg)
{
	if (FAILED(__super::NativeConstruct_Clone(pArg)))
		return E_FAIL;

	return S_OK;
}

void CHpBar::Tick(const _double& _dTimeDelta)
{
	__super::Tick(_dTimeDelta);
}

void CHpBar::Late_Tick(const _double& _dTimeDelta)
{
	__super::Late_Tick(_dTimeDelta);
}

HRESULT CHpBar::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;
	return S_OK;
}

CHpBar* CHpBar::Create(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext)
{
	CHpBar* pInstance = new CHpBar(_pDevice, _pContext);
	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		Safe_Release(pInstance);
		MSG_BOX("CHpBar::Create To Failed");
	}
	return pInstance;
}

CGameObject* CHpBar::Clone(void* _pArg)
{
	CHpBar* pInstance = new CHpBar(*this);
	if (FAILED(pInstance->NativeConstruct_Clone(_pArg)))
	{
		Safe_Release(pInstance);
		MSG_BOX("CHpBar::Clone To Failed");
	}
	return pInstance;
}

void CHpBar::Free()
{
	__super::Free();
}
