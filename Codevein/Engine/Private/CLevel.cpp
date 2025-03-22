#include "..\Public\CLevel.h"

CLevel::CLevel(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: m_pDevice(_pDevice), m_pContext(_pContext)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

HRESULT CLevel::NativeConstruct()
{

	return S_OK;
}

void CLevel::Tick(const _double& _dTimeDelta)
{
}

void CLevel::Late_Tick(const _double& _dTimeDelta)
{
}

HRESULT CLevel::Render()
{
	return S_OK;
}

void CLevel::Free()
{
	Safe_Release(m_pContext);
	Safe_Release(m_pDevice);
}
