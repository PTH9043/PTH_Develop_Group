#include "EngineDefine.h"
#include "UCommand.h"
#include "UDevice.h"
#include "URootSignature.h"

_uint UCommand::s_iRenderFence;

UCommand::UCommand() :
	m_cpFence{ nullptr },
	m_iCurrentFence{ 0 },
	m_hFenceEvent{ nullptr },
	m_cpCommandQueue{ nullptr },
	m_isGpuWorkEnd{ false }
{
}

void UCommand::Free()
{
	::CloseHandle(m_hFenceEvent);
	m_hFenceEvent = NULL;
}

HRESULT UCommand::NativeConstruct(CSHPTRREF<UDevice> _spDevice, const ComPtr<Dx12CommandQueue>& _cpCommandQueue)
{
	RETURN_CHECK(nullptr == _spDevice, E_FAIL);
	// CreateFence 
	{
		// CPU, GPU 동기화 부분으로 사용됨
		RETURN_CHECK_DXOBJECT(_spDevice->GetDV()->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_cpFence)),
			E_FAIL);
		m_hFenceEvent = ::CreateEvent(nullptr, FALSE, FALSE, nullptr);

		m_cpCommandQueue = _cpCommandQueue;
	}
	return S_OK;
}

void UCommand::WaitForSynchronization()
{

}

void UCommand::SubmitGpuData()
{
	GetGpuCmdList()->Close();
	// 그래픽스 커맨드 실행
	ID3D12CommandList* CmdListArr[] = { GetGpuCmdList().Get() };
	GetCmdQue()->ExecuteCommandLists(CMD_LIST_VALUE, CmdListArr);
}

void UCommand::GpuCpuSynchronization()
{
	++m_iCurrentFence;
	// 현재 울타리 지점까지의 명령들을 표시하도록 울타리 값을 전진 시킨다.
	// Signal
	m_cpCommandQueue->Signal(m_cpFence.Get(), m_iCurrentFence);
	// 넘치지 않는지 확인 
	if (m_iCurrentFence > m_cpFence->GetCompletedValue())
	{
		// GPU가 현재 울타리 지점에 도달했으면 이벤트 발동
		m_cpFence->SetEventOnCompletion(m_iCurrentFence, m_hFenceEvent);
		// 이벤트를 기다린다.
		WaitForSingleObject(m_hFenceEvent, INFINITE);
	}
}

void UCommand::BindRootSignature(CSHPTRREF<URootSignature> _spRootSignature)
{
	m_cpGraphicsCmdList->SetGraphicsRootSignature(_spRootSignature->GetRootSignature().Get());
}

void UCommand::Clear()
{

}

HRESULT UCommand::CreateGraphicsList(CSHPTRREF<UDevice> _spDevice, const ComPtr<Dx12CommandAllocator>& _cpAllocator,
	const D3D12_COMMAND_LIST_TYPE& _eType, const _uint _iNodeMask)
{
	RETURN_CHECK_DXOBJECT(_spDevice->GetDV()->CreateCommandList(_iNodeMask, _eType,
		_cpAllocator.Get(), nullptr, IID_PPV_ARGS(&m_cpGraphicsCmdList)), E_FAIL);

	return S_OK;
}

void UCommand::SetEventOnCompletion(const _uint _iFenceValue)
{
	m_cpFence->SetEventOnCompletion(_iFenceValue, m_hFenceEvent);
}

void UCommand::WaitSingleObject()
{
	WaitForSingleObject(m_hFenceEvent, INFINITE);
}

void  UCommand::Signal(_uint& _iFenceValue)
{
	m_cpCommandQueue->Signal(m_cpFence.Get(), _iFenceValue);
}

void UCommand::CreateCommandQueue(CSHPTRREF<UDevice> _spDevice, const D3D12_COMMAND_LIST_TYPE& _eType)
{
	D3D12_COMMAND_QUEUE_DESC GraphicsCommandQueue{};
	GraphicsCommandQueue.Type = _eType;
	GraphicsCommandQueue.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	_spDevice->GetDV()->CreateCommandQueue(&GraphicsCommandQueue,
		IID_PPV_ARGS(&m_cpCommandQueue));
}
