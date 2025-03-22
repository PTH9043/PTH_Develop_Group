#include "CTarget_Manager.h"
#include "CShader.h"
#include "CVIBuffer_Rect.h"

IMPLEMENT_SINGLETON(CTarget_Manager)


CTarget_Manager::CTarget_Manager()
{

}

HRESULT CTarget_Manager::NativeConstruct(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
#ifdef _DEBUG
	m_pShader = CShader::Create(_pDevice, _pContext, TEXT("..\\Bin\\ShaderFiles\\Shader_Deffered.hlsl"), VTXTEX_DECLARATION::Element, VTXTEX_DECLARATION::iNumElement);
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

	m_pDevice = _pDevice;
	m_pContext = _pContext;
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
#endif
	return S_OK;
}

HRESULT CTarget_Manager::Add_RenderTarget(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext,
	const _tchar* _pRenderTargetTag ,const CRenderTarget::RENTDESC& _tDesc)
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

HRESULT CTarget_Manager::Begin_MRT(ID3D11DeviceContext* _pContext, const _tchar* _pMRTTag)
{
	list<CRenderTarget*>* pMRTList = Find_MRT(_pMRTTag);
	if (nullptr == pMRTList)
		return E_FAIL;

	_pContext->OMGetRenderTargets(1, &m_pBackBufferView, &m_pDepthStencilView);
	ID3D11RenderTargetView* pRTVs[8] = { nullptr };
	_uint			iNumRenderTargets = 0;
	for (auto& pRenderTarget : *pMRTList)
	{
		pRenderTarget->Clear();
		pRTVs[iNumRenderTargets++] = pRenderTarget->Get_RTV();
	}
	_pContext->OMSetRenderTargets(iNumRenderTargets, pRTVs, m_pDepthStencilView);
	return S_OK;
}

HRESULT CTarget_Manager::End_MRT(ID3D11DeviceContext* _pContext, const _tchar* _pMRTTag)
{
	ID3D11ShaderResourceView* pSRVs[8] = { nullptr };

	_pContext->PSSetShaderResources(0, 8, pSRVs);

	_pContext->OMSetRenderTargets(1, &m_pBackBufferView, m_pDepthStencilView);
	Safe_Release(m_pBackBufferView);
	Safe_Release(m_pDepthStencilView);
	return S_OK;
}

HRESULT CTarget_Manager::Begin_MRT_NotClear(ID3D11DeviceContext* _pContext, const _tchar* _pMRTTag)
{
	list<CRenderTarget*>* pMRTList = Find_MRT(_pMRTTag);
	if (nullptr == pMRTList)
		return E_FAIL;

	_pContext->OMGetRenderTargets(1, &m_pBackBufferView, &m_pDepthStencilView);
	ID3D11RenderTargetView* pRTVs[8] = { nullptr };
	_uint			iNumRenderTargets = 0;
	for (auto& pRenderTarget : *pMRTList)
	{
		pRTVs[iNumRenderTargets++] = pRenderTarget->Get_RTV();
	}
	_pContext->OMSetRenderTargets(iNumRenderTargets, pRTVs, m_pDepthStencilView);

	return S_OK;
}
HRESULT CTarget_Manager::Clear_MRT(ID3D11DeviceContext* _pContext, const _tchar* _pMRTTag)
{
	list<CRenderTarget*>* pMRTList = Find_MRT(_pMRTTag);
	if (nullptr == pMRTList)
		return E_FAIL;

	{
		_pContext->OMGetRenderTargets(1, &m_pBackBufferView, &m_pDepthStencilView);
		ID3D11RenderTargetView* pRTVs[8] = { nullptr };

		for (auto& pRenderTarget : *pMRTList)
			pRenderTarget->Clear();
	}
	{
		ID3D11ShaderResourceView* pSRVs[8] = { nullptr };
		_pContext->PSSetShaderResources(0, 8, pSRVs);
		_pContext->OMSetRenderTargets(1, &m_pBackBufferView, m_pDepthStencilView);
		Safe_Release(m_pBackBufferView);
		Safe_Release(m_pDepthStencilView);
	}

	return S_OK;
}

ID3D11ShaderResourceView* CTarget_Manager::Get_SRV(const _tchar* _pRenderTargetTag) const
{
	CRenderTarget* pRenderTarget = Find_RenderTarget(_pRenderTargetTag);
	if (nullptr == pRenderTarget)
		return nullptr;

	return pRenderTarget->Get_SRV();
}
#ifdef _DEBUG
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

			m_pShader->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4));
			m_pShader->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4));

			pRenderTarget->Render(m_pShader, m_pVIBuffer);
		}
	}

	return S_OK;
}
ID3D11Texture2D* CTarget_Manager::Get_DebugTexture(const _tchar* _pRenderTargetTag)
{
	CRenderTarget* pRenderTarget = Find_RenderTarget(_pRenderTargetTag);
	if (nullptr == pRenderTarget)
		return nullptr;
	
	m_pShader->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4));
	m_pShader->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4));

	pRenderTarget->Render(m_pShader, m_pVIBuffer);

	return pRenderTarget->Get_Texture();
}
#endif
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

void CTarget_Manager::Free()
{
	for (auto& Pair : m_MRTs)
	{
		for (auto& pRenderTarget : Pair.second)
			Safe_Release(pRenderTarget);

		Pair.second.clear();
	}
	m_MRTs.clear();

	for (map<const _tchar*,CRenderTarget*>::iterator it = m_RenderTargets.begin(); it != m_RenderTargets.end();)
	{
		CRenderTarget* pRenderTarget = (*it).second;
		it = m_RenderTargets.erase(it);
		Safe_Release(pRenderTarget);
	}

	m_RenderTargets.clear();

#ifdef _DEBUG
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
	Safe_Release(m_pShader);
	Safe_Release(m_pVIBuffer);
#endif
}
