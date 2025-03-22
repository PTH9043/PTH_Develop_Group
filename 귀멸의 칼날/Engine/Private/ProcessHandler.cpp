#include "ProcessHandler.h"

CProcessHandler::CProcessHandler()
{
}

HRESULT CProcessHandler::NativeConstruct(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const PHDESC& _tPHDesc)

{
	D3D11_TEXTURE2D_DESC tTextureDesc;
	ZeroMemory(&tTextureDesc, sizeof(D3D11_TEXTURE2D_DESC));
	tTextureDesc.Width = (_uint)_tPHDesc.fWidth;
	tTextureDesc.Height = (_uint)_tPHDesc.fHeight;
	tTextureDesc.MipLevels = 1;
	tTextureDesc.ArraySize = 1;
	tTextureDesc.Format = DXGI_FORMAT_R16G16B16A16_TYPELESS;
	tTextureDesc.SampleDesc.Count = 1;
	tTextureDesc.SampleDesc.Quality = 0;
	tTextureDesc.Usage = D3D11_USAGE_DEFAULT;
	tTextureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	tTextureDesc.CPUAccessFlags = 0;
	tTextureDesc.MiscFlags = 0;

	D3D11_SHADER_RESOURCE_VIEW_DESC tSRVDesc;
	ZeroMemory(&tSRVDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
	tSRVDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
	tSRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	tSRVDesc.Texture2D.MipLevels = 1;

	D3D11_UNORDERED_ACCESS_VIEW_DESC tUAVDesc;
	ZeroMemory(&tUAVDesc, sizeof(D3D11_UNORDERED_ACCESS_VIEW_DESC));
	tUAVDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
	tUAVDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;

	if (FAILED(_pDevice->CreateTexture2D(&tTextureDesc, nullptr, &m_pTexture)))
		return E_FAIL;
	if (FAILED(_pDevice->CreateShaderResourceView(m_pTexture, &tSRVDesc, &m_pSRV)))
		return E_FAIL;
	if (FAILED(_pDevice->CreateUnorderedAccessView(m_pTexture, &tUAVDesc, &m_pUAV)))
		return E_FAIL;

    return S_OK;
}

CProcessHandler* CProcessHandler::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const PHDESC& _tPHDesc)
{
	CProcessHandler* pProcessHandler = new CProcessHandler();
	if (FAILED(pProcessHandler->NativeConstruct(_pDevice, _pContext, _tPHDesc)))
	{
		Safe_Release(pProcessHandler);
		MSG_BOX("pProcessHandler::Create To Failed");
	}
	return pProcessHandler;
}

void CProcessHandler::Free()
{
	__super::Free();

	Safe_Release_DV(m_pSRV);
	Safe_Release_DV(m_pUAV);
	Safe_Release_DV(m_pTexture);
	Safe_Release_DV(m_pBuffer);
}
