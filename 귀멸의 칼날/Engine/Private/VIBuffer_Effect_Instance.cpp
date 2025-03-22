#include "VIBuffer_Effect_Instance.h"
#include "GameInstance.h"

CVIBuffer_Effect_Instance::CVIBuffer_Effect_Instance(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	:CVIBuffer(_pDevice, _pContext)
{
}

CVIBuffer_Effect_Instance::CVIBuffer_Effect_Instance(const CVIBuffer_Effect_Instance& _rhs)
	: CVIBuffer(_rhs)
	, m_iNumInstance(_rhs.m_iNumInstance)
	, m_iStrideInstance(_rhs.m_iStrideInstance)
	, m_tInstanceDesc(_rhs.m_tInstanceDesc)
{
	//ZeroMemory(&m_tInstanceDesc, sizeof(INSTANCEDESC));
}

HRESULT CVIBuffer_Effect_Instance::NativeConstruct_Prototype(_uint iNumInstance)
{
	m_iNumInstance = iNumInstance;

	return S_OK;
}

HRESULT CVIBuffer_Effect_Instance::NativeConstruct_Clone(void* _pArg)
{
	if (FAILED(SetUp_VertexBuffer()))
		return E_FAIL;

	if (FAILED(SetUp_IndexBuffer()))
		return E_FAIL;

	if (FAILED(SetUp_InstanceBuffer()))
		return E_FAIL;

	Reset_Pariticle();
	Reset_InstanceBuffer();

	return S_OK;
}

void CVIBuffer_Effect_Instance::Tick(const _double& _dTimeDelta)
{
	if (nullptr == m_pContext)
		return;

	m_fTime += (_float)_dTimeDelta;
	if (m_tInstanceDesc.fLoop == 0.f)
	{
		_vector vMovement;

		if (m_tInstanceDesc.KeyFrame_1_TimeEnd > m_fTime)
		{
			m_KeyFrameDesc.KeyFrame_TimeEnd = m_tInstanceDesc.KeyFrame_1_TimeEnd;
			m_KeyFrameDesc.KeyFrame_RelaxSpeed = m_tInstanceDesc.KeyFrame_1_RelaxSpeed;
			m_KeyFrameDesc.KeyFrame_Speed = m_tInstanceDesc.KeyFrame_1_Speed;
			m_KeyFrameDesc.KeyFrame_fRelaxRotation = m_tInstanceDesc.KeyFrame_1_fRelaxRotation;
			m_KeyFrameDesc.KeyFrame_Movement = m_tInstanceDesc.KeyFrame_1_Movement;
		}
		else if (m_tInstanceDesc.KeyFrame_2_TimeEnd > m_fTime)
		{
			m_KeyFrameDesc.KeyFrame_TimeEnd = m_tInstanceDesc.KeyFrame_2_TimeEnd;
			m_KeyFrameDesc.KeyFrame_RelaxSpeed = m_tInstanceDesc.KeyFrame_2_RelaxSpeed;
			m_KeyFrameDesc.KeyFrame_Speed = m_tInstanceDesc.KeyFrame_2_Speed;
			m_KeyFrameDesc.KeyFrame_fRelaxRotation = m_tInstanceDesc.KeyFrame_2_fRelaxRotation;
			m_KeyFrameDesc.KeyFrame_Movement = m_tInstanceDesc.KeyFrame_2_Movement;
		}
		else if (m_tInstanceDesc.KeyFrame_3_TimeEnd > m_fTime)
		{
			m_KeyFrameDesc.KeyFrame_TimeEnd = m_tInstanceDesc.KeyFrame_3_TimeEnd;
			m_KeyFrameDesc.KeyFrame_RelaxSpeed = m_tInstanceDesc.KeyFrame_3_RelaxSpeed;
			m_KeyFrameDesc.KeyFrame_Speed = m_tInstanceDesc.KeyFrame_3_Speed;
			m_KeyFrameDesc.KeyFrame_fRelaxRotation = m_tInstanceDesc.KeyFrame_3_fRelaxRotation;
			m_KeyFrameDesc.KeyFrame_Movement = m_tInstanceDesc.KeyFrame_3_Movement;
		}
		else if (m_tInstanceDesc.KeyFrame_4_TimeEnd > m_fTime)
		{
			m_KeyFrameDesc.KeyFrame_TimeEnd = m_tInstanceDesc.KeyFrame_4_TimeEnd;
			m_KeyFrameDesc.KeyFrame_RelaxSpeed = m_tInstanceDesc.KeyFrame_4_RelaxSpeed;
			m_KeyFrameDesc.KeyFrame_Speed = m_tInstanceDesc.KeyFrame_4_Speed;
			m_KeyFrameDesc.KeyFrame_fRelaxRotation = m_tInstanceDesc.KeyFrame_4_fRelaxRotation;
			m_KeyFrameDesc.KeyFrame_Movement = m_tInstanceDesc.KeyFrame_4_Movement;
		}

		D3D11_MAPPED_SUBRESOURCE			SubResource;
		m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

		_float Size = m_tInstanceDesc.vScale.x;
		_float Size_Speed = m_tInstanceDesc.vScale_Speed.x;

		for (_uint i = 0; i < m_iNumInstance; ++i)
		{
			m_vecParticle[i].fSpeed = m_vecParticle[i].fSpeed * m_KeyFrameDesc.KeyFrame_RelaxSpeed;
			m_vecParticle[i].fSpeed += m_KeyFrameDesc.KeyFrame_Speed * (_float)_dTimeDelta;

			vMovement = XMLoadFloat4(&m_vecParticle[i].vMovement);
			XMStoreFloat4(&m_vecParticle[i].vMovement, XMVector3Normalize(vMovement + XMLoadFloat4(&m_KeyFrameDesc.KeyFrame_Movement)));

			// 빌보드 안할때
			if (false == m_tInstanceDesc.bBillboard)
			{
				XMStoreFloat4(&((VTXEFFECTINSTANCE*)SubResource.pData)[i].vTranslation,
					XMLoadFloat4(&((VTXEFFECTINSTANCE*)SubResource.pData)[i].vTranslation) + (vMovement * (m_vecParticle[i].fSpeed) * (_float)_dTimeDelta));
				_vector vLook = (vMovement * Size) + (vMovement * (m_vecParticle[i].fSpeed * Size * Size_Speed));
				// vLook = vLook * (Size + Size * Size_Speed * m_vecParticle[i].fSpeed)
				XMStoreFloat4(&((VTXEFFECTINSTANCE*)SubResource.pData)[i].vLook, vLook);
			}
			// 빌보드 일때
			else
			{
				_float	fSize = m_vecParticle[i].fBillboardSize;
				_matrix CamMatrix = XMMatrixInverse(nullptr, CGameInstance::Get_Instance()->Get_TransformMatrix(CPipeLine::D3DTS_VIEW));
				XMStoreFloat4(&((VTXEFFECTINSTANCE*)SubResource.pData)[i].vRight, CamMatrix.r[0] * fSize);
				XMStoreFloat4(&((VTXEFFECTINSTANCE*)SubResource.pData)[i].vUp, CamMatrix.r[1] * fSize);
				XMStoreFloat4(&((VTXEFFECTINSTANCE*)SubResource.pData)[i].vLook, CamMatrix.r[2] * fSize);
				XMStoreFloat4(&((VTXEFFECTINSTANCE*)SubResource.pData)[i].vTranslation, 
					XMLoadFloat4(&((VTXEFFECTINSTANCE*)SubResource.pData)[i].vTranslation) + (vMovement * (m_vecParticle[i].fSpeed) * (_float)_dTimeDelta));
			}
		}

		m_pContext->Unmap(m_pVBInstance, 0);
	}
	else
	{
		if (m_fMaxTime + m_tInstanceDesc.fLoop < m_fTime)
			return;

		//m_fTime += (_float)_dTimeDelta;			// 왜 두번 더해주지? 그래서 중간에 멈추나?
		_float Time = m_fMaxTime - m_fTime;		// 남은 시간
		_float Loop = m_tInstanceDesc.fLoop;

		D3D11_MAPPED_SUBRESOURCE			SubResource;
		m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

		_float Size = m_tInstanceDesc.vScale.x;
		_float Size_Speed = m_tInstanceDesc.vScale_Speed.x;
		_vector		vLook;
		for (_uint i = 0; i < m_iNumInstance; ++i)
		{
			m_vecParticle[i].fTime -= (_float)_dTimeDelta;

			if (0 > m_vecParticle[i].fTime)
			{
				if (m_tInstanceDesc.fLoop < Time /*- 1.f*/)
				{
					m_vecParticle[i].fSpeed = m_tInstanceDesc.fSpeed + (rand() % 100 * m_tInstanceDesc.fSpeedRand / 100);
					m_vecParticle[i].vMovement = _float4(m_tInstanceDesc.vMovement.x + (rand() % 100 * m_tInstanceDesc.vMovementRand.x / 100), 
														m_tInstanceDesc.vMovement.y + (rand() % 100 * m_tInstanceDesc.vMovementRand.y / 100), 
														m_tInstanceDesc.vMovement.z + (rand() % 100 * m_tInstanceDesc.vMovementRand.z / 100), 0.f);
					m_vecParticle[i].fTime = m_tInstanceDesc.fLoop;
					_vector		vLook = XMVector3Normalize(XMLoadFloat4(&m_vecParticle[i].vMovement));
					_vector		vAxis = XMVectorSet(0.f, 1.f, 0.f, 0.f);
					_vector		vRight = XMVector3Cross(vAxis, vLook);
					_vector		vUp = XMVector3Cross(vLook, vRight);
					_vector		vPos = XMVectorSet(m_tInstanceDesc.vPosition.x + (rand() % 100 * m_tInstanceDesc.vPositionRand.x / 100), 
													m_tInstanceDesc.vPosition.y + (rand() % 100 * m_tInstanceDesc.vPositionRand.y / 100), 
													m_tInstanceDesc.vPosition.z + (rand() % 100 * m_tInstanceDesc.vPositionRand.z / 100), 1.f);

					XMStoreFloat4(&m_vecParticle[i].vMovement, vLook);
					XMStoreFloat4(&((VTXEFFECTINSTANCE*)SubResource.pData)[i].vLook, vLook * (Size));
					XMStoreFloat4(&((VTXEFFECTINSTANCE*)SubResource.pData)[i].vRight, XMVector3Normalize(vRight) * Size);
					XMStoreFloat4(&((VTXEFFECTINSTANCE*)SubResource.pData)[i].vUp, XMVector3Normalize(vUp) * Size);
					XMStoreFloat4(&((VTXEFFECTINSTANCE*)SubResource.pData)[i].vTranslation, vPos + vLook * m_tInstanceDesc.fLastTime);

					m_KeyFrameDesc.KeyFrame_TimeEnd = m_tInstanceDesc.KeyFrame_1_TimeEnd;
					m_KeyFrameDesc.KeyFrame_RelaxSpeed = m_tInstanceDesc.KeyFrame_1_RelaxSpeed;
					m_KeyFrameDesc.KeyFrame_Speed = m_tInstanceDesc.KeyFrame_1_Speed;
					m_KeyFrameDesc.KeyFrame_fRelaxRotation = m_tInstanceDesc.KeyFrame_1_fRelaxRotation;
					m_KeyFrameDesc.KeyFrame_Movement = m_tInstanceDesc.KeyFrame_1_Movement;
				}
				else
				{
					m_vecParticle[i].fTime = 0.f;
				}

			}
			else if (m_tInstanceDesc.KeyFrame_3_TimeEnd < m_vecParticle[i].fTime)
			{
				m_KeyFrameDesc.KeyFrame_TimeEnd = m_tInstanceDesc.KeyFrame_4_TimeEnd;
				m_KeyFrameDesc.KeyFrame_RelaxSpeed = m_tInstanceDesc.KeyFrame_4_RelaxSpeed;
				m_KeyFrameDesc.KeyFrame_Speed = m_tInstanceDesc.KeyFrame_4_Speed;
				m_KeyFrameDesc.KeyFrame_fRelaxRotation = m_tInstanceDesc.KeyFrame_4_fRelaxRotation;
				m_KeyFrameDesc.KeyFrame_Movement = m_tInstanceDesc.KeyFrame_4_Movement;
			}
			else if (m_tInstanceDesc.KeyFrame_2_TimeEnd < m_vecParticle[i].fTime)
			{
				m_KeyFrameDesc.KeyFrame_TimeEnd = m_tInstanceDesc.KeyFrame_3_TimeEnd;
				m_KeyFrameDesc.KeyFrame_RelaxSpeed = m_tInstanceDesc.KeyFrame_3_RelaxSpeed;
				m_KeyFrameDesc.KeyFrame_Speed = m_tInstanceDesc.KeyFrame_3_Speed;
				m_KeyFrameDesc.KeyFrame_fRelaxRotation = m_tInstanceDesc.KeyFrame_3_fRelaxRotation;
				m_KeyFrameDesc.KeyFrame_Movement = m_tInstanceDesc.KeyFrame_3_Movement;
			}
			else if (m_tInstanceDesc.KeyFrame_1_TimeEnd < m_vecParticle[i].fTime)
			{
				m_KeyFrameDesc.KeyFrame_TimeEnd = m_tInstanceDesc.KeyFrame_2_TimeEnd;
				m_KeyFrameDesc.KeyFrame_RelaxSpeed = m_tInstanceDesc.KeyFrame_2_RelaxSpeed;
				m_KeyFrameDesc.KeyFrame_Speed = m_tInstanceDesc.KeyFrame_2_Speed;
				m_KeyFrameDesc.KeyFrame_fRelaxRotation = m_tInstanceDesc.KeyFrame_2_fRelaxRotation;
				m_KeyFrameDesc.KeyFrame_Movement = m_tInstanceDesc.KeyFrame_2_Movement;
			}
			else if (0.f < m_vecParticle[i].fTime)
			{
				m_KeyFrameDesc.KeyFrame_TimeEnd = m_tInstanceDesc.KeyFrame_1_TimeEnd;
				m_KeyFrameDesc.KeyFrame_RelaxSpeed = m_tInstanceDesc.KeyFrame_1_RelaxSpeed;
				m_KeyFrameDesc.KeyFrame_Speed = m_tInstanceDesc.KeyFrame_1_Speed;
				m_KeyFrameDesc.KeyFrame_fRelaxRotation = m_tInstanceDesc.KeyFrame_1_fRelaxRotation;
				m_KeyFrameDesc.KeyFrame_Movement = m_tInstanceDesc.KeyFrame_1_Movement;
			}

			m_vecParticle[i].fSpeed = m_vecParticle[i].fSpeed * m_KeyFrameDesc.KeyFrame_RelaxSpeed;
			m_vecParticle[i].fSpeed += m_KeyFrameDesc.KeyFrame_Speed * (_float)_dTimeDelta;

			vLook = XMLoadFloat4(&m_vecParticle[i].vMovement);
			if (false == m_tInstanceDesc.bBillboard)
			{
				XMStoreFloat4(&m_vecParticle[i].vMovement, XMVector3Normalize(vLook + XMLoadFloat4(&m_KeyFrameDesc.KeyFrame_Movement)));
				XMStoreFloat4(&((VTXEFFECTINSTANCE*)SubResource.pData)[i].vTranslation, 
					XMLoadFloat4(&((VTXEFFECTINSTANCE*)SubResource.pData)[i].vTranslation) + (vLook * (m_vecParticle[i].fSpeed) * (_float)_dTimeDelta));
				vLook = (vLook * Size) + (vLook * (m_vecParticle[i].fSpeed * Size * Size_Speed));
				XMStoreFloat4(&((VTXEFFECTINSTANCE*)SubResource.pData)[i].vLook, vLook * m_vecParticle[i].fTime / Loop);
				XMStoreFloat4(&((VTXEFFECTINSTANCE*)SubResource.pData)[i].vRight, XMVector3Normalize(XMLoadFloat4(&((VTXEFFECTINSTANCE*)SubResource.pData)[i].vRight)) * Size * m_vecParticle[i].fTime / Loop);
				XMStoreFloat4(&((VTXEFFECTINSTANCE*)SubResource.pData)[i].vUp, XMVector3Normalize(XMLoadFloat4(&((VTXEFFECTINSTANCE*)SubResource.pData)[i].vUp)) * Size * m_vecParticle[i].fTime / Loop);
			}
			else
			{
				_float	fSize = m_vecParticle[i].fBillboardSize;
				_matrix CamMatrix = XMMatrixInverse(nullptr, CGameInstance::Get_Instance()->Get_TransformMatrix(CPipeLine::D3DTS_VIEW));
				XMStoreFloat4(&m_vecParticle[i].vMovement, XMVector3Normalize(vLook + XMLoadFloat4(&m_KeyFrameDesc.KeyFrame_Movement)));
				XMStoreFloat4(&((VTXEFFECTINSTANCE*)SubResource.pData)[i].vTranslation, 
					XMLoadFloat4(&((VTXEFFECTINSTANCE*)SubResource.pData)[i].vTranslation) + (vLook * (m_vecParticle[i].fSpeed) * (_float)_dTimeDelta));
				XMStoreFloat4(&((VTXEFFECTINSTANCE*)SubResource.pData)[i].vRight, CamMatrix.r[0] * fSize * m_vecParticle[i].fTime / Loop);
				XMStoreFloat4(&((VTXEFFECTINSTANCE*)SubResource.pData)[i].vUp, CamMatrix.r[1] * fSize * m_vecParticle[i].fTime / Loop);
				XMStoreFloat4(&((VTXEFFECTINSTANCE*)SubResource.pData)[i].vLook, CamMatrix.r[2] * fSize * m_vecParticle[i].fTime / Loop);
			}
		}
		m_pContext->Unmap(m_pVBInstance, 0);
	}
}

HRESULT CVIBuffer_Effect_Instance::Render()
{
	if (nullptr == m_pContext)
		return E_FAIL;

	ID3D11Buffer* pVertexBuffers[] = { m_pVB, m_pVBInstance };
	_uint				iStrides[] = { m_iStride, m_iStrideInstance };
	_uint				iOffset[] = { 0, 0 };

	m_pContext->IASetVertexBuffers(0, m_iNumBuffers, pVertexBuffers, iStrides, iOffset);
	m_pContext->IASetIndexBuffer(m_pIB, m_eFormat, 0);
	m_pContext->IASetPrimitiveTopology(m_eTopology);
	m_pContext->DrawIndexedInstanced(6, m_iNumInstance, 0, 0, 0);

	return S_OK;
}

HRESULT CVIBuffer_Effect_Instance::CreateInstanceBuffer(const D3D11_BUFFER_DESC& _tBufferDesc, const D3D11_SUBRESOURCE_DATA& _tBufferSubResourceData)
{
	if (nullptr == m_pDevice)
		return E_FAIL;

	m_pDevice->CreateBuffer(&_tBufferDesc, &_tBufferSubResourceData, &m_pVBInstance);

	return S_OK;
}

HRESULT CVIBuffer_Effect_Instance::ChangeBufferCount(_uint iCount)
{
	if (nullptr == m_pVB || nullptr == m_pIB || nullptr == m_pVBInstance)
		return E_FAIL;

	Safe_Release_DV(m_pVB);
	Safe_Release_DV(m_pIB);
	Safe_Release_DV(m_pVBInstance);

	m_fTime = 0.f;
	if (FAILED(NativeConstruct_Prototype(iCount)))
		return E_FAIL;
	if (FAILED(NativeConstruct_Clone(nullptr)))
		return E_FAIL;

	return S_OK;
}

void CVIBuffer_Effect_Instance::Reset()
{
	m_fTime = 0.f;

	if (FAILED(SetUp_VertexBuffer()))
	{
		MSG_BOX("Failed To Reset : VertexBuffer");
		return;
	}
	if (FAILED(SetUp_InstanceBuffer()))
	{
		MSG_BOX("Failed To Reset : InstanceBuffer");
		return;
	}

	Reset_Pariticle();
	Reset_InstanceBuffer();
}


HRESULT CVIBuffer_Effect_Instance::SetUp_VertexBuffer()
{
	Safe_Release_DV(m_pVB);

	m_iNumVertices = 4;
	m_iNumBuffers = 2;
	m_iStride = sizeof(VTXTEX);

	D3D11_BUFFER_DESC tBufferDesc;
	D3D11_SUBRESOURCE_DATA tBufferSubResourceData;
	ZeroMemory(&tBufferDesc, sizeof(D3D11_BUFFER_DESC));
	tBufferDesc.ByteWidth = m_iStride * m_iNumVertices;
	tBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	tBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	tBufferDesc.StructureByteStride = m_iStride;
	tBufferDesc.CPUAccessFlags = 0;
	tBufferDesc.MiscFlags = 0;

	VTXTEX* pVertices = new VTXTEX[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXTEX) * m_iNumVertices);

	if (m_tInstanceDesc.bBillboard)
	{
		pVertices[0].vPosition = _float3(-0.5f, 0.5f, 0.f);
		pVertices[0].vTexUV = _float2(0.0f, 0.0f);

		pVertices[1].vPosition = _float3(0.5f, 0.5f, 0.f);
		pVertices[1].vTexUV = _float2(1.0f, 0.0f);

		pVertices[2].vPosition = _float3(0.5f, -0.5f, 0.f);
		pVertices[2].vTexUV = _float2(1.0f, 1.0f);

		pVertices[3].vPosition = _float3(-0.5f, -0.5f, 0.f);
		pVertices[3].vTexUV = _float2(0.0f, 1.0f);
	}
	else
	{
		pVertices[0].vPosition = _float3(0.f, 0.5f, -0.5f);
		pVertices[0].vTexUV = _float2(0.0f, 0.0f);

		pVertices[1].vPosition = _float3(0.f, 0.5f, 0.5f);
		pVertices[1].vTexUV = _float2(1.0f, 0.0f);

		pVertices[2].vPosition = _float3(0.f, -0.5f, 0.5f);
		pVertices[2].vTexUV = _float2(1.0f, 1.0f);

		pVertices[3].vPosition = _float3(0.f, -0.5f, -0.5f);
		pVertices[3].vTexUV = _float2(0.0f, 1.0f);
	}

	ZeroMemory(&tBufferSubResourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	tBufferSubResourceData.pSysMem = pVertices;

	if (FAILED(__super::Create_VertexBuffer(tBufferDesc, tBufferSubResourceData)))
		return E_FAIL;

	Safe_Delete_Array(pVertices);

	return S_OK;
}

HRESULT CVIBuffer_Effect_Instance::SetUp_IndexBuffer()
{
	Safe_Release_DV(m_pIB);

	m_iNumPrimitive = 2 * m_iNumInstance;
	m_iNumIndicesPerPrimitive = 3;
	m_iBytePerPrimitive = sizeof(FACEINDICES16);
	m_eFormat = DXGI_FORMAT_R16_UINT;
	m_eTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	D3D11_BUFFER_DESC tBufferDesc;
	D3D11_SUBRESOURCE_DATA tBufferSubResourceData;
	ZeroMemory(&tBufferDesc, sizeof(D3D11_BUFFER_DESC));
	tBufferDesc.ByteWidth = m_iBytePerPrimitive * m_iNumPrimitive;
	tBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	tBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	tBufferDesc.StructureByteStride = 0;
	tBufferDesc.CPUAccessFlags = 0;
	tBufferDesc.MiscFlags = 0;

	FACEINDICES16* pIndices = new FACEINDICES16[m_iNumPrimitive];
	ZeroMemory(pIndices, sizeof(FACEINDICES16) * m_iNumPrimitive);

	_uint		iNumFaces = 0;
	for (_uint i = 0; i < m_iNumInstance; ++i)
	{
		pIndices[iNumFaces]._1 = 0;
		pIndices[iNumFaces]._2 = 1;
		pIndices[iNumFaces]._3 = 2;
		++iNumFaces;

		pIndices[iNumFaces]._1 = 0;
		pIndices[iNumFaces]._2 = 2;
		pIndices[iNumFaces]._3 = 3;
		++iNumFaces;
	}

	ZeroMemory(&tBufferSubResourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	tBufferSubResourceData.pSysMem = pIndices;
	if (FAILED(__super::Create_IndexBuffer(tBufferDesc, tBufferSubResourceData)))
		return E_FAIL;

	Safe_Delete_Array(pIndices);
	return S_OK;
}

HRESULT CVIBuffer_Effect_Instance::SetUp_InstanceBuffer()
{
	Safe_Release_DV(m_pVBInstance);

	m_iStrideInstance = sizeof(VTXEFFECTINSTANCE);

	D3D11_BUFFER_DESC tBufferDesc;
	D3D11_SUBRESOURCE_DATA tBufferSubResourceData;
	ZeroMemory(&tBufferDesc, sizeof(D3D11_BUFFER_DESC));
	tBufferDesc.ByteWidth = m_iStrideInstance * m_iNumInstance;
	tBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	tBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	tBufferDesc.StructureByteStride = m_iStrideInstance;
	tBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	tBufferDesc.MiscFlags = 0;

	VTXEFFECTINSTANCE* pInstanceVertices = new VTXEFFECTINSTANCE[m_iNumInstance];
	ZeroMemory(pInstanceVertices, sizeof(VTXEFFECTINSTANCE) * m_iNumInstance);


	for (_uint i = 0; i < m_iNumInstance; ++i)
	{
		pInstanceVertices[i].vRight = _float4(1.f, 0.f, 0.f, 0.f);
		pInstanceVertices[i].vUp = _float4(0.f, 1.f, 0.f, 0.f);
		pInstanceVertices[i].vLook = _float4(0.f, 0.f, 1.f, 0.f);
		pInstanceVertices[i].vTranslation = _float4((_float)(rand() % 3), 0.f, (_float)(rand() % 3), 1.f); //_float4((_float)(rand() % 3), 10.0f, (_float)(rand() % 3), 1.f);
	}

	ZeroMemory(&tBufferSubResourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	tBufferSubResourceData.pSysMem = pInstanceVertices;
	if (FAILED(CreateInstanceBuffer(tBufferDesc, tBufferSubResourceData)))
		return E_FAIL;
	Safe_Delete_Array(pInstanceVertices);

	return S_OK;
}

void CVIBuffer_Effect_Instance::Reset_Pariticle()
{
	m_vecParticle.clear();
	vector<PARTICLEDESC>().swap(m_vecParticle);
	PARTICLEDESC	tParticleDesc;
	for (_uint i = 0; i < m_iNumInstance; ++i)
	{
		ZeroMemory(&tParticleDesc, sizeof(PARTICLEDESC));
		tParticleDesc.fSpeed = 1.f;
		tParticleDesc.fTime = 0.f;
		tParticleDesc.fBillboardSize = 1.f;
		tParticleDesc.vMovement = _float4(0.f, 0.f, 1.f, 0.f);
		m_vecParticle.push_back(tParticleDesc);
	}

	if (m_tInstanceDesc.fLoop > 0)
	{
		m_fMaxTime = m_tInstanceDesc.fMaxTime;
		m_tInstanceDesc.KeyFrame_1_TimeEnd = (m_tInstanceDesc.fLoop * 1) / 4;
		m_tInstanceDesc.KeyFrame_2_TimeEnd = (m_tInstanceDesc.fLoop * 2) / 4;
		m_tInstanceDesc.KeyFrame_3_TimeEnd = (m_tInstanceDesc.fLoop * 3) / 4;
		m_tInstanceDesc.KeyFrame_4_TimeEnd = (m_tInstanceDesc.fLoop * 4) / 4;

	}
	else
	{
		m_fMaxTime = m_tInstanceDesc.fMaxTime;
		m_tInstanceDesc.KeyFrame_1_TimeEnd = (m_tInstanceDesc.fMaxTime * 1) / 4;
		m_tInstanceDesc.KeyFrame_2_TimeEnd = (m_tInstanceDesc.fMaxTime * 2) / 4;
		m_tInstanceDesc.KeyFrame_3_TimeEnd = (m_tInstanceDesc.fMaxTime * 3) / 4;
		m_tInstanceDesc.KeyFrame_4_TimeEnd = (m_tInstanceDesc.fMaxTime * 4) / 4;
	}
}

void CVIBuffer_Effect_Instance::Reset_InstanceBuffer()
{
	if (nullptr == m_pContext)
		return;

	D3D11_MAPPED_SUBRESOURCE			SubResource;
	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

	_float Size = m_tInstanceDesc.vScale.x;
	_float Size_Speed = m_tInstanceDesc.vScale_Speed.x;
	if (m_tInstanceDesc.bBillboard)
	{
		_matrix Mx = XMMatrixIdentity();

		for (_uint i = 0; i < m_iNumInstance; ++i)
		{
			m_vecParticle[i].fSpeed = m_tInstanceDesc.fSpeed + (rand() % 100 * m_tInstanceDesc.fSpeedRand / 100);
			m_vecParticle[i].vMovement = _float4(m_tInstanceDesc.vMovement.x + (rand() % 100 * m_tInstanceDesc.vMovementRand.x / 100), 
												m_tInstanceDesc.vMovement.y + (rand() % 100 * m_tInstanceDesc.vMovementRand.y / 100), 
												m_tInstanceDesc.vMovement.z + (rand() % 100 * m_tInstanceDesc.vMovementRand.z / 100), 0.f);
			m_vecParticle[i].fTime = rand() % 100 * (m_tInstanceDesc.fLoop / 100);
			m_vecParticle[i].fBillboardSize = Size + (rand() % 100 * Size_Speed / 100);

			_vector		vLook = XMVector3Normalize(XMLoadFloat4(&m_vecParticle[i].vMovement));
			_vector		vPos = XMVectorSet(m_tInstanceDesc.vPosition.x + (rand() % 100 * m_tInstanceDesc.vPositionRand.x / 100),
				m_tInstanceDesc.vPosition.y + (rand() % 100 * m_tInstanceDesc.vPositionRand.y / 100),
				m_tInstanceDesc.vPosition.z + (rand() % 100 * m_tInstanceDesc.vPositionRand.z / 100), 1.f);
			_float		fSize = m_vecParticle[i].fBillboardSize;

			XMStoreFloat4(&m_vecParticle[i].vMovement, vLook);
			XMStoreFloat4(&((VTXEFFECTINSTANCE*)SubResource.pData)[i].vLook, Mx.r[2] * fSize);
			XMStoreFloat4(&((VTXEFFECTINSTANCE*)SubResource.pData)[i].vRight, Mx.r[0] * fSize);
			XMStoreFloat4(&((VTXEFFECTINSTANCE*)SubResource.pData)[i].vUp, Mx.r[1] * fSize);
			XMStoreFloat4(&((VTXEFFECTINSTANCE*)SubResource.pData)[i].vTranslation, vPos + vLook * m_tInstanceDesc.fLastTime);

		}
	}
	else
	{
		for (_uint i = 0; i < m_iNumInstance; ++i)
		{
			m_vecParticle[i].fSpeed = m_tInstanceDesc.fSpeed + (rand() % 100 * m_tInstanceDesc.fSpeedRand / 100);
			m_vecParticle[i].vMovement = _float4(m_tInstanceDesc.vMovement.x + (rand() % 100 * m_tInstanceDesc.vMovementRand.x / 100),
				m_tInstanceDesc.vMovement.y + (rand() % 100 * m_tInstanceDesc.vMovementRand.y / 100),
				m_tInstanceDesc.vMovement.z + (rand() % 100 * m_tInstanceDesc.vMovementRand.z / 100), 0.f);
			m_vecParticle[i].fTime = rand() % 100 * (m_tInstanceDesc.fLoop / 100);
			_vector		vLook = XMVector3Normalize(XMLoadFloat4(&m_vecParticle[i].vMovement));
			_vector		vAxis = XMVectorSet(0.f, 1.f, 0.f, 0.f);
			_vector		vRight = XMVector3Cross(vAxis, vLook);
			_vector		vUp = XMVector3Cross(vLook, vRight);
			_vector		vPos = XMVectorSet(m_tInstanceDesc.vPosition.x + (rand() % 100 * m_tInstanceDesc.vPositionRand.x / 100),
				m_tInstanceDesc.vPosition.y + (rand() % 100 * m_tInstanceDesc.vPositionRand.y / 100),
				m_tInstanceDesc.vPosition.z + (rand() % 100 * m_tInstanceDesc.vPositionRand.z / 100), 1.f);

			XMStoreFloat4(&m_vecParticle[i].vMovement, vLook);
			XMStoreFloat4(&((VTXEFFECTINSTANCE*)SubResource.pData)[i].vLook, vLook * (Size));
			XMStoreFloat4(&((VTXEFFECTINSTANCE*)SubResource.pData)[i].vRight, XMVector3Normalize(vRight) * Size);
			XMStoreFloat4(&((VTXEFFECTINSTANCE*)SubResource.pData)[i].vUp, XMVector3Normalize(vUp) * Size);
			XMStoreFloat4(&((VTXEFFECTINSTANCE*)SubResource.pData)[i].vTranslation, vPos + vLook * m_tInstanceDesc.fLastTime);

		}
	}
	m_pContext->Unmap(m_pVBInstance, 0);
}

CVIBuffer_Effect_Instance* CVIBuffer_Effect_Instance::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, _uint iNumInstance)
{
	CVIBuffer_Effect_Instance* pInstance = new CVIBuffer_Effect_Instance(_pDevice, _pContext);
	if (FAILED(pInstance->NativeConstruct_Prototype(iNumInstance)))
	{
		MSG_BOX("Failed Create : CVIBuffer_Effect_Instance");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CVIBuffer_Effect_Instance::Clone(void* _pArg)
{
	CVIBuffer_Effect_Instance* pInstance = new CVIBuffer_Effect_Instance(*this);
	if (FAILED(pInstance->NativeConstruct_Clone(_pArg)))
	{
		MSG_BOX("Failed Cloned : CVIBuffer_Effect_Instance");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CVIBuffer_Effect_Instance::Free()
{
	__super::Free();

	Safe_Release_DV(m_pVBInstance);
	m_vecParticle.clear();
}
