#include "EngineDefine.h"
#include "UComputeRootSignature.h"
#include "UDevice.h"

UComputeRootSignature::UComputeRootSignature()
{
}

void UComputeRootSignature::Free()
{
}

HRESULT UComputeRootSignature::CreateRootSignature(CSHPTRREF<UDevice> _spDevice)
{
	ARRAY< CD3DX12_DESCRIPTOR_RANGE, 3> Ranges =
	{
		CD3DX12_DESCRIPTOR_RANGE(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, CBV_REGISTER_END, 0, GetRegisterSpace()), // b0~b14
		CD3DX12_DESCRIPTOR_RANGE(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, SRV_REGISTER_END, 0, GetRegisterSpace()), // t0~t14
		CD3DX12_DESCRIPTOR_RANGE(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, UAV_REGISTER_END, 0, GetRegisterSpace()), // u0~u14
	};
	// ConstantBuffer View를 만들고
	const _uint PARAM_SIZE = 1;
	ARRAY<CD3DX12_ROOT_PARAMETER, PARAM_SIZE> Param = {};
	// DescriptorTable을 Range 만큼 만든다. 
	Param[0].InitAsDescriptorTable(static_cast<_uint>(Ranges.size()), Ranges.data()); // 0번 -> b0 -> CBV

	D3D12_ROOT_SIGNATURE_DESC tDesc = CD3DX12_ROOT_SIGNATURE_DESC(static_cast<_uint>(Param.size()), Param.data(),
		static_cast<_uint>(GetSamplers().size()), GetSamplers().data());

	tDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_NONE;

	ComPtr<DxBlob> BlobSignature = nullptr;
	ComPtr<DxBlob> BlobError = nullptr;
	// RootSignature 상태를 알리고 
	RETURN_CHECK_DXOBJECT(::D3D12SerializeRootSignature(&tDesc, D3D_ROOT_SIGNATURE_VERSION_1,
		&BlobSignature, &BlobError), E_FAIL);
	return Create(_spDevice, BlobSignature);
}
