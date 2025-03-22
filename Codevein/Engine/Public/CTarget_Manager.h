#pragma once
#include "CBase.h"
#include "CRenderTarget.h"

BEGIN(Engine)
class CShader;
class CVIBuffer_Rect;

class CTarget_Manager : public CBase
{
	DECLARE_SINGLETON(CTarget_Manager);
public:
	explicit CTarget_Manager();
	virtual ~CTarget_Manager() = default;

public:
	// Native Construct
	HRESULT NativeConstruct(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);

	HRESULT Add_RenderTarget(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, 
		const _tchar* _pRenderTargetTag, const CRenderTarget::RENTDESC& _tDesc);
	HRESULT Add_MRT(const _tchar* _pMRTTag, const _tchar* _pRenderTargetTag);

	HRESULT Begin_MRT(ID3D11DeviceContext* _pContext, const _tchar* _pMRTTag);
	HRESULT End_MRT(ID3D11DeviceContext* _pContext, const _tchar* _pMRTTag);

	HRESULT Begin_MRT_NotClear(ID3D11DeviceContext* _pContext, const _tchar* _pMRTTag);
	HRESULT Clear_MRT(ID3D11DeviceContext* _pContext, const _tchar* _pMRTTag);
public:
	ID3D11ShaderResourceView* Get_SRV(const _tchar* _pRenderTargetTag) const ;

#ifdef _DEBUG
public:
	HRESULT Ready_Debug_Desc(const _tchar* _pRenderTargetTag, const _float2& _v2Pos, const _float2& _v2Size);
	HRESULT Render_Debug(const _tchar* _pMRTTag);
	ID3D11Texture2D* Get_DebugTexture(const _tchar* _pRenderTargetTag);
#endif

private:
	class CRenderTarget* Find_RenderTarget(const _tchar* _pRenderTargetTag) const;
	list<CRenderTarget*>* Find_MRT(const _tchar* _pMRTTag);

private:
	typedef map<const _tchar*, class CRenderTarget*> RENDERTARGETS;
	typedef map<const _tchar*, list<class CRenderTarget*>> MRTS;

	RENDERTARGETS						m_RenderTargets;
	MRTS												m_MRTs;
private:
	ID3D11RenderTargetView*		m_pBackBufferView = nullptr;
	ID3D11DepthStencilView*		m_pDepthStencilView = nullptr;

#ifdef _DEBUG
	ID3D11Device*							m_pDevice = nullptr;
	ID3D11DeviceContext*				m_pContext = nullptr;
	CShader*										m_pShader							= nullptr;
	CVIBuffer_Rect*							m_pVIBuffer				= nullptr;
	_float4x4										m_ViewMatrix;
	_float4x4										m_ProjMatrix;
#endif
public:
	virtual void Free() override;
};

END