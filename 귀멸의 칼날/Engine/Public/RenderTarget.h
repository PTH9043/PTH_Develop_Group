#pragma once
#include "Base.h"

BEGIN(Engine)

class CRenderTarget final : public CBase
{
public:
	typedef struct tagRenderTargetDesc
	{
		tagRenderTargetDesc() = default;
		tagRenderTargetDesc(const _float& _fWidth, const _float& _fHeight, DXGI_FORMAT _eFormat, const _float4& _vClearColor)
			: fWIdth(_fWidth), fHeight(_fHeight), eFormat(_eFormat), vClearColor(_vClearColor)
		{}
		_float fWIdth = 0.f;
		_float fHeight = 0.f;
		DXGI_FORMAT eFormat;
		_float4 vClearColor = _float4(0.f, 0.f, 0.f, 0.f);
	}RENTDESC;
public:
	explicit CRenderTarget(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual ~CRenderTarget() = default;

public:
	ID3D11RenderTargetView* Get_RTV() const { return m_pRTV; }
	ID3D11ShaderResourceView* Get_SRV() const { return m_pSRV; }
	ID3D11Texture2D* Get_Texture() const { return m_pTexture; }
public:
	HRESULT NativeConstruct(const RENTDESC& _tRedDesc);
	HRESULT Clear();

public:
//#ifdef _DEBUG
	HRESULT Render(class CShader* pShader, class CVIBuffer_Rect* pVIBuffer);
	HRESULT Ready_Debug_Desc(const _float2& _v2Pos, const _float2& _v2Size);
//#endif // _DEBUG
protected:
	ID3D11Device* m_pDevice = nullptr;
	ID3D11DeviceContext* m_pContext = nullptr;
protected:
	ID3D11Texture2D* m_pTexture = nullptr;
	ID3D11RenderTargetView* m_pRTV = nullptr;
	ID3D11ShaderResourceView* m_pSRV = nullptr;
protected:
	_float4												m_vClearColor = _float4(0.f, 0.f, 0.f, 0.f);
//#ifdef _DEBUG
private:
	_float4x4					m_WorldMatrix;
//#endif // _DEBUG
public:
	static CRenderTarget* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const RENTDESC& _tRedDesc);
	virtual void Free() override;
};

END