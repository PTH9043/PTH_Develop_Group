#pragma once
#include "UCommand.h"

BEGIN(Engine)

class UDevice;

class UGpuCommand : public UCommand{
	typedef struct tagResourceLoaderGroup {
		ComPtr<Dx12CommandAllocator>				cpCmdAlloc;
		ComPtr<Dx12GraphicsCommandList>		cpGraphicCmdList;
	}GRAPHICGROUP;
public:
	typedef struct tagFrameContext {
		ComPtr<Dx12CommandAllocator> cpAllocator{ nullptr };
		_uint														fenceValue{ 0 };
	}FRAMECONTEXT;
public:
	UGpuCommand();
	NO_COPY(UGpuCommand)
	DESTRUCTOR(UGpuCommand)
public:
	const ComPtr<Dx12GraphicsCommandList>& GetResCmdList() const { return m_arrResourceLoaderGroup[m_iGraphicResourceUploadIndex].cpGraphicCmdList; }
public:
	virtual void Free() override;
	virtual HRESULT NativeConstruct(CSHPTRREF<UDevice> _spDevice, const ComPtr<Dx12CommandQueue>& _cpCommandQueue) override;
public:
	virtual void WaitForSynchronization() override;
	void WaitForLastSubmittedFrame();
	FRAMECONTEXT* WaitForNextFrameResources(const HANDLE& _hSwapchainEvent, const _uint _iBackBufferIndex);
	void WaitForEndFrameResource();
	// WaitForGpuComplete
	void WaitForGpuResourceUpload();

	virtual void Clear() override;
private:
	static constexpr _uint										FRAME_CNT{3};
	static constexpr  _uint									RESOURCE_LOADER_CNT{5};
	using  GRAPHICGROUPS = ARRAY<GRAPHICGROUP, RESOURCE_LOADER_CNT>;
	// FameContext ฐüทร
	ARRAY<FRAMECONTEXT, FRAME_CNT>	m_arrFrameContexts;
	_uint																	m_iFrameValue;
	_uint																	m_iFenceLastSignaledValue;
	_uint																	m_iFrameIndex;

	// Resource Loader 
	GRAPHICGROUPS												m_arrResourceLoaderGroup;
	_uint																	m_iGraphicResourceUploadIndex;
	_bool																	m_isInitUpload;
};

END

