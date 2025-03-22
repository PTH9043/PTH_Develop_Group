#pragma once
#include "UBase.h"

BEGIN(Engine)
class URootSignature;

class UCommand abstract : public UBase{
public:
	UCommand();
	NO_COPY(UCommand)
	DESTRUCTOR(UCommand)
public:
	const ComPtr<Dx12Fence>& GetFence() const { return m_cpFence; }
	const ComPtr<Dx12CommandQueue>& GetCmdQue() { return m_cpCommandQueue; }
	const ComPtr<Dx12GraphicsCommandList>& GetGpuCmdList() { return m_cpGraphicsCmdList; }

	const _bool& IsGpuWorkEnd() const { return m_isGpuWorkEnd; }
public:
	virtual void Free() override;
	virtual HRESULT NativeConstruct(CSHPTRREF<UDevice> _spDevice, const ComPtr<Dx12CommandQueue>& _cpCommandQueue);
	virtual void WaitForSynchronization();
	void  SubmitGpuData();
	 void GpuCpuSynchronization();

	 virtual void BindRootSignature(CSHPTRREF<URootSignature> _spRootSignature);
	 virtual void Clear();
protected:
	const _uint& GetCurFenceValue() const { return m_iCurrentFence; }
	const HANDLE& GetFenceEvent() const { return m_hFenceEvent; }

	void SetCurrentFenceValue(const _uint _iCurFenceValue) { this->m_iCurrentFence = _iCurFenceValue; }

	HRESULT CreateGraphicsList(CSHPTRREF<UDevice> _spDevice,
		const ComPtr <Dx12CommandAllocator>& _cpAllocator, const D3D12_COMMAND_LIST_TYPE& _eType,
		const _uint _iNodeMask = 0);

	void SetEventOnCompletion(const _uint _iFenceValue);
	void WaitSingleObject();
	void Signal(_uint& _iFenceValue);

	// Create Command Queue
	void CreateCommandQueue(CSHPTRREF<UDevice> _spDevice, const D3D12_COMMAND_LIST_TYPE& _eType);
protected:
	static constexpr _uint										CMD_LIST_VALUE{1};
private:
	ComPtr<Dx12Fence>									m_cpFence;
	// Fences 
	_uint																	m_iCurrentFence;
	static	_uint														s_iRenderFence;
	// FameContext ฐüทร
	HANDLE																m_hFenceEvent;
	// Graphic Commands
	ComPtr<Dx12CommandQueue>			m_cpCommandQueue;
	ComPtr<Dx12GraphicsCommandList>		m_cpGraphicsCmdList;
	_bool																	m_isGpuWorkEnd;
};

END