#include "EngineDefine.h"
#include "UVIBufferTrail.h"
#include "UMethod.h"
#include "UShader.h"
#include "UGpuCommand.h"

UVIBufferTrail::UVIBufferTrail(CSHPTRREF<UDevice> _spDevice, const VIBUFFERTYPE _eBufferType)
	: UVIBuffer(_spDevice, VISPACE_TYPE::SPACE_2D, VIINSTANCE_TYPE::SINGLE, _eBufferType), m_pBufferDataBegin{nullptr}
{
}

UVIBufferTrail::UVIBufferTrail(const UVIBufferTrail& _rhs) :
	UVIBuffer(_rhs), m_pBufferDataBegin{ nullptr }
{
}

void UVIBufferTrail::Free()
{

	//GetVertexGpuBuffer()->Unmap(0, nullptr);


}

HRESULT UVIBufferTrail::NativeConstruct()
{
	RETURN_CHECK_FAILED(__super::NativeConstruct(), E_FAIL);	
	
	

	//RETURN_CHECK_FAILED(__super::NativeConstructClone(_vecDatas), E_FAIL);
	//{
	//	if (_vecDatas.size() > 0)
	//	{
	//		m_tTrailBifferDesc = UMethod::ConvertTemplate_Index<TRAILBUFFERDESC>(_vecDatas, 0);
	//	}
	//}

	//{
	//	// Vertex ����
	//	const _uint VERTEX_CNT = 102;
	//	
	//	{
	//		VECTOR<VTXDEFAULT>	Vertecies;
	//		Vertecies.resize(VERTEX_CNT);

	//		POSVECTOR VertexPos;
	//		VertexPos.resize(VERTEX_CNT);
	//		{
	//			for (_uint i = 0; i < VERTEX_CNT; i += 2)
	//			{
	//				_float3 vStartPos;
	//				XMStoreFloat3(&vStartPos, XMLoadFloat4x4(m_tTrailBifferDesc.pStartMatrix).r[3]);
	//				_float3 vEndPos;
	//				XMStoreFloat3(&vEndPos, XMLoadFloat4x4(m_tTrailBifferDesc.pEndMatrix).r[3]);

	//				Vertecies[i].vPosition = vStartPos;
	//				Vertecies[i + 1].vPosition = vEndPos;
	//			}
	//			for (_uint i = 0; i < VERTEX_CNT; i += 2)
	//			{
	//				Vertecies[i].vTexUV = _float2(0.f, 0.f);
	//				Vertecies[i + 1].vTexUV = _float2(0.f, 1.f);
	//			}
	//		}
	//		// Position ����
	//		for (_uint i = 0; i < VERTEX_CNT; ++i)
	//			VertexPos[i] = Vertecies[i].vPosition;

	//		RETURN_CHECK_FAILED(CreateVtxBuffer(VERTEX_CNT, sizeof(VTXDEFAULT), Vertecies.data(),
	//			D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST, VertexPos), E_FAIL);
	//	}
	//	
	//}
	//// Index
	//{
	//	// Index ����
	//	const _uint INDICES_CNT = 100;
	//	// ���
	//	INDICIES16* pIndices = Make::AllocBuffer<INDICIES16>(INDICES_CNT);
	//	ZeroMemory(pIndices, sizeof(INDICIES16) * INDICES_CNT);
	//	for (_uint i = 0; i < INDICES_CNT; i += 2)
	//	{
	//		pIndices[i]._1 = i + 3;
	//		pIndices[i]._2 = i + 1;
	//		pIndices[i]._3 = i;

	//		pIndices[i + 1]._1 = i + 2;
	//		pIndices[i + 1]._2 = i + 3;
	//		pIndices[i + 1]._3 = i;
	//	}
	//	RETURN_CHECK_FAILED(CreateIndexBuffer(INDICES_CNT,
	//		sizeof(INDICIES16), pIndices, DXGI_FORMAT_R16_UINT), E_FAIL);
	//}
	//HRESULT hr = GetVertexGpuBuffer()->Map(0, nullptr, reinterpret_cast<void**>(&m_pBufferDataBegin));

	return S_OK;
}

HRESULT UVIBufferTrail::NativeConstructClone(const VOIDDATAS& _vecDatas)
{
	RETURN_CHECK_FAILED(__super::NativeConstructClone(_vecDatas), E_FAIL);
	RETURN_CHECK_FAILED(CreateVtxBufferWithNoData(300, sizeof(VTXDEFAULT), nullptr,
		D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER), E_FAIL);
	//RETURN_CHECK_FAILED(CreateVtxBuffer(100, sizeof(VTXDEFAULT), nullptr,
	//	D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST), E_FAIL);

	m_stD3DVertexBufferView.BufferLocation = m_cpVertexGpuBuffer->GetGPUVirtualAddress();
	m_stD3DVertexBufferView.StrideInBytes = sizeof(VTXDEFAULT);
	m_stD3DVertexBufferView.SizeInBytes = sizeof(VTXDEFAULT) * 16;
	HRESULT hr = m_cpVertexGpuBuffer->Map(0, nullptr, reinterpret_cast<void**>(&m_pBufferDataBegin));
	
	return S_OK;


}

HRESULT UVIBufferTrail::Render(CSHPTRREF<UShader> _spShader, CSHPTRREF<UCommand> _spCommand, const _uint& _VerticesCnt)
{
	RETURN_CHECK(nullptr == _spShader || nullptr == _spCommand, E_FAIL);
	_spShader->CommitLocalShaderDatas(_spCommand);
	const ComPtr<Dx12GraphicsCommandList>& pGraphicCmdList = _spCommand->GetGpuCmdList();
	pGraphicCmdList->IASetPrimitiveTopology(m_ePrimitiveTopology);
	// Vertex
	{
		pGraphicCmdList->IASetVertexBuffers(0, 1, &m_stD3DVertexBufferView);
	}
	
	pGraphicCmdList->DrawInstanced(_VerticesCnt, 1, 0, 0);
	
	
	return S_OK;
}



void UVIBufferTrail::SetVertices(VECTOR<VTXDEFAULT>& pVertices, _int iVertexCount) {

	//m_stD3DVertexBufferView.SizeInBytes = sizeof(VTXDEFAULT) * iVertexCount;
	//m_iVertexCnt = iVertexCount;
	//memcpy(m_pBufferDataBegin, pVertices.data(), sizeof(VTXDEFAULT) * iVertexCount);
	
	if (GetVertexGpuBuffer() != nullptr) {
		
		
		/*if (FAILED(hr) || m_pBufferDataBegin == nullptr) {
			throw std::runtime_error("Failed to map GPU buffer.");
		}*/

		// ������ ����
		memcpy(m_pBufferDataBegin, pVertices.data(), sizeof(VTXDEFAULT) * iVertexCount);

	

		// ��Ÿ ��� ���� ����
		m_stD3DVertexBufferView.SizeInBytes = sizeof(VTXDEFAULT) * iVertexCount;
		m_iVertexCnt = iVertexCount;
	}
	
}


//HRESULT UVIBufferTrail::Update(_double TimeDelta, _float4x4 WorldMatrix, _bool bUpdate, _bool& bShow)
//{
//    m_StartMatrix = *m_tTrailBifferDesc.pStartMatrix;
//    m_EndMatrix = *m_tTrailBifferDesc.pEndMatrix;
//
//    _float4x4 BoneMatrixStart = XMLoadFloat4x4(&m_StartMatrix);
//    _float4x4 BoneMatrixEnd = XMLoadFloat4x4(&m_EndMatrix);
//
//    BoneMatrixStart.Set_Right(XMVector3Normalize(BoneMatrixStart.Get_Right()));
//    BoneMatrixStart.Set_Up(XMVector3Normalize(BoneMatrixStart.Get_Up()));
//    BoneMatrixStart.Set_Look(XMVector3Normalize(BoneMatrixStart.Get_Look()));
//    BoneMatrixEnd.Set_Right(XMVector3Normalize(BoneMatrixEnd.Get_Right()));
//    BoneMatrixEnd.Set_Up(XMVector3Normalize(BoneMatrixEnd.Get_Up()));
//    BoneMatrixEnd.Set_Look(XMVector3Normalize(BoneMatrixEnd.Get_Look()));
//
//    BoneMatrixStart = BoneMatrixStart * XMLoadFloat4x4(&m_tTrailBifferDesc.PivotMatrix);
//    BoneMatrixEnd = BoneMatrixEnd * XMLoadFloat4x4(&m_tTrailBifferDesc.PivotMatrix);
//
//    BoneMatrixStart *= WorldMatrix;
//    BoneMatrixEnd *= WorldMatrix;
//
//    _float3 vLook = XMVector3Normalize(BoneMatrixEnd.Get_Pos() - BoneMatrixStart.Get_Pos());
//    BoneMatrixStart.Set_Pos(BoneMatrixStart.Get_Pos() + vLook * m_tTrailBifferDesc.fReviseStart);
//    BoneMatrixEnd.Set_Pos(BoneMatrixEnd.Get_Pos() + vLook * m_tTrailBifferDesc.fReviseEnd);
//
//    if (!bUpdate)
//    {
//        m_EndTimeAcc += TimeDelta;
//        if (m_EndTime <= m_EndTimeAcc)
//        {
//            m_EndTimeAcc = 0.0;
//            m_iCount = 0;
//            m_bStart = false;
//            bShow = false;
//        }
//    }
//
//    if (!m_bStart)
//    {
//        m_bStart = true;
//        for (_uint i = 0; i < GetVertexCnt(); i += 2)
//        {
//            XMStoreFloat3(&m_pBufferDataBegin[i].vPosition, BoneMatrixStart.Get_Pos());
//            XMStoreFloat3(&m_pBufferDataBegin[i + 1].vPosition, BoneMatrixEnd.Get_Pos());
//        }
//        m_iCount += 2;
//    }
//
//    if (100 <= m_iCount)
//    {
//        _uint iRemoveCount = m_iCount / 10;
//        m_iCount -= iRemoveCount;
//        for (_uint i = 0; i < m_iCount; i += 2)
//        {
//            m_pBufferDataBegin[i].vPosition = m_pBufferDataBegin[iRemoveCount + i].vPosition;
//            m_pBufferDataBegin[i + 1].vPosition = m_pBufferDataBegin[iRemoveCount + i + 1].vPosition;
//        }
//    }
//
//    _int iEndIndex = m_iCatmullRomCount * 2 + m_iCount;
//    m_iCatmullRomIndex[2] = iEndIndex - 2;
//    m_iCatmullRomIndex[3] = iEndIndex;
//
//    m_pBufferDataBegin[iEndIndex - 2].vPosition = m_pBufferDataBegin[m_iCount - 2].vPosition;
//    m_pBufferDataBegin[iEndIndex - 1].vPosition = m_pBufferDataBegin[m_iCount - 1].vPosition;
//
//    XMStoreFloat3(&m_pBufferDataBegin[iEndIndex].vPosition, BoneMatrixStart.Get_Pos());
//    XMStoreFloat3(&m_pBufferDataBegin[iEndIndex + 1].vPosition, BoneMatrixEnd.Get_Pos());
//
//    for (_uint i = 0; i < m_iCatmullRomCount; ++i)
//    {
//        _uint index = i * 2 + m_iCount - 2;
//        _float fWeight = _float(i + 1) / (m_iCatmullRomCount + 1);
//
//        _float3 Pos = XMVectorCatmullRom(XMLoadFloat3(&m_pBufferDataBegin[m_iCatmullRomIndex[0]].vPosition),
//            XMLoadFloat3(&m_pBufferDataBegin[m_iCatmullRomIndex[1]].vPosition),
//            XMLoadFloat3(&m_pBufferDataBegin[m_iCatmullRomIndex[2]].vPosition),
//            XMLoadFloat3(&m_pBufferDataBegin[m_iCatmullRomIndex[3]].vPosition),
//            fWeight);
//        XMStoreFloat3(&m_pBufferDataBegin[index].vPosition, Pos);
//
//        Pos = XMVectorCatmullRom(XMLoadFloat3(&m_pBufferDataBegin[m_iCatmullRomIndex[0] + 1].vPosition),
//            XMLoadFloat3(&m_pBufferDataBegin[m_iCatmullRomIndex[1] + 1].vPosition),
//            XMLoadFloat3(&m_pBufferDataBegin[m_iCatmullRomIndex[2] + 1].vPosition),
//            XMLoadFloat3(&m_pBufferDataBegin[m_iCatmullRomIndex[3] + 1].vPosition),
//            fWeight);
//        XMStoreFloat3(&m_pBufferDataBegin[index + 1].vPosition, Pos);
//    }
//    m_iCount = iEndIndex + 2;
//
//    for (_uint i = 0; i < m_iCount; i += 2)
//    {
//        XMStoreFloat2(&m_pBufferDataBegin[i].vTexUV, XMVectorSet((_float)i / ((_float)m_iCount), 1.f, 0.f, 0.f));
//        XMStoreFloat2(&m_pBufferDataBegin[i + 1].vTexUV, XMVectorSet((_float)i / ((_float)m_iCount), 0.f, 0.f, 0.f));
//    }
//
//    SetVertexBufferViewSizeInBytes(sizeof(VTXDEFAULT) * m_iCount);
//
//    m_iCatmullRomIndex[0] = m_iCatmullRomIndex[1];
//    m_iCatmullRomIndex[1] = m_iCatmullRomIndex[2];
//
//    return S_OK;
//}