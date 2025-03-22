#pragma once
#include "Base.h"
#include "RenderTarget.h"

BEGIN(Engine)

class CShader;
class CVIBuffer_Rect;

class CTarget_Manager : public CBase
{
	DECLARE_SINGLETON(CTarget_Manager);
private:
	typedef map<const _tchar*, class CRenderTarget*> RENDERTARGETS;
	typedef map<const _tchar*, list<class CRenderTarget*>> MRTS;
	typedef list<class CRenderTarget*> MRT;
	typedef map<const _tchar*, CShader*> SHADERS;
public:
	explicit CTarget_Manager();
	virtual ~CTarget_Manager() = default;

public:
	// Native Construct
	HRESULT NativeConstruct(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	HRESULT Add_Shader(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext,
		const _tchar* _pShaderTag, const _tchar* _pShaderFilePath, D3D11_INPUT_ELEMENT_DESC* _pElement, const _uint& _iNumElement);
	HRESULT Add_RenderTarget(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext,
		const _tchar* _pRenderTargetTag, const CRenderTarget::RENTDESC& _tDesc);
	HRESULT Add_MRT(const _tchar* _pMRTTag, const _tchar* _pRenderTargetTag);
	HRESULT Create_LightDSV(_uint iSizeX, _uint iSizeY);

	HRESULT Begin_MRT(ID3D11DeviceContext* _pContext, const _tchar* _pMRTTag);
	HRESULT Begin_LightDepth_MRT(ID3D11DeviceContext* pDeviceContext, const _tchar* pMRTTag);
	HRESULT End_MRT(ID3D11DeviceContext* _pContext, const _tchar* _pMRTTag);
	HRESULT End_LightDepth_MRT(ID3D11DeviceContext* _pContext, const _tchar* _pMRTTag);
public:
	ID3D11ShaderResourceView* Get_SRV(const _tchar* _pRenderTargetTag) const;
	void Out_RenderTarget_Texture(map<_wstring, ID3D11ShaderResourceView*>* _pData);

	CTexture* Get_Distortion() { return m_pDistortion; }
	CShader* Get_Shader(const _tchar* _pShaderTag) const;
//#ifdef _DEBUG
public:
	HRESULT Ready_Debug_Desc(const _tchar* _pRenderTargetTag, const _float2& _v2Pos, const _float2& _v2Size);
	HRESULT Render_Debug(const _tchar* _pMRTTag);
	ID3D11Texture2D* Get_DebugTexture(const _tchar* _pRenderTargetTag);
//#endif

private:
	class CRenderTarget* Find_RenderTarget(const _tchar* _pRenderTargetTag) const;
	list<CRenderTarget*>* Find_MRT(const _tchar* _pMRTTag);

	RENDERTARGETS						m_RenderTargets;
	MRTS								m_MRTs;
	MRT*								m_pSelectMRT = nullptr;

private:
	ID3D11RenderTargetView* m_pBackBufferView = nullptr;
	ID3D11DepthStencilView* m_pDepthStencilView = nullptr;
	D3D11_VIEWPORT			m_tViewPort;

	ID3D11DepthStencilView* m_pLightDepthStencilView = nullptr;
	D3D11_VIEWPORT			m_tLightDepth_ViewPort;

	ID3D11Device*			m_pDevice = nullptr;
	ID3D11DeviceContext*	m_pContext = nullptr;

private:

	class CTexture* m_pDistortion = nullptr;
private:
	CShader* Find_Shader(const _tchar* _pShaderTag) const;

	SHADERS					m_Shaders;
	CShader*				m_pShader = nullptr;

//#ifdef _DEBUG
	CVIBuffer_Rect* m_pVIBuffer = nullptr;
	_float4x4										m_ViewMatrix;
	_float4x4										m_ProjMatrix;
//#endif
public:
	virtual void Free() override;
};

END