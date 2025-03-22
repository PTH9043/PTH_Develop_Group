#include "CIMGUI.h"

CIMGUI::CIMGUI(ID3D11Device * _pDevice, ID3D11DeviceContext * _pContext)
	:  m_pDevice(_pDevice), m_pContext(_pContext)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

HRESULT CIMGUI::NativeConstruct()
{
	return S_OK;
}

HRESULT CIMGUI::NativeConstruct_Tool()
{
	return S_OK;
}

HRESULT CIMGUI::Tick(const _double & _dDeltaSeconds)
{
	if (false == m_bOpen)
		return E_FAIL;

	return S_OK;
}

HRESULT CIMGUI::Render()
{
	if (false == m_bOpen)
		return E_FAIL;

	return S_OK;
}

HRESULT CIMGUI::Release_Tool()
{
	return E_NOTIMPL;
}

void CIMGUI::Free()
{
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}
