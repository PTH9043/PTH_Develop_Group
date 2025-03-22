#pragma once

#include "Base.h"
#include "ProcessHandler.h"

BEGIN(Engine)

class CVIBuffer_Rect;

class CPostProcessing final : public CBase
{
	DECLARE_SINGLETON(CPostProcessing);

private:
	explicit CPostProcessing();
	virtual ~CPostProcessing() = default;
	
public:
	ID3D11UnorderedAccessView* Get_UAV(const _tchar* pProcessHandlerTag) const;
	ID3D11ShaderResourceView* Get_SRV(const _tchar* pProcessHandlerTag) const;

public:
	HRESULT Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	HRESULT Scale_Blur(ID3D11ShaderResourceView* pInput);
	HRESULT DownScale_Blur(ID3D11ShaderResourceView* pInput, ID3D11UnorderedAccessView* pOutput, _float fBlur = 0.f);
	//HRESULT DownScale_Blur(ID3D11ShaderResourceView* pInput, _float fBlur = 0.f);
	HRESULT Render(ID3D11ShaderResourceView* pInput, CVIBuffer_Rect* pVIBuffer, _float4x4* pMatrix);
	HRESULT HDR(ID3D11ShaderResourceView* pInput, const _float& _fAvgLum);
private:
	HRESULT UpScale4x4(ID3D11ShaderResourceView* pInput, ID3D11UnorderedAccessView* pOutput);
	HRESULT DownScale4x4(ID3D11ShaderResourceView* pInput, ID3D11UnorderedAccessView* pOutput);
	HRESULT Blur_Object(ID3D11ShaderResourceView* pInput, ID3D11UnorderedAccessView* pOutput, _float fBlur);
	HRESULT Test1();

private:
	HRESULT SetUp_Shader();
	HRESULT SetUp_ProcessHandler();

private:
	ID3D11Device*			m_pDevice = nullptr;
	ID3D11DeviceContext*	m_pContext = nullptr;

	ID3D11RenderTargetView* m_pBackBufferView = nullptr;
	ID3D11DepthStencilView* m_pDepthStencilView = nullptr;

private:
	map<const _tchar*, class CProcessHandler*> m_ProcessHandlers;

private:
	HRESULT Add_ProcessHandler(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext,
		const _tchar* _pProcessHandlerTag, const CProcessHandler::PHDESC& _tDesc);
	class CProcessHandler* Find_ProcessHandler(const _tchar* _pProcessHandlerTag) const;

private:
	class CTarget_Manager* m_pTarget_Manager = nullptr;
	class CShader* m_pShader_PostProcessing = nullptr;
	class CShader* m_pShader_Blur = nullptr;
	class CShader* m_pShader_HDR = nullptr;

private: // Å×½ºÆ®
	ID3D11Buffer* m_pInputBuffer = nullptr;
	ID3D11ShaderResourceView* m_pInputSRV = nullptr;

	ID3D11Buffer* m_pOutputBuffer = nullptr;
	ID3D11UnorderedAccessView* m_pOutputUAV = nullptr;

private:
	ID3D11ShaderResourceView* m_pClearSRV[128] = { nullptr };
	ID3D11UnorderedAccessView* m_pClearUAV[128] = { nullptr };

private:
	_uint m_iWinSizeX = 0;
	_uint m_iWinSizeY = 0;

	_uint m_iLightSizeX = 8000;
	_uint m_iLightSizeY = 6000;

	_uint m_iDownScaleGroups = 0;
	_uint m_iGroups = 0;

	struct BONE
	{
		_float4 v1;
		_float4 v2;
		_float4 v3;
		_float4 v4;
	};

public:
	virtual void Free() override;
};


END
