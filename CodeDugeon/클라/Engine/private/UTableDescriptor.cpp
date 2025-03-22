#include "EngineDefine.h"
#include "UTableDescriptor.h"
#include "UDevice.h"
#include "UCommand.h"
#include "UMethod.h"

UTableDescriptor::UTableDescriptor() :
	m_spDevice{ nullptr },
	m_cpDescriptorHeap{ nullptr },
	m_iHandleSize{ 0 },
	m_iGroupSize{ 0 },
	m_iGroupCnt{ 0 },
	m_iCurrentGroupIndex{ 0 },
	m_iStaticValue{ 0 }
{
}

void UTableDescriptor::Free()
{
}

HRESULT UTableDescriptor::NativeConstruct(CSHPTRREF<UDevice> _spDevice, const _uint _iCnt, const _uint _iReigsterCnt, const _uint _iStatic)
{
	RETURN_CHECK(nullptr == _spDevice, E_FAIL);
	m_spDevice = _spDevice;

	D3D12_DESCRIPTOR_HEAP_DESC desc{};
	desc.NumDescriptors = _iCnt * (_iReigsterCnt - _iStatic);
	desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;

	RETURN_CHECK_DXOBJECT(_spDevice->GetDV()->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&m_cpDescriptorHeap)), E_FAIL);

	m_iHandleSize = _spDevice->GetDV()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	m_iGroupSize = m_iHandleSize * (_iReigsterCnt - _iStatic);
	m_iGroupCnt = _iCnt;
	m_iStaticValue = _iStatic;
	return S_OK;
}

void UTableDescriptor::BindAndClear(CSHPTRREF<UCommand> _spCommand)
{
	Dx12DescriptorHeap* pTableDescirptors[] = { m_cpDescriptorHeap.Get() };
	_spCommand->GetGpuCmdList()->SetDescriptorHeaps(COMMAND_BIND_RANGE, pTableDescirptors);
	m_iCurrentGroupIndex = 0;
}

void UTableDescriptor::Clear()
{
	m_iCurrentGroupIndex = 0;
}

void UTableDescriptor::BindCBV(const D3D12_CPU_DESCRIPTOR_HANDLE& _hCbvHandle, const CBV_REGISTER& _eCbvRegister)
{
	D3D12_CPU_DESCRIPTOR_HANDLE destHandle = GetCpuHandle(_eCbvRegister);
	CopyDesc(destHandle, _hCbvHandle);
}

void UTableDescriptor::BindSRV(const D3D12_CPU_DESCRIPTOR_HANDLE& _hSrvHandle, const SRV_REGISTER& _eSrvRegister)
{
	D3D12_CPU_DESCRIPTOR_HANDLE destHandle = GetCpuHandle(_eSrvRegister);
	CopyDesc(destHandle, _hSrvHandle);
}

void UTableDescriptor::BindUAV(const D3D12_CPU_DESCRIPTOR_HANDLE& _uavHandle, const UAV_REGISTER& _eReg)
{
	D3D12_CPU_DESCRIPTOR_HANDLE destHandle = GetCpuHandle(_eReg);
	CopyDesc(destHandle, _uavHandle);
	// 리소스 상태 변경
}

void UTableDescriptor::CommitGraphicTable(CSHPTRREF<UCommand> _spCommand)
{
	// CommitTable ->DescriptorHeapStart를 가져와서 Group만큼 Bind 하는 것이다. 
	_spCommand->GetGpuCmdList()->SetGraphicsRootDescriptorTable(GetStaticValue(), ReturnCommitTableHandle(_spCommand));
}

void UTableDescriptor::CommitComputeTable(CSHPTRREF<UCommand> _spCommand)
{
	_spCommand->GetGpuCmdList()->SetComputeRootDescriptorTable(GetStaticValue(), ReturnCommitTableHandle(_spCommand));
}

const D3D12_CPU_DESCRIPTOR_HANDLE UTableDescriptor::GetCpuHandle(const CBV_REGISTER& _eCbvRegister)
{
	return ComputeCpuHandle(static_cast<_uint>(_eCbvRegister));
}

const D3D12_CPU_DESCRIPTOR_HANDLE UTableDescriptor::GetCpuHandle(const  SRV_REGISTER& _eSrvRegister)
{
	return ComputeCpuHandle(static_cast<_uint>(_eSrvRegister));
}

const D3D12_CPU_DESCRIPTOR_HANDLE UTableDescriptor::GetCpuHandle(const UAV_REGISTER& _eReg)
{
	return ComputeCpuHandle(static_cast<_uint>(_eReg));
}

void UTableDescriptor::ReComputeVariable(const _uint _iReigsterCnt, const _uint _iStatic)
{
	m_iGroupSize = m_iHandleSize * (_iReigsterCnt - _iStatic);
	m_iStaticValue = _iStatic;
}

HRESULT UTableDescriptor::CreateDescriptor(const D3D12_DESCRIPTOR_HEAP_DESC& _DescriptorDesc)
{
	RETURN_CHECK_DXOBJECT(m_spDevice->GetDV()->CreateDescriptorHeap(&_DescriptorDesc,
		IID_PPV_ARGS(&m_cpDescriptorHeap)), E_FAIL);
	return S_OK;
}

void UTableDescriptor::CopyDesc(const D3D12_CPU_DESCRIPTOR_HANDLE& _hDescHandle, const D3D12_CPU_DESCRIPTOR_HANDLE& _hSrvHandle)
{
	GetDevice()->GetDV()->CopyDescriptors(DEST_RANGE, &_hDescHandle, &DEST_RANGE, SRC_RANGE,
		&_hSrvHandle, &SRC_RANGE, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
}

D3D12_GPU_DESCRIPTOR_HANDLE UTableDescriptor::ReturnCommitTableHandle(CSHPTRREF<UCommand> _spCommand)
{
	const ComPtr<Dx12GraphicsCommandList>& pComputeCmdList = _spCommand->GetGpuCmdList();
	D3D12_GPU_DESCRIPTOR_HANDLE handle = m_cpDescriptorHeap->GetGPUDescriptorHandleForHeapStart();
	handle.ptr += (m_iCurrentGroupIndex++ * m_iGroupSize);

	return handle;
}

const D3D12_CPU_DESCRIPTOR_HANDLE UTableDescriptor::ComputeCpuHandle(_uint _iReg)
{
	D3D12_CPU_DESCRIPTOR_HANDLE Handle = m_cpDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	Handle.ptr += (m_iCurrentGroupIndex * m_iGroupSize);
	Handle.ptr += ((_iReg - m_iStaticValue) * m_iHandleSize);
	return Handle;
}