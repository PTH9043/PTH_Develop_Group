#include "EngineDefine.h"
#include "URootSignature.h"
#include "UDevice.h"
#include "UGpuCommand.h"

URootSignature::URootSignature() :
	m_iRegisterSpace{ 0 },
	m_cpRootSignature{ NULL },
	m_arrSamplerDescs{}
{
}

void URootSignature::Free()
{
}

HRESULT URootSignature::NativeConstruct(CSHPTRREF<UDevice> _spDevice, const _uint _iRegisterSpace)
{
	RETURN_CHECK(nullptr == _spDevice, E_FAIL);
	m_iRegisterSpace = _iRegisterSpace;
	NativeSampler();
	// GraphicRoot
	RETURN_CHECK_FAILED(CreateRootSignature(_spDevice), E_FAIL);
	return S_OK;
}

void URootSignature::NativeSampler()
{
	// Sampler
	{
		m_arrSamplerDescs[S0] = CD3DX12_STATIC_SAMPLER_DESC(S0);
		m_arrSamplerDescs[S1] = CD3DX12_STATIC_SAMPLER_DESC(S1, // 샘플러 레지스터 슬롯
			D3D12_FILTER_MIN_MAG_MIP_LINEAR, // 필터 설정
			D3D12_TEXTURE_ADDRESS_MODE_CLAMP, // U 축 주소 모드
			D3D12_TEXTURE_ADDRESS_MODE_CLAMP, // V 축 주소 모드
			D3D12_TEXTURE_ADDRESS_MODE_CLAMP  // W 축 주소 모드
		);
		m_arrSamplerDescs[S2].Filter = D3D12_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT;
		m_arrSamplerDescs[S2].AddressU = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
		m_arrSamplerDescs[S2].AddressV = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
		m_arrSamplerDescs[S2].AddressW = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
		m_arrSamplerDescs[S2].MipLODBias = 0.0f;
		m_arrSamplerDescs[S2].MaxAnisotropy = 1;
		m_arrSamplerDescs[S2].ComparisonFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL; //D3D12_COMPARISON_FUNC_LESS
		m_arrSamplerDescs[S2].BorderColor = D3D12_STATIC_BORDER_COLOR_OPAQUE_WHITE; // D3D12_STATIC_BORDER_COLOR_OPAQUE_BLACK;
		m_arrSamplerDescs[S2].MinLOD = 0;
		m_arrSamplerDescs[S2].MaxLOD = D3D12_FLOAT32_MAX;
		m_arrSamplerDescs[S2].ShaderRegister = 2;
		m_arrSamplerDescs[S2].RegisterSpace = 0;
		m_arrSamplerDescs[S2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	}

	for (auto& iter : m_arrSamplerDescs)
	{
		iter.RegisterSpace = GetRegisterSpace();
	}
}

HRESULT URootSignature::CreateRootSignature(CSHPTRREF<UDevice> _spDevice)
{
	{
		// B0 는 전역 상태
		ARRAY< CD3DX12_DESCRIPTOR_RANGE, 2> Ranges =
		{
			// Constant Buffer 값을 뺀 것이다.  b0, b1 제외하고 나머지 b2부터는 tabledescriptor를 통해서 바인드하겠다.
			CD3DX12_DESCRIPTOR_RANGE(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, CBV_REGISTER_END - GRAPHICS_CONSTANT_BUFFER_VALUE,
			GRAPHICS_CONSTANT_BUFFER_VALUE, GetRegisterSpace()),
			CD3DX12_DESCRIPTOR_RANGE(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, SRV_REGISTER_END, 0, GetRegisterSpace()),
		};

		// ConstantBuffer View를 만들고, B0, B1 (전역) (GPUCOmmand)
		const _uint PARAM_SIZE = 4;
		ARRAY<CD3DX12_ROOT_PARAMETER, PARAM_SIZE> Param = {};
		Param[0].InitAsConstantBufferView(static_cast<_uint>(CBV_REGISTER::B0));
		Param[1].InitAsConstantBufferView(static_cast<_uint>(CBV_REGISTER::B1));
		Param[2].InitAsConstantBufferView(static_cast<_uint>(CBV_REGISTER::B2));
		// DescriptorTable을 Range 만큼 만든다. Table
		Param[3].InitAsDescriptorTable(static_cast<_uint>(Ranges.size()), Ranges.data()); // 0번 -> b0 -> CBV

		for (auto& iter : Param)
		{
			iter.Constants.RegisterSpace = GetRegisterSpace();
			iter.Descriptor.RegisterSpace = GetRegisterSpace();
		}

		D3D12_ROOT_SIGNATURE_DESC tDesc = CD3DX12_ROOT_SIGNATURE_DESC(static_cast<_uint>(Param.size()), Param.data(),
			static_cast<_uint>(m_arrSamplerDescs.size()), m_arrSamplerDescs.data());

		// 입력 조립기 단계 사용
		tDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT; // 입력 조립기 단계

		ComPtr<DxBlob> BlobSignature = nullptr;
		ComPtr<DxBlob> BlobError = nullptr;
		// RootSignature 상태를 알리고 
		RETURN_CHECK_DXOBJECT(::D3D12SerializeRootSignature(&tDesc, D3D_ROOT_SIGNATURE_VERSION_1,
			&BlobSignature, &BlobError), E_FAIL);
		RETURN_CHECK(nullptr != BlobError, E_FAIL);
		// CreateRootSignature 
		RETURN_CHECK_DXOBJECT(_spDevice->GetDV()->CreateRootSignature(0, BlobSignature->GetBufferPointer(),
			BlobSignature->GetBufferSize(), IID_PPV_ARGS(&m_cpRootSignature)), E_FAIL);
	}
	return S_OK;
}

HRESULT URootSignature::CreateComputeRootSignature(CSHPTRREF<UDevice> _spDevice)
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

HRESULT URootSignature::Create(CSHPTRREF<UDevice> _spDevice, const ComPtr<DxBlob>& _spBlob)
{
	RETURN_CHECK_DXOBJECT(_spDevice->GetDV()->CreateRootSignature(0, _spBlob->GetBufferPointer(),
		_spBlob->GetBufferSize(), IID_PPV_ARGS(&m_cpRootSignature)), E_FAIL);

	return S_OK;
}
