#include "CCamera_Logo.h"

CCamera_Logo::CCamera_Logo(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CCamera(_pDevice, _pContext)
{
}

CCamera_Logo::CCamera_Logo(const CCamera_Logo& _rhs)
	: CCamera(_rhs)
{
}

HRESULT CCamera_Logo::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCamera_Logo::NativeConstruct_Clone(void* _pArg)
{
	if (FAILED(__super::NativeConstruct_Clone(_pArg)))
		return E_FAIL;

	return S_OK;
}

void CCamera_Logo::Tick(const _double& _dTimeDelta)
{
	__super::Tick(_dTimeDelta);
}

void CCamera_Logo::Late_Tick(const _double& _dTimeDelta)
{
	__super::Late_Tick(_dTimeDelta);
}

HRESULT CCamera_Logo::Render()
{
	return S_OK;
}

CCamera_Logo* CCamera_Logo::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CCamera_Logo* pInstance = new CCamera_Logo(_pDevice, _pContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("CCamera_Logo::Create to Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CCamera_Logo::Clone(void* pArg)
{
	CCamera_Logo* pInstance = new CCamera_Logo(*this);

	if (FAILED(pInstance->NativeConstruct_Clone(pArg)))
	{
		MSG_BOX("CCamera_Tool::Clone to Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CCamera_Logo::Free()
{
	__super::Free();
}
