#include "EngineDefine.h"
#include "URenderCommand.h"
#include "UDevice.h"
#include "UGameInstance.h"

URenderCommand::URenderCommand() :
	m_iAllocatorIndex{ 0 }
{
}

void URenderCommand::Free()
{
}

HRESULT URenderCommand::NativeConstruct(CSHPTRREF<UDevice> _spDevice, const ComPtr<Dx12CommandQueue>& _cpCommandQueue)
{
	RETURN_CHECK_FAILED(__super::NativeConstruct(_spDevice, _cpCommandQueue), E_FAIL);
	const ComPtr<Dx12Device>& DEVICE = _spDevice->GetDV();

	//CreateCommandQueue(_spDevice, D3D12_COMMAND_LIST_TYPE_DIRECT);

	for (auto& iter : m_arrRenderAllocators)
	{
		// Create 
		RETURN_CHECK_DXOBJECT(DEVICE->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT,
			IID_PPV_ARGS(&iter)), E_FAIL);
	}

	RETURN_CHECK_FAILED(CreateGraphicsList(_spDevice, m_arrRenderAllocators[0],
		D3D12_COMMAND_LIST_TYPE_DIRECT), E_FAIL);

	RETURN_CHECK_FAILED(GetGpuCmdList()->Close(), E_FAIL);
	return S_OK;
}

void URenderCommand::WaitForSynchronization()
{
	SubmitGpuData();
	GpuCpuSynchronization();
}

void URenderCommand::Clear()
{
	m_arrRenderAllocators[m_iAllocatorIndex]->Reset();
	GetGpuCmdList()->Reset(m_arrRenderAllocators[m_iAllocatorIndex].Get(), nullptr);

	m_iAllocatorIndex = (m_iAllocatorIndex + 1) % m_arrRenderAllocators.size();
}