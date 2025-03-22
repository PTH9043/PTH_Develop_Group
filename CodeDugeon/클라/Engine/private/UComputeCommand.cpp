#include "EngineDefine.h"
#include "UComputeCommand.h"
#include "UDevice.h"
#include "URootSignature.h"
#include "UGameInstance.h"


UComputeCommand::UComputeCommand() :
	UCommand(), m_cpCommandAllocator{nullptr}
{
}

void UComputeCommand::Free()
{
}

HRESULT UComputeCommand::NativeConstruct(CSHPTRREF<UDevice> _spDevice, const ComPtr<Dx12CommandQueue>& _cpCommandQueue)
{
	RETURN_CHECK_FAILED(__super::NativeConstruct(_spDevice, _cpCommandQueue), E_FAIL);
	const ComPtr<Dx12Device>& DEVICE = _spDevice->GetDV();
	D3D12_COMMAND_QUEUE_DESC ComputeQueuDesc{};
	ComputeQueuDesc.Type = D3D12_COMMAND_LIST_TYPE_COMPUTE;
	ComputeQueuDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	// Create 
	RETURN_CHECK_DXOBJECT(DEVICE->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_COMPUTE,
		IID_PPV_ARGS(&m_cpCommandAllocator)), E_FAIL);

	RETURN_CHECK_FAILED(CreateGraphicsList(_spDevice, m_cpCommandAllocator,
		D3D12_COMMAND_LIST_TYPE_COMPUTE), E_FAIL);

	RETURN_CHECK_FAILED(GetGpuCmdList()->Close(), E_FAIL);

	Clear();
	return S_OK;
}

void UComputeCommand::WaitForSynchronization()
{
	GetGpuCmdList()->Close();
	// 그래픽스 커맨드 실행
	ID3D12CommandList* CmdListArr[] = { GetGpuCmdList().Get() };
	GetCmdQue()->ExecuteCommandLists(CMD_LIST_VALUE, CmdListArr);
	// 동기화
	GpuCpuSynchronization();
	Clear();
}

void UComputeCommand::BindRootSignature(CSHPTRREF<URootSignature> _spRootSignature)
{
	GetGpuCmdList()->SetComputeRootSignature(_spRootSignature->GetRootSignature().Get());
}

void UComputeCommand::Clear()
{
	m_cpCommandAllocator->Reset();
	GetGpuCmdList()->Reset(m_cpCommandAllocator.Get(), nullptr);
}
