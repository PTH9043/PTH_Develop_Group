#include "ImGuiClass.h"

CImGuiClass::CImGuiClass(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: m_pDevice(_pDevice), m_pContext(_pContext)
{
	Safe_AddRef_DV(m_pDevice);
	Safe_AddRef_DV(m_pContext);
}

HRESULT CImGuiClass::NativeConstruct()
{

	return S_OK;
}

HRESULT CImGuiClass::NativeConstruct_InnerValue()
{
	return S_OK;
}

HRESULT CImGuiClass::Tick(const _double& _dTimeDelta)
{
	if (false == m_bOpen)
		return E_FAIL;

	return S_OK;
}

HRESULT CImGuiClass::Late_Tick(const _double& _dTimeDelta)
{
	if (false == m_bOpen)
		return E_FAIL;

	return S_OK;
}

HRESULT CImGuiClass::Render()
{
	if (false == m_bOpen)
		return E_FAIL;

	return S_OK;
}

HRESULT CImGuiClass::Release_InnerValue()
{
	return E_NOTIMPL;
}

void CImGuiClass::Show_Window()
{
}

void CImGuiClass::Free()
{
	Safe_Release_DV(m_pDevice);
	Safe_Release_DV(m_pContext);
}

