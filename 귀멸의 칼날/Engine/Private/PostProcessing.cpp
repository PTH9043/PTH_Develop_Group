#include "PostProcessing.h"

#include "ProcessHandler.h"
#include "Target_Manager.h"
#include "VIBuffer_Rect.h"
#include "Shader.h"

IMPLEMENT_SINGLETON(CPostProcessing)

CPostProcessing::CPostProcessing()
{
}

ID3D11UnorderedAccessView* CPostProcessing::Get_UAV(const _tchar* pProcessHandlerTag) const
{
	CProcessHandler* pHandler = Find_ProcessHandler(pProcessHandlerTag);
	if (Is_Nullptr(pHandler))
		return nullptr;
	
	return pHandler->Get_UAV();
}

ID3D11ShaderResourceView* CPostProcessing::Get_SRV(const _tchar* pProcessHandlerTag) const
{
	CProcessHandler* pHandler = Find_ProcessHandler(pProcessHandlerTag);
	if (Is_Nullptr(pHandler))
		return nullptr;

	return pHandler->Get_SRV();
}

HRESULT CPostProcessing::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	m_pDevice = pDevice;
	m_pContext = pContext;
	Safe_AddRef_DV(m_pDevice);
	Safe_AddRef_DV(m_pContext);

	pContext->OMGetRenderTargets(1, &m_pBackBufferView, &m_pDepthStencilView);

	m_pTarget_Manager = CTarget_Manager::Get_Instance();
	Safe_AddRef(m_pTarget_Manager);

	_uint iNumViewPorts = 1;
	D3D11_VIEWPORT tViewPort;
	m_pContext->RSGetViewports(&iNumViewPorts, &tViewPort);

	m_iWinSizeX = (_uint)tViewPort.Width;
	m_iWinSizeY = (_uint)tViewPort.Height;
	m_iDownScaleGroups = (UINT)ceil((_float)(m_iWinSizeX * m_iWinSizeY / 16) / 1024.f);
	m_iGroups = (UINT)ceil((_float)(m_iWinSizeX * m_iWinSizeY) / 1024.f);

	if (FAILED(SetUp_Shader()))
	{
		MSG_BOX("Failed To Initialize PostProcessing : SetUp_Shader");
		return E_FAIL;
	}

	if (FAILED(SetUp_ProcessHandler()))
	{
		MSG_BOX("Failed To Initialize PostProcessing : SetUp_ProcessHandler");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CPostProcessing::Render(ID3D11ShaderResourceView* pInput, CVIBuffer_Rect* pVIBuffer, _float4x4* pMatrix)
{
	//if (Is_Nullptr(pInput) || Is_Nullptr(pVIBuffer) || Is_Nullptr(pMatrix))
	//	return E_FAIL;

	if (FAILED(m_pShader_PostProcessing->Set_SRV("g_InputTexture", Get_SRV(PH_BLUR))))
		return E_FAIL;

	if (FAILED(m_pShader_PostProcessing->Set_RawValue("g_WorldMatrix", &pMatrix[0], sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShader_PostProcessing->Set_RawValue("g_ViewMatrix", &pMatrix[1], sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShader_PostProcessing->Set_RawValue("g_ProjMatrix", &pMatrix[2], sizeof(_float4x4))))
		return E_FAIL;

	m_pShader_PostProcessing->Begin(0);

	pVIBuffer->Render();

	return S_OK;
}

HRESULT CPostProcessing::Scale_Blur(ID3D11ShaderResourceView* pInput)
{
	ID3D11RenderTargetView* pRTVNullptr = nullptr;
	m_pContext->OMSetRenderTargets(1, &pRTVNullptr, nullptr);

	if (FAILED(DownScale4x4(pInput, Get_UAV(PH_DOWNSCALE))))
		return E_FAIL;
	if (FAILED(Blur_Object(Get_SRV(PH_DOWNSCALE), Get_UAV(PH_BLUR), 0.f)))
		return E_FAIL;
	if (FAILED(UpScale4x4(Get_SRV(PH_BLUR), Get_UAV(PH_UPSCALE))))
		return E_FAIL;

	m_pContext->OMSetRenderTargets(1, &m_pBackBufferView, m_pDepthStencilView);

	return S_OK;
}

HRESULT CPostProcessing::DownScale_Blur(ID3D11ShaderResourceView* pInput, ID3D11UnorderedAccessView* pOutput, _float fBlur)
{
	ID3D11RenderTargetView* pRTVNullptr = nullptr;
	m_pContext->OMSetRenderTargets(1, &pRTVNullptr, nullptr);

	if (FAILED(DownScale4x4(pInput, Get_UAV(PH_DOWNSCALE))))
		return E_FAIL;
	if (FAILED(Blur_Object(Get_SRV(PH_DOWNSCALE), pOutput, fBlur)))
		return E_FAIL;
	//if (FAILED(Blur_Object(pInput, Get_UAV(PH_BLUR))))
	//	return E_FAIL;

	m_pContext->OMSetRenderTargets(1, &m_pBackBufferView, m_pDepthStencilView);

	return S_OK;
}

HRESULT CPostProcessing::HDR(ID3D11ShaderResourceView* pInput, const _float& _fAvgLum)
{
	ID3D11RenderTargetView* pRTVNullptr = nullptr;
	m_pContext->OMSetRenderTargets(1, &pRTVNullptr, nullptr);
	_float m_fAdaptation = 1.f;
	_float m_fBloomThreshold = 1.1f;
	_float m_fBloomScale = 0.74f;
	_float AverageLum = 1.f;
	_float AverageLumFinal = 1.f;
	_float AverageValues1D = 1.f;
	{
		{
			_int vRes[2] = { (_int)m_iWinSizeX, (_int)m_iWinSizeY };

			if (FAILED(m_pShader_HDR->Set_RawValue("Res", vRes, sizeof(_int) * 2)))
				return E_FAIL;

			_int iDomain = vRes[0] * vRes[1];
			if (FAILED(m_pShader_HDR->Set_RawValue("Domain", &iDomain, sizeof(_int))))
				return E_FAIL;
			if (FAILED(m_pShader_HDR->Set_RawValue("GroupSize", &m_iDownScaleGroups, sizeof(_int))))
				return E_FAIL;
			if (FAILED(m_pShader_HDR->Set_RawValue("Adaptation", &m_fAdaptation, sizeof(_float))))
				return E_FAIL;
			if (FAILED(m_pShader_HDR->Set_RawValue("fBloomThreshold", &m_fBloomThreshold, sizeof(_float))))
				return E_FAIL;
			if (FAILED(m_pShader_HDR->Set_RawValue("AverageLum", &AverageLum, sizeof(_float))))
				return E_FAIL;
			if (FAILED(m_pShader_HDR->Set_RawValue("AverageLumFinal", &AverageLumFinal, sizeof(_float))))
				return E_FAIL;
			if (FAILED(m_pShader_HDR->Set_RawValue("AverageValues1D", &AverageValues1D, sizeof(_float))))
				return E_FAIL;

			if (FAILED(m_pShader_HDR->Set_UAV("HDRDownScale", Get_UAV(PH_TEMP))))
				return E_FAIL;

			if (FAILED(m_pShader_HDR->Set_SRV("HDRTex", pInput)))
				return E_FAIL;

			m_pShader_HDR->Begin(0);
			m_pContext->Dispatch(m_iGroups, 1, 1);
		}
		{
			_uint vRes[2] = { m_iWinSizeX , m_iWinSizeY };

			if (FAILED(m_pShader_HDR->Set_RawValue("Res", vRes, sizeof(_uint) * 2)))
				return E_FAIL;

			_uint iDomain = vRes[0] * vRes[1];
			if (FAILED(m_pShader_HDR->Set_RawValue("Domain", &iDomain, sizeof(_uint))))
				return E_FAIL;
			if (FAILED(m_pShader_HDR->Set_RawValue("GroupSize", &m_iDownScaleGroups, sizeof(_uint))))
				return E_FAIL;
			if (FAILED(m_pShader_HDR->Set_RawValue("Adaptation", &m_fAdaptation, sizeof(_float))))
				return E_FAIL;
			if (FAILED(m_pShader_HDR->Set_RawValue("fBloomThreshold", &m_fBloomThreshold, sizeof(_float))))
				return E_FAIL;

			if (FAILED(m_pShader_HDR->Set_UAV("HDRDownScale", Get_UAV(PH_HDR))))
				return E_FAIL;

			if (FAILED(m_pShader_HDR->Set_SRV("HDRTex", Get_SRV(PH_TEMP))))
				return E_FAIL;

			m_pShader_HDR->Begin(1);
			m_pContext->Dispatch(m_iGroups, 1, 1);
		}
		{
			_uint vRes[2] = { m_iWinSizeX, m_iWinSizeY };

			if (FAILED(m_pShader_HDR->Set_RawValue("Res", vRes, sizeof(_uint) * 2)))
				return E_FAIL;

			_float fData[] = { _fAvgLum };
			if (FAILED(m_pShader_HDR->Set_RawValue("AvgLum", fData, sizeof(_float))))
				return E_FAIL;

			if (FAILED(m_pShader_HDR->Set_UAV("Bloom", Get_UAV(PH_TEMP))))
				return E_FAIL;

			if (FAILED(m_pShader_HDR->Set_SRV("HDRDownScaleTex", Get_SRV(PH_HDR))))
				return E_FAIL;

			m_pShader_HDR->Begin(2);
			m_pContext->Dispatch(m_iGroups, 1, 1);
		}
		m_pContext->CSSetShaderResources(0, 128, m_pClearSRV);
		m_pContext->CSSetUnorderedAccessViews(0, 2, m_pClearUAV, nullptr);
	}
	m_pContext->OMSetRenderTargets(1, &m_pBackBufferView, m_pDepthStencilView);
	return S_OK;
}

HRESULT CPostProcessing::UpScale4x4(ID3D11ShaderResourceView* pInput, ID3D11UnorderedAccessView* pOutput)
{
	_uint vRes[2] = { m_iWinSizeX, m_iWinSizeY};

	if (FAILED(m_pShader_Blur->Set_RawValue("Res", vRes, sizeof(_uint) * 2)))
		return E_FAIL;

	if (FAILED(m_pShader_Blur->Set_UAV("Output", pOutput)))
		return E_FAIL;

	if (FAILED(m_pShader_Blur->Set_SRV("Input", pInput)))
		return E_FAIL;

	m_pShader_Blur->Begin(3);
	m_pContext->Dispatch(m_iGroups, 1, 1);

	m_pContext->CSSetShaderResources(0, 128, m_pClearSRV);
	m_pContext->CSSetUnorderedAccessViews(0, 1, m_pClearUAV, nullptr);

	return S_OK;
}

HRESULT CPostProcessing::DownScale4x4(ID3D11ShaderResourceView* pInput, ID3D11UnorderedAccessView* pOutput)
{
	_uint iScale = 4;
	// 320, 180
	_uint vRes[2] = { m_iWinSizeX / iScale, m_iWinSizeY / iScale };

	if (FAILED(m_pShader_Blur->Set_RawValue("Res", vRes, sizeof(_uint) * 2)))
		return E_FAIL;

	if (FAILED(m_pShader_Blur->Set_UAV("Output", pOutput)))
		return E_FAIL;

	if (FAILED(m_pShader_Blur->Set_SRV("Input", pInput)))
		return E_FAIL;

	m_pShader_Blur->Begin(2);
	m_pContext->Dispatch(m_iDownScaleGroups, 1, 1);

	m_pContext->CSSetShaderResources(0, 128, m_pClearSRV);
	m_pContext->CSSetUnorderedAccessViews(0, 1, m_pClearUAV, nullptr);

	return S_OK;
}

HRESULT CPostProcessing::Blur_Object(ID3D11ShaderResourceView* pInput, ID3D11UnorderedAccessView* pOutput, _float fBlur)
{
	// Y축 블러
	_uint iScale = 4;
	_uint vRes[2] = { m_iWinSizeX / iScale, m_iWinSizeY / iScale };
	_uint iGroups = (UINT)ceil((_float)(m_iWinSizeX * m_iWinSizeY / 16) / 128.f);

	if (FAILED(m_pShader_Blur->Set_RawValue("Res", vRes, sizeof(_uint) * 2)))
		return E_FAIL;

	if (FAILED(m_pShader_Blur->Set_RawValue("Blur", &fBlur, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShader_Blur->Set_UAV("Output", Get_UAV(PH_TEMP))))
		return E_FAIL;

	if (FAILED(m_pShader_Blur->Set_SRV("Input", pInput)))
		return E_FAIL;

	m_pShader_Blur->Begin(4);
	m_pContext->Dispatch(iGroups, 1, 1);
	//m_pContext->Dispatch((_uint)ceil((m_iWinSizeX / iScale) / (128.f - 12.f)), (_uint)ceil(m_iWinSizeY / iScale), 1);

	// X축 블러
	if (FAILED(m_pShader_Blur->Set_UAV("Output", pOutput)))
		return E_FAIL;

	if (FAILED(m_pShader_Blur->Set_SRV("Input", Get_SRV(PH_TEMP))))
		return E_FAIL;

	m_pShader_Blur->Begin(5);
	m_pContext->Dispatch(iGroups, 1, 1);
	//m_pContext->Dispatch((_uint)ceil(m_iWinSizeX / iScale), (_uint)ceil((m_iWinSizeY / iScale) / (128.f - 12.f)), 1);

	m_pContext->CSSetShaderResources(0, 128, m_pClearSRV);
	m_pContext->CSSetUnorderedAccessViews(0, 1, m_pClearUAV, nullptr);

	return S_OK;
}

HRESULT CPostProcessing::Test1()
{
	if (FAILED(m_pShader_Blur->Set_SRV("gInput", m_pInputSRV)))
		return E_FAIL;
	if (FAILED(m_pShader_Blur->Set_UAV("gOutput", m_pOutputUAV)))
		return E_FAIL;
	m_pShader_Blur->Begin(4);
	m_pContext->Dispatch(1, 1, 1);

	D3D11_BUFFER_DESC tBufferDesc;
	ZeroMemory(&tBufferDesc, sizeof(D3D11_BUFFER_DESC));
	tBufferDesc.BindFlags = 0;
	tBufferDesc.StructureByteStride = sizeof(BONE);
	tBufferDesc.ByteWidth = 32 * sizeof(BONE);
	tBufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	tBufferDesc.Usage = D3D11_USAGE_STAGING;
	tBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;

	ID3D11Buffer* pDebugBuffer = nullptr;
	if (FAILED(m_pDevice->CreateBuffer(&tBufferDesc, nullptr, &pDebugBuffer)))
		return E_FAIL;

	m_pContext->CopyResource(pDebugBuffer, m_pOutputBuffer);

	D3D11_MAPPED_SUBRESOURCE mappedData;
	m_pContext->Map(pDebugBuffer, 0, D3D11_MAP_READ, 0, &mappedData);
	BONE* data = reinterpret_cast<BONE*>(mappedData.pData);

	for (_uint i = 0; i < 32; i++)
	{
		BONE bone = data[i];
 		_int a = 10;
	}

	m_pContext->Unmap(pDebugBuffer, 0);
	Safe_Release_DV(pDebugBuffer);

	return S_OK;
}

HRESULT CPostProcessing::SetUp_Shader()
{
	m_pShader_PostProcessing = CShader::Create(m_pDevice, m_pContext, L"Shader_PostProcessing.hlsl", 
		VTXTEX_DECLARATION::Element, VTXTEX_DECLARATION::iNumElement);
	if (Is_Nullptr(m_pShader_PostProcessing))
		return E_FAIL;

	m_pShader_Blur = CShader::Create(m_pDevice, m_pContext, L"Shader_Blur.hlsl", nullptr, 0, CShader::CS);
	if (Is_Nullptr(m_pShader_Blur))
		return E_FAIL;

	m_pShader_HDR = CShader::Create(m_pDevice, m_pContext, L"Shader_Hdr.hlsl", nullptr, 0, CShader::CS);
	if (Is_Nullptr(m_pShader_HDR))
		return E_FAIL;

	return S_OK;
}

HRESULT CPostProcessing::SetUp_ProcessHandler()
{
	CProcessHandler::PHDESC tDesc;

	tDesc = CProcessHandler::PHDESC((_float)m_iLightSizeX / 4, (_float)m_iLightSizeY / 4);
	if (FAILED(Add_ProcessHandler(m_pDevice, m_pContext, PH_SHADOW4x4, tDesc)))
		return E_FAIL;

	tDesc = CProcessHandler::PHDESC((_float)m_iWinSizeX / 4, (_float)m_iWinSizeY / 4);
	if (FAILED(Add_ProcessHandler(m_pDevice, m_pContext, PH_TEMP, tDesc)))
		return E_FAIL;

	tDesc = CProcessHandler::PHDESC((_float)m_iWinSizeX / 4, (_float)m_iWinSizeY / 4);
	if (FAILED(Add_ProcessHandler(m_pDevice, m_pContext, PH_DOWNSCALE, tDesc)))
		return E_FAIL;

	tDesc = CProcessHandler::PHDESC((_float)m_iWinSizeX / 4, (_float)m_iWinSizeY / 4);
	if (FAILED(Add_ProcessHandler(m_pDevice, m_pContext, PH_BLUR, tDesc)))
		return E_FAIL;

	tDesc = CProcessHandler::PHDESC((_float)m_iWinSizeX / 4, (_float)m_iWinSizeY / 4);
	if (FAILED(Add_ProcessHandler(m_pDevice, m_pContext, PH_BLUR2, tDesc)))
		return E_FAIL;

	tDesc = CProcessHandler::PHDESC((_float)m_iWinSizeX, (_float)m_iWinSizeY);
	if (FAILED(Add_ProcessHandler(m_pDevice, m_pContext, PH_UPSCALE, tDesc)))
		return E_FAIL;

	tDesc = CProcessHandler::PHDESC((_float)m_iWinSizeX, (_float)m_iWinSizeY);
	if (FAILED(Add_ProcessHandler(m_pDevice, m_pContext, PH_HDR, tDesc)))
		return E_FAIL;

	//========================================= Buffer ==================================================

	_uint iNumElements = 32;
	// Input
	
	D3D11_BUFFER_DESC tBufferDesc;
	ZeroMemory(&tBufferDesc, sizeof(D3D11_BUFFER_DESC));
	tBufferDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	tBufferDesc.StructureByteStride = sizeof(BONE);
	tBufferDesc.ByteWidth = iNumElements * sizeof(BONE);
	tBufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	tBufferDesc.Usage = D3D11_USAGE_DEFAULT;

	D3D11_SUBRESOURCE_DATA tData;
	BONE* pData = new BONE[iNumElements];
	for (_uint i = 0; i < iNumElements; i++)
	{
		pData[i].v1 = _float4(1.f, 0.f, 0.f, 0.f);
		pData[i].v2 = _float4(0.f, 1.f, 0.f, 0.f);
		pData[i].v3 = _float4(0.f, 0.f, 1.f, 0.f);
		pData[i].v4 = _float4(0.f, 0.f, 0.f, 1.f);
	}
	tData.pSysMem = &pData[0];
	if (FAILED(m_pDevice->CreateBuffer(&tBufferDesc, &tData, &m_pInputBuffer)))
		return E_FAIL;
	Safe_Delete_Array(pData);

	D3D11_SHADER_RESOURCE_VIEW_DESC tSRVDesc;
	ZeroMemory(&tSRVDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
	tSRVDesc.Format = DXGI_FORMAT_UNKNOWN;
	tSRVDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX;
	tSRVDesc.Buffer.FirstElement = 0;
	tSRVDesc.Buffer.NumElements = iNumElements;
	if (FAILED(m_pDevice->CreateShaderResourceView(m_pInputBuffer, &tSRVDesc, &m_pInputSRV)))
		return E_FAIL;

	// Output
	ZeroMemory(&tBufferDesc, sizeof(D3D11_BUFFER_DESC));
	tBufferDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS;
	tBufferDesc.StructureByteStride = sizeof(BONE);
	tBufferDesc.ByteWidth = iNumElements * sizeof(BONE);
	tBufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	tBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	if (FAILED(m_pDevice->CreateBuffer(&tBufferDesc, nullptr, &m_pOutputBuffer)))
		return E_FAIL;

	D3D11_UNORDERED_ACCESS_VIEW_DESC tUAVDesc;
	ZeroMemory(&tUAVDesc, sizeof(D3D11_UNORDERED_ACCESS_VIEW_DESC));
	tUAVDesc.Format = DXGI_FORMAT_UNKNOWN;
	tUAVDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	tUAVDesc.Buffer.FirstElement = 0;
	tUAVDesc.Buffer.NumElements = iNumElements;
	if (FAILED(m_pDevice->CreateUnorderedAccessView(m_pOutputBuffer, &tUAVDesc, &m_pOutputUAV)))
		return E_FAIL;

	return S_OK;
}

HRESULT CPostProcessing::Add_ProcessHandler(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const _tchar* _pProcessHandlerTag, const CProcessHandler::PHDESC& _tDesc)
{
	if (nullptr != Find_ProcessHandler(_pProcessHandlerTag))
		return E_FAIL;

	CProcessHandler* pProcessHandler = CProcessHandler::Create(_pDevice, _pContext, _tDesc);
	if (Is_Nullptr(pProcessHandler))
		return E_FAIL;

	m_ProcessHandlers.emplace(pair<const _tchar*, CProcessHandler*>(_pProcessHandlerTag, pProcessHandler));
	return S_OK;
}

CProcessHandler* CPostProcessing::Find_ProcessHandler(const _tchar* _pProcessHandlerTag) const
{
	auto	iter = find_if(m_ProcessHandlers.begin(), m_ProcessHandlers.end(), CTag_Finder(_pProcessHandlerTag));
	if (iter == m_ProcessHandlers.end())
		return nullptr;
	return iter->second;
}

void CPostProcessing::Free()
{
	Safe_Release_DV(m_pContext);
	Safe_Release_DV(m_pDevice);
	Safe_Release_DV(m_pBackBufferView);
	Safe_Release_DV(m_pDepthStencilView);

	Safe_Release_DV(m_pInputBuffer);
	Safe_Release_DV(m_pInputSRV);
	Safe_Release_DV(m_pOutputBuffer);
	Safe_Release_DV(m_pOutputUAV);

	for (auto& pair : m_ProcessHandlers)
		Safe_Release(pair.second);
	m_ProcessHandlers.clear();

	Safe_Release(m_pShader_PostProcessing);
	Safe_Release(m_pShader_Blur);
	Safe_Release(m_pShader_HDR);
	Safe_Release(m_pTarget_Manager);
}

