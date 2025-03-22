#include "ImGuiClass_Tool.h"

CImGuiClass_Tool::CImGuiClass_Tool(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: m_pDevice(_pDevice), m_pContext(_pContext)
{
	Safe_AddRef_DV(m_pDevice);
	Safe_AddRef_DV(m_pContext);
}

HRESULT CImGuiClass_Tool::NativeConstruct()
{
	return S_OK;
}

HRESULT CImGuiClass_Tool::NativeConstruct_InnerValue()
{
	return S_OK;
}

HRESULT CImGuiClass_Tool::Tick(const _double& _dTimeDelta)
{
	if (false == m_bOpen)
		return E_FAIL;

	return S_OK;
}

HRESULT CImGuiClass_Tool::Late_Tick(const _double& _dTimeDelta)
{
	if (false == m_bOpen)
		return E_FAIL;

	return S_OK;
}

HRESULT CImGuiClass_Tool::Render()
{
	if (false == m_bOpen)
		return E_FAIL;

	return S_OK;
}

HRESULT CImGuiClass_Tool::Release_InnerValue()
{
	return S_OK;
}

void CImGuiClass_Tool::Show_Window()
{
}

void CImGuiClass_Tool::Free()
{
	Safe_Release_DV(m_pDevice);
	Safe_Release_DV(m_pContext);
}

