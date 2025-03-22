#include "..\Public\Component.h"

CComponent::CComponent(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: m_pDevice(_pDevice), m_pContext(_pContext)
	, m_bCloned(false)
{
	Safe_AddRef_DV(m_pDevice);
	Safe_AddRef_DV(m_pContext);
}

CComponent::CComponent(const CComponent& Prototype)
	: m_pDevice(Prototype.m_pDevice), m_pContext(Prototype.m_pContext)
	, m_bCloned(true)
{
	Safe_AddRef_DV(m_pDevice);
	Safe_AddRef_DV(m_pContext);
}

HRESULT CComponent::NativeConstruct_Prototype()
{
	return S_OK;
}

HRESULT CComponent::NativeConstruct_Clone(void* pArg)
{
	m_bCloned = true;
	return S_OK;
}

void CComponent::Free()
{
	Safe_Release_DV(m_pDevice);
	Safe_Release_DV(m_pContext);
}
