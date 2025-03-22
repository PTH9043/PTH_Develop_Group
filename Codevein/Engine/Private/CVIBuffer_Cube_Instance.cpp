#include "CVIBuffer_Cube_Instance.h"
#include "CGameInstance.h"

CVIBuffer_Cube_Instance::CVIBuffer_Cube_Instance(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    : CVIBuffer(_pDevice, _pContext)
{
}

CVIBuffer_Cube_Instance::CVIBuffer_Cube_Instance(const CVIBuffer_Cube_Instance& _rhs)
    : CVIBuffer(_rhs),  m_iStrideInstance(_rhs.m_iStrideInstance), m_iNumInstance(_rhs.m_iNumInstance)
    , m_CubeInstanceList(_rhs.m_CubeInstanceList)
{
//    Create_IndexBuffer_Instance(m_iNumInstance);
}

void CVIBuffer_Cube_Instance::Get_Data(VTXCUBEINSTANCE** _ppData, const _uint& _iIndex)
{
    D3D11_MAPPED_SUBRESOURCE		SubResource;

    m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

    *_ppData = &((VTXCUBEINSTANCE*)SubResource.pData)[_iIndex];

    m_pContext->Unmap(m_pVBInstance, 0);
}

void CVIBuffer_Cube_Instance::Set_Data(VTXCUBEINSTANCE* _pData, const _uint& _iIndex)
{
    D3D11_MAPPED_SUBRESOURCE		SubResource;

    m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

    ((VTXCUBEINSTANCE*)SubResource.pData)[_iIndex] = *_pData;

    m_pContext->Unmap(m_pVBInstance, 0);
}

EFFECTPARTICLEDESC* CVIBuffer_Cube_Instance::Get_InstanceData(const _uint& _iIndex)
{
    if (m_CubeInstanceList.size() <= _iIndex)
        return nullptr;

    return &m_CubeInstanceList[_iIndex];
}

void CVIBuffer_Cube_Instance::Set_InstnaceData(const EFFECTPARTICLEDESC& _tInfo, const _uint& _iIndex)
{
    if (m_CubeInstanceList.size() <= _iIndex)
        return;
    D3D11_MAPPED_SUBRESOURCE		SubResource;
    m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

    m_CubeInstanceList[_iIndex] = _tInfo;
    m_CubeInstanceList[_iIndex].Reset();
    ((VTXCUBEINSTANCE*)SubResource.pData)[_iIndex].vColor = m_CubeInstanceList[_iIndex].vParticleColor;
    ((VTXCUBEINSTANCE*)SubResource.pData)[_iIndex].vBlendColor = m_CubeInstanceList[_iIndex].vResultBlendColor;

    m_pContext->Unmap(m_pVBInstance, 0);
}

HRESULT CVIBuffer_Cube_Instance::NativeConstruct_Prototype()
{
    Create_VertexBuffer_Instance();
    Create_IndexBuffer_Instance(1);
    m_CubeInstanceList.resize(1);
    Create_VertexBuffer_Instance(1);
    return S_OK;
}

HRESULT CVIBuffer_Cube_Instance::NativeConstruct_Clone(void* _pArg)
{
    Create_VertexBuffer_Instance(m_iNumInstance);
    m_bCloned = true;
    return S_OK;
}

HRESULT CVIBuffer_Cube_Instance::Render()
{
    if (nullptr == m_pContext)
        return E_FAIL;

    ID3D11Buffer* pVertexBuffers[] = { m_pVB, m_pVBInstance };

    _uint				iStrides[] = { m_iStride,   m_iStrideInstance };

    _uint				iOffset[] = { 0,   0 };

    m_pContext->IASetVertexBuffers(0, m_iNumBuffers, pVertexBuffers, iStrides, iOffset);
    m_pContext->IASetIndexBuffer(m_pIB, m_eFormat, 0);
    m_pContext->IASetPrimitiveTopology(m_eTopology);
    m_pContext->DrawIndexedInstanced(36, m_iNumInstance, 0, 0, 0);

    return S_OK;
}

HRESULT CVIBuffer_Cube_Instance::Change_Cube_Count(const _uint& _iNumInstance)
{
    vector<EFFECTPARTICLEDESC> tRectInstanceInfo;
    _uint iSize = (_uint)m_CubeInstanceList.size();

    if (iSize < _iNumInstance)
    {
        tRectInstanceInfo.resize(_iNumInstance);
        for (_uint i = 0; i < iSize; ++i)
        {
            tRectInstanceInfo[i] = m_CubeInstanceList[i];
        }
        m_CubeInstanceList.clear();
        m_CubeInstanceList = tRectInstanceInfo;
    }
    else
    {
        tRectInstanceInfo.resize(_iNumInstance);
        for (_uint i = 0; i < _iNumInstance; ++i)
        {
            tRectInstanceInfo[i] = m_CubeInstanceList[i];
        }
        m_CubeInstanceList.clear();
        m_CubeInstanceList = tRectInstanceInfo;
    }

    if (FAILED(Create_VertexBuffer_Instance(_iNumInstance)))
        return E_FAIL;
    if (FAILED(Create_IndexBuffer_Instance(_iNumInstance)))
        return E_FAIL;

    return S_OK;
}

HRESULT CVIBuffer_Cube_Instance::Reset_Cube_Count()
{
    m_CubeInstanceList.clear();
    m_CubeInstanceList.resize(1);
    if (FAILED(Create_VertexBuffer_Instance(1)))
        return E_FAIL;
    if (FAILED(Create_IndexBuffer_Instance(1)))
        return E_FAIL;
    return S_OK;
}

void CVIBuffer_Cube_Instance::Delete_Instance(const _uint& _iIndex)
{
    _uint iIndex = 0;
    for (vector< EFFECTPARTICLEDESC>::iterator it = m_CubeInstanceList.begin(); it != m_CubeInstanceList.end();)
    {
        if (_iIndex == iIndex)
        {
            it = m_CubeInstanceList.erase(it);
        }
        else
            ++it;
        ++iIndex;
    }
}

void CVIBuffer_Cube_Instance::Update(CTransform* _pTransform, const _double& _dTimeDelta)
{
    if (nullptr == _pTransform)
        return;

    Update_Dir(_pTransform, _dTimeDelta);
}

void CVIBuffer_Cube_Instance::Late_Update(CTransform* _pTransform, const _double& _dTimeDelta)
{
}

void CVIBuffer_Cube_Instance::Update_Dir(CTransform* _pTransform, const _double& _dTimeDelta)
{
    D3D11_MAPPED_SUBRESOURCE		SubResource;

    m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

    for (_uint i = 0; i < m_CubeInstanceList.size(); ++i)
    {
        if (m_CubeInstanceList[i].bLoop)
        {
            if (m_CubeInstanceList[i].fSaveTime > m_CubeInstanceList[i].fLifeTime)
            {
                ((VTXRECTINSTANCE*)SubResource.pData)[i].vRight = _float4(m_CubeInstanceList[i].vSize.x, 0.f, 0.f, 0.f);
                ((VTXRECTINSTANCE*)SubResource.pData)[i].vUp = _float4(0.f, m_CubeInstanceList[i].vSize.y, 0.f, 0.f);
                ((VTXRECTINSTANCE*)SubResource.pData)[i].vLook = _float4(0.f, 0.f, m_CubeInstanceList[i].vSize.z, 0.f);
                ((VTXRECTINSTANCE*)SubResource.pData)[i].vTranslation = _float4(m_CubeInstanceList[i].vInitPos.x, m_CubeInstanceList[i].vInitPos.y,
                    m_CubeInstanceList[i].vInitPos.z, 1.f);
                ((VTXRECTINSTANCE*)SubResource.pData)[i].vColor = _float4(m_CubeInstanceList[i].vParticleColor.x, m_CubeInstanceList[i].vParticleColor.y,
                    m_CubeInstanceList[i].vParticleColor.z, m_CubeInstanceList[i].vParticleColor.w);
                m_CubeInstanceList[i].Reset();
            }
        }
        m_CubeInstanceList[i].Update_Data(_dTimeDelta);
        _float3 v3Right, v3Up, v3Look, vPos;
        _vector vChanagePos =  m_CubeInstanceList[i].Translate_Pos(
            XMLoadFloat4(&((VTXCUBEINSTANCE*)SubResource.pData)[i].vTranslation), _dTimeDelta);


        _matrix RotationMatrix;
        RotationMatrix = XMMatrixRotationRollPitchYaw(XMConvertToRadians(m_CubeInstanceList[i].vResultRotAnlge.x),
            XMConvertToRadians(m_CubeInstanceList[i].vResultRotAnlge.y),
            XMConvertToRadians(m_CubeInstanceList[i].vResultRotAnlge.z));

        _float3 v3Scale;
        v3Scale.x = m_CubeInstanceList[i].vSize.x + m_CubeInstanceList[i].vResultPmScaleSize.x;
        v3Scale.y = m_CubeInstanceList[i].vSize.y + m_CubeInstanceList[i].vResultPmScaleSize.y;
        v3Scale.z = m_CubeInstanceList[i].vSize.z + m_CubeInstanceList[i].vResultPmScaleSize.z;

        _float3 vR = _float3(1.f, 0.f, 0.f), vU = _float3(0.f, 1.f, 0.f), vL = _float3(0.f, 0.f, 1.f);
        _vector		vRight = XMLoadFloat3(&vR),
            vUp = XMLoadFloat3(&vU), vLook = XMLoadFloat3(&vL);

        vRight = XMVector3TransformNormal(vRight * v3Scale.x, RotationMatrix);
        vUp = XMVector3TransformNormal(vUp * v3Scale.y, RotationMatrix);
        vLook = XMVector3TransformNormal(vLook * v3Scale.z, RotationMatrix);

        _matrix Coord = XMMatrixRotationRollPitchYaw(XMConvertToRadians(m_CubeInstanceList[i].vRotTranslate.x),
            XMConvertToRadians(m_CubeInstanceList[i].vRotTranslate.y),
            XMConvertToRadians(m_CubeInstanceList[i].vRotTranslate.z));

        vChanagePos +=   XMVector3TransformNormal(vChanagePos, Coord) * m_CubeInstanceList[i].fRotLength;

        XMStoreFloat3(&v3Right, vRight);
        XMStoreFloat3(&v3Up, vUp);
        XMStoreFloat3(&v3Look, vLook);
        XMStoreFloat3(&vPos, vChanagePos);

        ((VTXCUBEINSTANCE*)SubResource.pData)[i].vRight = _float4(v3Right.x, v3Right.y, v3Right.z, 0.f);
        ((VTXCUBEINSTANCE*)SubResource.pData)[i].vUp = _float4(v3Up.x, v3Up.y, v3Up.z, 0.f);
        ((VTXCUBEINSTANCE*)SubResource.pData)[i].vLook = _float4(v3Look.x, v3Look.y, v3Look.z, 0.f);
        ((VTXCUBEINSTANCE*)SubResource.pData)[i].vTranslation = _float4(vPos.x, vPos.y, vPos.z, 1.f);
        ((VTXCUBEINSTANCE*)SubResource.pData)[i].vBlendColor = m_CubeInstanceList[i].vResultBlendColor;
    }
    m_pContext->Unmap(m_pVBInstance, 0);
}

void CVIBuffer_Cube_Instance::Reset_Data()
{
    D3D11_MAPPED_SUBRESOURCE		SubResource;

    m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);
    for (_uint i = 0; i < m_CubeInstanceList.size(); ++i)
    {
        ((VTXCUBEINSTANCE*)SubResource.pData)[i].vRight = _float4(m_CubeInstanceList[i].vSize.x, 0.f, 0.f, 0.f);
        ((VTXCUBEINSTANCE*)SubResource.pData)[i].vUp = _float4(0.f, m_CubeInstanceList[i].vSize.y, 0.f, 0.f);
        ((VTXCUBEINSTANCE*)SubResource.pData)[i].vLook = _float4(0.f, 0.f, m_CubeInstanceList[i].vSize.z, 0.f);
        ((VTXCUBEINSTANCE*)SubResource.pData)[i].vTranslation = _float4(m_CubeInstanceList[i].vInitPos.x, m_CubeInstanceList[i].vInitPos.y,
            m_CubeInstanceList[i].vInitPos.z, 1.f);
        ((VTXCUBEINSTANCE*)SubResource.pData)[i].vColor = _float4(m_CubeInstanceList[i].vParticleColor.x, m_CubeInstanceList[i].vParticleColor.y,
            m_CubeInstanceList[i].vParticleColor.z, m_CubeInstanceList[i].vParticleColor.w);
        m_CubeInstanceList[i].Reset();
    }
    m_pContext->Unmap(m_pVBInstance, 0);
}

void CVIBuffer_Cube_Instance::Save(const HANDLE& _hFile, DWORD* _dwByte)
{
    if (true == m_bCloned)
        return;

    WriteFile(_hFile, &m_iNumInstance, sizeof(_uint), _dwByte, nullptr);
    WriteFile(_hFile, &m_iStrideInstance, sizeof(_uint), _dwByte, nullptr);

    for (auto& iter : m_CubeInstanceList)
    {
        WriteFile(_hFile, &iter, sizeof(EFFECTPARTICLEDESC), _dwByte, nullptr);
        WriteFile(_hFile, &iter.vSize, sizeof(_float3), _dwByte, nullptr);
    }
}

void CVIBuffer_Cube_Instance::Load(const HANDLE& _hFile, DWORD* _dwByte)
{
    if (true == m_bCloned)
        return;
    m_CubeInstanceList.clear();
    ReadFile(_hFile, &m_iNumInstance, sizeof(_uint), _dwByte, nullptr);
    ReadFile(_hFile, &m_iStrideInstance, sizeof(_uint), _dwByte, nullptr);

    for (_uint i = 0; i < m_iNumInstance; ++i)
    {
        EFFECTPARTICLEDESC tDesc;
        ReadFile(_hFile, &tDesc, sizeof(EFFECTPARTICLEDESC), _dwByte, nullptr);
        _float3 v3Size = _float3(0.f, 0.f, 0.f);
        ReadFile(_hFile, &v3Size, sizeof(_float3), _dwByte, nullptr);
        tDesc.vSize = v3Size;
        m_CubeInstanceList.push_back(tDesc);
    }

    Create_VertexBuffer_Instance(m_iNumInstance);
    Create_IndexBuffer_Instance(m_iNumInstance);
}

HRESULT CVIBuffer_Cube_Instance::Create_VertexBuffer_Instance()
{
    // Need To Data
    D3D11_BUFFER_DESC tVertexBufferDesc;
    D3D11_SUBRESOURCE_DATA tVertexBufferSubResourceData;
#pragma region Vertex_Buffer
    m_iStride = sizeof(VTXCUBETEX);
    m_iNumVertices = 8;
    m_iNumBuffers = 2;
    // Buffer
    ZeroMemory(&tVertexBufferDesc, sizeof(D3D11_BUFFER_DESC));
    tVertexBufferDesc.ByteWidth = m_iStride * m_iNumVertices;
    tVertexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
    tVertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    tVertexBufferDesc.StructureByteStride = m_iStride;
    tVertexBufferDesc.CPUAccessFlags = 0;
    tVertexBufferDesc.MiscFlags = 0;
    // Vertex
    VTXCUBETEX* pVertices = new VTXCUBETEX[m_iNumVertices];
    ZeroMemory(pVertices, sizeof(VTXCUBETEX) * m_iNumVertices);

    m_pVerticesPos = new _float3[m_iNumVertices];
    ZeroMemory(m_pVerticesPos, sizeof(_float3) * m_iNumVertices);

    m_pVerticesPos[0] = pVertices[0].v3Position = _float3(-0.5f, 0.5f, -0.5f);
    pVertices[0].v3UvPosition = pVertices[0].v3Position;

    m_pVerticesPos[1] = pVertices[1].v3Position = _float3(0.5f, 0.5f, -0.5f);
    pVertices[1].v3UvPosition = pVertices[1].v3Position;

    m_pVerticesPos[2] = pVertices[2].v3Position = _float3(0.5f, -0.5f, -0.5f);
    pVertices[2].v3UvPosition = pVertices[2].v3Position;

    m_pVerticesPos[3] = pVertices[3].v3Position = _float3(-0.5f, -0.5f, -0.5f);
    pVertices[3].v3UvPosition = pVertices[3].v3Position;

    m_pVerticesPos[4] = pVertices[4].v3Position = _float3(-0.5f, 0.5f, 0.5f);
    pVertices[4].v3UvPosition = pVertices[4].v3Position;

    m_pVerticesPos[5] = pVertices[5].v3Position = _float3(0.5f, 0.5f, 0.5f);
    pVertices[5].v3UvPosition = pVertices[5].v3Position;

    m_pVerticesPos[6] = pVertices[6].v3Position = _float3(0.5f, -0.5f, 0.5f);
    pVertices[6].v3UvPosition = pVertices[6].v3Position;

    m_pVerticesPos[7] = pVertices[7].v3Position = _float3(-0.5f, -0.5f, 0.5f);
    pVertices[7].v3UvPosition = pVertices[7].v3Position;

    ZeroMemory(&tVertexBufferSubResourceData, sizeof(D3D11_SUBRESOURCE_DATA));
    tVertexBufferSubResourceData.pSysMem = pVertices;

    if (FAILED(__super::Create_VertexBuffer(tVertexBufferDesc, tVertexBufferSubResourceData)))
        return E_FAIL;

    Safe_Delete_Array(pVertices);
#pragma endregion Vertex_Buffer
    return S_OK;
}

HRESULT CVIBuffer_Cube_Instance::Create_VertexBuffer_Instance(const _uint& _iNum)
{
    if (0 == _iNum)
        return E_FAIL;

    if (nullptr != m_pVBInstance)
        Safe_Release(m_pVBInstance);

    if (m_CubeInstanceList.size() <= 1)
        m_CubeInstanceList.resize(_iNum);

    m_iNumInstance = _iNum;
    m_iStrideInstance = sizeof(VTXCUBEINSTANCE);

    D3D11_BUFFER_DESC tBufferDesc;
    ZeroMemory(&tBufferDesc, sizeof(D3D11_BUFFER_DESC));
    tBufferDesc.ByteWidth = m_iStrideInstance * m_iNumInstance;
    tBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    tBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    tBufferDesc.StructureByteStride = m_iStrideInstance;
    tBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    tBufferDesc.MiscFlags = 0;

    VTXCUBEINSTANCE* pInstanceVertices = new VTXCUBEINSTANCE[m_iNumInstance];
    ZeroMemory(pInstanceVertices, sizeof(VTXCUBEINSTANCE) * m_iNumInstance);

    for (_uint i = 0; i < _iNum; ++i)
    {
        pInstanceVertices[i].vTranslation = _float4(m_CubeInstanceList[i].vInitPos.x, 
            m_CubeInstanceList[i].vInitPos.y, m_CubeInstanceList[i].vInitPos.z, 1.f);
        pInstanceVertices[i].vRight = _float4(m_CubeInstanceList[i].vSize.x, 0.f, 0.f, 0.f);
        pInstanceVertices[i].vUp = _float4(0.f, m_CubeInstanceList[i].vSize.y, 0.f, 0.f);
        pInstanceVertices[i].vLook = _float4(0.f, 0.f, m_CubeInstanceList[i].vSize.z, 0.f);
        pInstanceVertices[i].vColor = m_CubeInstanceList[i].vParticleColor;
        pInstanceVertices[i].vBlendColor = m_CubeInstanceList[i].vBlendColor;
    }
    D3D11_SUBRESOURCE_DATA tBufferSubResourceData;
    ZeroMemory(&tBufferSubResourceData, sizeof(D3D11_SUBRESOURCE_DATA));
    tBufferSubResourceData.pSysMem = pInstanceVertices;

    if (FAILED(m_pDevice->CreateBuffer(&tBufferDesc, &tBufferSubResourceData, &m_pVBInstance)))
        return E_FAIL;

    Safe_Delete_Array(pInstanceVertices);

    return S_OK;
}

HRESULT CVIBuffer_Cube_Instance::Create_IndexBuffer_Instance(const _uint& _iNum)
{
    if (true == m_bCloned)
        return E_FAIL;

    if (0 == _iNum)
        return E_FAIL;

    if (nullptr != m_pIB)
        Safe_Release(m_pIB);

    m_iNumInstance = _iNum;
    m_iNumPrimitive = 12 * _iNum;
    m_iNumIndicesPerPrimitive = 3;
    m_iBytePerPrimitive = sizeof(FACEINDICES16);
    m_eFormat = DXGI_FORMAT_R16_UINT;
    m_eTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

#pragma region INDEX_BUFFER

    D3D11_BUFFER_DESC tBufferDesc;
    ZeroMemory(&tBufferDesc, sizeof(D3D11_BUFFER_DESC));

    tBufferDesc.ByteWidth = m_iBytePerPrimitive * m_iNumPrimitive;
    tBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
    tBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    tBufferDesc.StructureByteStride = 0;
    tBufferDesc.CPUAccessFlags = 0;
    tBufferDesc.MiscFlags = 0;

    FACEINDICES16* pIndices = new FACEINDICES16[m_iNumPrimitive];
    ZeroMemory(pIndices, sizeof(FACEINDICES16) * m_iNumPrimitive);

    _uint			iNumFaces = 0;

    for (_uint i = 0; i < m_iNumInstance; ++i)
    {
        /* +x */
        {
            pIndices[iNumFaces]._1 = 1; pIndices[iNumFaces]._2 = 5; pIndices[iNumFaces]._3 = 6;
            ++iNumFaces;
            pIndices[iNumFaces]._1 = 1; pIndices[iNumFaces]._2 = 6; pIndices[iNumFaces]._3 = 2;
            ++iNumFaces;
        }
        /* -x */
        {
            pIndices[iNumFaces]._1 = 4; pIndices[iNumFaces]._2 = 0; pIndices[iNumFaces]._3 = 3;
            ++iNumFaces;
            pIndices[iNumFaces]._1 = 4; pIndices[iNumFaces]._2 = 3; pIndices[iNumFaces]._3 = 7;
            ++iNumFaces;
        }
        /* +y */
        {
            pIndices[iNumFaces]._1 = 4; pIndices[iNumFaces]._2 = 5; pIndices[iNumFaces]._3 = 1;
            ++iNumFaces;
            pIndices[iNumFaces]._1 = 4; pIndices[iNumFaces]._2 = 1; pIndices[iNumFaces]._3 = 0;
            ++iNumFaces;
        }
        /* -y */
        {
            pIndices[iNumFaces]._1 = 3; pIndices[iNumFaces]._2 = 2; pIndices[iNumFaces]._3 = 6;
            ++iNumFaces;
            pIndices[iNumFaces]._1 = 3; pIndices[iNumFaces]._2 = 6; pIndices[iNumFaces]._3 = 7;
            ++iNumFaces;
        }
        /* +z */
        {
            pIndices[iNumFaces]._1 = 5; pIndices[iNumFaces]._2 = 4; pIndices[iNumFaces]._3 = 7;
            ++iNumFaces;
            pIndices[iNumFaces]._1 = 5; pIndices[iNumFaces]._2 = 7; pIndices[iNumFaces]._3 = 6;
            ++iNumFaces;
        }
        /* -z */
        {
            pIndices[iNumFaces]._1 = 0; pIndices[iNumFaces]._2 = 1; pIndices[iNumFaces]._3 = 2;
            ++iNumFaces;
            pIndices[iNumFaces]._1 = 0; pIndices[iNumFaces]._2 = 2; pIndices[iNumFaces]._3 = 3;
            ++iNumFaces;
        }
    }

    D3D11_SUBRESOURCE_DATA tBufferSubResourceData;
    ZeroMemory(&tBufferSubResourceData, sizeof(D3D11_SUBRESOURCE_DATA));
    tBufferSubResourceData.pSysMem = pIndices;

    if (FAILED(__super::Create_IndexBuffer(tBufferDesc, tBufferSubResourceData)))
        return E_FAIL;

    Safe_Delete_Array(pIndices);

#pragma endregion INDEX_BUFFER
    return S_OK;
}

CVIBuffer_Cube_Instance* CVIBuffer_Cube_Instance::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CVIBuffer_Cube_Instance* pInstance = new CVIBuffer_Cube_Instance(_pDevice, _pContext);
    if (FAILED(pInstance->NativeConstruct_Prototype()))
    {
        Safe_Release(pInstance);
        MSG_BOX("CVIBuffer_Cube_Instance::Create To Failed");
    }
    return pInstance;
}

CComponent* CVIBuffer_Cube_Instance::Clone(void* _pArg)
{
    CVIBuffer_Cube_Instance* pInstance = new CVIBuffer_Cube_Instance(*this);
    if (FAILED(pInstance->NativeConstruct_Clone(_pArg)))
    {
        Safe_Release(pInstance);
        MSG_BOX("CVIBuffer_Cube_Instance::Create To Failed");
    }
    return pInstance;
}

void CVIBuffer_Cube_Instance::Free()
{
    Safe_Release(m_pVBInstance);
    __super::Free();
}
