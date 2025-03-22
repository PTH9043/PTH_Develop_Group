#pragma once
#include "UBase.h"

BEGIN(Engine)
class UTexture;
class UDevice;
class UGpuCommand;
class UComputeCommand;
class USwapChain;
class UGraphicsTableDescriptor;
class UGlobalConstantBuffer;
class UShaderConstantBuffer;
class URenderTargetGroup;
class URootSignature;
class UTableDescriptor;
class UComputeRootSignature;


class UGraphicDevice : public UBase {
public:
	UGraphicDevice();
	NO_COPY(UGraphicDevice)
		DESTRUCTOR(UGraphicDevice)
public:
	CSHPTRREF<USwapChain> GetSwapChain() const { return m_spSwapChain; }
	CSHPTRREF<UDevice>			GetDevice() const { return m_spDevice; }
	CSHPTRREF<UGpuCommand>		GetGpuCommand() const { return m_spGpuCommand; }
	CSHPTRREF<UComputeCommand>		GetComputeCommand() const { return m_spComputeCommand; }
	CSHPTRREF<UTableDescriptor>	GetTableDescriptor() const { return m_spTableDescriptor; }
	CSHPTRREF<UTableDescriptor>	GetComputeTableDescriptor() const { return m_spComputeTableDescriptor; }
	CSHPTRREF<URootSignature>	GetRootSignature() const { return m_spRootSignature; }
	CSHPTRREF<UComputeRootSignature>	GetComputeRootSignature() const { return m_spComputeRootSignature; }

	const _bool IsGamePaused() const { return m_isGamePaused; }
	const _bool IsScreenMinimized() const { return m_isScreenMinimized; }
	const _bool IsScreenMaximized() const { return m_isScreenMaximized; }
	const _bool IsScreenFullScreen() const { return m_isScreenFullscreen; }
	const _bool IsMssa4xState() const { return m_isMssa4xState; }
	const _uint& GetMssa4xQuality() const { return m_iMssa4xQuality; }

	const D3D12_VIEWPORT& GetD3DViewport() const { return m_stD3DViewport; }
	const D3D12_RECT& GetD3DWindowSizeRect() const { return m_stD3DWindowSizeRect; }
	CSHPTRREF<GRAPHICDESC> GetGraphicDesc() const { return m_spGraphicDesc; }


	void SetDefaultRtGroup(CSHPTRREF<URenderTargetGroup> _rtGroup) { this->m_spDefaultRenderTargetGroup = _rtGroup; }
public:
	// Free
	virtual void	 Free() override;
	// Ready
	HRESULT	ReadyGraphicDevice(const GRAPHICDESC& _stGraphicsDesc, OUTPUTDATA& _stOutDesc);
	void	 OmSetDefaultRenderTarget();
	SHPTR<UTexture> GetDefaultBackTexture();

	// Resize 
	HRESULT OnResize(const _uint& _iWinSizeX, const _uint& _iWinSizeY, const GRAPHICDESC::WINMODE _eWindowMode);
	// Clear BackBuffer View
	HRESULT MainRenderBegin();
	HRESULT MainRenderEnd();

	//4-27 Ãß°¡
	void SetTemporaryViewPort(D3D12_VIEWPORT _viewport,D3D12_RECT _rect);
	void SetDefaultViewPort();
private:
	void ResizeViewPort();
private:
	// DxClass
	SHPTR<UDevice>												m_spDevice;
	SHPTR<UGpuCommand>								m_spGpuCommand;
	SHPTR<UComputeCommand>								m_spComputeCommand;
	SHPTR<UTableDescriptor>							m_spTableDescriptor;
	SHPTR<UTableDescriptor>							m_spComputeTableDescriptor;
	SHPTR<URootSignature>								m_spRootSignature;
	SHPTR<UComputeRootSignature>				m_spComputeRootSignature;
	SHPTR<USwapChain>										m_spSwapChain;
	// Default Rtv 
	SHPTR< URenderTargetGroup>					m_spDefaultRenderTargetGroup;
	// Bool Values
	_bool																	m_isGamePaused;
	_bool																	m_isScreenMinimized;
	_bool																	m_isScreenMaximized;
	_bool																	m_isScreenResize;
	_bool																	m_isScreenFullscreen;
	_bool																	m_isMssa4xState;
	_uint																	m_iMssa4xQuality;
	// ViewPort, Rect
	D3D12_VIEWPORT											m_stD3DViewport;
	D3D12_RECT													m_stD3DWindowSizeRect;
	// GraphicDesc
	SHPTR<GRAPHICDESC>									m_spGraphicDesc;
};

END