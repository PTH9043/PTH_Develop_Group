#include "CIMGUI_MD.h"

CIMGUI_MD::CIMGUI_MD(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: m_pDevice(_pDevice), m_pContext(_pContext)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

HRESULT CIMGUI_MD::NativeConstruct()
{
	return S_OK;
}

HRESULT CIMGUI_MD::Tick(const _double& _dDeltaSeconds)
{
	if (false == m_bOpen)
		return E_FAIL;

	return S_OK;
}

HRESULT CIMGUI_MD::Render()
{
	if (false == m_bOpen)
		return E_FAIL;

	return S_OK;
}

void CIMGUI_MD::Show_Window()
{
}

void CIMGUI_MD::Free()
{
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}
