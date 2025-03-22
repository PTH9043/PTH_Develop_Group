#include "EngineDefine.h"
#include "UShader.h"
#include "UMethod.h"
#include "UGameInstance.h"
#include "URootSignature.h"
#include "UDevice.h"
#include "UGpuCommand.h"
#include "UTableDescriptor.h"
#include "UShaderConstantBuffer.h"
#include "UShaderStructedBuffer.h"
#include "UTexture.h"


UShader::PIPELINECONTAINER					UShader::s_m_PipeLineContainer;

UShader::UShader(CSHPTRREF<UDevice> _pDevice, CSHPTRREF<URootSignature> _spRootSignature, const SHADERDESC& _tDesc)
	: UResource(_pDevice),
	m_spCommand{ nullptr },
	m_spTableDescriptor{ nullptr },
	m_spRootSignature{ _spRootSignature },
	m_stShaderDesc{ _tDesc }
{
}

UShader::UShader(const UShader& _rhs)
	: UResource(_rhs),
	m_ShaderBlopContainer{ _rhs.m_ShaderBlopContainer },
	m_spTableDescriptor{ _rhs.m_spTableDescriptor },
	m_spRootSignature{ _rhs.m_spRootSignature },
	m_stShaderDesc{ _rhs.m_stShaderDesc },
	m_spCommand{ _rhs.m_spCommand }
{
}


void UShader::Free()
{
	m_spTableDescriptor.reset();
}

HRESULT UShader::NativeConstruct()
{
	RETURN_CHECK_FAILED(__super::NativeConstruct(), E_FAIL);
	RETURN_CHECK_FAILED(CreateShader(), E_FAIL);
	return S_OK;
}

HRESULT UShader::NativeConstructClone(const VOIDDATAS& _stDatas)
{
	return __super::NativeConstructClone(_stDatas);
}

HRESULT UShader::BindSRVBuffer(const SRV_REGISTER _eReg, CSHPTRREF<UTexture> _spTexture)
{
	RETURN_CHECK(nullptr == _spTexture, E_FAIL);
	RETURN_CHECK(_eReg >= SRV_REGISTER::SRV_END, E_FAIL);

	m_spTableDescriptor->BindSRV(_spTexture->GetShaderViewBeginHandle(), _eReg);
	return S_OK;
}

HRESULT UShader::BindCBVBuffer(CSHPTRREF<UShaderConstantBuffer> _spShaderConstantBuffer, const void* _pBuffer,
	const _uint _iSize)
{
	RETURN_CHECK(nullptr == _spShaderConstantBuffer, E_FAIL);
	// Bind Data
	return _spShaderConstantBuffer->PushData(m_spTableDescriptor, _pBuffer, _iSize);
}

HRESULT UShader::BindSRVBuffer(const SRV_REGISTER _eReg, CSHPTRREF<UShaderStructedBuffer> _spStructedBuffer)
{
	RETURN_CHECK(nullptr == _spStructedBuffer, E_FAIL);
	_spStructedBuffer->PushSRVData(m_spTableDescriptor, _eReg);
	return S_OK;
}

void UShader::BeginSettingDatas(CSHPTRREF<UCommand> _spCommand)
{
	GetTableDescriptor()->BindAndClear(_spCommand);
	SettingPipeLineState(_spCommand);
	// Set Compute Root Signature
	_spCommand->GetGpuCmdList()->SetComputeRootSignature(GetRootSignature()->GetRootSignature().Get());
}

void UShader::SettingPipeLineState(CSHPTRREF<UCommand> _spCommand)
{
	SHPTR<UGameInstance> spGameInstance = GET_INSTANCE(UGameInstance);
	spGameInstance->SettingPipeLineState(m_stShaderDesc.wstrShaderName, _spCommand);
}

void UShader::CommitLocalShaderDatas(CSHPTRREF<UCommand> _spCommand)
{
	m_spTableDescriptor->CommitGraphicTable(_spCommand);
}

HRESULT UShader::CreateShader()
{
	RETURN_CHECK(nullptr == m_stShaderDesc.pElement, E_FAIL);
	RETURN_CHECK(m_stShaderDesc.eType != GRAPHICISCONTROLTYPE::GRAPHICS, E_FAIL);
	D3D12_GRAPHICS_PIPELINE_STATE_DESC		d3dGraphicsPipeLineStateDesc;
	ZeroMemory(&d3dGraphicsPipeLineStateDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));

	for (auto& iter : m_stShaderDesc.ShaderList)
	{
		// 쉐이더 cso 파일을 로드함
		_wstring wstrPath{ L"..\\Bin\\" };
		wstrPath += m_stShaderDesc.wstrShaderName + L"_" + GetShaderType(iter) + L".cso";
		// 쉐이더 파일을 불러옴
		ComPtr<DxBlob> ShaderFile{ UMethod::LoadBinary(wstrPath) };
		m_ShaderBlopContainer.insert(std::pair<SHADERMAININFO, ComPtr<DxBlob>>(iter, ShaderFile));
		// ByteCode에 집어 넣고 PipeLine 쉐이더 종류에 맞게 집어 넣는다. 
		D3D12_SHADER_BYTECODE ByteCode{};
		ZeroMemory(&ByteCode, sizeof(D3D12_SHADER_BYTECODE));
		ByteCode = { ShaderFile->GetBufferPointer(), ShaderFile->GetBufferSize() };
		switch (iter)
		{
		case SHADERMAININFO::VS_MAIN:
			d3dGraphicsPipeLineStateDesc.VS = ByteCode;
			break;
		case SHADERMAININFO::DS_MAIN:
			d3dGraphicsPipeLineStateDesc.DS = ByteCode;
			break;
		case SHADERMAININFO::GS_MAIN:
			d3dGraphicsPipeLineStateDesc.GS = ByteCode;
			break;
		case SHADERMAININFO::PS_MAIN:
			d3dGraphicsPipeLineStateDesc.PS = ByteCode;
			break;
		case SHADERMAININFO::HS_MAIN:
			d3dGraphicsPipeLineStateDesc.HS = ByteCode;
			break;
		}
	}
	// Get Instance
	SHPTR<UGameInstance> pGameInstance = GET_INSTANCE(UGameInstance);
	// 파이프라인 스테이트를 만들기 위해 값들을 세팅한다. 
	d3dGraphicsPipeLineStateDesc.InputLayout = { m_stShaderDesc.pElement, m_stShaderDesc.iNumElement };
	d3dGraphicsPipeLineStateDesc.pRootSignature = m_spRootSignature->GetRootSignature().Get();
	d3dGraphicsPipeLineStateDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	d3dGraphicsPipeLineStateDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	d3dGraphicsPipeLineStateDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	d3dGraphicsPipeLineStateDesc.SampleMask = UINT_MAX;
	d3dGraphicsPipeLineStateDesc.PrimitiveTopologyType = GetToplologyType(m_stShaderDesc.eTopology);
	// RenderTargets 들이 있다면 그것들을 Shader 파일에서 사용하기 위해 추가한다. 
	// 기본 NumRenderTargets의 값은 1이다. 
	d3dGraphicsPipeLineStateDesc.NumRenderTargets = m_stShaderDesc.iRenderTargets;
	for (_uint i = 0; i < m_stShaderDesc.iRenderTargets; ++i) {
		d3dGraphicsPipeLineStateDesc.RTVFormats[i] = m_stShaderDesc.RenderFormats[i];
	}
	d3dGraphicsPipeLineStateDesc.SampleDesc.Count = pGameInstance->IsMssa4xState() ? 4 : 1;
	d3dGraphicsPipeLineStateDesc.SampleDesc.Quality = pGameInstance->IsMssa4xState() ? pGameInstance->GetMssa4xQuality() : 0;
	d3dGraphicsPipeLineStateDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;

	// 각 타입에 맞게 값들을 세팅해준다. 
	switch (m_stShaderDesc.eRasterRizerType)
	{
	case RASTERIZER_TYPE::CULL_BACK:
		d3dGraphicsPipeLineStateDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
		d3dGraphicsPipeLineStateDesc.RasterizerState.CullMode = D3D12_CULL_MODE_BACK;
		break;
	case RASTERIZER_TYPE::CULL_FRONT:
		d3dGraphicsPipeLineStateDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
		d3dGraphicsPipeLineStateDesc.RasterizerState.CullMode = D3D12_CULL_MODE_FRONT;
		break;
	case RASTERIZER_TYPE::CULL_NONE:
		d3dGraphicsPipeLineStateDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
		d3dGraphicsPipeLineStateDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
		break;
	case RASTERIZER_TYPE::WIREFRAME:
		d3dGraphicsPipeLineStateDesc.RasterizerState.FillMode = D3D12_FILL_MODE_WIREFRAME;
		d3dGraphicsPipeLineStateDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
		break;
	}
	// Check Depth Steincil Type 
	UMethod::ChangeDepthStencilInfo(m_stShaderDesc.eDepthStencilType, d3dGraphicsPipeLineStateDesc.DepthStencilState);

	for (_uint i = 0; i < m_stShaderDesc.iRenderTargets; ++i)
	{
		D3D12_RENDER_TARGET_BLEND_DESC& rt = d3dGraphicsPipeLineStateDesc.BlendState.RenderTarget[i];
		// SrcBlend = Pixel Shader
		// DestBlend = Render Target
		switch (m_stShaderDesc.eBlendType)
		{
		case BLEND_TYPE::DEFAULT:
			rt.BlendEnable = FALSE;
			rt.LogicOpEnable = FALSE;
			rt.SrcBlend = D3D12_BLEND_ONE;
			rt.DestBlend = D3D12_BLEND_ZERO;
			break;
		case BLEND_TYPE::ALPHA_BLEND:
			rt.BlendEnable = TRUE;
			rt.LogicOpEnable = FALSE;
			rt.SrcBlend = D3D12_BLEND_SRC_ALPHA;
			rt.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
			break;
		case BLEND_TYPE::ONE_TO_ONE_BLEND:
			rt.BlendEnable = TRUE;
			rt.LogicOpEnable = FALSE;
			rt.SrcBlend = D3D12_BLEND_ONE;
			rt.DestBlend = D3D12_BLEND_ONE;
			break;
		case BLEND_TYPE::ADD_BLEND:
			rt.BlendEnable = true;
			rt.LogicOpEnable = FALSE;
			rt.SrcBlend = D3D12_BLEND_ONE;
			rt.DestBlend = D3D12_BLEND_ONE;
			rt.BlendOp = D3D12_BLEND_OP_ADD;
			break;
		}
	}

	ComPtr<Dx12PipelineState> cpPipeLineState;
	// 파이프라인 생성
	RETURN_CHECK_DXOBJECT(GetDevice()->GetDV()->CreateGraphicsPipelineState(&d3dGraphicsPipeLineStateDesc,
		IID_PPV_ARGS(&cpPipeLineState)), E_FAIL);

	SHPTR<UGameInstance> spGameInstance = GET_INSTANCE(UGameInstance);
	spGameInstance->AddPipeLineState(m_stShaderDesc.wstrShaderName, cpPipeLineState);
	return S_OK;
}

void UShader::InsertShaderBlopContainer(const SHADERMAININFO _eInfo, const ComPtr<DxBlob>& _blob)
{
	m_ShaderBlopContainer.insert(std::pair<SHADERMAININFO, ComPtr<DxBlob>>(_eInfo, _blob));
}

const _wstring UShader::GetShaderType(const SHADERMAININFO& _eShaderType)
{
	_wstring strName{ L"" };
	if (SHADERMAININFO::VS_MAIN == _eShaderType)
		strName = L"VS";
	else if (SHADERMAININFO::HS_MAIN == _eShaderType)
		strName = L"HS";
	else if (SHADERMAININFO::DS_MAIN == _eShaderType)
		strName = L"DS";
	else if (SHADERMAININFO::GS_MAIN == _eShaderType)
		strName = L"GS";
	else if (SHADERMAININFO::PS_MAIN == _eShaderType)
		strName = L"PS";
	else if (SHADERMAININFO::CS_MAIN == _eShaderType)
		strName = L"CS";
	return strName;
}
D3D12_PRIMITIVE_TOPOLOGY_TYPE UShader::GetToplologyType(const D3D12_PRIMITIVE_TOPOLOGY _eTopology)
{
	switch (_eTopology)
	{
	case D3D_PRIMITIVE_TOPOLOGY_POINTLIST:
		return D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;
	case D3D_PRIMITIVE_TOPOLOGY_LINELIST:
	case D3D_PRIMITIVE_TOPOLOGY_LINESTRIP:
	case D3D_PRIMITIVE_TOPOLOGY_LINELIST_ADJ:
	case D3D_PRIMITIVE_TOPOLOGY_LINESTRIP_ADJ:
	case D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST_ADJ:
	case D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP_ADJ:
		return D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;
	case D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST:
	case D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP:
		return D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	case D3D_PRIMITIVE_TOPOLOGY_1_CONTROL_POINT_PATCHLIST:
	case D3D_PRIMITIVE_TOPOLOGY_2_CONTROL_POINT_PATCHLIST:
	case D3D_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST:
	case D3D_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST:
	case D3D_PRIMITIVE_TOPOLOGY_5_CONTROL_POINT_PATCHLIST:
	case D3D_PRIMITIVE_TOPOLOGY_6_CONTROL_POINT_PATCHLIST:
	case D3D_PRIMITIVE_TOPOLOGY_7_CONTROL_POINT_PATCHLIST:
	case D3D_PRIMITIVE_TOPOLOGY_8_CONTROL_POINT_PATCHLIST:
	case D3D_PRIMITIVE_TOPOLOGY_9_CONTROL_POINT_PATCHLIST:
	case D3D_PRIMITIVE_TOPOLOGY_10_CONTROL_POINT_PATCHLIST:
	case D3D_PRIMITIVE_TOPOLOGY_11_CONTROL_POINT_PATCHLIST:
	case D3D_PRIMITIVE_TOPOLOGY_12_CONTROL_POINT_PATCHLIST:
	case D3D_PRIMITIVE_TOPOLOGY_13_CONTROL_POINT_PATCHLIST:
	case D3D_PRIMITIVE_TOPOLOGY_14_CONTROL_POINT_PATCHLIST:
	case D3D_PRIMITIVE_TOPOLOGY_15_CONTROL_POINT_PATCHLIST:
	case D3D_PRIMITIVE_TOPOLOGY_16_CONTROL_POINT_PATCHLIST:
	case D3D_PRIMITIVE_TOPOLOGY_17_CONTROL_POINT_PATCHLIST:
	case D3D_PRIMITIVE_TOPOLOGY_18_CONTROL_POINT_PATCHLIST:
	case D3D_PRIMITIVE_TOPOLOGY_19_CONTROL_POINT_PATCHLIST:
	case D3D_PRIMITIVE_TOPOLOGY_20_CONTROL_POINT_PATCHLIST:
	case D3D_PRIMITIVE_TOPOLOGY_21_CONTROL_POINT_PATCHLIST:
	case D3D_PRIMITIVE_TOPOLOGY_22_CONTROL_POINT_PATCHLIST:
	case D3D_PRIMITIVE_TOPOLOGY_23_CONTROL_POINT_PATCHLIST:
	case D3D_PRIMITIVE_TOPOLOGY_24_CONTROL_POINT_PATCHLIST:
	case D3D_PRIMITIVE_TOPOLOGY_25_CONTROL_POINT_PATCHLIST:
	case D3D_PRIMITIVE_TOPOLOGY_26_CONTROL_POINT_PATCHLIST:
	case D3D_PRIMITIVE_TOPOLOGY_27_CONTROL_POINT_PATCHLIST:
	case D3D_PRIMITIVE_TOPOLOGY_28_CONTROL_POINT_PATCHLIST:
	case D3D_PRIMITIVE_TOPOLOGY_29_CONTROL_POINT_PATCHLIST:
	case D3D_PRIMITIVE_TOPOLOGY_30_CONTROL_POINT_PATCHLIST:
	case D3D_PRIMITIVE_TOPOLOGY_31_CONTROL_POINT_PATCHLIST:
	case D3D_PRIMITIVE_TOPOLOGY_32_CONTROL_POINT_PATCHLIST:
		return D3D12_PRIMITIVE_TOPOLOGY_TYPE_PATCH;
	default:
		return D3D12_PRIMITIVE_TOPOLOGY_TYPE_UNDEFINED;
	}
}
