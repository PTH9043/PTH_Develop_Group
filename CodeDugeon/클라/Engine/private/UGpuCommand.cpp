#include "EngineDefine.h"
#include "UGpuCommand.h"
#include "UDevice.h"

UGpuCommand::UGpuCommand() :
	UCommand(),
	m_arrFrameContexts{},
	m_iFrameValue{ 0 },
	m_iFenceLastSignaledValue{ 0 },
	m_iFrameIndex{ 0 },
	m_arrResourceLoaderGroup{},
	m_iGraphicResourceUploadIndex{ 0 },
	m_isInitUpload{ false }
{
}

void UGpuCommand::Free()
{
}

HRESULT UGpuCommand::NativeConstruct(CSHPTRREF<UDevice> _spDevice, const ComPtr<Dx12CommandQueue>& _cpCommandQueue)
{
	RETURN_CHECK_FAILED(__super::NativeConstruct(_spDevice, _cpCommandQueue), E_FAIL);
	// Create Command 
	{
		// 직접 명령 할당자 생성
		for (_uint i = 0; i < FRAME_CNT; ++i) {
			RETURN_CHECK_DXOBJECT(_spDevice->GetDV()->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT,
				IID_PPV_ARGS(&m_arrFrameContexts[i].cpAllocator)), E_FAIL);
		}
		{
			// GPU가 하나인 시스템에서는 0, Direct or Bundle 초기 상태 (그리기 명령은 nullptr 지정)
			// Command List에서는 Open Close 개념이 있는데 Open에는 명령어 집어 넣고 Close 한 다음 제출
			// Create CommandList

			RETURN_CHECK_FAILED(CreateGraphicsList(_spDevice, m_arrFrameContexts[0].cpAllocator,
				D3D12_COMMAND_LIST_TYPE::D3D12_COMMAND_LIST_TYPE_DIRECT), E_FAIL);
			RETURN_CHECK_FAILED(GetGpuCmdList()->Close(), E_FAIL);
		}
		{
			for (auto& ResourceUpLoader : m_arrResourceLoaderGroup)
			{
				// GPU가 하나인 시스템에서는 0, Direct or Bundle 초기 상태 (그리기 명령은 nullptr 지정)
				// Create Command Allocator
				RETURN_CHECK_FAILED(_spDevice->GetDV()->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT,
					IID_PPV_ARGS(&ResourceUpLoader.cpCmdAlloc)), E_FAIL);
				// Command List에서는 Open Close 개념이 있는데 Open에는 명령어 집어 넣고 자원할당을 위해 close는 하지 않음
				// Create CommandList
				RETURN_CHECK_FAILED(_spDevice->GetDV()->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT,
					ResourceUpLoader.cpCmdAlloc.Get(), nullptr, IID_PPV_ARGS(&ResourceUpLoader.cpGraphicCmdList)), E_FAIL);
			}
		}
	}
	return S_OK;
}

void UGpuCommand::Clear()
{
	m_arrFrameContexts[m_iFrameIndex].cpAllocator->Reset();
	GetGpuCmdList()->Reset(m_arrFrameContexts[m_iFrameIndex].cpAllocator.Get(), nullptr);
}

void UGpuCommand::WaitForSynchronization()
{
	GetGpuCmdList()->Close();
	// 그래픽스 커맨드 실행
	ID3D12CommandList* CmdListArr[] = { GetGpuCmdList().Get() };
	GetCmdQue()->ExecuteCommandLists(CMD_LIST_VALUE, CmdListArr);
	// 동기화
	GpuCpuSynchronization();

	Clear();
}

void UGpuCommand::WaitForLastSubmittedFrame()
{
	FRAMECONTEXT* frameCtx = &m_arrFrameContexts[m_iFrameValue];

	_uint fenceValue = frameCtx->fenceValue;
	if (fenceValue == 0)
		return; // No fence was signaled

	frameCtx->fenceValue = 0;
	if (GetFence()->GetCompletedValue() >= fenceValue)
		return;

	SetEventOnCompletion(fenceValue);
	WaitSingleObject();
}

UGpuCommand::FRAMECONTEXT* UGpuCommand::WaitForNextFrameResources(const HANDLE& _hSwapchainEvent, const _uint _iBackBufferIndex)
{
	m_iFrameValue = (m_iFrameValue + 1) % FRAME_CNT;

	HANDLE waitableObjects[] = { _hSwapchainEvent, NULL };
	DWORD numWaitableObjects = 1;

	m_iFrameIndex = m_iFrameValue;
	FRAMECONTEXT* frameCtx = &m_arrFrameContexts[m_iFrameIndex];
	_uint fenceValue = frameCtx->fenceValue;

	if (0 != fenceValue)
	{
		frameCtx->fenceValue = 0;
		SetEventOnCompletion(fenceValue);
		waitableObjects[1] = GetFenceEvent();
		numWaitableObjects = 2;
	}

	WaitForMultipleObjects(numWaitableObjects, waitableObjects, TRUE, 1);
	return frameCtx;
}

void UGpuCommand::WaitForEndFrameResource()
{
	_uint fenceValue = m_iFenceLastSignaledValue + 1;
	GetCmdQue()->Signal(GetFence().Get(), fenceValue);
	m_iFenceLastSignaledValue = fenceValue;
	m_arrFrameContexts[m_iFrameIndex].fenceValue = fenceValue;
}

void UGpuCommand::WaitForGpuResourceUpload()
{
	_uint iIndex = 0;
	if (m_isInitUpload)
	{
		if (m_iGraphicResourceUploadIndex > 0)
		{
			iIndex = m_iGraphicResourceUploadIndex - 1;
		}
		else
		{
			iIndex = RESOURCE_LOADER_CNT - 1;
		}
		m_arrResourceLoaderGroup[iIndex].cpCmdAlloc->Reset();
		m_arrResourceLoaderGroup[iIndex].cpGraphicCmdList->Reset(m_arrResourceLoaderGroup[iIndex].cpCmdAlloc.Get(), nullptr);
	}
	m_isInitUpload = true;
	// close
	m_arrResourceLoaderGroup[m_iGraphicResourceUploadIndex].cpGraphicCmdList->Close();
	// Excute
	ID3D12CommandList* cmdListArr[] = { m_arrResourceLoaderGroup[m_iGraphicResourceUploadIndex].cpGraphicCmdList.Get() };
	GetCmdQue()->ExecuteCommandLists(_countof(cmdListArr), cmdListArr);
	// Synchronization
	GpuCpuSynchronization();
	// 1밀리 세컨드만큼 기다린다. 
	ThreadMiliRelax(1);

	++m_iGraphicResourceUploadIndex;
	if (m_iGraphicResourceUploadIndex >= RESOURCE_LOADER_CNT)
	{
		m_iGraphicResourceUploadIndex = 0;
	}
}
