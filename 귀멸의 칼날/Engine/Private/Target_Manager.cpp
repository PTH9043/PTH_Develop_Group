#include "Target_Manager.h"
#include "Shader.h"
#include "VIBuffer_Rect.h"
#include "Texture.h"

IMPLEMENT_SINGLETON(CTarget_Manager)


CTarget_Manager::CTarget_Manager()
{
//#ifdef _DEBUG
	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixIdentity());
//#endif // _DEBUG
}

HRESULT CTarget_Manager::NativeConstruct(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
//#ifdef _DEBUG
	m_pShader = CShader::Create(_pDevice, _pContext, TEXT("Shader_Deferred.hlsl"), VTXTEX_DECLARATION::Element, VTXTEX_DECLARATION::iNumElement);
	if (nullptr == m_pShader)
		return E_FAIL;

	m_pVIBuffer = CVIBuffer_Rect::Create(_pDevice, _pContext);
	if (nullptr == m_pVIBuffer)
		return E_FAIL;


	D3D11_VIEWPORT		Viewport;
	ZeroMemory(&Viewport, sizeof(D3D11_VIEWPORT));

	_uint		iNumViewports = 1;
	_pContext->RSGetViewports(&iNumViewports, &Viewport);

	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixTranspose(XMMatrixOrthographicLH(Viewport.Width, Viewport.Height, 0.f, 1.f)));

//#endif
	m_pDevice = _pDevice;
	m_pContext = _pContext;
	Safe_AddRef_DV(m_pDevice);
	Safe_AddRef_DV(m_pContext);

	_pContext->OMGetRenderTargets(1, &m_pBackBufferView, &m_pDepthStencilView);

	if (FAILED(Add_Shader(_pDevice, _pContext, SHADER_DEFERRED, L"Shader_Deferred.hlsl", VTXTEX_DECLARATION::Element, VTXTEX_DECLARATION::iNumElement)))
		return E_FAIL;
	if (FAILED(Add_Shader(_pDevice, _pContext, SHADER_EDGEGLOW, L"Shader_EdgeGlow.hlsl", VTXTEX_DECLARATION::Element, VTXTEX_DECLARATION::iNumElement)))
		return E_FAIL;
	if (FAILED(Add_Shader(_pDevice, _pContext, SHADER_LIGHT, L"Shader_Light.hlsl", VTXTEX_DECLARATION::Element, VTXTEX_DECLARATION::iNumElement)))
		return E_FAIL;
	if (FAILED(Add_Shader(_pDevice, _pContext, SHADER_DISTORTION, L"Shader_Distortion.hlsl", VTXTEX_DECLARATION::Element, VTXTEX_DECLARATION::iNumElement)))
		return E_FAIL;
	if (FAILED(Add_Shader(_pDevice, _pContext, SHADER_3DGLOW, L"Shader_3DGlow.hlsl", VTXTEX_DECLARATION::Element, VTXTEX_DECLARATION::iNumElement)))
		return E_FAIL;
	if (FAILED(Add_Shader(_pDevice, _pContext, SHADER_MOTIONBLUR, L"Shader_MotionBlur.hlsl", VTXTEX_DECLARATION::Element, VTXTEX_DECLARATION::iNumElement)))
		return E_FAIL;
	
	PATHS Path = { L"Trail",L"Test" };
	m_pDistortion = CTexture::Create(m_pDevice, m_pContext, &Path);
	if (nullptr == m_pDistortion)
		return E_FAIL;

	return S_OK;
}

HRESULT CTarget_Manager::Add_Shader(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext,
	const _tchar* _pShaderTag, const _tchar* _pShaderFilePath, D3D11_INPUT_ELEMENT_DESC* _pElement, const _uint& _iNumElement)
{
	if (nullptr != Find_Shader(_pShaderTag))
		return E_FAIL;

	CShader* pShader = CShader::Create(_pDevice, _pContext, _pShaderFilePath, _pElement, _iNumElement);
	if (Is_Nullptr(pShader))
		return E_FAIL;

	m_Shaders.emplace(pair<const _tchar*, CShader*>(_pShaderTag, pShader));
	return S_OK;
}

HRESULT CTarget_Manager::Add_RenderTarget(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext,
	const _tchar* _pRenderTargetTag, const CRenderTarget::RENTDESC& _tDesc)
{
	if (nullptr != Find_RenderTarget(_pRenderTargetTag))
		return E_FAIL;

	CRenderTarget* pRenderTarget = CRenderTarget::Create(_pDevice, _pContext, _tDesc);
	if (nullptr == pRenderTarget)
		return E_FAIL;

	m_RenderTargets.emplace(pair<const _tchar*, CRenderTarget*>(_pRenderTargetTag, pRenderTarget));
	return S_OK;
}

HRESULT CTarget_Manager::Add_MRT(const _tchar* _pMRTTag, const _tchar* _pRenderTargetTag)
{
	CRenderTarget* pRenderTarget = Find_RenderTarget(_pRenderTargetTag);
	if (nullptr == pRenderTarget)
		return E_FAIL;

	list<CRenderTarget*>* pMRTList = Find_MRT(_pMRTTag);
	if (nullptr == pMRTList)
	{
		list<CRenderTarget*> MRTList;
		MRTList.push_back(pRenderTarget);
		m_MRTs.emplace(_pMRTTag, MRTList);
	}
	else
		pMRTList->push_back(pRenderTarget);

	Safe_AddRef(pRenderTarget);
	return S_OK;
}

HRESULT CTarget_Manager::Create_LightDSV(_uint iSizeX, _uint iSizeY)
{
	ID3D11Texture2D* pDepthStencilTexture = nullptr;

	D3D11_TEXTURE2D_DESC	TextureDesc;
	ZeroMemory(&TextureDesc, sizeof(D3D11_TEXTURE2D_DESC));

	TextureDesc.Width = iSizeX;
	TextureDesc.Height = iSizeY;
	TextureDesc.MipLevels = 1;
	TextureDesc.ArraySize = 1;
	TextureDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;

	TextureDesc.SampleDesc.Quality = 0;
	TextureDesc.SampleDesc.Count = 1;

	TextureDesc.Usage = D3D11_USAGE_DEFAULT;
	TextureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	TextureDesc.CPUAccessFlags = 0;
	TextureDesc.MiscFlags = 0;

	if (FAILED(m_pDevice->CreateTexture2D(&TextureDesc, nullptr, &pDepthStencilTexture)))
		return E_FAIL;

	if (FAILED(m_pDevice->CreateDepthStencilView(pDepthStencilTexture, nullptr, &m_pLightDepthStencilView)))
		return E_FAIL;

	Safe_Release_DV(pDepthStencilTexture);

	ZeroMemory(&m_tLightDepth_ViewPort, sizeof(D3D11_VIEWPORT));
	m_tLightDepth_ViewPort.TopLeftX = 0.f;
	m_tLightDepth_ViewPort.TopLeftY = 0.f;
	m_tLightDepth_ViewPort.Width = (_float)iSizeX;
	m_tLightDepth_ViewPort.Height = (_float)iSizeY;
	m_tLightDepth_ViewPort.MinDepth = 0.f;
	m_tLightDepth_ViewPort.MaxDepth = 1.f;

	return S_OK;
}

HRESULT CTarget_Manager::Begin_MRT(ID3D11DeviceContext* _pContext, const _tchar* _pMRTTag)
{
	list<CRenderTarget*>* pMRTList = Find_MRT(_pMRTTag);
	if (nullptr == pMRTList)
		return E_FAIL;

	m_pSelectMRT = pMRTList;

	ID3D11RenderTargetView* pRTVs[8] = { nullptr };
	_int			iNumRenderTargets = 0;
	for (auto& pRenderTarget : *pMRTList)
	{
		pRenderTarget->Clear();
		pRTVs[iNumRenderTargets++] = pRenderTarget->Get_RTV();
	}
	if (iNumRenderTargets < 0 || iNumRenderTargets > 8)
		return E_FAIL;

	_pContext->OMSetRenderTargets(iNumRenderTargets, pRTVs, m_pDepthStencilView);
	return S_OK;
}

HRESULT CTarget_Manager::Begin_LightDepth_MRT(ID3D11DeviceContext* pDeviceContext, const _tchar* pMRTTag)
{
	list<CRenderTarget*>* pMRTList = Find_MRT(pMRTTag);
	if (nullptr == pMRTList)
		return E_FAIL;

	ID3D11ShaderResourceView* pSRV[128] = { nullptr };
	pDeviceContext->PSSetShaderResources(0, 128, pSRV);

	_uint iNumViewports = 1;
	pDeviceContext->RSGetViewports(&iNumViewports, &m_tViewPort);

	_uint iNumRenderTargets = 0;
	ID3D11RenderTargetView* pMRT[8] = { nullptr };

	for (auto& pRenderTarget : *pMRTList)
	{
		pRenderTarget->Clear();
		pMRT[iNumRenderTargets++] = pRenderTarget->Get_RTV();
	}
	pDeviceContext->OMSetRenderTargets(iNumRenderTargets, pMRT, m_pLightDepthStencilView);
	pDeviceContext->ClearDepthStencilView(m_pLightDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);
	pDeviceContext->RSSetViewports(1, &m_tLightDepth_ViewPort);

	return S_OK;
}

HRESULT CTarget_Manager::End_MRT(ID3D11DeviceContext* _pContext, const _tchar* _pMRTTag)
{
	ID3D11ShaderResourceView* pClearSRV[128] = { nullptr };
	_pContext->PSSetShaderResources(0, 128, pClearSRV);

	_pContext->OMSetRenderTargets(1, &m_pBackBufferView, m_pDepthStencilView);
	return S_OK;
}

HRESULT CTarget_Manager::End_LightDepth_MRT(ID3D11DeviceContext* _pContext, const _tchar* _pMRTTag)
{
	ID3D11ShaderResourceView* pSRV[128] = { nullptr };
	_pContext->PSSetShaderResources(0, 128, pSRV);

	_pContext->OMSetRenderTargets(1, &m_pBackBufferView, m_pDepthStencilView);
	_pContext->RSSetViewports(1, &m_tViewPort);
	return S_OK;
}

ID3D11ShaderResourceView* CTarget_Manager::Get_SRV(const _tchar* _pRenderTargetTag) const
{
	CRenderTarget* pRenderTarget = Find_RenderTarget(_pRenderTargetTag);
	if (nullptr == pRenderTarget)
		return nullptr;

	return pRenderTarget->Get_SRV();
}

void CTarget_Manager::Out_RenderTarget_Texture(map<_wstring, ID3D11ShaderResourceView*>* _pData)
{
	if (nullptr == _pData)
		return;

	for (auto& iter : m_RenderTargets)
	{
		(*_pData).emplace(pair<_wstring, ID3D11ShaderResourceView*>(iter.first, iter.second->Get_SRV()));
	}
}

CShader* CTarget_Manager::Get_Shader(const _tchar* _pShaderTag) const
{
	CShader* pShader = Find_Shader(_pShaderTag);
	if (Is_Nullptr(pShader))
		return nullptr;

	return pShader;
}

//#ifdef _DEBUG
HRESULT CTarget_Manager::Ready_Debug_Desc(const _tchar* _pRenderTargetTag, const _float2& _v2Pos, const _float2& _v2Size)
{
	CRenderTarget* pRenderTarget = Find_RenderTarget(_pRenderTargetTag);
	if (nullptr == pRenderTarget)
		return E_FAIL;

	return pRenderTarget->Ready_Debug_Desc(_v2Pos, _v2Size);
}

HRESULT CTarget_Manager::Render_Debug(const _tchar* _pMRTTag)
{
	list<CRenderTarget*>* pMRTList = Find_MRT(_pMRTTag);
	if (nullptr == pMRTList)
		return E_FAIL;

	for (auto& pRenderTarget : *pMRTList)
	{
		if (nullptr != pRenderTarget)
		{
			/* 셰이더에 값을 던진다.(텍스쳐, 각 렌더타겟들의 디버깅장면을 어디에 어떤사이즈로 그릴지?!) */
			/* 셰이더 */
			CShader* pShader = Find_Shader(SHADER_DEFERRED);
			if (Is_Nullptr(pShader))
				return E_FAIL;
			pShader->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4));
			pShader->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4));

			pRenderTarget->Render(pShader, m_pVIBuffer);
		}
	}

	return S_OK;
}
ID3D11Texture2D* CTarget_Manager::Get_DebugTexture(const _tchar* _pRenderTargetTag)
{
	CRenderTarget* pRenderTarget = Find_RenderTarget(_pRenderTargetTag);
	if (nullptr == pRenderTarget)
		return nullptr;

	CShader* pShader = Find_Shader(SHADER_DEFERRED);
	if (Is_Nullptr(pShader))
		return nullptr;

	pShader->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4));
	pShader->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4));

	pRenderTarget->Render(pShader, m_pVIBuffer);

	return pRenderTarget->Get_Texture();
}
//#endif

CRenderTarget* CTarget_Manager::Find_RenderTarget(const _tchar* _pRenderTargetTag) const
{
	auto	iter = find_if(m_RenderTargets.begin(), m_RenderTargets.end(), CTag_Finder(_pRenderTargetTag));
	if (iter == m_RenderTargets.end())
		return nullptr;
	return iter->second;
}

list<CRenderTarget*>* CTarget_Manager::Find_MRT(const _tchar* _pMRTTag)
{
	auto	iter = find_if(m_MRTs.begin(), m_MRTs.end(), CTag_Finder(_pMRTTag));
	if (iter == m_MRTs.end())
		return nullptr;
	return &iter->second;
}

CShader* CTarget_Manager::Find_Shader(const _tchar* _pShaderTag) const
{
	auto	iter = find_if(m_Shaders.begin(), m_Shaders.end(), CTag_Finder(_pShaderTag));
	if (iter == m_Shaders.end())
		return nullptr;
	return iter->second;
}

void CTarget_Manager::Free()
{
	for (auto& Pair : m_MRTs)
	{
		for (auto& pRenderTarget : Pair.second)
			Safe_Release(pRenderTarget);

		Pair.second.clear();
	}
	m_MRTs.clear();

	for (map<const _tchar*, CRenderTarget*>::iterator it = m_RenderTargets.begin(); it != m_RenderTargets.end();)
	{
		CRenderTarget* pRenderTarget = (*it).second;
		it = m_RenderTargets.erase(it);
		Safe_Release(pRenderTarget);
	}
	m_RenderTargets.clear();

	for (auto& pair : m_Shaders)
		Safe_Release(pair.second);
	m_Shaders.clear();

	Safe_Release_DV(m_pBackBufferView);
	Safe_Release_DV(m_pDepthStencilView);
	Safe_Release_DV(m_pLightDepthStencilView);

	Safe_Release_DV(m_pDevice);
	Safe_Release_DV(m_pContext);

	Safe_Release(m_pDistortion);

//#ifdef _DEBUG
	Safe_Release(m_pShader);
	Safe_Release(m_pVIBuffer);
//#endif
}
