#pragma once

#include "UBase.h"

BEGIN(Engine)
class UTexture;
class UDevice;
class UGpuCommand;
class UCommand;

struct RENDERTARGET
{
	SHPTR<UTexture>		pTexture{ nullptr };
	ARRAY<_float, 4>			arrClearColor;
	RTDESC							stRtDesc;
	// SetColor
	void SetColor(const _float4& _vColor) { ::memcpy(&arrClearColor, &_vColor.x, sizeof(_float4)); }
};

class URenderTargetGroup : public UBase {
public:
	using RESOURCEBARRIERS = ARRAY< D3D12_RESOURCE_BARRIER, 8>;
public:
	URenderTargetGroup();
	NO_COPY(URenderTargetGroup)
		DESTRUCTOR(URenderTargetGroup)
public:
	virtual void Free() override;
	HRESULT NativeConstruct(CSHPTRREF<UDevice> _spDevice, const VECTOR<RTDESC>& _vecRtDesc, CSHPTRREF<UTexture> _spDepthStencilTexture);
	HRESULT NativeConstruct(CSHPTRREF<UDevice> _spDevice, const VECTOR<RENDERTARGET>& _vecRenderTargets, CSHPTRREF<UTexture> _spDepthStencilTexture);
	// Remake
	void RemakeRenderTargets(CSHPTRREF<UDevice> _spDevice, CSHPTRREF<UTexture> _spDepthStencilTexture, CSHPTRREF<GRAPHICDESC> _spGraphicDesc);
private:
	HRESULT ReadyTargets(CSHPTRREF<UDevice> _spDevice);
public:

	// SetRenderTargets 
	void OmSetRenderTargets(const _uint _index, const _uint _iOffset, CSHPTRREF<UCommand> _spCommand);
	void OmSetRenderTargets(CSHPTRREF<UCommand> _spCommand);
	//Clear
	void ClearRenderTargetView(const _uint _index, CSHPTRREF<UCommand> _spCommand);
	void ClearRenderTargetView(CSHPTRREF<UCommand> _spCommand);

	void WaitTargetToResource(CSHPTRREF<UCommand> _spCommand);
	void WaitResourceToTarget(CSHPTRREF<UCommand> _spCommand);

	// Get Texture 
	SHPTR<UTexture> GetRenderTargetTexture(const RTOBJID _eObJID);
	SHPTR<UTexture> GetRenderTargetTexture(const _uint _iObjID);
	//SetColor
	void SetRenderTargetColor(const _uint _iObjID, _float4 color);
	void SetRenderTargetColor(const RTOBJID _eObJID, _float4 color);

private:
	// RenderTargets
	VECTOR<RENDERTARGET>					m_vecRenderTargets;
	UNORMAP<RTOBJID, _uint>			m_FindRts;
	_uint															m_iRenderTargetCount;
	// DepthStencilTexture 
	SHPTR<UTexture>									m_spDepthStencilTexture;
	D3D12_CPU_DESCRIPTOR_HANDLE	m_stD3DDepthStencilHeapBegin;
	// RTV Serize
	ComPtr<Dx12DescriptorHeap>			m_cpRenderTargetHeap;
	_uint															m_iRenderTargetHeapSize;
	D3D12_CPU_DESCRIPTOR_HANDLE	m_stD3DRenderTargetHeapBegin;
	// ResourceBarrier
	RESOURCEBARRIERS								m_arrD3DTargetToResource;
	RESOURCEBARRIERS								m_arrD3DResourceToTarget;
};

END

