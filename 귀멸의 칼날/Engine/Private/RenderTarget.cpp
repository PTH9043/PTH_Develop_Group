#include "RenderTarget.h"
#include "Shader.h"
#include "VIBuffer_Rect.h"

CRenderTarget::CRenderTarget(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: m_pDevice(_pDevice), m_pContext(_pContext)
{
	Safe_AddRef_DV(m_pDevice);
	Safe_AddRef_DV(m_pContext);
//#ifdef _DEBUG
	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixIdentity());
//#endif
}

HRESULT CRenderTarget::NativeConstruct(const RENTDESC& _tRedDesc)
{
	m_vClearColor = _tRedDesc.vClearColor;

	ID3D11Texture2D* pTexture = nullptr;

	D3D11_TEXTURE2D_DESC	TextureDesc;
	ZeroMemory(&TextureDesc, sizeof(D3D11_TEXTURE2D_DESC));

	TextureDesc.Width = (_uint)(_tRedDesc.fWIdth);
	TextureDesc.Height = (_uint)(_tRedDesc.fHeight);
	TextureDesc.MipLevels = 1;
	TextureDesc.ArraySize = 1;
	TextureDesc.Format = _tRedDesc.eFormat;

	TextureDesc.SampleDesc.Quality = 0;
	TextureDesc.SampleDesc.Count = 1;
	TextureDesc.Usage = D3D11_USAGE_DEFAULT;
	TextureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	TextureDesc.CPUAccessFlags = 0;
	TextureDesc.MiscFlags = 0;

	if (FAILED(m_pDevice->CreateTexture2D(&TextureDesc, nullptr, &m_pTexture)))
		return E_FAIL;

	// Render Target View 
	if (FAILED(m_pDevice->CreateRenderTargetView(m_pTexture, nullptr, &m_pRTV)))
		return E_FAIL;

	// Shader Resource View
	if (FAILED(m_pDevice->CreateShaderResourceView(m_pTexture, nullptr, &m_pSRV)))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderTarget::Clear()
{
	if (nullptr == m_pContext)
		return E_FAIL;

	m_pContext->ClearRenderTargetView(m_pRTV, (_float*)&m_vClearColor);

	return S_OK;
}


//#ifdef _DEBUG
HRESULT CRenderTarget::Render(CShader* pShader, CVIBuffer_Rect* pVIBuffer)
{
	pShader->Set_RawValue("g_WorldMatrix", &m_WorldMatrix, sizeof(_float4x4));

	pShader->Set_SRV("g_Texture", m_pSRV);

	pShader->Begin(0);

	pVIBuffer->Render();
	return S_OK;
}

HRESULT CRenderTarget::Ready_Debug_Desc(const _float2& _v2Pos, const _float2& _v2Size)
{
	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixIdentity());

	m_WorldMatrix._11 = _v2Size.x;
	m_WorldMatrix._22 = _v2Size.y;

	D3D11_VIEWPORT		Viewport;
	ZeroMemory(&Viewport, sizeof(D3D11_VIEWPORT));

	_uint		iNumViewports = 1;
	m_pContext->RSGetViewports(&iNumViewports, &Viewport);

	m_WorldMatrix._41 = _v2Pos.x + _v2Size.x * 0.5f - Viewport.Width * 0.5f;
	m_WorldMatrix._42 = -(_v2Pos.y + _v2Size.y * 0.5f) + Viewport.Height * 0.5f;

	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixTranspose(XMLoadFloat4x4(&m_WorldMatrix)));

	return S_OK;
}
//#endif 

CRenderTarget* CRenderTarget::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const RENTDESC& _tRedDesc)
{
	CRenderTarget* pRenderTarget = new CRenderTarget(_pDevice, _pContext);
	if (FAILED(pRenderTarget->NativeConstruct(_tRedDesc)))
	{
		Safe_Release(pRenderTarget);
		MSG_BOX("RenderTarget::Create To Failed");
	}
	return pRenderTarget;
}

void CRenderTarget::Free()
{
	Safe_Release_DV(m_pRTV);
	Safe_Release_DV(m_pSRV);
	Safe_Release_DV(m_pTexture);
	Safe_Release_DV(m_pDevice);
	Safe_Release_DV(m_pContext);
}
