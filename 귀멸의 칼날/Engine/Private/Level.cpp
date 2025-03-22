#include "..\Public\Level.h"

CLevel::CLevel(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: m_pDevice(_pDevice), m_pContext(_pContext)
{
	Safe_AddRef_DV(m_pDevice);
	Safe_AddRef_DV(m_pContext);
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
	Safe_Release_DV(m_pContext);
	Safe_Release_DV(m_pDevice);
}
